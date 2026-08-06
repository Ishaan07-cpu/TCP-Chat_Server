#include "utility.h"

#include <ctime>
#include <cctype>

string trim(const string &str){
    size_t start=0;
    int end=(int)str.size()-1;

    while(start<str.size() && isspace((unsigned char)str[start])){
        start++;
    }

    while(end>=(int)start && isspace((unsigned char)str[end])){
        end--;
    }

    return str.substr(start,end-start+1);
}

string getTimeStamp(){
    time_t now=time(nullptr);

    tm currentTime;
    localtime_r(&now,&currentTime);

    char buffer[20];

    strftime(buffer,sizeof(buffer),"[%H:%M:%S]",&currentTime);

    return buffer;
}