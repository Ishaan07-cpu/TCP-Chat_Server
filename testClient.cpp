#include <iostream>
#include <string>
#include <cstring>
#include <thread>

#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

using namespace std;

const int PORT = 3490;
const int BUFFER_SIZE = 1024;
const string SERVER_IP = "127.0.0.1";

void receiveMessage(int sockfd){
    char buffer[BUFFER_SIZE];

    while(true){
        int bytesReceived = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);

        if(bytesReceived == 0){
            cout << "\nServer Disconnected!" << endl;
            break;
        }

        if(bytesReceived == -1){
            perror("recv");
            break;
        }

        buffer[bytesReceived] = '\0';

        cout << "\n" << buffer << endl;
        cout << "You : " << flush;
    }
}

int main(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd == -1){
        perror("socket");
        return 1;
    }

    sockaddr_in server{};

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    if(inet_pton(AF_INET, SERVER_IP.c_str(), &server.sin_addr) <= 0){
        perror("inet_pton");
        close(sockfd);
        return 1;
    }

    if(connect(sockfd, (sockaddr*)&server, sizeof(server)) == -1){
        perror("connect");
        close(sockfd);
        return 1;
    }

    cout << "Connected to Server!" << endl;

    char buffer[BUFFER_SIZE];

    int bytesReceived = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);

    if(bytesReceived <= 0){
        cout << "Failed to receive username prompt." << endl;
        close(sockfd);
        return 1;
    }

    buffer[bytesReceived] = '\0';

    cout << buffer;

    string username;
    getline(cin, username);

    if(send(sockfd, username.c_str(), username.size(), 0) == -1){
        perror("send");
        close(sockfd);
        return 1;
    }

    thread receiverThread(receiveMessage, sockfd);

    while(true){
        cout << "You : ";

        string message;
        getline(cin, message);

        if(message.empty()){
            continue;
        }

        if(message == "exit"){
            cout << "Disconnecting..." << endl;
            break;
        }

        if(send(sockfd, message.c_str(), message.size(), 0) == -1){
            perror("send");
            break;
        }
    }

    shutdown(sockfd, SHUT_RDWR);

    receiverThread.join();

    close(sockfd);

    return 0;
}