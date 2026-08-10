#pragma once

#include <string>

class Config {
public:
    Config(const std::string& filename);

    int getPort() const;
    std::string getLogFile() const;
    int getMaxClients() const;

private:
    int port;
    std::string logFile;
    int maxClients;
};
