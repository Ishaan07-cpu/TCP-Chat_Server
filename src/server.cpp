#include <cstring>
#include "server.h"
#include "Logger.h"
#include "utility.h"

const int BUFFER_SIZE=1024;

const int MAX_USERNAME_LENGTH=20;
const int MAX_USERNAME_ATTEMPTS=5;

const string USERNAME_PROMPT="Enter Username : ";
const string EMPTY_USERNAME="Username cannot be empty.\n";
const string USERNAME_TOO_LONG="Username cannot exceed 20 characters.\n";
const string USERNAME_EXISTS="Username already taken.\n";

ChatServer::ChatServer(const Config &config): serverSocket(-1), clients() ,usernames() ,clientsMutex(), running(false) ,port(config.getPort()), maxClients(config.getMaxClients()),logger(config.getLogFile()){}

bool ChatServer::sendMessage(int clientSocket,const string &message){
    if(send(clientSocket,message.c_str(),message.size(),MSG_NOSIGNAL)==-1){
        perror("send");
        logger.log(LogLevel::ERROR,"Failed to send message to socket "+to_string(clientSocket));
        return false;
    }

    return true;
}

bool ChatServer::performHandshake(int clientSocket,string &username){
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
                logger.log(LogLevel::INFO,username+" joined the chat");

                if(!sendMessage(clientSocket,
                "\n========================================\n"
                " Registration Successful\n"
                "========================================\n"
                "Welcome : "+username+"\n"
                "Type /help to view commands.\n"
                "Enjoy chatting!\n"
                "========================================\n")){

                    usernames.erase(username);
                    clients.pop_back();
                    logger.log(LogLevel::ERROR,"Failed to send welcome message to "+username);

                    return false;
                }

                return true;
            }
        }

        sendMessage(clientSocket,USERNAME_EXISTS);
    }

    sendMessage(clientSocket,
    "Maximum username attempts exceeded.\n");

    logger.log(LogLevel::WARNING,"Client failed username registration after maximum attempts");

    return false;
}

int ChatServer::getSocketByUsername(const string &userName){
    lock_guard<mutex> lock(clientsMutex);

    auto it=find_if(clients.begin(),clients.end(),[&userName](const Client &client){ return client.username==userName; });

    if(it!=clients.end()){
        return it->socket;
    }

    return -1;
}

string ChatServer::getUsername(int clientSocket){
    lock_guard<mutex> lock(clientsMutex);

    auto it=find_if(clients.begin(),clients.end(),[clientSocket](const Client &client){ return client.socket==clientSocket; });

    if(it!=clients.end()){
        return it->username;
    }

    return "";
}

