#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <thread>
#include <mutex>
#include <algorithm>
#include <cctype>
#include <ctime>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>

using namespace std;

const int PORT=3490;
const int BUFFER_SIZE=1024;

const int MAX_USERNAME_LENGTH=20;
const int MAX_USERNAME_ATTEMPTS=5;

const string USERNAME_PROMPT="Enter Username : ";
const string EMPTY_USERNAME="Username cannot be empty.\n";
const string USERNAME_TOO_LONG="Username cannot exceed 20 characters.\n";
const string USERNAME_EXISTS="Username already taken.\n";

struct Client{
    int socket;
    string username;
};

string trim(const string &str){
    size_t start=0;
    int end=(int)str.size()-1;

    while(start<str.size() && isspace((unsigned char)str[start])){
        start++;
    }

    while(end>=(int)start && isspace((unsigned char)str[end])){
        end--;
    }

    return str.substr(start,end-start+1);
}

string getTimeStamp(){
    time_t now=time(nullptr);
    tm *currentTime=localtime(&now);

    char buffer[20];

    strftime(buffer,sizeof(buffer),"[%H:%M:%S]",currentTime);

    return buffer;
}

bool sendMessage(int clientSocket,const string &message){
    if(send(clientSocket,message.c_str(),message.size(),MSG_NOSIGNAL)==-1){
        perror("send");
        return false;
    }

    return true;
}

class ChatServer{
private:
    int serverSocket=-1;
    vector<Client> clients;
    unordered_set<string> usernames;
    mutex clientsMutex;
    bool running=false;

    bool performHandshake(int clientSocket,string &username){
        char buffer[BUFFER_SIZE];

        for(int attempt=1;attempt<=MAX_USERNAME_ATTEMPTS;attempt++){

            if(!sendMessage(clientSocket,
            "\n========================================\n"
            "        TCP CHAT SERVER\n"
            "========================================\n"
            "Enter Username : ")){
                return false;
            }

            int bytesReceived=recv(clientSocket,buffer,BUFFER_SIZE-1,0);

            if(bytesReceived<=0){
                if(bytesReceived==-1){
                    perror("recv");
                }

                return false;
            }

            buffer[bytesReceived]='\0';

            username=trim(buffer);

            if(username.empty()){
                sendMessage(clientSocket,EMPTY_USERNAME);
                continue;
            }

            if(username.size()>MAX_USERNAME_LENGTH){
                sendMessage(clientSocket,USERNAME_TOO_LONG);
                continue;
            }

            {
                lock_guard<mutex> lock(clientsMutex);

                if(!usernames.count(username)){
                    usernames.insert(username);
                    clients.push_back({clientSocket,username});

                    sendMessage(clientSocket,
                    "\n========================================\n"
                    " Registration Successful\n"
                    "========================================\n"
                    "Welcome : "+username+"\n"
                    "Type /help to view commands.\n"
                    "Enjoy chatting!\n"
                    "========================================\n");

                    return true;
                }
            }

            sendMessage(clientSocket,USERNAME_EXISTS);
        }

        sendMessage(clientSocket,
        "Maximum username attempts exceeded.\n");

        return false;
    }
        int getSocketByUsername(const string &userName){
        lock_guard<mutex> lock(clientsMutex);

        auto it=find_if(clients.begin(),clients.end(),[&userName](const Client &client){ return client.username==userName; });

        if(it!=clients.end()){
            return it->socket;
        }

        return -1;
    }

    string getUsername(int clientSocket){
        lock_guard<mutex> lock(clientsMutex);

        auto it=find_if(clients.begin(),clients.end(),[clientSocket](const Client &client){ return client.socket==clientSocket; });

        if(it!=clients.end()){
            return it->username;
        }

        return "";
    }

    bool changeUsername(int clientSocket,const string &newUsername,string &oldUsername){
        lock_guard<mutex> lock(clientsMutex);

        if(usernames.count(newUsername)){
            return false;
        }

        auto it=find_if(clients.begin(),clients.end(),[clientSocket](const Client &client){ return client.socket==clientSocket; });

        if(it==clients.end()){
            return false;
        }

        oldUsername=it->username;

        usernames.erase(oldUsername);

        it->username=newUsername;

        usernames.insert(newUsername);

        return true;
    }

