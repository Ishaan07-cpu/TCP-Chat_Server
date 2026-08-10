#include "server.h"
#include "Config.h"

int main() {
    Config config("config.txt");

    ChatServer server(config);

    if(!server.start()){
        cout<<"\n=========================================="<<endl;
        cout<<"Failed to start server."<<endl;
        cout<<"=========================================="<<endl;
        return 1;
    }

    server.stop();

    return 0;
}