#include "server.h"



int main(){
    ChatServer server;

    if(!server.start()){
        cout<<"\n=========================================="<<endl;
        cout<<"Failed to start server."<<endl;
        cout<<"=========================================="<<endl;
        return 1;
    }

    server.stop();

    return 0;
}