    bool processInput(const string &message,const string &username,int clientSocket){
        if(message.empty()){
            return true;
        }

        if(message[0]=='/'){
            processCommand(message,username,clientSocket);
        }
        else{
            string formattedMessage=getTimeStamp()+" ["+username+"] "+message+"\n";

            cout<<formattedMessage;

            broadcast(formattedMessage,clientSocket);
        }return true;
    }
    bool processCommand(const string &message,const string &username,int clientSocket){

        if(message=="/help"){

            sendMessage(clientSocket,
            "\n========================================\n"
            "           AVAILABLE COMMANDS\n"
            "========================================\n"
            "/help                                Show commands\n"
            "/list                                Show online users\n"
            "/nick <name>                         Change username\n"
            "/msg <username> <private_message>    Send Private Messages\n"
            "========================================\n");

            return true;
        }

        if(message=="/list"){

            string list;

            {
                lock_guard<mutex> lock(clientsMutex);

                list="\n========================================\n";
                list+="         ONLINE USERS ("+to_string(clients.size())+")\n";
                list+="========================================\n";

                int index=1;

                for(const Client &client:clients){
                    list+=to_string(index++)+". "+client.username+"\n";
                }

                list+="========================================\n";
            }

            sendMessage(clientSocket,list);

            return true;
        }
        if(message.find("/nick")==0){

            size_t pos=message.find(' ');

            if(pos==string::npos){
                sendMessage(clientSocket,"Usage : /nick <new_username>\n");
                return true;
            }

            string newUsername=trim(message.substr(pos+1));

            if(newUsername.empty()){
                sendMessage(clientSocket,EMPTY_USERNAME);
                return true;
            }

            if(newUsername.size()>MAX_USERNAME_LENGTH){
                sendMessage(clientSocket,USERNAME_TOO_LONG);
                return true;
            }

            string oldUsername=getUsername(clientSocket);

            if(newUsername==oldUsername){
                sendMessage(clientSocket,
                "You are already using this username.\n");
                return true;
            }

            if(changeUsername(clientSocket,newUsername,oldUsername)){

                sendMessage(clientSocket,
                "\n========================================\n"
                " Username Updated Successfully\n"
                "----------------------------------------\n"
                "Old : "+oldUsername+"\n"
                "New : "+newUsername+"\n"
                "========================================\n");

                broadcast(getTimeStamp()+" [SERVER] "+oldUsername+
                " is now known as "+newUsername+"\n",clientSocket);
            }
            else{
                sendMessage(clientSocket,USERNAME_EXISTS);
            }

            return true;
        }
        if(message.find("/msg")==0){

            size_t pos=message.find(' ');

            if(pos==string::npos){
                sendMessage(clientSocket,"Usage : /msg <username> <message>\n");
                return true;
            }

            string restMsg=trim(message.substr(pos+1));

            pos=restMsg.find(' ');

            if(pos==string::npos){
                sendMessage(clientSocket,"Usage : /msg <username> <message>\n");
                return true;
            }

            string privateUsername=restMsg.substr(0,pos);

            if(privateUsername==username){
                sendMessage(clientSocket,"You cannot send a private message to yourself.\n");
                return true;
            }

            string privateMessage=trim(restMsg.substr(pos+1));

            if(privateMessage.empty()){
                sendMessage(clientSocket,"Message cannot be empty.\n");
                return true;
            }

            int sock=getSocketByUsername(privateUsername);

            if(sock==-1){
                sendMessage(clientSocket,"User does not exist.\n");
                return true;
            }

            sendMessage(sock,
            "[PRIVATE]\n"
            +username+" : "+privateMessage+"\n");

            sendMessage(clientSocket,
            "[PRIVATE -> "+privateUsername+"]\n"
            +privateMessage+"\n");

            return true;
        }
        if (message.find("/quit")==0){
            sendMessage(clientSocket,"GoodBye "+username);
            return 0;
        }

        sendMessage(clientSocket,"Unknown command. Type /help for help.\n");
        return true;
    }
        void handleClient(int clientSocket){
        char buffer[BUFFER_SIZE];
        string username;

        if(!performHandshake(clientSocket,username)){
            close(clientSocket);
            return;
        }

        cout<<"\n========================================"<<endl;
        cout<<"[CONNECTED] "<<username<<endl;

        {
            lock_guard<mutex> lock(clientsMutex);
            cout<<"Online Users : "<<clients.size()<<endl;
        }

        cout<<"========================================"<<endl;

        broadcast(getTimeStamp()+" [SERVER] "+username+
        " joined the chat.\n",clientSocket);

        while(running){

            int bytesReceived=recv(clientSocket,buffer,BUFFER_SIZE-1,0);

            if(bytesReceived<=0){

                if(bytesReceived==-1){
                    perror("recv");
                }

                break;
            }

            buffer[bytesReceived]='\0';

            string message=trim(buffer);

            if(message.empty()){
                continue;
            }

            if(!processInput(message,username,clientSocket)){
                break;
            }

            username=getUsername(clientSocket);
        }

        string removedUsername=removeClient(clientSocket);

        if(!removedUsername.empty()){

            broadcast(getTimeStamp()+" [SERVER] "+
            removedUsername+" left the chat.\n",
            clientSocket);

            cout<<"\n========================================"<<endl;
            cout<<"[DISCONNECTED] "<<removedUsername<<endl;

            {
                lock_guard<mutex> lock(clientsMutex);
                cout<<"Online Users : "<<clients.size()<<endl;
            }

            cout<<"========================================"<<endl;
        }

        close(clientSocket);
    }

