#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>

using namespace std;

const int PORT=3490;
const int BUFFER_SIZE=1024;
const int MAX_EVENTS=1024;
const size_t MAX_OUTPUT_BUFFER=1024*1024;

bool setNonBlocking(int fd){
    int flags=fcntl(fd,F_GETFL,0);

    if(flags==-1){
        perror("fcntl F_GETFL");
        return false;
    }

    if(fcntl(fd,F_SETFL,flags|O_NONBLOCK)==-1){
        perror("fcntl F_SETFL");
        return false;
    }

    return true;
}

bool modifyEpoll(int epollFd,int clientFd,uint32_t events){
    epoll_event event{};

    event.events=events;
    event.data.fd=clientFd;

    if(epoll_ctl(epollFd,EPOLL_CTL_MOD,clientFd,&event)==-1){
        perror("epoll_ctl MOD");
        return false;
    }

    return true;
}

void removeClient(int epollFd,int clientFd,unordered_map<int,string>& outputBuffers){
    if(epoll_ctl(epollFd,EPOLL_CTL_DEL,clientFd,nullptr)==-1){
        if(errno!=EBADF){
            perror("epoll_ctl DEL");
        }
    }

    close(clientFd);
    outputBuffers.erase(clientFd);

    cout<<"Client disconnected: "<<clientFd<<endl;
}

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

    if(!setNonBlocking(serverSocket)){
        close(serverSocket);
        return 1;
    }

    sockaddr_in serverAddress{};

    serverAddress.sin_family=AF_INET;
    serverAddress.sin_addr.s_addr=INADDR_ANY;
    serverAddress.sin_port=htons(PORT);

    if(bind(serverSocket,reinterpret_cast<sockaddr*>(&serverAddress),sizeof(serverAddress))==-1){
        perror("bind");
        close(serverSocket);
        return 1;
    }

    if(listen(serverSocket,SOMAXCONN)==-1){
        perror("listen");
        close(serverSocket);
        return 1;
    }

    cout<<"Server listening on port "<<PORT<<endl;

    int epollFd=epoll_create1(0);

    if(epollFd==-1){
        perror("epoll_create1");
        close(serverSocket);
        return 1;
    }

    epoll_event serverEvent{};

    serverEvent.events=EPOLLIN|EPOLLET;
    serverEvent.data.fd=serverSocket;

    if(epoll_ctl(epollFd,EPOLL_CTL_ADD,serverSocket,&serverEvent)==-1){
        perror("epoll_ctl ADD server");
        close(epollFd);
        close(serverSocket);
        return 1;
    }

    unordered_map<int,string> outputBuffers;

    epoll_event events[MAX_EVENTS];

    while(true){
        int activity=epoll_wait(epollFd,events,MAX_EVENTS,-1);

        if(activity==-1){
            if(errno==EINTR){
                continue;
            }

            perror("epoll_wait");
            break;
        }

        for(int i=0;i<activity;i++){
            int currentFd=events[i].data.fd;
            uint32_t currentEvents=events[i].events;

            if(currentFd==serverSocket){

                while(true){
                    sockaddr_in clientAddress{};
                    socklen_t clientLength=sizeof(clientAddress);

                    int clientSocket=accept(serverSocket,reinterpret_cast<sockaddr*>(&clientAddress),&clientLength);

                    if(clientSocket==-1){
                        if(errno==EINTR){
                            continue;
                        }

                        if(errno==EAGAIN||errno==EWOULDBLOCK){
                            break;
                        }

                        perror("accept");
                        break;
                    }

                    if(!setNonBlocking(clientSocket)){
                        close(clientSocket);
                        continue;
                    }

                    epoll_event clientEvent{};

                    clientEvent.events=EPOLLIN|EPOLLRDHUP|EPOLLET;
                    clientEvent.data.fd=clientSocket;

                    if(epoll_ctl(epollFd,EPOLL_CTL_ADD,clientSocket,&clientEvent)==-1){
                        perror("epoll_ctl ADD client");
                        close(clientSocket);
                        continue;
                    }

                    outputBuffers[clientSocket]="";

                    cout<<"Client connected: "<<clientSocket<<endl;
                }

                continue;
            }

            if(currentEvents&EPOLLERR){
                removeClient(epollFd,currentFd,outputBuffers);
                continue;
            }

            if(currentEvents&EPOLLIN){
                char buffer[BUFFER_SIZE];

                while(true){
                    ssize_t bytesReceived=recv(currentFd,buffer,sizeof(buffer),0);

                    if(bytesReceived>0){
                        cout<<"Client "<<currentFd<<" : "<<string(buffer,bytesReceived)<<endl;

                        vector<int> clientsToDisconnect;

                        for(auto& client:outputBuffers){
                            int otherFd=client.first;

                            if(otherFd==currentFd){
                                continue;
                            }

                            string& output=client.second;

                            if(output.size()+static_cast<size_t>(bytesReceived)>MAX_OUTPUT_BUFFER){
                                clientsToDisconnect.push_back(otherFd);
                                continue;
                            }

                            output.append(buffer,bytesReceived);

                            uint32_t newEvents=EPOLLIN|EPOLLOUT|EPOLLRDHUP|EPOLLET;

                            if(!modifyEpoll(epollFd,otherFd,newEvents)){
                                clientsToDisconnect.push_back(otherFd);
                            }
                        }

                        for(int clientFd:clientsToDisconnect){
                            removeClient(epollFd,clientFd,outputBuffers);
                        }

                        continue;
                    }

                    if(bytesReceived==0){
                        removeClient(epollFd,currentFd,outputBuffers);
                        break;
                    }

                    if(bytesReceived==-1){
                        if(errno==EINTR){
                            continue;
                        }

                        if(errno==EAGAIN||errno==EWOULDBLOCK){
                            break;
                        }

                        perror("recv");
                        removeClient(epollFd,currentFd,outputBuffers);
                        break;
                    }
                }
            }

            if(currentEvents&EPOLLOUT){
                auto it=outputBuffers.find(currentFd);

                if(it==outputBuffers.end()){
                    continue;
                }

                string& output=it->second;

                while(!output.empty()){
                    ssize_t bytesSent=send(currentFd,output.data(),output.size(),MSG_NOSIGNAL);

                    if(bytesSent>0){
                        output.erase(0,static_cast<size_t>(bytesSent));
                        continue;
                    }

                    if(bytesSent==-1&&errno==EINTR){
                        continue;
                    }

                    if(bytesSent==-1&&(errno==EAGAIN||errno==EWOULDBLOCK)){
                        break;
                    }

                    perror("send");
                    removeClient(epollFd,currentFd,outputBuffers);
                    break;
                }

                auto client=outputBuffers.find(currentFd);

                if(client!=outputBuffers.end()){
                    if(client->second.empty()){
                        uint32_t newEvents=EPOLLIN|EPOLLRDHUP|EPOLLET;

                        modifyEpoll(epollFd,currentFd,newEvents);
                    }
                }
            }
        }
    }

    for(auto& client:outputBuffers){
        close(client.first);
    }

    close(serverSocket);
    close(epollFd);

    return 0;
}