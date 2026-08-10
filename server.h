#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <thread>
#include <mutex>
#include <algorithm>
#include <cctype>
#include "Logger.h"
#include "Config.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
using namespace std;

extern const int BUFFER_SIZE;

extern const int MAX_USERNAME_LENGTH;
extern const int MAX_USERNAME_ATTEMPTS;

extern const string USERNAME_PROMPT;
extern const string EMPTY_USERNAME;
extern const string USERNAME_TOO_LONG;
extern const string USERNAME_EXISTS;

struct Client {
    int socket;
    string username;
};

class ChatServer {
private:
    int serverSocket;
    vector<Client> clients;
    unordered_set<string> usernames;
    mutex clientsMutex;
    bool running = false;

    int port;
    int maxClients;

    Logger logger;

    bool sendMessage(int clientSocket, const string &message);
    bool performHandshake(int clientSocket, string &username);
    int getSocketByUsername(const string &userName);
    string getUsername(int clientSocket);
    bool changeUsername(int clientSocket, const string &newUsername, string &oldUsername);
    bool processInput(const string &message, const string &username, int clientSocket);
    bool processCommand(const string &message, const string &username, int clientSocket);
    void handleClient(int clientSocket);
    string removeClient(int clientSocket);
    void broadcast(const string &message, int excludeSocket = -1);
    void acceptClients();
    bool createSocket();
    bool configureSocket();
    bool bindSocket();
    bool startListening();
    void printBanner();

public:
    ChatServer(const Config &config);

    bool start();
    void stop();
};