    string removeClient(int clientSocket){
        lock_guard<mutex> lock(clientsMutex);

        string username;

        auto it=find_if(clients.begin(),clients.end(),[clientSocket](const Client &client){ return client.socket==clientSocket; });

        if(it!=clients.end()){
            username=it->username;
            usernames.erase(username);
            clients.erase(it);
        }

        return username;
    }

    void broadcast(const string &message,int excludeSocket=-1){
        vector<int> sockets;

        {
            lock_guard<mutex> lock(clientsMutex);

            for(const Client &client:clients){
                if(client.socket!=excludeSocket){
                    sockets.push_back(client.socket);
                }
            }
        }

        for(int clientSocket:sockets){
            if(!sendMessage(clientSocket,message)){
                cout<<"[ERROR] Failed to send message to socket "<<clientSocket<<endl;
            }
        }
    }
        void acceptClients(){
        while(running){

            sockaddr_in clientAddress{};
            socklen_t clientLength=sizeof(clientAddress);

            int clientSocket=accept(serverSocket,(sockaddr*)&clientAddress,&clientLength);

            if(clientSocket==-1){

                if(!running){
                    break;
                }

                perror("accept");
                continue;
            }

            char clientIP[INET_ADDRSTRLEN];

            inet_ntop(AF_INET,&clientAddress.sin_addr,clientIP,INET_ADDRSTRLEN);

            cout<<"\n========================================"<<endl;
            cout<<"[NEW CONNECTION]"<<endl;
            cout<<"IP      : "<<clientIP<<endl;
            cout<<"Port    : "<<ntohs(clientAddress.sin_port)<<endl;
            cout<<"========================================"<<endl;

            thread clientThread(&ChatServer::handleClient,this,clientSocket);

            clientThread.detach();
        }
    }

    bool createSocket(){
        serverSocket=socket(AF_INET,SOCK_STREAM,0);

        if(serverSocket==-1){
            perror("socket");
            return false;
        }

        return true;
    }

    bool configureSocket(){
        int opt=1;

        if(setsockopt(serverSocket,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt))==-1){

            perror("setsockopt");
            close(serverSocket);
            serverSocket=-1;
            return false;
        }

        return true;
    }

    bool bindSocket(){
        sockaddr_in serverAddress{};

        serverAddress.sin_family=AF_INET;
        serverAddress.sin_addr.s_addr=INADDR_ANY;
        serverAddress.sin_port=htons(PORT);

        if(bind(serverSocket,(sockaddr*)&serverAddress,sizeof(serverAddress))==-1){

            perror("bind");
            close(serverSocket);
            serverSocket=-1;
            return false;
        }

        return true;
    }

    bool startListening(){
        if(listen(serverSocket,SOMAXCONN)==-1){
            perror("listen");
            close(serverSocket);
            serverSocket=-1;
            return false;
        }

        return true;
    }
        void printBanner(){
        cout<<"\n=========================================="<<endl;
        cout<<"           TCP CHAT SERVER"<<endl;
        cout<<"=========================================="<<endl;
        cout<<"Port            : "<<PORT<<endl;
        cout<<"Buffer Size     : "<<BUFFER_SIZE<<" Bytes"<<endl;
        cout<<"Max Username    : "<<MAX_USERNAME_LENGTH<<endl;
        cout<<"Max Attempts    : "<<MAX_USERNAME_ATTEMPTS<<endl;
        cout<<"Commands        : /help /list /nick /msg"<<endl;
        cout<<"Status          : Listening..."<<endl;
        cout<<"Started At      : "<<getTimeStamp()<<endl;
        cout<<"=========================================="<<endl;
    }

public:
    bool start(){
        if(!createSocket()){
            return false;
        }

        if(!configureSocket()){
            return false;
        }

        if(!bindSocket()){
            return false;
        }

        if(!startListening()){
            return false;
        }

        running=true;

        printBanner();

        acceptClients();

        return true;
    }

    void stop(){
        running=false;

        if(serverSocket!=-1){
            close(serverSocket);
            serverSocket=-1;
        }

        cout<<"\n=========================================="<<endl;
        cout<<"        SERVER SHUT DOWN"<<endl;
        cout<<"=========================================="<<endl;
    }
};

int main(){
    ChatServer server;

    if(!server.start()){
        cout<<"\n=========================================="<<endl;
        cout<<"Failed to start server."<<endl;
        cout<<"=========================================="<<endl;
        return 1;
    }

    server.stop();

    return 0;
}