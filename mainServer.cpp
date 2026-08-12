#include "server.h"
#include "Config.h"
#include <csignal>
#include <thread>
#include <chrono>
#include <atomic>
using namespace std;

volatile sig_atomic_t signalRecieved=0;

void signalHandling(int signal){
    signalRecieved=1;
}

int main() {
    Config config("config.txt");

    ChatServer server(config);

    signal(SIGINT,signalHandling);
    thread serverThread([&server](){
        server.start();
    });

    while (!signalRecieved) {
        this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    server.stop();
    if (serverThread.joinable()) {
        serverThread.join();
    }

    return 0;
}