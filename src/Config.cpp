#include "Config.h"
#include "utility.h"
#include <fstream>
#include <stdexcept>

Config::Config(const std::string &filename)
    : port(8080),
      logFile("server.log"),
      maxClients(100) {

    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open configuration file");
    }

    std::string line;

    while (std::getline(file, line)) {
        line = trim(line);

        if (line.empty()) {
            continue;
        }

        size_t delimiter = line.find('=');

        if (delimiter == std::string::npos) {
            throw std::runtime_error(
                "Invalid configuration line: " + line
            );
        }

        std::string key = trim(line.substr(0, delimiter));
        std::string value = trim(line.substr(delimiter + 1));

        if (key == "PORT") {
            port = std::stoi(value);
        }
        else if (key == "LOG_FILE") {
            logFile = value;
        }
        else if (key == "MAX_CLIENTS") {
            maxClients = std::stoi(value);
        }
        else {
            throw std::runtime_error(
                "Unknown configuration key: " + key
            );
        }
    }

    if (port < 1 || port > 65535) {
        throw std::runtime_error("Invalid PORT");
    }

    if (logFile.empty()) {
        throw std::runtime_error("LOG_FILE cannot be empty");
    }

    if (maxClients <= 0) {
        throw std::runtime_error("Invalid MAX_CLIENTS");
    }
}

int Config::getPort() const {
    return port;
}

std::string Config::getLogFile() const {
    return logFile;
}

int Config::getMaxClients() const {
    return maxClients;
}