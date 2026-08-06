#include "client.h"

ChatClient::ChatClient(){
    clientSocket=-1;
}

bool ChatClient::createSocket(){
    clientSocket=socket(AF_INET,SOCK_STREAM,0);

    if(clientSocket==-1){
        perror("socket");
        return false;
    }

    return true;
}

bool ChatClient::connectToServer(){
    sockaddr_in serverAddress{};

    serverAddress.sin_family=AF_INET;
    serverAddress.sin_port=htons(PORT);

    if(inet_pton(AF_INET,SERVER_IP.c_str(),&serverAddress.sin_addr)<=0){
        perror("inet_pton");
        close(clientSocket);
        clientSocket=-1;
        return false;
    }

    if(connect(clientSocket,(sockaddr*)&serverAddress,sizeof(serverAddress))==-1){
        perror("connect");
        close(clientSocket);
        clientSocket=-1;
        return false;
    }

    cout<<"Connected to Server!"<<endl;

    return true;
}

bool ChatClient::performHandshake(){
    char buffer[BUFFER_SIZE];

    int bytesReceived=recv(clientSocket,buffer,BUFFER_SIZE-1,0);

    if(bytesReceived<=0){
        cout<<"Failed to receive username prompt."<<endl;
        return false;
    }

    buffer[bytesReceived]='\0';

    cout<<buffer;

    string username;
    getline(cin,username);

    if(send(clientSocket,username.c_str(),username.size(),MSG_NOSIGNAL)==-1){
        perror("send");
        return false;
    }

    return true;
}

void ChatClient::receiveMessages(){
    char buffer[BUFFER_SIZE];

    while(true){
        int bytesReceived=recv(clientSocket,buffer,BUFFER_SIZE-1,0);

        if(bytesReceived==0){
            cout<<"\nServer Disconnected!"<<endl;
            break;
        }

        if(bytesReceived==-1){
            perror("recv");
            break;
        }

        buffer[bytesReceived]='\0';

        cout<<"\n"<<buffer<<endl;
        cout<<"You : "<<flush;
    }
}

bool ChatClient::start(){
    if(!createSocket()){
        return false;
    }

    if(!connectToServer()){
        return false;
    }

    if(!performHandshake()){
        return false;
    }

    thread receiverThread(&ChatClient::receiveMessages,this);

    while(true){
        cout<<"You : ";

        string message;
        getline(cin,message);

        if(message.empty()){
            continue;
        }

        if(message=="exit"){
            cout<<"Disconnecting..."<<endl;
            break;
        }

        if(send(clientSocket,message.c_str(),message.size(),MSG_NOSIGNAL)==-1){
            perror("send");
            break;
        }
    }

    shutdown(clientSocket,SHUT_RDWR);

    receiverThread.join();

    return true;
}

void ChatClient::stop(){
    if(clientSocket!=-1){
        close(clientSocket);
        clientSocket=-1;
    }

    cout<<"Client Closed."<<endl;
}