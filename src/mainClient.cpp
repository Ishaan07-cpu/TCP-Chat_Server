#include "client.h"

const int PORT=3490;
const int BUFFER_SIZE=1024;

const string SERVER_IP="127.0.0.1";

int main(){
    ChatClient client;

    if(!client.start()){
        cout<<"Failed to start client."<<endl;
        return 1;
    }

    client.stop();

    return 0;
}