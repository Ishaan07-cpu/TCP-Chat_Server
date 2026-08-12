#pragma once

#include<iostream>
#include<string>
#include<thread>
#include<atomic>

#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>

using namespace std;

extern const int PORT;
extern const int BUFFER_SIZE;
extern const string SERVER_IP;

class ChatClient{
private:
    int clientSocket;
    atomic<bool> connected;

    bool createSocket();
    bool connectToServer();
    bool performHandshake();

    void receiveMessages();

public:
    ChatClient();

    bool start();
    void stop();
};