bool ChatServer::changeUsername(int clientSocket,const string &newUsername,string &oldUsername){
    lock_guard<mutex> lock(clientsMutex);

    if(usernames.count(newUsername)){
        logger.log(
        LogLevel::WARNING,oldUsername+" attempted to change username to "+newUsername+" but it was already taken");
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

    logger.log(LogLevel::INFO,oldUsername+" changed username to "+newUsername);

    return true;
}

bool ChatServer::processInput(const string &input,const string &username,int clientSocket){
    ParsedCommand parsed = commandParser.parse(input);

    if(!parsed.command.empty() && parsed.command[0] == '/'){
        return processCommand(parsed,username,clientSocket);
    }

    string formattedMessage =getTimeStamp()+" ["+username+"] "+input+"\n";

    logger.log(LogLevel::INFO,username+" : "+input);

    cout << formattedMessage;

    broadcast(formattedMessage,clientSocket);

    return true;
}

bool ChatServer::processCommand(const ParsedCommand &parsed,const string &username,int clientSocket){
    if(parsed.command=="/help"){

        sendMessage(clientSocket,
        "\n========================================\n"
        "           AVAILABLE COMMANDS\n"
        "========================================\n"
        "/help                                Show commands\n"
        "/list                                Show online users\n"
        "/nick <name>                         Change username\n"
        "/msg <username> <private_message>    Send Private Messages\n"
        "/quit                                Disconnect\n"
        "========================================\n");

        return true;
    }

    if(parsed.command=="/list"){

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

    if(parsed.command == "/nick"){

        if(parsed.arguments.size() != 1){
            sendMessage(clientSocket,"Usage : /nick <new_username>\n");
            return true;
        }

        string newUsername = trim(parsed.arguments[0]);

        if(newUsername.empty()){
            sendMessage(clientSocket,EMPTY_USERNAME);
            return true;
        }

        if(newUsername.size() > MAX_USERNAME_LENGTH){
            sendMessage(clientSocket,USERNAME_TOO_LONG);
            return true;
        }

        if(newUsername.find(' ') != string::npos){
            sendMessage(
                clientSocket,
                "Username cannot contain spaces.\n"
            );
            return true;
        }

        string oldUsername = getUsername(clientSocket);

        if(newUsername == oldUsername){
            sendMessage(clientSocket,"You are already using this username.\n");
            return true;
        }

        if(changeUsername(clientSocket,newUsername,oldUsername)){

            sendMessage(
                clientSocket,
                "\n========================================\n"
                " Username Updated Successfully\n"
                "----------------------------------------\n"
                "Old : "+oldUsername+"\n"
                "New : "+newUsername+"\n"
                "========================================\n"
            );

            broadcast(getTimeStamp()+" [SERVER] "+oldUsername+" is now known as "+newUsername+"\n",clientSocket);
        }
        else{
            sendMessage(clientSocket,USERNAME_EXISTS);
        }

        return true;
    }

    if(parsed.command == "/msg"){

        if(parsed.arguments.size() < 2){
            sendMessage(clientSocket,"Usage : /msg <username> <message>\n");
            return true;
        }

        string privateUsername = parsed.arguments[0];

        string privateMessage;

        for(size_t i = 1; i < parsed.arguments.size(); i++){

            if(i > 1){
                privateMessage += " ";
            }

            privateMessage += parsed.arguments[i];
        }

        privateMessage = trim(privateMessage);

        if(privateMessage.empty()){
            sendMessage(clientSocket,"Message cannot be empty.\n");
            return true;
        }

        if(privateUsername == username){
            sendMessage(clientSocket,"You cannot send a private message to yourself.\n");
            return true;
        }

        int sock = getSocketByUsername(privateUsername);

        if(sock == -1){

            sendMessage(clientSocket,"User does not exist.\n");

            logger.log(LogLevel::WARNING,username +" attempted to message offline user " +privateUsername);

            return true;
        }

        sendMessage(sock,"[PRIVATE]\n" +username +" : " +privateMessage +"\n");

        sendMessage(clientSocket,"[PRIVATE -> " +privateUsername +"]\n" +privateMessage +"\n");

        logger.log(LogLevel::INFO,username +" -> " +privateUsername +": " +privateMessage);

        return true;
    }

    if(parsed.command == "/quit"){
        sendMessage(clientSocket,"GoodBye "+username+"\n");
        logger.log(LogLevel::INFO,username + " requested to quit");
        return false;
    }

    sendMessage(clientSocket,"Unknown command. Type /help for help.\n");

    logger.log(LogLevel::WARNING,"Unknown command from "+username+": ");

    return true;
}

void ChatServer::handleClient(int clientSocket){
    string username;

    if(!performHandshake(clientSocket,username)){
        close(clientSocket);
        {
            lock_guard<mutex> lock(clientsMutex);
            activeSockets.erase(clientSocket);
        }

        lock_guard<mutex> lock(clientsMutex);

        auto it=find_if(clients.begin(),clients.end(),
        [clientSocket](const Client &client){
            return client.socket==clientSocket;
        });

        if(it!=clients.end()){
            usernames.erase(it->username);
            clients.erase(it);
        }

        logger.log(LogLevel::INFO,"Client disconnected during handshake");

        return;
    }

    broadcast(getTimeStamp()+" [SERVER] "+username+" joined the chat.\n",clientSocket);

    char buffer[BUFFER_SIZE];

    while(running){
        memset(buffer,0,BUFFER_SIZE);

        int bytesReceived=recv(clientSocket,buffer,
        BUFFER_SIZE-1,0);

        if(bytesReceived<=0){
            break;
        }

        buffer[bytesReceived]='\0';

        string message=trim(buffer);

        if(!processInput(message,username,clientSocket)){
            break;
        }

        string currentUsername=getUsername(clientSocket);

        if(!currentUsername.empty()){
            username=currentUsername;
        }
    }

    string disconnectedUsername=removeClient(clientSocket);

    if(!disconnectedUsername.empty()){
        string disconnect=getTimeStamp()+" [SERVER] "+disconnectedUsername+" disconnected.\n";
        broadcast(disconnect,clientSocket);
        cout<<disconnect;
    }

    close(clientSocket);

    logger.log(LogLevel::INFO,username+" client thread ended");
    {
        lock_guard<mutex> lock(clientsMutex);
        activeSockets.erase(clientSocket);
    }
}

string ChatServer::removeClient(int clientSocket){
    lock_guard<mutex> lock(clientsMutex);

    auto it=find_if(clients.begin(),clients.end(),
    [clientSocket](const Client &client){
        return client.socket==clientSocket;
    });

    if(it==clients.end()){
        return "";
    }

    string username=it->username;

    usernames.erase(username);
    clients.erase(it);

    logger.log(LogLevel::INFO,
    username+" removed from client list");

    return username;
}

void ChatServer::broadcast(const string &message,int excludeSocket){
    lock_guard<mutex> lock(clientsMutex);

    for(const Client &client:clients){
        if(client.socket==excludeSocket){
            continue;
        }

        sendMessage(client.socket,message);
    }
}

void ChatServer::acceptClients(){
    while(running){
        sockaddr_in clientAddress{};
        socklen_t clientAddressLength=sizeof(clientAddress);

        int clientSocket=accept(
            serverSocket,
            reinterpret_cast<sockaddr*>(&clientAddress),
            &clientAddressLength
        );

        if(clientSocket==-1){
            if(running){
                perror("accept");
                logger.log(LogLevel::ERROR,
                "Failed to accept client connection");
            }

            continue;
        }
        {
            lock_guard<mutex> lock(clientsMutex);
            activeSockets.insert(clientSocket);
        }

        {
            lock_guard<mutex> lock(clientsMutex);

            if(clients.size()>=static_cast<size_t>(maxClients)){
                sendMessage(
                    clientSocket,
                    "Server is full. Please try again later.\n"
                );
                activeSockets.erase(clientSocket);
                close(clientSocket);

                logger.log(LogLevel::WARNING,"Connection rejected: maximum clients reached");

                continue;
            }
        }

        string clientIP=inet_ntoa(clientAddress.sin_addr);
        int clientPort=ntohs(clientAddress.sin_port);

        cout<<"\nNew connection from "<<clientIP<<" | Port : "<<clientPort<<" | Socket : "<<clientSocket<<endl;

        logger.log(LogLevel::INFO,"New connection from "+clientIP+" | Port : "+to_string(clientPort)+" | Socket : "+to_string(clientSocket));

        clientThreads.emplace_back(&ChatServer::handleClient, this, clientSocket);
    }
}

bool ChatServer::createSocket(){
    serverSocket=socket(AF_INET,SOCK_STREAM,0);

    if(serverSocket==-1){
        perror("socket");
        logger.log(LogLevel::ERROR,"Failed to create server socket");
        return false;
    }

    logger.log(LogLevel::INFO,"Server socket created");

    return true;
}

bool ChatServer::configureSocket(){
    int opt=1;

    if(setsockopt(serverSocket,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt))==-1){
        perror("setsockopt");
        logger.log(LogLevel::ERROR,"Failed to configure server socket");

        close(serverSocket);
        serverSocket=-1;

        return false;
    }

    logger.log(LogLevel::INFO,"Server socket configured");

    return true;
}

bool ChatServer::bindSocket(){
    sockaddr_in serverAddress{};

    serverAddress.sin_family=AF_INET;
    serverAddress.sin_addr.s_addr=INADDR_ANY;
    serverAddress.sin_port=htons(port);

    if(bind(serverSocket,
            reinterpret_cast<sockaddr*>(&serverAddress),
            sizeof(serverAddress))==-1){

        perror("bind");

        logger.log(
            LogLevel::ERROR,
            "Failed to bind server socket to port "+to_string(port)
        );

        close(serverSocket);
        serverSocket=-1;

        return false;
    }

    logger.log(
        LogLevel::INFO,
        "Server socket bound to port "+to_string(port)
    );

    return true;
}

bool ChatServer::startListening(){
    if(listen(serverSocket,10)==-1){
        perror("listen");

        logger.log(
            LogLevel::ERROR,
            "Failed to start listening"
        );

        close(serverSocket);
        serverSocket=-1;

        return false;
    }

    logger.log(
        LogLevel::INFO,
        "Server is listening for connections"
    );

    return true;
}

void ChatServer::printBanner(){
    cout<<"\n=========================================="<<endl;
    cout<<"           TCP CHAT SERVER"<<endl;
    cout<<"=========================================="<<endl;
    cout<<"Port            : "<<port<<endl;
    cout<<"Buffer Size     : "<<BUFFER_SIZE<<" Bytes"<<endl;
    cout<<"Max Clients     : "<<maxClients<<endl;
    cout<<"Max Username    : "<<MAX_USERNAME_LENGTH<<endl;
    cout<<"Max Attempts    : "<<MAX_USERNAME_ATTEMPTS<<endl;
    cout<<"Commands        : /help /list /nick /msg /quit"<<endl;
    cout<<"Status          : Listening..."<<endl;
    cout<<"Started At      : "<<getTimeStamp()<<endl;
    cout<<"=========================================="<<endl;
}

bool ChatServer::start(){
    if(running){
        logger.log(LogLevel::WARNING,"Server is already running");

        return false;
    }

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

    
    logger.log(LogLevel::INFO, "Server started on port "+to_string(port));
    
    printBanner();
    
    running=true;
    acceptClients();
    
    for(auto& thread : clientThreads){
        if(thread.joinable()){
            thread.join();
        }
    }
    return true;
}

void ChatServer::stop(){
    if(!running){
        return;
    }

    running = false;

    if(serverSocket != -1){
        shutdown(serverSocket, SHUT_RDWR);
        close(serverSocket);
        serverSocket = -1;
    }

    vector<int> clientSockets;

    {
        lock_guard<mutex> lock(clientsMutex);

        for (int socket : activeSockets) {
            clientSockets.push_back(socket);
        }
    }

    for (int socket : clientSockets) {
        shutdown(socket, SHUT_RDWR);
    }

    logger.log(LogLevel::INFO,"Server stopped");

    cout<<"\n=========================================="<<endl;
    cout<<"           SERVER STOPPED"<<endl;
    cout<<"=========================================="<<endl;
}