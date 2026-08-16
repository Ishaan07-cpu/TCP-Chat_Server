#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

using namespace std;

const int PORT=3490;
const int BUFFER_SIZE=1024;

int main(){

    int serverSocket=socket(AF_INET,SOCK_STREAM,0);

    if(serverSocket==-1){
        perror("socket");
        return 1;
    }

    int opt=1;

    if(setsockopt(serverSocket,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt))==-1){
        perror("setsockopt");
        close(serverSocket);
        return 1;
    }

    sockaddr_in serverAddress{};

    serverAddress.sin_family=AF_INET;
    serverAddress.sin_port=htons(PORT);
    serverAddress.sin_addr.s_addr=INADDR_ANY;

    if(bind(serverSocket,(sockaddr*)&serverAddress,sizeof(serverAddress))==-1){
        perror("bind");
        close(serverSocket);
        return 1;
    }

    if(listen(serverSocket,10)==-1){
        perror("listen");
        close(serverSocket);
        return 1;
    }

    cout<<"Server listening on port "<<PORT<<endl;

    fd_set masterSet;
    FD_ZERO(&masterSet);
    FD_SET(serverSocket,&masterSet);

    int maxFd=serverSocket;

    while(true){

        fd_set readSet=masterSet;

        int activity=select(maxFd+1,&readSet,nullptr,nullptr,nullptr);

        if(activity==-1){
            perror("select");
            break;
        }

        if(FD_ISSET(serverSocket,&readSet)){

            sockaddr_in clientAddress{};
            socklen_t clientLength=sizeof(clientAddress);

            int clientSocket=accept(serverSocket,(sockaddr*)&clientAddress,&clientLength);

            if(clientSocket==-1){
                perror("accept");
            }
            else{

                cout<<"New client connected"
                    <<" | Socket : "<<clientSocket<<endl;

                FD_SET(clientSocket,&masterSet);

                if(clientSocket>maxFd){
                    maxFd=clientSocket;
                }
            }
        }

        for(int fd=0;fd<=maxFd;fd++){

            if(fd==serverSocket){
                continue;
            }

            if(!FD_ISSET(fd,&readSet)){
                continue;
            }

            char buffer[BUFFER_SIZE];

            int bytesReceived=recv(fd,buffer,sizeof(buffer)-1,0);

            if(bytesReceived==0){

                cout<<"Client disconnected"
                    <<" | Socket : "<<fd<<endl;

                close(fd);
                FD_CLR(fd,&masterSet);

                continue;
            }

            if(bytesReceived==-1){

                perror("recv");

                close(fd);
                FD_CLR(fd,&masterSet);

                continue;
            }

            buffer[bytesReceived]='\0';

            cout<<"Client "<<fd<<" : "<<buffer<<endl;

            for(int otherFd=0;otherFd<=maxFd;otherFd++){

                if(otherFd==serverSocket){
                    continue;
                }

                if(otherFd==fd){
                    continue;
                }

                if(!FD_ISSET(otherFd,&masterSet)){
                    continue;
                }

                send(otherFd,buffer,bytesReceived,MSG_NOSIGNAL);
            }
        }
    }

    for(int fd=0;fd<=maxFd;fd++){

        if(FD_ISSET(fd,&masterSet)){
            close(fd);
        }
    }

    return 0;
}