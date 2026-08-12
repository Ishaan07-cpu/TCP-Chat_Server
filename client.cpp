#include "client.h"
#include <sys/select.h>
#include <cerrno>

ChatClient::ChatClient(){
    clientSocket=-1;
    connected=false;
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

    connected=true;

    cout<<"Connected to Server!"<<endl;

    return true;
}

bool ChatClient::performHandshake(){
    char buffer[BUFFER_SIZE];

    int bytesReceived=recv(clientSocket,buffer,BUFFER_SIZE-1,0);

    if(bytesReceived<=0){
        cout<<"Server Disconnected!"<<endl;
        connected=false;
        return false;
    }

    buffer[bytesReceived]='\0';

    cout<<buffer<<flush;

    while(connected){
        fd_set readfds;

        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO,&readfds);
        FD_SET(clientSocket,&readfds);

        int maxFD=max(STDIN_FILENO,clientSocket);

        int result=select(maxFD+1,&readfds,nullptr,nullptr,nullptr);

        if(result==-1){
            if(errno==EINTR){
                continue;
            }

            perror("select");
            connected=false;
            return false;
        }

        if(FD_ISSET(clientSocket,&readfds)){
            bytesReceived=recv(clientSocket,buffer,BUFFER_SIZE-1,0);

            if(bytesReceived<=0){
                cout<<"\nServer Disconnected!"<<endl;
                connected=false;
                return false;
            }

            buffer[bytesReceived]='\0';
            cout<<buffer<<flush;
        }

        if(FD_ISSET(STDIN_FILENO,&readfds)){
            string username;

            getline(cin,username);

            if(!connected){
                return false;
            }

            if(send(clientSocket,username.c_str(),username.size(),MSG_NOSIGNAL)==-1){
                perror("send");
                connected=false;
                return false;
            }

            return true;
        }
    }

    return false;
}

void ChatClient::receiveMessages(){
    char buffer[BUFFER_SIZE];

    while(connected){
        int bytesReceived=recv(clientSocket,buffer,BUFFER_SIZE-1,0);

        if(bytesReceived==0){
            connected=false;
            cout<<"\nServer Disconnected!"<<endl;
            break;
        }

        if(bytesReceived==-1){
            connected=false;
            perror("recv");
            break;
        }

        buffer[bytesReceived]='\0';

        cout<<"\n"<<buffer<<endl;

        if(connected){
            cout<<"You : "<<flush;
        }
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

    cout<<"You : "<<flush;

    while(connected){
        fd_set readfds;

        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO,&readfds);

        timeval timeout{};

        timeout.tv_sec=0;
        timeout.tv_usec=100000;

        int result=select(STDIN_FILENO+1,&readfds,nullptr,nullptr,&timeout);

        if(result==-1){
            if(errno==EINTR){
                continue;
            }

            perror("select");
            connected=false;
            break;
        }

        if(!connected){
            break;
        }

        if(result==0){
            continue;
        }

        if(FD_ISSET(STDIN_FILENO,&readfds)){
            string message;

            getline(cin,message);

            if(cin.eof()){
                connected=false;
                break;
            }

            if(message.empty()){
                cout<<"You : "<<flush;
                continue;
            }

            if(message=="/quit"){
                cout<<"Disconnecting..."<<endl;
                connected=false;
                break;
            }

            if(send(clientSocket,message.c_str(),message.size(),MSG_NOSIGNAL)==-1){
                perror("send");
                connected=false;
                break;
            }

            if(connected){
                cout<<"You : "<<flush;
            }
        }
    }

    shutdown(clientSocket,SHUT_RDWR);

    if(receiverThread.joinable()){
        receiverThread.join();
    }

    return true;
}

void ChatClient::stop(){
    connected=false;

    if(clientSocket!=-1){
        shutdown(clientSocket,SHUT_RDWR);
        close(clientSocket);
        clientSocket=-1;
    }

    cout<<"Client Closed."<<endl;
}