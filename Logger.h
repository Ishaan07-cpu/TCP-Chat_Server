#pragma once

#include<string>
#include<fstream>
#include<mutex>

enum class LogLevel{
    INFO,
    WARNING,
    ERROR
};

class Logger{
public:
    Logger(const std::string &filename);
    ~Logger();

    void log(LogLevel level,const std::string &message);

private:
    std::ofstream logFile;
    std::mutex logMutex;
    LogLevel minimumLevel;
};