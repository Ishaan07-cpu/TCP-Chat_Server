#include"Logger.h"
#include<stdexcept>
#include<chrono>
#include<sstream>
#include<iomanip>

using namespace std;

Logger::Logger(const string &filename)
    : logFile(filename,ios::app),
      minimumLevel(LogLevel::INFO)
{
    if(!logFile.is_open()){
        throw runtime_error("Failed to open log file.");
    }
}

Logger::~Logger(){
    if(logFile.is_open()){
        logFile.close();
    }
}

string levelToString(LogLevel level){
    switch(level){
        case LogLevel::INFO:
            return "INFO";

        case LogLevel::WARNING:
            return "WARNING";

        case LogLevel::ERROR:
            return "ERROR";
    }

    return "UNKNOWN";
}

string getCurrentTime(){
    auto now=chrono::system_clock::now();

    time_t currentTime=chrono::system_clock::to_time_t(now);

    tm localTime{};

#ifdef _WIN32
    localtime_s(&localTime,&currentTime);
#else
    localtime_r(&currentTime,&localTime);
#endif

    ostringstream timestamp;

    timestamp<<put_time(&localTime,"%Y-%m-%d %H:%M:%S");

    return timestamp.str();
}

void Logger::log(LogLevel level,const string &message){
    lock_guard<mutex> lock(logMutex);

    if(level<minimumLevel){
        return;
    }

    logFile<<getCurrentTime()
           <<" ["<<levelToString(level)<<"] "
           <<message
           <<'\n';

    logFile.flush();
}