cmake_minimum_required(VERSION 3.10)

project(ChatProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_executable(ChatServer
    mainServer.cpp
    server.cpp
    Config.cpp
    utility.cpp
    CommandParser.cpp
    Logger.cpp
)

target_link_libraries(ChatServer PRIVATE Threads::Threads)

add_executable(ChatClient
    mainClient.cpp
    client.cpp
    utility.cpp
)

target_link_libraries(ChatClient PRIVATE Threads::Threads)