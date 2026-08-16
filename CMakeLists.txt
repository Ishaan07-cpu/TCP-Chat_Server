cmake_minimum_required(VERSION 3.10)

project(TCPChatServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ChatServer
    src/mainServer.cpp
    src/server.cpp
    src/Config.cpp
    src/Logger.cpp
    src/CommandParser.cpp
    src/utility.cpp
)

target_include_directories(ChatServer PRIVATE include)

add_executable(ChatClient
    src/mainClient.cpp
    src/client.cpp
)

target_include_directories(ChatClient PRIVATE include)