#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <poll.h>
#include <vector>
#include <fcntl.h>
#include <cerrno>

using namespace std;

const int PORT=3490;
const int BUFFER_SIZE=1024;

int main(){
    int serverSocket=socket(AF_INET,SOCK_STREAM,0);

    if(serverSocket==-1){
        perror("socket");
        return 1;
    }

    int flags=fcntl(serverSocket,F_GETFL,0);

    if(flags==-1){
        perror("fcntl");
        close(serverSocket);
        return 1;
    }

    if(fcntl(serverSocket,F_SETFL,flags|O_NONBLOCK)==-1){
        perror("fcntl");
        close(serverSocket);
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

    vector<pollfd> pollFds;
    vector<string> outputBuffers;

    pollfd serverPollFd{};

    serverPollFd.fd=serverSocket;
    serverPollFd.events=POLLIN;
    serverPollFd.revents=0;

    pollFds.push_back(serverPollFd);
    outputBuffers.push_back("");

    while(true){
        int activity=poll(pollFds.data(),pollFds.size(),-1);

        if(activity==-1){
            if(errno==EINTR){
                continue;
            }

            perror("poll");
            break;
        }

        for(size_t i=0;i<pollFds.size();i++){

            if(pollFds[i].fd==serverSocket){

                if(!(pollFds[i].revents&POLLIN)){
                    continue;
                }

                sockaddr_in clientAddress{};
                socklen_t clientLength=sizeof(clientAddress);

                int clientSocket=accept(serverSocket,(sockaddr*)&clientAddress,&clientLength);

                if(clientSocket==-1){
                    if(errno==EAGAIN||errno==EWOULDBLOCK){
                        continue;
                    }

                    perror("accept");
                    continue;
                }

                int clientFlags=fcntl(clientSocket,F_GETFL,0);

                if(clientFlags==-1){
                    perror("fcntl");
                    close(clientSocket);
                    continue;
                }

                if(fcntl(clientSocket,F_SETFL,clientFlags|O_NONBLOCK)==-1){
                    perror("fcntl");
                    close(clientSocket);
                    continue;
                }

                cout<<"New client connected"
                    <<" | Socket : "<<clientSocket
                    <<endl;

                pollfd clientPollFd{};

                clientPollFd.fd=clientSocket;
                clientPollFd.events=POLLIN;
                clientPollFd.revents=0;

                pollFds.push_back(clientPollFd);
                outputBuffers.push_back("");

                continue;
            }

            if(pollFds[i].revents&(POLLHUP|POLLERR|POLLNVAL)){

                int clientSocket=pollFds[i].fd;

                cout<<"Client disconnected"
                    <<" | Socket : "<<clientSocket
                    <<endl;

                close(clientSocket);

                pollFds.erase(pollFds.begin()+i);
                outputBuffers.erase(outputBuffers.begin()+i);

                i--;

                continue;
            }

            if(pollFds[i].revents&POLLIN){

                int clientSocket=pollFds[i].fd;
                char buffer[BUFFER_SIZE];

                int bytesReceived=recv(clientSocket,buffer,sizeof(buffer)-1,0);

                if(bytesReceived==0){

                    cout<<"Client disconnected"
                        <<" | Socket : "<<clientSocket
                        <<endl;

                    close(clientSocket);

                    pollFds.erase(pollFds.begin()+i);
                    outputBuffers.erase(outputBuffers.begin()+i);

                    i--;

                    continue;
                }

                if(bytesReceived==-1){

                    if(errno==EAGAIN||errno==EWOULDBLOCK){
                        continue;
                    }

                    perror("recv");
                    close(clientSocket);

                    pollFds.erase(pollFds.begin()+i);
                    outputBuffers.erase(outputBuffers.begin()+i);

                    i--;

                    continue;
                }

                buffer[bytesReceived]='\0';

                cout<<"Client "
                    <<clientSocket
                    <<" : "
                    <<buffer
                    <<endl;

                for(size_t j=0;j<pollFds.size();j++){

                    if(pollFds[j].fd==serverSocket){
                        continue;
                    }

                    if(pollFds[j].fd==clientSocket){
                        continue;
                    }

                    outputBuffers[j].append(buffer,bytesReceived);

                    pollFds[j].events|=POLLOUT;
                }
            }

            if(i>=pollFds.size()){
                continue;
            }

            if(pollFds[i].revents&POLLOUT){

                int clientSocket=pollFds[i].fd;

                if(outputBuffers[i].empty()){
                    pollFds[i].events&=~POLLOUT;
                    continue;
                }

                int bytesSent=send(
                    clientSocket,
                    outputBuffers[i].data(),
                    outputBuffers[i].size(),
                    MSG_NOSIGNAL
                );

                if(bytesSent>0){

                    outputBuffers[i].erase(0,bytesSent);

                    if(outputBuffers[i].empty()){
                        pollFds[i].events&=~POLLOUT;
                    }
                }
                else if(bytesSent==-1){

                    if(errno==EAGAIN||errno==EWOULDBLOCK){
                        continue;
                    }

                    perror("send");
                    close(clientSocket);

                    pollFds.erase(pollFds.begin()+i);
                    outputBuffers.erase(outputBuffers.begin()+i);

                    i--;

                    continue;
                }
            }
        }
    }

    for(auto &pfd:pollFds){
        if(pfd.fd!=-1){
            close(pfd.fd);
        }
    }

    return 0;
}