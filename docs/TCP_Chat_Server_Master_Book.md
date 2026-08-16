# Building a Production-Oriented TCP Chat Server in C++

## From Blocking POSIX Sockets to Linux `epoll` and Event-Driven Networking

**Master Book:** `TCP_Chat_Server_Master_Book.md`\
**Project:** TCP Chat Server\
**Language:** C++\
**Platform:** Linux / Ubuntu WSL\
**Networking:** POSIX sockets / TCP\
**Final I/O Model:** Non-blocking, edge-triggered Linux `epoll`\
**Purpose:** Systems programming, backend engineering, interview
preparation, and preparation for a Redis clone

------------------------------------------------------------------------

> **Book rule:** This document follows the actual TCP Chat Server
> learning journey. Project history, code evolution, mistakes,
> debugging, corrections, and design decisions should be preserved.
> Material that goes beyond the original project should be clearly
> labeled as a **Production-Level Extension**.

## How to Use This Book

Each technical chapter should use this structure where applicable:

1.  **Concept**
2.  **Why It Exists**
3.  **How It Works**
4.  **Our Code**
5.  **Line-by-Line Explanation**
6.  **Internal / Kernel View**
7.  **Common Mistakes**
8.  **Production Considerations**
9.  **Interview Questions**

The book should answer not only **what** was written, but **why** the
architecture evolved.

------------------------------------------------------------------------

# Project Evolution at a Glance

``` text
Basic TCP Server
      ↓
Sequential Server
      ↓
Thread-per-Client
      ↓
Mutex + Shared Client State
      ↓
Chat Commands + Usernames
      ↓
ChatServer OOP Refactor
      ↓
Logging + Configuration
      ↓
select()
      ↓
poll()
      ↓
Non-Blocking I/O
      ↓
Output Buffering + POLLOUT
      ↓
epoll()
      ↓
Edge-Triggered epoll
      ↓
EPOLLOUT + Partial Writes
      ↓
Backpressure
      ↓
Final Event-Driven Learning Server
      ↓
Redis Clone Preparation
```

------------------------------------------------------------------------

# Table of Contents

## Part I --- Project Foundations

### Chapter 1 --- What We Are Building

### Chapter 2 --- Understanding the Linux Socket Model

## Part II --- C++ Foundations Used in the Project

### Chapter 3 --- C++ Building Blocks

### Chapter 4 --- STL and Modern C++ Used by the Server

### Chapter 5 --- Concurrency in C++

### Chapter 6 --- Compilation, Linking and Project Structure

## Part III --- Linux Socket Programming

### Chapter 7 --- `socket()`

### Chapter 8 --- `bind()`

### Chapter 9 --- `listen()`

### Chapter 10 --- `accept()`

### Chapter 11 --- `connect()`

### Chapter 12 --- `send()` and `recv()`

### Chapter 13 --- `close()` and `shutdown()`

### Chapter 14 --- Addresses and Byte Order

### Chapter 15 --- Linux Error Handling

## Part IV --- TCP Fundamentals

### Chapter 16 --- TCP vs UDP

### Chapter 17 --- TCP Connection Establishment

### Chapter 18 --- TCP Is a Byte Stream

### Chapter 19 --- Closing TCP Connections

### Chapter 20 --- Flow Control, Congestion Control and Backpressure

## Part V --- Building the First Server

### Chapter 21 --- The Sequential TCP Server

### Chapter 22 --- Client Implementation

### Chapter 23 --- Debugging the First Server

## Part VI --- Thread-Per-Client Server

### Chapter 24 --- Why We Need Concurrency

### Chapter 25 --- `std::thread`

### Chapter 26 --- Shared Client State

### Chapter 27 --- Race Conditions

### Chapter 28 --- Mutexes and `lock_guard`

### Chapter 29 --- Username Management

### Chapter 30 --- Broadcasting

### Chapter 31 --- Private Messaging and Commands

### Chapter 32 --- Client Disconnects and Resource Ownership

## Part VII --- Professionalizing the Architecture

### Chapter 33 --- Why Refactor Into `ChatServer`?

### Chapter 34 --- `ChatServer` Architecture

### Chapter 35 --- Separation of Responsibilities

### Chapter 36 --- Protocol Commands and Application Logic

## Part VIII --- Logging and Configuration

### Chapter 37 --- Designing the Logger

### Chapter 38 --- `Logger`

### Chapter 39 --- Logger Linker Problems

### Chapter 40 --- Configuration System

## Part IX --- Git and GitHub

### Chapter 41 --- Git From Zero

### Chapter 42 --- Working Tree and Staging Area

### Chapter 43 --- Commits and History

### Chapter 44 --- Remotes and GitHub

### Chapter 45 --- Branches, Merge and Rebase

### Chapter 46 --- Debugging Git Problems

## Part X --- `select()`

### Chapter 47 --- Why Thread-Per-Client Doesn't Scale Forever

### Chapter 48 --- I/O Multiplexing

### Chapter 49 --- `fd_set`

### Chapter 50 --- `select()`

### Chapter 51 --- Building the `select()` Server

### Chapter 52 --- Master Set vs Temporary Set

### Chapter 53 --- `maxFd`

### Chapter 54 --- Read Readiness

### Chapter 55 --- Non-Blocking `select()`

## Part XI --- `poll()`

### Chapter 56 --- Why `poll()`?

### Chapter 57 --- `struct pollfd`

### Chapter 58 --- `events` vs `revents`

### Chapter 59 --- `POLLIN`

### Chapter 60 --- `POLLOUT`

### Chapter 61 --- `POLLHUP` and `POLLERR`

### Chapter 62 --- Building the `poll()` Server

### Chapter 63 --- Non-Blocking `poll()`

## Part XII --- Non-Blocking I/O

### Chapter 64 --- Blocking vs Non-Blocking

### Chapter 65 --- `fcntl()`

### Chapter 66 --- `F_GETFL`

### Chapter 67 --- `F_SETFL`

### Chapter 68 --- `O_NONBLOCK`

### Chapter 69 --- `EAGAIN` / `EWOULDBLOCK`

### Chapter 70 --- `EINTR`

### Chapter 71 --- The Non-Blocking I/O Decision Table

## Part XIII --- Output Buffering

### Chapter 72 --- Why `send()` Is Not Guaranteed to Send Everything

### Chapter 73 --- Partial Writes

### Chapter 74 --- Output Buffers

### Chapter 75 --- `POLLOUT`

### Chapter 76 --- Retrying Failed Writes

### Chapter 77 --- Parallel Vectors: Teaching Implementation

### Chapter 78 --- Improving the Data Model

## Part XIV --- `epoll` Fundamentals

### Chapter 79 --- Why `epoll()`?

### Chapter 80 --- `epoll_create1()`

### Chapter 81 --- `epoll_ctl()`

### Chapter 82 --- `epoll_wait()`

### Chapter 83 --- `epoll_event`

### Chapter 84 --- `EPOLL_CTL_ADD`

### Chapter 85 --- `EPOLL_CTL_MOD`

### Chapter 86 --- `EPOLL_CTL_DEL`

### Chapter 87 --- The Kernel Interest List

### Chapter 88 --- Ready Events vs Connected Clients

## Part XV --- Non-Blocking `epoll`

### Chapter 89 --- Non-Blocking `epoll`

### Chapter 90 --- Accept Until `EAGAIN`

### Chapter 91 --- Receive Until `EAGAIN`

### Chapter 92 --- Send Until `EAGAIN`

### Chapter 93 --- Handling `EINTR`

## Part XVI --- Edge-Triggered `epoll`

### Chapter 94 --- Level-Triggered vs Edge-Triggered

### Chapter 95 --- `EPOLLET`

### Chapter 96 --- Why Edge Triggering Changes the Programming Model

### Chapter 97 --- Why `EPOLLET + Blocking I/O` Is Dangerous

### Chapter 98 --- `EPOLLET + O_NONBLOCK`

### Chapter 99 --- Draining the Socket

## Part XVII --- Writable Events

### Chapter 100 --- `EPOLLIN`

### Chapter 101 --- `EPOLLOUT`

### Chapter 102 --- `EPOLLRDHUP`

### Chapter 103 --- `EPOLLERR`

### Chapter 104 --- `EPOLLHUP`

### Chapter 105 --- Combining Events

### Chapter 106 --- Why We Don't Keep `EPOLLOUT` Enabled Constantly

## Part XVIII --- Final `epoll` Output Architecture

### Chapter 107 --- `unordered_map<int, string> outputBuffers`

### Chapter 108 --- FD → Output Buffer

### Chapter 109 --- Broadcasting

### Chapter 110 --- Enabling `EPOLLOUT`

### Chapter 111 --- Draining Output

### Chapter 112 --- Removing `EPOLLOUT`

## Part XIX --- Broadcast Architecture

### Chapter 113 --- Connected vs Ready Clients

### Chapter 114 --- The Incorrect `epoll_wait()` Broadcast Idea

### Chapter 115 --- Correct Broadcast Architecture

## Part XX --- Backpressure

### Chapter 116 --- The Slow Client Problem

### Chapter 117 --- Unbounded Output Buffers

### Chapter 118 --- Memory Exhaustion

### Chapter 119 --- `MAX_OUTPUT_BUFFER`

### Chapter 120 --- Disconnecting Slow Clients

### Chapter 121 --- `clientsToDisconnect`

### Chapter 122 --- Iterator Invalidation

### Chapter 123 --- Safe Container Mutation

## Part XXI --- Client Lifecycle

### Chapter 124 --- Accept

### Chapter 125 --- Register

### Chapter 126 --- Read

### Chapter 127 --- Process

### Chapter 128 --- Queue Output

### Chapter 129 --- Write

### Chapter 130 --- Disconnect

### Chapter 131 --- `EPOLL_CTL_DEL`

### Chapter 132 --- `close()`

### Chapter 133 --- FD Reuse and Stale State

## Part XXII --- Final Architecture

### Chapter 134 --- Complete Final `epoll` Server

### Chapter 135 --- Complete Code Walkthrough

### Chapter 136 --- Function-by-Function Analysis

### Chapter 137 --- Kernel-Level Event Flow

## Part XXIII --- Complete Event Flows

### Chapter 138 --- Server Startup

### Chapter 139 --- New Client

### Chapter 140 --- Client Sends

### Chapter 141 --- Broadcast

### Chapter 142 --- `EPOLLOUT`

### Chapter 143 --- Partial Send

### Chapter 144 --- `EAGAIN`

### Chapter 145 --- Disconnect

### Chapter 146 --- Error

### Chapter 147 --- Backpressure

### Chapter 148 --- Shutdown

## Part XXIV --- Architectural Evolution

### Chapter 149 --- Thread-Per-Client

### Chapter 150 --- `select()`

### Chapter 151 --- Non-Blocking `select()`

### Chapter 152 --- `poll()`

### Chapter 153 --- Non-Blocking `poll()`

### Chapter 154 --- Output Buffering

### Chapter 155 --- `epoll()`

### Chapter 156 --- Edge-Triggered `epoll`

### Chapter 157 --- `EPOLLOUT`

### Chapter 158 --- Backpressure

## Part XXV --- `select()` vs `poll()` vs `epoll()`

### Chapter 159 --- Deep Comparison

### Chapter 160 --- 100,000 Clients / 50 Ready

### Chapter 161 --- Scalability Model

### Chapter 162 --- Portability vs Performance

## Part XXVI --- Threads vs Event-Driven Networking

### Chapter 163 --- Thread-Per-Client

### Chapter 164 --- Single Event Loop

### Chapter 165 --- Multi-Threaded Event Loops

### Chapter 166 --- Why "`epoll = one thread`" Is Wrong

## Part XXVII --- Networking Bugs and Debugging

### Chapter 167 --- Compilation Errors

### Chapter 168 --- Linker Errors

### Chapter 169 --- Runtime Errors

### Chapter 170 --- TCP Errors

### Chapter 171 --- `EAGAIN`

### Chapter 172 --- Partial Sends

### Chapter 173 --- `EPOLLRDHUP`

### Chapter 174 --- Incorrect Broadcast Logic

### Chapter 175 --- Iterator Invalidation

### Chapter 176 --- Git Problems

## Part XXVIII --- Reading Your Own Systems Code

### Chapter 177 --- What Enters a Function?

### Chapter 178 --- What Leaves?

### Chapter 179 --- What State Changes?

### Chapter 180 --- What Can Block?

### Chapter 181 --- What Can Fail?

### Chapter 182 --- What Can Race?

### Chapter 183 --- What Allocates?

### Chapter 184 --- What Closes a Socket?

## Part XXIX --- Interview Preparation

### Chapter 185 --- C++ Questions

### Chapter 186 --- Linux Questions

### Chapter 187 --- TCP Questions

### Chapter 188 --- Socket Questions

### Chapter 189 --- `select()`

### Chapter 190 --- `poll()`

### Chapter 191 --- `epoll()`

### Chapter 192 --- Non-Blocking I/O

### Chapter 193 --- Edge Triggering

### Chapter 194 --- Output Buffering

### Chapter 195 --- Backpressure

### Chapter 196 --- Concurrency

### Chapter 197 --- System Design

### Chapter 198 --- Project-Specific Questions

### Chapter 199 --- Senior-Level Questions

## Part XXX --- Redis Connection

### Chapter 200 --- From Chat Server to Redis

### Chapter 201 --- TCP Layer → Redis Network Layer

### Chapter 202 --- `epoll` → Event Loop

### Chapter 203 --- Input Buffer → Protocol Parser

### Chapter 204 --- Output Buffer → Response Queue

### Chapter 205 --- Commands → Dispatcher

### Chapter 206 --- Client State → Redis Client Object

### Chapter 207 --- Backpressure → Resource Protection

## Part XXXI --- Production Gap Analysis

### Chapter 208 --- What Our Server Already Does

### Chapter 209 --- What It Doesn't Yet Do

### Chapter 210 --- Authentication

### Chapter 211 --- Authorization

### Chapter 212 --- TLS

### Chapter 213 --- Rate Limiting

### Chapter 214 --- Timeouts

### Chapter 215 --- Metrics

### Chapter 216 --- Structured Logging

### Chapter 217 --- Testing

### Chapter 218 --- Fuzzing

### Chapter 219 --- Security

### Chapter 220 --- Multi-Core Scaling

## Part XXXII --- Glossary

A--Z reference for important C++, Linux, TCP, socket, concurrency, and
event-driven networking terms.

## Part XXXIII --- Final Cheat Sheets

-   Socket lifecycle
-   TCP lifecycle
-   `select`
-   `poll`
-   `epoll`
-   non-blocking I/O
-   `EAGAIN`
-   `EWOULDBLOCK`
-   `EINTR`
-   `EPOLLIN`
-   `EPOLLOUT`
-   `EPOLLRDHUP`
-   `EPOLLERR`
-   `EPOLLHUP`
-   `EPOLL_CTL_*`
-   threads vs event loop
-   Git
-   CMake
-   debugging

## Part XXXIV --- Interview Explanations

For every major topic:

``` text
30-second answer
      ↓
2-minute answer
      ↓
5-minute deep answer
```

## Part XXXV --- Tell Me About Your Project

A polished but accurate interview narrative:

``` text
Problem
  ↓
Initial Architecture
  ↓
Thread-per-Client
  ↓
select
  ↓
poll
  ↓
epoll
  ↓
Non-Blocking I/O
  ↓
Edge Triggering
  ↓
Output Buffering
  ↓
Backpressure
  ↓
Lessons Learned
  ↓
Future Work
```

## Part XXXVI --- Future Learning Roadmap

-   Already learned
-   Need to reinforce
-   Advanced networking
-   Linux internals
-   Operating systems
-   Advanced C++
-   System design
-   Distributed systems
-   Redis clone

------------------------------------------------------------------------

# Chapter Dependency Map

``` text
C++ Foundations
      ↓
Linux File Descriptors
      ↓
POSIX Socket APIs
      ↓
TCP Fundamentals
      ↓
Sequential TCP Server
      ↓
Multiple Clients
      ↓
Threads + Shared State
      ↓
Mutex / Synchronization
      ↓
ChatServer Architecture
      ↓
Logging + Configuration
      ↓
Thread-Per-Client Limitations
      ↓
select()
      ↓
poll()
      ↓
Non-Blocking I/O
      ↓
Partial Reads / Writes
      ↓
Output Buffering
      ↓
epoll()
      ↓
EPOLLET
      ↓
Drain Until EAGAIN
      ↓
EPOLLOUT
      ↓
Backpressure
      ↓
Final Event-Driven Server
      ↓
Redis Network Architecture
```

------------------------------------------------------------------------

# Reconstructed Code Evolution

1.  Sequential POSIX TCP server
2.  TCP client
3.  Thread-per-client server
4.  Shared `vector<Client>` with mutex protection
5.  Username management and broadcasting
6.  Full command-based chat server
7.  `ChatServer` class refactor
8.  Logger and configuration
9.  `select()` server
10. `poll()` server
11. Non-blocking `poll()`
12. `poll()` with output buffers
13. Basic `epoll()`
14. Non-blocking `epoll()`
15. Edge-triggered `epoll()`
16. `epoll()` with `EPOLLOUT`
17. Output buffering with `unordered_map<int, string>`
18. Backpressure and safe slow-client removal

------------------------------------------------------------------------

# Known Bugs and Debugging Cases to Preserve

The book should preserve the actual debugging journey, including
applicable cases such as:

-   Windows Winsock code being used on Linux
-   linker errors
-   missing definitions
-   declaration/definition mismatches
-   `sizeof(message)` vs actual message length
-   `message.c_str()` when calling `send()`
-   executable/path problems under WSL
-   duplicate or invalid username handling
-   race conditions around shared client state
-   `join()` blocking the accept loop
-   `select()` master-set vs temporary-set confusion
-   using a client descriptor before it existed
-   `poll()` `events` vs `revents`
-   vector erase/index handling
-   `EINTR`
-   `EAGAIN`
-   `EWOULDBLOCK`
-   assuming `send()` transmits the entire buffer
-   missing output buffering
-   incorrect `POLLOUT`/`EPOLLOUT` handling
-   confusing all connected clients with epoll's ready-event list
-   edge-triggered reads that do not drain until `EAGAIN`
-   `EPOLLRDHUP` arriving with readable data
-   erasing an `unordered_map` entry while iterating over the map
-   unbounded output buffers and slow clients

Each case should eventually be documented as:

``` text
Symptom
  ↓
Incorrect Mental Model
  ↓
Root Cause
  ↓
Fix
  ↓
General Lesson
```

------------------------------------------------------------------------

# Explicitly Learned Concepts

## C++

-   classes and structs
-   constructors
-   references
-   pointers
-   `const`
-   `std::string`
-   `std::vector`
-   `std::unordered_map`
-   `std::unordered_set`
-   iterators
-   lambdas
-   STL algorithms
-   `std::thread`
-   `std::mutex`
-   `std::lock_guard`
-   enums
-   RAII and ownership concepts
-   `size_t`
-   `ssize_t`
-   headers/source files
-   compilation and linking

## Linux / POSIX

-   file descriptors
-   `errno`
-   `perror`
-   `fcntl`
-   `O_NONBLOCK`
-   socket lifecycle

## Networking

-   TCP
-   IPv4
-   ports
-   `sockaddr`
-   `sockaddr_in`
-   `socket`
-   `bind`
-   `listen`
-   `accept`
-   `connect`
-   `send`
-   `recv`
-   `close`
-   `shutdown`
-   `setsockopt`
-   `SO_REUSEADDR`
-   `inet_pton`
-   `htons`

## Concurrency

-   thread-per-client
-   `join`
-   `detach`
-   race conditions
-   critical sections
-   mutexes
-   resource ownership

## I/O Multiplexing

-   `select`
-   `poll`
-   `epoll`
-   read readiness
-   write readiness
-   connected sockets vs ready sockets

## Event-Driven Networking

-   non-blocking I/O
-   `EAGAIN`
-   `EWOULDBLOCK`
-   `EINTR`
-   `EPOLLIN`
-   `EPOLLOUT`
-   `EPOLLRDHUP`
-   `EPOLLERR`
-   `EPOLLHUP`
-   `EPOLLET`
-   output buffers
-   partial writes
-   backpressure
-   slow-client protection

------------------------------------------------------------------------

# Production-Level Extensions

The following topics should be clearly labeled as extensions unless they
are later implemented in the project:

-   authentication
-   authorization
-   TLS
-   rate limiting
-   connection timeouts
-   idle-client management
-   structured metrics
-   fuzzing
-   security hardening
-   multi-core event loops
-   worker pools
-   advanced buffer structures
-   persistent storage
-   production deployment

------------------------------------------------------------------------

# Final Architecture Overview

``` text
                         Linux Kernel
                             │
                        epoll_wait()
                             │
                 ┌───────────┴───────────┐
                 │                       │
          Listening Socket         Client Sockets
                 │                       │
              EPOLLIN             EPOLLIN / EPOLLOUT
                 │                       │
        accept until EAGAIN       recv until EAGAIN
                 │                       │
                 │                 Process Message
                 │                       │
                 │                    Broadcast
                 │                       │
                 │              Recipient Output Buffers
                 │                       │
                 │                 Enable EPOLLOUT
                 │                       │
                 │                 send until EAGAIN
                 │                       │
                 │               Disable EPOLLOUT when
                 │                  buffer is empty
                 │
                 └─────────────── Event Loop ───────────────┘
```

## Core Final-Server Principles

1.  Sockets are non-blocking.
2.  `epoll` tracks I/O readiness.
3.  Edge-triggered sockets are drained until `EAGAIN`.
4.  `recv()` returning data does not imply the entire logical message
    has arrived.
5.  `send()` may perform partial writes.
6.  Unsent data belongs in an application-level output buffer.
7.  `EPOLLOUT` is enabled only when pending output exists.
8.  Slow clients cannot be allowed to consume unlimited memory.
9.  Socket cleanup must remove kernel registration, close the FD, and
    remove application state.
10. Event-driven networking avoids dedicating one thread to every
    mostly-idle connection.

------------------------------------------------------------------------

# Book Progress

-   [x] Master book created
-   [x] Table of contents created
-   [x] Chapter dependency map created
-   [x] Project evolution outline created
-   [x] Known debugging topics listed
-   [x] Explicitly learned concepts listed
-   [x] Production extensions separated
-   [ ] Part I --- Project Foundations
-   [ ] Part II --- C++ Foundations
-   [ ] Part III --- Linux Socket Programming
-   [ ] Part IV --- TCP Fundamentals
-   [ ] Remaining parts
-   [ ] Final code walkthrough
-   [ ] Interview preparation
-   [ ] Redis connection
-   [ ] Final glossary and cheat sheets

------------------------------------------------------------------------

# Continuation Rule

When continuing this book, proceed from the next unfinished chapter.

**Do not restart the book.**\
**Do not summarize completed chapters unless revision is requested.**\
**Do not silently invent missing project history.**

Continuation command:

> `CONTINUE MASTER BOOK`

Starting point:

> **Part I --- Project Foundations**\
> **Chapter 1 --- What We Are Building**


# PART I — PROJECT FOUNDATIONS

---

# Chapter 1 — What We Are Building

## 1.1 The Project in One Sentence

The project is a **TCP chat server written in C++ on Linux using POSIX sockets**, whose architecture evolved from a simple blocking server into an event-driven server using **non-blocking sockets and Linux `epoll`**.

The important part of the project is not merely sending chat messages.

The real objective is understanding how a network server works internally:

```text
Client
   ↓
TCP connection
   ↓
Linux socket
   ↓
Server event loop
   ↓
Read data
   ↓
Process message
   ↓
Queue output
   ↓
Write when socket is ready
```

The project therefore acts as a systems-programming exercise.

---

## 1.2 Why Build a Chat Server?

A chat server is deceptively simple.

At the application level, the requirement appears to be:

> Client A sends a message → Client B receives it.

But implementing that correctly forces us to understand several layers:

```text
Application
    ↓
Chat commands / messages
    ↓
Server connection management
    ↓
Concurrency / event handling
    ↓
Socket API
    ↓
TCP
    ↓
IP
    ↓
Linux kernel
    ↓
Network interface
```

This makes a chat server a useful project for learning backend and systems programming.

The project eventually raises questions such as:

- What exactly is a socket?
- Why does `accept()` return another file descriptor?
- Why can one server have many client sockets?
- Why does `recv()` sometimes return less data than expected?
- Why can `send()` send only part of a message?
- Why can blocking I/O prevent a server from handling other clients?
- Why do we need threads?
- Why do we eventually replace threads with I/O multiplexing?
- Why are `select()`, `poll()`, and `epoll()` different?
- Why does `epoll` require non-blocking sockets?
- Why does edge-triggered `epoll` require draining until `EAGAIN`?
- Why do we need output buffers?
- Why does `EPOLLOUT` matter?
- What happens when one client is much slower than everyone else?

These questions are the actual systems-programming lessons of the project.

---

# 1.3 Client/Server Architecture

The fundamental architecture is:

```text
                  TCP CONNECTION
        ┌─────────────────────────────────┐
        │                                 │
        ▼                                 ▼

+---------------+                 +---------------+
|    Client A   |                 |    Client B   |
|               |                 |               |
|  send()       |                 |  recv()       |
+-------+-------+                 +-------+-------+
        │                                 ▲
        │                                 │
        └──────────────┐     ┌────────────┘
                       ▼     │
                 +-------------+
                 | Chat Server |
                 |             |
                 | accept()    |
                 | recv()      |
                 | send()      |
                 | broadcast   |
                 +-------------+
```

The server is the central point through which clients communicate.

A typical message flow is:

```text
Client A
   │
   │ "hello"
   ▼
Server
   │
   │ receives bytes
   │
   │ determines recipients
   │
   ├──────────────► Client B
   │
   ├──────────────► Client C
   │
   └──────────────► Client D
```

The server therefore has two broad responsibilities:

### Connection management

```text
listen
accept
track clients
detect disconnects
close sockets
```

### Message management

```text
receive
process
broadcast
buffer
send
```

---

# 1.4 What TCP Gives Us

The chat server uses:

```text
SOCK_STREAM
```

with:

```text
AF_INET
```

This means we are using an IPv4 TCP stream socket.

TCP gives the application a reliable byte stream between two endpoints.

Conceptually:

```text
Client
  │
  │ TCP
  │
  ▼
Server
```

TCP handles lower-level responsibilities such as:

- reliable delivery
- ordering
- retransmission
- flow control
- connection establishment
- connection termination

The chat server therefore does **not** implement packet retransmission itself.

Instead, it works with the byte stream exposed by the TCP socket.

---

# 1.5 TCP Is a Byte Stream

One of the most important concepts for this project is:

> TCP does not preserve application message boundaries.

Suppose the client performs:

```cpp
send(socket, "HELLO", 5, 0);
```

The server might receive:

```text
HEL
```

and later:

```text
LO
```

Or it could receive all five bytes at once.

Therefore:

```text
1 send()
```

does not necessarily mean:

```text
1 recv()
```

and:

```text
1 recv()
```

does not necessarily represent:

```text
1 application message
```

This becomes extremely important later when designing a proper application protocol.

---

# 1.6 Socket Descriptors

On Linux, a socket is represented to the application by a **file descriptor**.

For example:

```cpp
int serverSocket = socket(...);
```

might return:

```text
3
```

A later client connection might produce:

```text
4
```

Another:

```text
5
```

Another:

```text
6
```

So the server might conceptually have:

```text
FD 3 → listening socket
FD 4 → Client A
FD 5 → Client B
FD 6 → Client C
```

This becomes fundamental to the later `select()`, `poll()`, and `epoll()` architecture.

---

# 1.7 Listening Socket vs Client Socket

This distinction must be completely clear.

## Listening socket

Created by:

```cpp
socket()
bind()
listen()
```

Its job is to accept new connections.

Conceptually:

```text
serverSocket
     │
     └── waiting for new TCP connections
```

When a connection arrives:

```cpp
accept(serverSocket, ...)
```

returns a **new socket descriptor**.

---

## Client socket

The descriptor returned by:

```cpp
accept()
```

represents the connection to one particular client.

For example:

```text
serverSocket = 3

accept() → 4
accept() → 5
accept() → 6
```

Now:

```text
3 → listening socket
4 → Client A
5 → Client B
6 → Client C
```

The listening socket remains available to accept more connections.

This is one of the most important ideas in the entire project.

---

# 1.8 Connection Lifecycle

A simplified server-side lifecycle is:

```text
socket()
   ↓
bind()
   ↓
listen()
   ↓
accept()
   ↓
recv() / send()
   ↓
connection closes
   ↓
close()
```

The client follows a related lifecycle:

```text
socket()
   ↓
connect()
   ↓
send() / recv()
   ↓
shutdown()/close()
```

The full conceptual picture is:

```text
                SERVER
                  │
               socket()
                  │
                bind()
                  │
               listen()
                  │
                  ▼
           waiting for clients
                  │
               accept()
                  │
                  ▼
             client socket
                  │
          ┌───────┴───────┐
          │               │
        recv()          send()
          │               │
          └───────┬───────┘
                  │
              disconnect
                  │
                close()
```

---

# 1.9 Why `accept()` Returns a New Socket

This is worth understanding deeply.

The listening socket has one job:

```text
accept new connections
```

It should not become the socket used for communication with every client.

Instead:

```text
Listening socket
      │
      ├── accept → Client socket A
      ├── accept → Client socket B
      ├── accept → Client socket C
      └── accept → Client socket D
```

This lets the server simultaneously:

- continue accepting new connections
- communicate with existing clients

Without this separation, the server could not maintain independent connections cleanly.

---

# 1.10 Broadcasting

The main chat behavior is broadcasting.

Suppose:

```text
Clients:
A
B
C
D
```

Client A sends:

```text
Hello everyone
```

The server receives it through A's socket.

Then the server sends the data to:

```text
B
C
D
```

but not back to A in the basic implementation.

Conceptually:

```text
                 "Hello"
                    │
                    ▼
              +-----------+
              |  Server   |
              +-----------+
               /    |    \
              /     |     \
             ▼      ▼      ▼
            B       C       D
```

Broadcasting is therefore an application-level responsibility.

TCP itself does not know anything about:

```text
chat rooms
users
broadcasts
commands
```

TCP only provides the communication stream.

---

# 1.11 Multiple Clients

A basic sequential server can handle one client at a time.

That creates the first major architectural problem.

Imagine:

```text
Client A connects
       ↓
server blocks on A
       ↓
Client B connects
       ↓
server cannot process B immediately
```

The server needs a mechanism for dealing with multiple connections.

The project explored multiple approaches:

```text
Sequential
    ↓
Thread-per-client
    ↓
select()
    ↓
poll()
    ↓
epoll()
```

This progression is one of the central stories of the project.

---

# 1.12 Thread-Per-Client Architecture

The first scalable improvement was conceptually:

```text
                 Server
                   │
       ┌───────────┼───────────┐
       ▼           ▼           ▼
    Client A    Client B    Client C
       │           │           │
    Thread A    Thread B    Thread C
```

Each client gets its own execution context.

The thread can block on:

```cpp
recv()
```

without preventing another client's thread from running.

This makes the programming model relatively easy to understand.

However, it introduces another problem:

> What happens when the number of clients becomes very large?

For example:

```text
100,000 clients
       ↓
100,000 threads
```

The server now pays for:

- thread stacks
- scheduling
- context switching
- thread management
- synchronization around shared state

This motivates event-driven I/O.

---

# 1.13 Event-Driven Architecture

Instead of:

```text
one client
     ↓
one thread
```

we eventually move toward:

```text
many clients
     ↓
one event loop
     ↓
only process sockets that are ready
```

The final architecture is conceptually:

```text
             1000s of connections
                     │
                     ▼
                epoll_wait()
                     │
            ready events returned
                     │
          ┌──────────┼──────────┐
          ▼          ▼          ▼
       Client 4   Client 19   Client 73
          │          │          │
        recv       recv       send
```

The server spends its execution time on sockets that actually need attention.

---

# 1.14 The Evolution of the Project

The project was not designed from the beginning as a final `epoll` server.

That is important.

The architecture evolved because each implementation exposed a new problem.

## Stage 1 — Basic TCP Server

Learn:

```text
socket
bind
listen
accept
recv
send
close
```

Goal:

> Make one TCP connection work.

---

## Stage 2 — Multiple Clients

Problem:

```text
One connection blocks the server.
```

Solution:

```text
threads
```

---

## Stage 3 — Shared Client State

Once multiple threads communicate with shared structures, synchronization becomes necessary.

This introduced concepts such as:

```text
mutex
lock_guard
race conditions
shared state
```

---

## Stage 4 — Chat Features

The server gained application-level behavior such as:

```text
usernames
broadcasting
commands
private messaging
```

---

## Stage 5 — `select()`

The next question became:

> Can one execution thread manage many sockets without one thread per client?

`select()` introduced I/O multiplexing.

---

## Stage 6 — `poll()`

`poll()` provided a more flexible descriptor representation than the traditional `select()` descriptor sets.

The server now worked with:

```cpp
vector<pollfd>
```

and checked:

```cpp
revents
```

after `poll()` returned.

---

## Stage 7 — Non-Blocking I/O

The server was then changed to use:

```text
O_NONBLOCK
```

This changed the programming model.

Instead of assuming:

```text
recv() waits until data exists
```

the application learned to handle:

```text
EAGAIN
EWOULDBLOCK
EINTR
```

---

## Stage 8 — Output Buffering

A major realization was:

> `send()` is not guaranteed to send the entire buffer in one call.

Therefore:

```text
Application message
       ↓
Output buffer
       ↓
send()
       ↓
partial send possible
       ↓
remaining bytes stay buffered
```

This led to:

```text
POLLOUT
```

and eventually:

```text
EPOLLOUT
```

---

## Stage 9 — `epoll()`

The project then moved to Linux's scalable event-notification mechanism:

```text
epoll_create1()
epoll_ctl()
epoll_wait()
```

The important conceptual difference was:

```text
select/poll:
application examines the monitored collection

epoll:
kernel maintains registered interest and returns ready events
```

---

## Stage 10 — Edge-Triggered `epoll`

The final architecture uses:

```text
EPOLLET
```

This requires a different discipline.

When a socket becomes ready, the application must generally keep reading or writing until the operation would block:

```text
recv()
recv()
recv()
recv()
EAGAIN
```

and:

```text
send()
send()
send()
EAGAIN
```

This is why non-blocking sockets are essential to the final design.

---

# 1.15 Final Architecture

The final learning architecture can be summarized as:

```text
                         ┌──────────────────────┐
                         │      Linux Kernel    │
                         │                      │
                         │  TCP/IP + sockets    │
                         │  epoll interest list │
                         └──────────┬───────────┘
                                    │
                              epoll_wait()
                                    │
                                    ▼
                         ┌──────────────────────┐
                         │      Event Loop      │
                         └──────────┬───────────┘
                                    │
                    ┌───────────────┴───────────────┐
                    │                               │
                    ▼                               ▼
             Listening Socket                 Client Socket
                    │                               │
                 EPOLLIN                    EPOLLIN / EPOLLOUT
                    │                               │
                 accept()                         recv()
                    │                               │
                    │                          process message
                    │                               │
                    │                           broadcast
                    │                               │
                    │                         output buffer
                    │                               │
                    │                           send()
                    │                               │
                    │                         partial write?
                    │                               │
                    │                           EAGAIN
                    │
                    └───────────────┬───────────────┘
                                    │
                                    ▼
                               Event Loop
```

---

# 1.16 The Most Important Mental Model

Do not think of the final server as:

> "A program that waits for clients."

Think of it as:

> **A program that maintains connection state and repeatedly reacts to kernel-reported I/O readiness.**

The event loop is essentially:

```text
wait
 ↓
receive ready events
 ↓
process them
 ↓
update connection state
 ↓
wait again
```

That mental model will be used throughout the rest of this book.

---

# 1.17 What the Final Server Does NOT Solve

This project is intentionally a learning-oriented systems server.

It should not be described as a complete production messaging platform.

It does not automatically provide:

- TLS
- authentication
- authorization
- persistent message storage
- distributed deployment
- horizontal scaling
- database-backed users
- production-grade observability
- sophisticated protocol framing
- multi-process scaling

Those are separate engineering problems.

The value of this project is that it builds the low-level foundation required to understand those systems.

---

# 1.18 Project Goals

The project has several goals.

### Goal 1 — Understand Linux networking

Not just memorize:

```cpp
socket()
bind()
listen()
accept()
```

but understand what each operation means.

### Goal 2 — Understand TCP

Especially:

```text
byte streams
connection lifecycle
partial reads
partial writes
flow control
```

### Goal 3 — Understand concurrency

Understand why:

```text
thread-per-client
```

works and where it becomes expensive.

### Goal 4 — Understand I/O multiplexing

Understand the progression:

```text
select
poll
epoll
```

### Goal 5 — Understand non-blocking programming

Understand:

```text
EAGAIN
EWOULDBLOCK
EINTR
```

as normal control-flow conditions rather than mysterious errors.

### Goal 6 — Understand event-driven servers

Understand:

```text
read readiness
write readiness
event loops
interest registration
ready events
```

### Goal 7 — Prepare for larger systems projects

The concepts transfer directly into high-performance network servers and the planned Redis clone.

---

# 1.19 Interview Explanation

If asked:

> "Tell me about your TCP Chat Server."

A strong accurate answer is:

> "I built a TCP chat server in C++ on Linux using POSIX sockets. I started with the basic socket lifecycle and then evolved the server as I encountered scalability and I/O problems. I explored a thread-per-client design, then built versions using `select()` and `poll()`, moved to non-blocking sockets, added output buffering to handle partial writes, and finally built an event-driven Linux `epoll` version using edge-triggered I/O. The final architecture uses an event loop, non-blocking sockets, `EPOLLIN` for reads, `EPOLLOUT` for pending writes, output buffers for unsent data, and a bounded output buffer to protect against slow clients."

This is much stronger than saying:

> "I made a chat app in C++."

The first answer demonstrates systems understanding.

---

# 1.20 Chapter 1 — Key Takeaways

You should now be able to explain:

- what the project does
- why TCP is used
- what a socket is
- what a file descriptor represents
- the difference between a listening socket and a client socket
- why `accept()` returns a new socket
- the TCP server lifecycle
- why TCP is a byte stream
- why multiple clients create a concurrency problem
- why thread-per-client was useful
- why the architecture eventually moved toward event-driven I/O
- the evolution from sequential → threads → `select` → `poll` → `epoll`
- why output buffering became necessary
- why the final server uses non-blocking sockets
- why edge-triggered `epoll` requires draining operations

---

# Interview Questions — Chapter 1

### Basic

1. What is a TCP socket?
2. What is a file descriptor?
3. What is the difference between a listening socket and a connected socket?
4. Why does `accept()` return a new file descriptor?
5. What does `listen()` do?
6. Why is TCP called a byte-stream protocol?

### Intermediate

7. Why can't one `recv()` call be assumed to return one complete message?
8. Why can `send()` perform a partial write?
9. Why does a server need to maintain state for each connected client?
10. Why does thread-per-client become expensive at large connection counts?
11. What problem does I/O multiplexing solve?

### Advanced

12. What is the conceptual difference between `poll()` and `epoll()`?
13. Why does edge-triggered `epoll` require non-blocking sockets?
14. Why must a server drain a socket until `EAGAIN` under edge triggering?
15. Why does a high-performance server need an output buffer?
16. What happens if a client cannot consume data as quickly as the server produces it?

---

# Chapter 2 — Understanding the Linux Socket Model

## 2.1 The Socket as a Kernel Object

The application sees:

```cpp
int clientSocket = 4;
```

But the integer itself is not the socket.

It is a **file descriptor** referring to a kernel-managed resource.

Conceptually:

```text
User Space

int clientSocket = 4
        │
        ▼
+-------------------+
| File descriptor 4 |
+---------┬---------+
          │
          ▼

Kernel Space

+---------------------------+
| Socket / TCP state        |
| send buffer               |
| receive buffer            |
| connection state          |
| protocol information      |
+---------------------------+
```

This distinction becomes especially important with `epoll`.

The application manipulates file descriptors.

The kernel maintains the underlying socket state.

---

## 2.2 Why Linux Uses File Descriptors

Linux exposes many resources through file descriptors.

Examples include:

```text
regular files
pipes
terminals
sockets
```

Therefore networking APIs fit naturally into the Unix/Linux I/O model.

A socket can participate in mechanisms such as:

```text
readiness notification
close()
fcntl()
poll()
select()
epoll()
```

This is one reason the Unix socket API is so powerful.

---

## 2.3 The Server Socket Lifecycle

The server performs:

```cpp
socket()
```

then:

```cpp
bind()
```

then:

```cpp
listen()
```

then:

```cpp
accept()
```

Each call changes the state or role of the socket.

Conceptually:

```text
socket()
   │
   ▼
Created socket
   │
 bind()
   ▼
Local address assigned
   │
 listen()
   ▼
Listening socket
   │
 accept()
   ▼
Connected client socket
```

The listening socket remains listening after `accept()`.

---

## 2.4 What `bind()` Means

`bind()` associates the socket with a local address.

In this project:

```cpp
serverAddress.sin_family = AF_INET;
serverAddress.sin_port = htons(PORT);
serverAddress.sin_addr.s_addr = INADDR_ANY;
```

and:

```cpp
bind(
    serverSocket,
    (sockaddr*)&serverAddress,
    sizeof(serverAddress)
);
```

Conceptually:

```text
serverSocket
      │
      ▼
local endpoint
      │
      ├── IPv4
      ├── port 3490
      └── local interfaces
```

Without a listening endpoint associated with a local address, the server cannot provide the intended network service.

---

## 2.5 What `listen()` Changes

`listen()` turns the socket into a listening socket.

Conceptually:

```text
Before listen():

socket
  ↓
ordinary socket

After listen():

socket
  ↓
listening socket
  ↓
waiting for incoming connections
```

The backlog argument:

```cpp
listen(serverSocket, 10);
```

is related to the kernel's handling of pending connections.

It should not be interpreted as:

> "The server can only have 10 clients."

The server can have more established connections.

The argument is not the total number of clients the application can ever support.

---

## 2.6 `accept()` and the Pending Connection Queue

When a client connects, the kernel handles the TCP connection establishment.

The server eventually calls:

```cpp
accept()
```

to obtain a connected socket.

Conceptually:

```text
Client
   │
   │ TCP connection attempt
   ▼
Kernel
   │
   ▼
pending connection
   │
accept()
   │
   ▼
new connected socket FD
```

With non-blocking sockets, if there is no connection available when the application attempts `accept()`, the operation can return:

```text
EAGAIN
```

or:

```text
EWOULDBLOCK
```

In edge-triggered `epoll`, the server therefore uses the pattern:

```cpp
while (true)
{
    accept();

    if (EAGAIN)
        break;
}
```

This is not an arbitrary coding style.

It follows directly from the edge-triggered readiness model.

---

## 2.7 Why `O_NONBLOCK` Matters

A blocking socket can make a call wait.

For example:

```cpp
recv(fd, buffer, size, 0);
```

may wait if no data is available.

That is acceptable in some designs.

It becomes dangerous in an edge-triggered event loop.

Imagine:

```text
epoll says socket is readable
       ↓
recv()
       ↓
more data may exist
       ↓
recv() again
       ↓
no more data
```

With a blocking socket, that final `recv()` could wait indefinitely.

With a non-blocking socket:

```text
recv()
   ↓
no data currently available
   ↓
EAGAIN
```

The event loop can stop processing that socket and return to:

```cpp
epoll_wait()
```

Therefore:

```text
EPOLLET
   +
O_NONBLOCK
   +
drain until EAGAIN
```

is a fundamental combination in this architecture.

---

## 2.8 The Kernel/Application Boundary

A useful mental model is:

```text
APPLICATION
────────────────────────────────
C++ code
socket FD
buffers
event loop
protocol logic
────────────────────────────────
KERNEL
TCP state
socket receive buffer
socket send buffer
epoll state
network stack
────────────────────────────────
NETWORK
```

When the client sends data:

```text
Network
   ↓
TCP stack
   ↓
kernel socket receive buffer
   ↓
epoll readiness
   ↓
application recv()
```

When the application sends:

```text
application send()
   ↓
kernel socket send buffer
   ↓
TCP stack
   ↓
network
```

This explains an important point:

> `send()` does not mean "the bytes are physically delivered to the other machine right now."

It means the application is handing data to the kernel's socket/networking machinery.

---

# 2.9 Readiness vs Data

A readiness notification does not mean:

> "A complete chat message is available."

It means approximately:

> "An operation you requested can currently make progress without blocking."

For example:

```text
EPOLLIN
```

means the socket is readable.

That could correspond to:

- some application data
- EOF
- an error condition that affects reading

The application still has to call:

```cpp
recv()
```

and inspect the result.

Likewise:

```text
EPOLLOUT
```

means writing can make progress.

It does not mean:

> "Your entire output buffer will definitely be sent."

Partial writes remain possible.

---

# 2.10 Chapter 2 Key Takeaways

The key mental model is:

```text
Application FD
      ↓
Kernel socket state
      ↓
TCP state
      ↓
Network
```

And the server lifecycle is:

```text
socket
  ↓
bind
  ↓
listen
  ↓
accept
  ↓
connected socket
  ↓
recv / send
  ↓
close
```

The later `epoll` architecture builds directly on this model.

---

# Part I Review Checklist

Before moving to Part II, you should be able to explain without looking at notes:

```text
[ ] What is a socket?
[ ] What is a file descriptor?
[ ] Why does Linux represent sockets as FDs?
[ ] Difference between listening and connected sockets
[ ] socket → bind → listen → accept
[ ] Why accept returns a new FD
[ ] What TCP provides
[ ] Why TCP is a byte stream
[ ] Why recv() does not equal one message
[ ] Why send() can be partial
[ ] Why multiple clients require concurrency
[ ] Why thread-per-client eventually has scaling costs
[ ] Why event-driven I/O exists
[ ] What non-blocking means
[ ] What EAGAIN means
[ ] Why EPOLLET + O_NONBLOCK work together
[ ] Difference between readiness and actual application data
```

---

# Part I — Final Mental Model

If you remember only one diagram from Part I, remember this:

```text
                    CLIENT
                      │
                  TCP connection
                      │
                      ▼
               ┌──────────────┐
               │ Linux Kernel │
               │ TCP / Socket │
               └──────┬───────┘
                      │
                 File Descriptor
                      │
                      ▼
               ┌──────────────┐
               │ Chat Server  │
               │              │
               │ Event Loop   │
               │      │       │
               │      ▼       │
               │   recv()     │
               │      │       │
               │   process    │
               │      │       │
               │   buffer     │
               │      │       │
               │   send()     │
               └──────────────┘
```

The rest of the book is the process of turning this simple picture into a robust implementation.


# PART II — C++ FOUNDATIONS USED IN THE PROJECT

---

# Chapter 3 — C++ Building Blocks

The chat server is a networking project, but the networking APIs are surrounded by ordinary C++.

The important principle is:

> The networking problem and the C++ problem are separate layers.

A socket descriptor is an integer:

```cpp
int clientSocket;
```

but a real server needs additional state:

```cpp
struct Client
{
    int socket;
    string username;
};
```

This is where C++ becomes useful for modeling the application.

## 3.1 `struct`

A `struct` groups related data.

For the chat server, a client is naturally represented by several related values.

Instead of:

```text
socket array
username array
```

we can represent one client as:

```text
Client
 ├── socket
 └── username
```

This improves readability and keeps related state together.

## 3.2 Classes

As the project became larger, the server was refactored into a `ChatServer` class.

The reason was not simply "because OOP is good."

The reason was ownership.

The server owns concepts such as:

```text
clients
server socket
commands
configuration
logging
lifecycle
```

A class provides one object around that state and its operations.

## 3.3 References

Functions such as:

```cpp
void changeUsername(Client& client);
```

can modify the original object without copying it.

A `const` reference:

```cpp
const string& message
```

avoids an unnecessary copy while preventing modification.

## 3.4 `const`

`const` communicates intent.

For example:

```cpp
void log(const string& message);
```

means the function does not need to modify the caller's string.

This becomes increasingly important as systems code grows.

---

# Chapter 4 — STL and Modern C++ Used by the Server

The project uses several standard containers.

## 4.1 `vector`

A `vector` stores elements contiguously.

It was used for collections such as:

```cpp
vector<Client> clients;
```

and during the `poll()` implementation:

```cpp
vector<pollfd> pollFds;
```

The important property is simple iteration.

```cpp
for (auto& client : clients)
{
    // process client
}
```

## 4.2 `unordered_map`

The later `epoll` server used:

```cpp
unordered_map<int, string> outputBuffers;
```

The conceptual mapping is:

```text
socket FD
   ↓
pending output
```

For example:

```text
4 → "hello"
5 → ""
6 → "some pending data..."
```

This matches the event-driven architecture naturally because the file descriptor is the key.

## 4.3 `unordered_set`

An `unordered_set<string>` is useful when the primary requirement is uniqueness.

Username management is a natural example:

```text
Alice
Bob
Charlie
```

The set can answer whether a username already exists.

## 4.4 Iterators and Erase

One recurring systems-programming issue is that modifying a container while iterating can invalidate the iteration state.

For a vector:

```cpp
pollFds.erase(pollFds.begin() + i);
i--;
```

was used carefully after removing an element.

This is not merely syntax.

The important lesson is:

> Whenever an operation changes a container, understand what happens to the iterator or index you are currently using.

## 4.5 Strings Are Buffers Too

The project eventually used:

```cpp
string outputBuffer;
```

as an application-level byte buffer.

A C++ `string` is not restricted to human-readable text. It can hold arbitrary bytes, although binary protocols may require additional care.

The important operations included:

```cpp
append()
erase()
data()
size()
```

These became directly relevant to partial writes.

---

# Chapter 5 — Concurrency in C++

## 5.1 Why Threads Were Introduced

The sequential server has a fundamental limitation.

If the server is blocked handling Client A:

```text
Server
  ↓
recv(A)
  ↓
waiting...
```

it cannot conveniently process Client B in the same execution path.

The thread-per-client model changes that:

```text
accept(A)
   ↓
Thread A

accept(B)
   ↓
Thread B

accept(C)
   ↓
Thread C
```

Now each client can block independently.

## 5.2 `std::thread`

The basic abstraction is:

```cpp
thread clientThread(&ChatServer::handleClient, this, clientSocket);
```

The important conceptual point is that the server has created another execution context.

## 5.3 `join()`

A joined thread is waited on.

Conceptually:

```text
main thread
    ↓
join()
    ↓
wait until worker finishes
```

This is useful when the parent genuinely needs to wait.

But if the server accepts clients in a loop, joining a client handler immediately can cause a serious architectural problem:

```text
accept Client A
    ↓
start handler
    ↓
join()
    ↓
main thread waits
    ↓
Client B cannot be accepted
```

That observation was important in understanding why `join()` is not automatically the correct choice.

## 5.4 `detach()`

A detached thread runs independently of the joining thread.

That made the thread-per-client server convenient:

```text
accept
  ↓
detach handler
  ↓
continue accepting
```

But detached threads introduce ownership and shutdown complexity.

The parent no longer has a direct join point.

This is one reason thread-per-client is easy to start but harder to make fully controlled.

---

# Chapter 6 — Race Conditions and Synchronization

Once multiple threads exist, shared data becomes dangerous.

Suppose two clients modify:

```cpp
vector<Client> clients;
```

at the same time.

Possible operations include:

```text
add client
remove client
find username
change username
broadcast
```

Without synchronization, operations can interfere.

## 6.1 Race Condition

A race condition occurs when the result depends on the timing/interleaving of concurrent operations.

Example:

```text
Thread A                 Thread B

read clients             read clients
modify clients            modify clients
write clients             write clients
```

Both may observe stale state.

## 6.2 Mutex

A mutex protects a critical section.

Conceptually:

```text
lock
  ↓
access shared state
  ↓
unlock
```

## 6.3 `lock_guard`

The project used:

```cpp
lock_guard<mutex> lock(clientMutex);
```

This is safer than manually remembering to unlock because the guard releases the mutex automatically when it leaves scope.

This is an example of RAII:

> Acquire a resource in an object's lifetime and release it automatically.

## 6.4 The Cost of Synchronization

A mutex solves correctness problems, but it does not make concurrency free.

If many threads frequently contend on the same lock:

```text
Thread A ─┐
Thread B ─┼──> mutex
Thread C ─┤
Thread D ─┘
```

the lock itself can become a scalability bottleneck.

This becomes one of the reasons event-driven architectures are attractive for connection management.

---

# PART III — LINUX SOCKET PROGRAMMING

---

# Chapter 7 — `socket()`

The first operation is:

```cpp
int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
```

Three important ideas are encoded here.

### `AF_INET`

Use IPv4 addressing.

### `SOCK_STREAM`

Request a stream socket, normally TCP.

### `0`

Allow the system to select the appropriate protocol for the socket type and address family.

If successful:

```text
socket()
   ↓
file descriptor
```

If unsuccessful:

```cpp
-1
```

and `errno` provides error information.

---

# Chapter 8 — `bind()`

A newly created socket is not automatically attached to the desired server address.

The server constructs:

```cpp
sockaddr_in serverAddress{};
```

and sets:

```cpp
serverAddress.sin_family = AF_INET;
serverAddress.sin_port = htons(PORT);
serverAddress.sin_addr.s_addr = INADDR_ANY;
```

Then:

```cpp
bind(
    serverSocket,
    (sockaddr*)&serverAddress,
    sizeof(serverAddress)
);
```

## Why `htons()`?

Network protocols use network byte order.

The host CPU may use a different byte ordering.

Therefore:

```cpp
htons(PORT)
```

converts the port to network byte order.

---

# Chapter 9 — `listen()`

After binding:

```cpp
listen(serverSocket, 10);
```

changes the socket into a listening socket.

The server is now conceptually:

```text
ready to accept TCP connections
```

The backlog parameter concerns pending connection handling; it should not be interpreted as the server's total client capacity.

---

# Chapter 10 — `accept()`

The server calls:

```cpp
int clientSocket = accept(
    serverSocket,
    (sockaddr*)&clientAddress,
    &clientLength
);
```

The returned descriptor represents the individual connection.

This creates the critical distinction:

```text
serverSocket
    ↓
accept new clients

clientSocket
    ↓
communicate with one client
```

The listening descriptor continues to exist.

---

# Chapter 11 — `connect()`

The client creates its own socket and calls:

```cpp
connect(
    clientSocket,
    (sockaddr*)&serverAddress,
    sizeof(serverAddress)
);
```

This requests a TCP connection to the server endpoint.

The client and server then have a bidirectional stream.

---

# Chapter 12 — `send()` and `recv()`

## `send()`

Conceptually:

```text
application
    ↓
send()
    ↓
kernel socket send buffer
    ↓
TCP stack
```

## `recv()`

Conceptually:

```text
network
    ↓
TCP stack
    ↓
kernel socket receive buffer
    ↓
recv()
    ↓
application buffer
```

A critical lesson:

> Neither operation maps directly to application messages.

`recv()` reports how many bytes were copied into the application buffer.

```cpp
ssize_t bytesReceived = recv(...);
```

The value must be interpreted.

### Positive

Data was received.

### Zero

The peer has performed an orderly shutdown of its sending side.

### `-1`

An error or non-blocking condition occurred.

---

# Chapter 13 — `close()` and `shutdown()`

`close(fd)` releases the file descriptor from the process.

For sockets, this participates in connection cleanup.

`shutdown()` provides more specific control over communication directions.

Conceptually:

```text
shutdown(SHUT_WR)
```

means:

> Stop sending.

Whereas:

```text
close()
```

releases the descriptor itself.

Understanding ownership matters because a server must not leak sockets.

---

# Chapter 14 — Addresses and Byte Order

## `sockaddr`

`sockaddr` is a generic address structure used by socket APIs.

## `sockaddr_in`

`sockaddr_in` represents an IPv4 socket address.

The common pattern is:

```cpp
sockaddr_in address{};
```

then:

```cpp
(sockaddr*)&address
```

when passing it to generic socket APIs.

The conceptual relationship is:

```text
sockaddr
   ↑
generic API type

sockaddr_in
   ↑
IPv4-specific representation
```

This is why both appear together.

---

# Chapter 15 — Linux Error Handling

Systems calls commonly report failure through:

```text
return value
+
errno
```

For example:

```cpp
if (serverSocket == -1)
{
    perror("socket");
}
```

`perror()` prints a human-readable description based on `errno`.

Important errors later in the project included:

```text
EINTR
EAGAIN
EWOULDBLOCK
```

These are not all equivalent to "the program is broken."

In non-blocking event-driven code, some are normal control-flow signals.

---

# PART IV — TCP FUNDAMENTALS

---

# Chapter 16 — TCP vs UDP

The project uses TCP because the application requires a reliable ordered stream.

TCP provides:

```text
connection
ordering
retransmission
flow control
reliable byte delivery
```

UDP is datagram-oriented and does not provide the same connection/reliability semantics.

The project is specifically designed to teach TCP server architecture.

---

# Chapter 17 — TCP Connection Establishment

A TCP connection is established through the familiar three-way handshake:

```text
Client                  Server

SYN  ------------------>

     <------------------ SYN-ACK

ACK  ------------------>
```

After establishment, both sides can exchange data.

The application normally does not manually implement these packets.

The TCP stack handles them.

The socket API exposes the resulting connection to the application.

---

# Chapter 18 — TCP Is a Byte Stream

This is one of the most important chapters for the future Redis clone.

Suppose the client sends:

```text
"HELLO"
```

then:

```text
"WORLD"
```

The receiver might see:

```text
"HELLOWORLD"
```

or:

```text
"HEL"
"LOWO"
"RLD"
```

or another segmentation.

Therefore the application protocol must define how messages are framed.

A future protocol parser must solve:

```text
TCP byte stream
      ↓
application messages
```

This distinction is essential.

---

# Chapter 19 — Closing TCP Connections

TCP termination can involve FIN and ACK exchanges.

At the application level, an important observation is:

```cpp
recv(...) == 0
```

which indicates orderly end-of-stream from the peer.

The server then performs application cleanup:

```text
remove client state
     ↓
remove event registration
     ↓
close FD
```

Later, `EPOLLRDHUP` becomes another useful notification mechanism for peer shutdown.

---

# Chapter 20 — Flow Control, Congestion Control and Backpressure

TCP itself has mechanisms for controlling how much data can be in flight.

But the application has another problem:

```text
fast producer
     ↓
slow client
```

If the server continuously generates output faster than a client can consume it, the application's output buffer can grow.

That is **application-level backpressure**.

Later the project introduced:

```cpp
MAX_OUTPUT_BUFFER
```

to prevent unlimited memory growth.

This is an important distinction:

```text
TCP flow control
        ≠
application output-buffer policy
```

Both matter, but they operate at different layers.

---

# PART V — BUILDING THE FIRST SERVER

---

# Chapter 21 — The Sequential TCP Server

The first architecture follows the simplest possible lifecycle:

```text
socket
 ↓
bind
 ↓
listen
 ↓
accept
 ↓
recv
 ↓
send
 ↓
close
```

The code is intentionally simple.

The purpose is not scalability.

The purpose is to establish the basic socket lifecycle before adding complexity.

---

# Chapter 22 — Client Implementation

The client performs:

```text
socket
 ↓
construct server address
 ↓
inet_pton
 ↓
connect
 ↓
send / recv
 ↓
close
```

The client is useful because it lets us test the server independently.

A server is not understood fully until we understand the client-server interaction.

---

# Chapter 23 — Debugging the First Server

The project encountered platform and compilation issues while moving to Linux.

One important distinction was:

```text
Windows Winsock
        vs
Linux POSIX sockets
```

Windows commonly uses Winsock headers and initialization.

Linux uses headers such as:

```cpp
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
```

This was an important practical lesson:

> Networking APIs are operating-system interfaces, not just generic C++ libraries.

Another practical issue was executable handling under WSL.

A program must be compiled for the environment in which it is executed.

---

# PART VI — THREAD-PER-CLIENT SERVER

---

# Chapter 24 — Why We Need Concurrency

The sequential server has a fundamental problem.

Suppose:

```text
Client A connects
```

and the server waits in:

```cpp
recv(A)
```

If A stops sending data, the server can remain blocked.

Meanwhile:

```text
Client B
Client C
Client D
```

may also want service.

The server needs a concurrency model.

---

# Chapter 25 — `std::thread`

The thread-per-client design uses:

```text
accept()
   ↓
create thread
   ↓
handle client
   ↓
continue accepting
```

Conceptually:

```text
                    Main Thread
                         │
              accept new clients
                         │
          ┌──────────────┼──────────────┐
          ▼              ▼              ▼
      Thread A        Thread B        Thread C
          │              │              │
       recv(A)        recv(B)        recv(C)
```

This is straightforward and excellent for learning.

---

# Chapter 26 — Shared Client State

The server needs to know which clients exist.

A model such as:

```cpp
struct Client
{
    int socket;
    string username;
};
```

allows:

```cpp
vector<Client> clients;
```

The collection becomes shared state.

Operations include:

```text
addClient
removeClient
getUsername
getSocketByUsername
changeUsername
broadcast
```

This creates the need for synchronization.

---

# Chapter 27 — Race Conditions

Imagine:

```text
Thread A → remove client
Thread B → broadcast to clients
```

If both modify or inspect the same vector at the wrong time, the result can be invalid.

The important principle is:

> Concurrent access to mutable shared state must have a defined synchronization strategy.

---

# Chapter 28 — Mutexes and `lock_guard`

The project used:

```cpp
mutex clientMutex;
```

and:

```cpp
lock_guard<mutex> lock(clientMutex);
```

The protected region becomes the critical section.

The goal is not:

> "Put a mutex everywhere."

The goal is:

> Protect shared state while keeping the critical section as small and well-defined as possible.

---

# Chapter 29 — Username Management

The chat server introduced usernames.

A username must satisfy application-level rules.

The server therefore needs operations such as:

```text
check uniqueness
assign username
find username
change username
remove username
```

An `unordered_set<string>` is useful for uniqueness.

A lookup structure such as:

```text
username → socket
```

can also make private messaging easier.

---

# Chapter 30 — Broadcasting

Broadcasting requires iterating over connected clients.

Conceptually:

```text
sender
  ↓
server
  ↓
all eligible recipients
```

The server must decide:

```text
Should sender receive its own message?
Should disconnected clients be skipped?
What happens if send fails?
```

These are application-level design decisions.

---

# Chapter 31 — Private Messaging and Commands

The server eventually supported commands including:

```text
/help
/list
/nick
/msg
/quit
```

This created a simple application protocol.

The architecture became:

```text
handleClient()
      ↓
processInput()
      ↓
processCommand()
      ↓
command-specific helper
```

This separation made the server easier to reason about.

A later architectural improvement is a reusable `ParsedCommand` structure, but that should be treated as a later refactoring rather than pretending it was present from the beginning.

---

# Chapter 32 — Client Disconnects and Resource Ownership

A client can disconnect normally or unexpectedly.

The server must ensure:

```text
client state removed
socket closed
other clients notified if appropriate
```

The core ownership question is:

> Who is responsible for closing this descriptor?

Clear ownership prevents leaks and double-closing.

This lesson becomes even more important in the later `epoll` architecture.

---

# PART VII — PROFESSIONALIZING THE ARCHITECTURE

---

# Chapter 33 — Why Refactor Into `ChatServer`?

The early server accumulated:

```text
global state
helper functions
client management
networking
commands
logging
```

A class provides a single server object containing the state and behavior.

Instead of global variables:

```text
server state
```

becomes:

```text
ChatServer instance
```

The goal is encapsulation, not OOP for its own sake.

---

# Chapter 34 — `ChatServer` Architecture

The server was organized around functions including:

```text
performHandshake()
processInput()
processCommand()
handleClient()
broadcast()
getUsername()
getSocketByUsername()
changeUsername()
removeClient()
acceptClients()
createSocket()
configureSocket()
bindSocket()
startListening()
printBanner()
start()
stop()
```

This creates a useful layered structure:

```text
Lifecycle
   ↓
Connection handling
   ↓
Client handling
   ↓
Input processing
   ↓
Command processing
   ↓
Application behavior
```

---

# Chapter 35 — Separation of Responsibilities

A function should have a clear responsibility.

For example:

```text
createSocket()
```

should create the socket.

```text
bindSocket()
```

should bind it.

```text
startListening()
```

should put it into listening state.

This makes errors easier to locate.

If startup fails:

```text
socket?
bind?
listen?
```

can be diagnosed separately.

---

# Chapter 36 — Protocol Commands and Application Logic

The networking layer should not need to understand every chat command.

A useful conceptual separation is:

```text
Network bytes
     ↓
input processing
     ↓
command interpretation
     ↓
application behavior
```

This separation becomes directly useful when building a Redis clone, where the network layer and command engine are also distinct components.

---

# PART VIII — LOGGING AND CONFIGURATION

---

# Chapter 37 — Designing the Logger

A server needs logs because terminal output alone is not enough.

Logs answer questions such as:

```text
When did the server start?
Which client connected?
Which client disconnected?
What failed?
What configuration was used?
When did an unexpected error occur?
```

The project introduced a logger concept with levels such as:

```cpp
enum class LogLevel
{
    INFO,
    WARNING,
    ERROR
};
```

---

# Chapter 38 — Logger

The logger concept contains:

```text
file stream
mutex
minimum level
log()
```

A typical design is:

```text
Logger
 ├── log file
 ├── log mutex
 ├── minimum level
 └── log()
```

The file should normally be opened in append mode if we want previous logs preserved.

Thread safety matters because multiple client threads may log simultaneously.

---

# Chapter 39 — Logger Linker Problems

The project also demonstrated an important C++ distinction:

```text
declaration
```

is not the same as:

```text
definition
```

A header can contain:

```cpp
class Logger
{
public:
    Logger(const string& filename);
};
```

but the linker still needs a matching definition:

```cpp
Logger::Logger(const string& filename)
{
    ...
}
```

If the declaration exists but the implementation is missing from the linked object files, the linker can produce an undefined-reference error.

This is a useful systems lesson:

> Compilation checks whether the code is syntactically and semantically valid within a translation unit; linking connects definitions across translation units.

---

# Chapter 40 — Configuration System

Hardcoded values such as:

```text
port
log filename
server limits
```

eventually become configuration candidates.

A configuration file allows:

```text
code
  ↓
read configuration
  ↓
runtime values
```

instead of recompiling for every deployment change.

The project also encountered configuration-file path problems.

The important lesson was that relative paths are interpreted relative to the process's current working directory, not necessarily relative to the source file.

---

# PART IX — GIT AND GITHUB

---

# Chapter 41 — Git From Zero

Git records project history.

The important mental model is:

```text
working directory
       ↓
staging area
       ↓
commit
       ↓
repository history
```

Git is not merely a way to upload files.

It is a version-control system.

---

# Chapter 42 — Working Tree and Staging Area

If a source file changes:

```text
working tree
```

contains the modification.

After:

```bash
git add file.cpp
```

the change enters the staging area.

Then:

```bash
git commit
```

creates a snapshot in history.

---

# Chapter 43 — Commits and History

A useful commit describes a meaningful change.

For this project, examples could be:

```text
Implement TCP client
Add thread-per-client handling
Add username management
Refactor server into ChatServer
Implement select-based server
Implement poll-based server
Add non-blocking I/O
Add output buffering
Implement epoll event loop
```

The history itself becomes evidence of architectural evolution.

---

# Chapter 44 — Remotes and GitHub

A remote repository allows the project history to be stored and shared.

The important distinction is:

```text
local Git repository
        ↓
remote GitHub repository
```

A push transfers local commits to the remote.

The GitHub README should explain:

```text
what the project is
why it exists
how to build it
how to run it
architecture
what was learned
future work
```

---

# Chapter 45 — Branches, Merge and Rebase

Branches provide separate lines of development.

A simplified model:

```text
main
 │
 ├── feature
 │
 └── feature
```

Merging combines histories.

Rebasing changes the base of a sequence of commits.

For a learning project, the most important thing is understanding what the commands actually do rather than memorizing command sequences.

---

# Chapter 46 — Debugging Git Problems

The project encountered practical Git issues such as:

```text
rejected push
divergent branches
deleted files
modified files
build files accidentally tracked
```

A useful debugging strategy is:

```bash
git status
git log --oneline
git diff
git branch
git remote -v
```

Do not guess what Git thinks.

Ask Git for its current state.

---

# PART X — `select()`

---

# Chapter 47 — Why Thread-Per-Client Doesn't Scale Forever

Thread-per-client has a simple model:

```text
1000 clients
   ↓
1000 threads
```

The server does not necessarily scan all sockets, but it maintains a large number of execution contexts.

Costs include:

- thread stack memory
- scheduler work
- context switching
- synchronization
- thread lifecycle management

The architectural question becomes:

> Can one thread monitor many sockets and process only the sockets that need attention?

That is the motivation for I/O multiplexing.

---

# Chapter 48 — I/O Multiplexing

I/O multiplexing allows an application to ask the operating system:

> Which of these file descriptors are ready for a particular I/O operation?

The important word is **ready**.

The mechanism does not process the application protocol.

It only helps answer:

```text
Which socket can I work on without immediately blocking?
```

---

# Chapter 49 — `fd_set`

`select()` uses descriptor sets.

Conceptually:

```text
master set
 ├── server socket
 ├── client A
 ├── client B
 └── client C
```

The application passes sets to the kernel.

After `select()` returns, the sets are modified to indicate readiness.

Therefore a common pattern is:

```cpp
fd_set readSet = masterSet;
select(..., &readSet, ...);
```

The master set is preserved for the next iteration.

---

# Chapter 50 — `select()`

The basic pattern is:

```cpp
while (true)
{
    fd_set readSet = masterSet;

    int activity = select(
        maxFd + 1,
        &readSet,
        nullptr,
        nullptr,
        nullptr
    );

    // inspect descriptors
}
```

The server socket is also part of the monitored set.

If:

```cpp
FD_ISSET(serverSocket, &readSet)
```

is true, a new connection is available.

For a client descriptor:

```cpp
FD_ISSET(clientSocket, &readSet)
```

means reading can make progress.

---

# Chapter 51 — Building the `select()` Server

The implementation followed:

```text
FD_ZERO(masterSet)
FD_SET(serverSocket)
maxFd = serverSocket

loop:
    copy masterSet → readSet
    select()
    inspect ready descriptors
```

New clients are added using:

```cpp
FD_SET(clientSocket, &masterSet);
```

Disconnected clients are removed:

```cpp
FD_CLR(clientSocket, &masterSet);
```

---

# Chapter 52 — Master Set vs Temporary Set

This distinction caused confusion initially but is fundamental.

### Master set

Represents:

> All descriptors currently being monitored.

### Temporary read set

Represents:

> Which descriptors were reported readable by this particular `select()` call.

Therefore:

```text
masterSet
    = long-lived interest

readSet
    = current readiness result
```

The next `select()` needs the full master set again.

---

# Chapter 53 — `maxFd`

The traditional `select()` call uses:

```cpp
select(maxFd + 1, ...)
```

The first argument tells the kernel how many descriptor positions should be considered.

Therefore:

```text
maxFd + 1
```

is required rather than simply the number of clients.

---

# Chapter 54 — Read Readiness

A listening socket becomes readable when a connection can be accepted.

A connected TCP socket becomes readable when:

- data is available
- the peer has closed the connection
- another relevant read-side condition exists

Therefore:

```text
readable
```

does not necessarily mean:

```text
positive bytes available
```

The application still calls `accept()` or `recv()` and interprets the result.

---

# Chapter 55 — Non-Blocking `select()`

Non-blocking descriptors can be combined with readiness notification.

The important idea remains:

```text
readiness notification
       ↓
attempt operation
       ↓
process result
```

This leads naturally into the `poll()` implementation.

---

# PART XI — `poll()`

---

# Chapter 56 — Why `poll()`?

`poll()` provides a different interface for monitoring descriptors.

Instead of `fd_set`, the project used:

```cpp
vector<pollfd> pollFds;
```

This avoids the traditional fixed-size descriptor-set representation of `select()` and provides a more direct descriptor-array model.

However, `poll()` does not magically return only the ready descriptors.

The application still examines the returned `revents` fields.

---

# Chapter 57 — `struct pollfd`

The important structure is:

```cpp
struct pollfd
{
    int fd;
    short events;
    short revents;
};
```

Conceptually:

```text
fd       → descriptor
events   → what we want to monitor
revents  → what actually happened
```

---

# Chapter 58 — `events` vs `revents`

This distinction is critical.

The application sets:

```cpp
pollFds[i].events = POLLIN;
```

This means:

> Monitor this descriptor for readable activity.

The kernel fills:

```cpp
pollFds[i].revents
```

after `poll()` returns.

So:

```text
events
   ↓
request

revents
   ↓
result
```

---

# Chapter 59 — `POLLIN`

`POLLIN` represents readable input.

For the listening socket:

```text
POLLIN
   ↓
accept() may make progress
```

For a client:

```text
POLLIN
   ↓
recv() may make progress
```

Again, the application must inspect the actual operation result.

---

# Chapter 60 — `POLLOUT`

`POLLOUT` indicates that writing can make progress.

This becomes particularly important once the server introduces output buffers.

Instead of blindly calling:

```cpp
send()
```

the server can:

```text
queue output
   ↓
enable POLLOUT
   ↓
poll reports writable
   ↓
send some bytes
   ↓
keep remaining bytes buffered
```

---

# Chapter 61 — `POLLHUP` and `POLLERR`

These events provide information about hangups and errors.

The server should not treat every non-`POLLIN` event as ordinary input.

A robust event loop checks exceptional conditions explicitly.

---

# Chapter 62 — Building the `poll()` Server

The basic structure became:

```text
vector<pollfd>
        ↓
poll()
        ↓
for each entry
        ↓
inspect revents
        ↓
accept / recv / send / cleanup
```

The server socket occupies one entry.

Client sockets occupy the others.

When a client disconnects:

```text
close()
erase pollfd
```

The vector's indexing must then be handled carefully.

---

# Chapter 63 — Non-Blocking `poll()`

The project then made sockets non-blocking using:

```cpp
fcntl(fd, F_GETFL, 0);
```

followed by:

```cpp
fcntl(
    fd,
    F_SETFL,
    flags | O_NONBLOCK
);
```

The server then handled:

```text
EAGAIN
EWOULDBLOCK
EINTR
```

explicitly.

This was a major step toward the final event-driven architecture.

---

# PART XII — NON-BLOCKING I/O

---

# Chapter 64 — Blocking vs Non-Blocking

### Blocking

An operation can wait.

```text
recv()
  ↓
no data
  ↓
wait
```

### Non-blocking

The operation returns immediately if it cannot currently make progress.

```text
recv()
  ↓
no data
  ↓
EAGAIN
```

Non-blocking I/O is not "faster" by itself.

It changes the control model.

---

# Chapter 65 — `fcntl()`

`fcntl()` manipulates file-descriptor properties.

The project uses it to obtain current flags:

```cpp
int flags = fcntl(fd, F_GETFL, 0);
```

and update them:

```cpp
fcntl(fd, F_SETFL, flags | O_NONBLOCK);
```

The important pattern is:

```text
get existing flags
      ↓
modify desired flag
      ↓
set flags back
```

---

# Chapter 66 — `F_GETFL`

`F_GETFL` retrieves the descriptor's file status flags.

The code stores them:

```cpp
int flags = fcntl(fd, F_GETFL, 0);
```

The server should preserve existing flags when adding:

```text
O_NONBLOCK
```

rather than replacing everything blindly.

---

# Chapter 67 — `F_SETFL`

`F_SETFL` updates the file status flags.

The typical operation is:

```cpp
fcntl(
    fd,
    F_SETFL,
    flags | O_NONBLOCK
);
```

The bitwise OR preserves the other relevant flags while adding non-blocking mode.

---

# Chapter 68 — `O_NONBLOCK`

Once enabled:

```text
read operation
   ↓
can proceed?
   ├── yes → return data
   └── no  → EAGAIN/EWOULDBLOCK
```

Likewise for writes.

This allows the event loop to remain responsive.

---

# Chapter 69 — `EAGAIN` / `EWOULDBLOCK`

In non-blocking code, these usually mean:

> The operation cannot make progress right now.

For `recv()`:

```text
no more data currently available
```

For `send()`:

```text
kernel cannot accept more data currently
```

The correct response is often:

```text
stop this operation
return to event loop
wait for another readiness notification
```

This is a crucial mental shift.

`EAGAIN` is not necessarily a fatal error.

---

# Chapter 70 — `EINTR`

A system call can be interrupted by a signal.

For example:

```text
epoll_wait()
recv()
accept()
```

may return `-1` with:

```text
errno == EINTR
```

The operation may need to be retried.

Therefore the project adopted patterns such as:

```cpp
if (errno == EINTR)
{
    continue;
}
```

The correct response depends on the operation and surrounding state.

---

# Chapter 71 — The Non-Blocking I/O Decision Table

| Operation | Result | Meaning | Typical Action |
|---|---|---|---|
| `accept()` | positive FD | client available | register client |
| `accept()` | `EAGAIN` | no more pending clients | stop accepting |
| `recv()` | `> 0` | bytes received | process bytes |
| `recv()` | `0` | peer closed | cleanup |
| `recv()` | `EAGAIN` | no more data now | stop reading |
| `recv()` | `EINTR` | interrupted | retry |
| `send()` | `> 0` | bytes sent | remove sent bytes |
| `send()` | `EAGAIN` | cannot send now | retain buffer |
| `send()` | `EINTR` | interrupted | retry |
| `send()` | other error | real failure | cleanup |

This table is one of the most useful references in the entire project.

---

# PART XIII — OUTPUT BUFFERING

---

# Chapter 72 — Why `send()` Is Not Guaranteed to Send Everything

Suppose:

```text
output = 10 KB
```

The server calls:

```cpp
send(fd, output.data(), output.size(), 0);
```

The result might be:

```text
4096 bytes sent
```

That does not mean the other:

```text
6144 bytes
```

were lost.

They were simply not sent by that call.

Therefore:

```text
application output
       ↓
pending output buffer
       ↓
send()
       ↓
partial write
       ↓
retain unsent bytes
```

---

# Chapter 73 — Partial Writes

If:

```cpp
bytesSent = send(...);
```

returns:

```text
N
```

then only the first N bytes were accepted by the kernel.

The application must retain:

```text
remaining bytes
```

This is the reason the project moved from direct broadcast `send()` calls toward explicit output buffers.

---

# Chapter 74 — Output Buffers

Each client needs a place for pending output.

The early teaching implementation used parallel structures:

```text
pollFds
outputBuffers
```

with matching indices.

Later, the epoll architecture used:

```cpp
unordered_map<int, string> outputBuffers;
```

which represents:

```text
FD → pending bytes
```

This is conceptually cleaner because the socket descriptor directly identifies its state.

---

# Chapter 75 — `POLLOUT`

The server should not continuously call `send()` on every client.

Instead:

```text
output buffer becomes non-empty
        ↓
enable POLLOUT
        ↓
poll()
        ↓
socket becomes writable
        ↓
send()
```

When the buffer becomes empty:

```text
disable POLLOUT
```

This is important because a socket may remain writable most of the time.

Keeping writable monitoring permanently enabled can cause the event loop to repeatedly wake up for a socket that has nothing to send.

---

# Chapter 76 — Retrying Failed Writes

A non-blocking write can return:

```text
EAGAIN
```

The correct response is not:

```text
delete the message
```

Instead:

```text
keep unsent bytes
        ↓
wait for writable readiness
        ↓
try again
```

This is the foundation of reliable application-level output buffering.

---

# Chapter 77 — Parallel Vectors: Teaching Implementation

The project used:

```cpp
vector<pollfd> pollFds;
vector<string> outputBuffers;
```

with the invariant:

```text
pollFds[i]
     ↕
outputBuffers[i]
```

This was intentionally kept because it was easier to understand while learning.

The trade-off is that every insertion, deletion, and index change must preserve the relationship.

If:

```cpp
pollFds.erase(pollFds.begin() + i);
```

then:

```cpp
outputBuffers.erase(outputBuffers.begin() + i);
```

must also happen.

---

# Chapter 78 — Improving the Data Model

A more scalable conceptual representation is:

```cpp
struct Connection
{
    int fd;
    string inputBuffer;
    string outputBuffer;
};
```

or:

```text
unordered_map<int, Connection>
```

This keeps per-client state together.

However, an important engineering lesson is:

> A more sophisticated data structure is not automatically better for learning.

The parallel-vector implementation was useful because it made the mapping explicit before introducing a richer connection object.

---

# PART XIV — `EPOLL` FUNDAMENTALS

---

# Chapter 79 — Why `epoll()`?

The project compared the approaches conceptually:

```text
Thread-per-client
    ↓
many execution contexts

select()
    ↓
descriptor sets + scanning

poll()
    ↓
descriptor array + scanning

epoll()
    ↓
kernel-maintained interest list
    ↓
ready events returned
```

The important difference is the readiness model.

With `epoll`, the application registers interest:

```text
FD 4 → EPOLLIN
FD 5 → EPOLLIN
FD 6 → EPOLLIN
```

Then waits:

```cpp
epoll_wait()
```

The kernel returns ready events.

The application processes:

```text
returned events
```

rather than scanning every registered connection just to discover readiness.

---

# Chapter 80 — `epoll_create1()`

The server creates an epoll instance:

```cpp
int epollFd = epoll_create1(0);
```

The returned descriptor represents the epoll instance.

Conceptually:

```text
epollFd
   ↓
kernel-maintained event interest
```

---

# Chapter 81 — `epoll_ctl()`

`epoll_ctl()` manages the interest list.

Operations include:

```text
EPOLL_CTL_ADD
EPOLL_CTL_MOD
EPOLL_CTL_DEL
```

The pattern is:

```text
epoll_ctl(
    epollFd,
    operation,
    socketFd,
    &event
);
```

---

# Chapter 82 — `epoll_wait()`

The main event loop calls:

```cpp
int activity = epoll_wait(
    epollFd,
    events,
    MAX_EVENTS,
    -1
);
```

The kernel waits until relevant events occur.

The returned integer tells us how many events were placed in the array.

For example:

```text
activity = 3
```

means three events were returned.

The application then processes:

```cpp
for (int i = 0; i < activity; i++)
{
    ...
}
```

---

# Chapter 83 — `epoll_event`

The event structure contains:

```cpp
uint32_t events;
epoll_data_t data;
```

The project commonly used:

```cpp
event.data.fd = socketFd;
```

Then:

```cpp
int currentFd = events[i].data.fd;
```

This gives the event handler the descriptor associated with the readiness notification.

---

# Chapter 84 — `EPOLL_CTL_ADD`

When the server socket is created:

```cpp
epoll_ctl(
    epollFd,
    EPOLL_CTL_ADD,
    serverSocket,
    &serverEvent
);
```

The server is registering interest in the listening socket.

For example:

```cpp
serverEvent.events = EPOLLIN;
```

means the event loop wants read-side readiness notifications.

---

# Chapter 85 — `EPOLL_CTL_MOD`

When a client's output buffer becomes non-empty, the server modifies its monitored events:

```text
EPOLLIN
+
EPOLLOUT
```

Later, when the output buffer becomes empty:

```text
EPOLLIN
```

is sufficient again.

This is dynamic interest management.

---

# Chapter 86 — `EPOLL_CTL_DEL`

When a client is removed:

```cpp
epoll_ctl(
    epollFd,
    EPOLL_CTL_DEL,
    clientSocket,
    nullptr
);
```

removes the descriptor from the epoll interest list.

Then:

```cpp
close(clientSocket);
```

releases the socket.

Both application state and kernel registration need to be cleaned up.

---

# Chapter 87 — The Kernel Interest List

The conceptual model is:

```text
Application
    │
    │ epoll_ctl(ADD)
    ▼
Kernel
 ┌───────────────────────┐
 │ interest list         │
 │                       │
 │ socket 4 → EPOLLIN    │
 │ socket 5 → EPOLLIN    │
 │ socket 6 → EPOLLIN    │
 └───────────────────────┘
```

When activity occurs:

```text
socket 5 becomes readable
        ↓
kernel records readiness
        ↓
epoll_wait() returns event for 5
```

This is the key architectural difference from manually checking every descriptor.

---

# Chapter 88 — Ready Events vs Connected Clients

This distinction is critical.

Suppose:

```text
100,000 connected clients
50 currently ready
```

The server's `epoll_wait()` result may contain approximately:

```text
50 events
```

The application processes the returned events.

It does not need to scan all 100,000 registered descriptors merely to determine which are ready.

However, the server may still maintain a separate collection of connected-client state.

Therefore:

```text
connected clients
       ≠
ready events
```

This distinction becomes especially important for broadcasting.

---

# PART XV — NON-BLOCKING EPOLL

---

# Chapter 89 — Non-Blocking `epoll`

`epoll` itself is an event-notification mechanism.

It does not automatically make sockets non-blocking.

The sockets are explicitly configured using:

```cpp
fcntl(fd, F_SETFL, flags | O_NONBLOCK);
```

The final architecture therefore uses:

```text
epoll
+
non-blocking sockets
```

---

# Chapter 90 — Accept Until `EAGAIN`

With edge-triggered behavior, the server should not assume one `accept()` handles all newly available connections.

The pattern is:

```cpp
while (true)
{
    int clientSocket = accept(...);

    if (clientSocket == -1)
    {
        if (errno == EAGAIN ||
            errno == EWOULDBLOCK)
        {
            break;
        }

        ...
    }

    // register client
}
```

The logic is:

```text
accept
accept
accept
...
EAGAIN
 ↓
no more currently available
 ↓
return to event loop
```

---

# Chapter 91 — Receive Until `EAGAIN`

For edge-triggered client sockets:

```cpp
while (true)
{
    ssize_t bytesReceived = recv(...);

    if (bytesReceived > 0)
    {
        // process bytes
        continue;
    }

    if (bytesReceived == 0)
    {
        // disconnect
        break;
    }

    if (errno == EAGAIN ||
        errno == EWOULDBLOCK)
    {
        break;
    }
}
```

The purpose is to drain the available data.

Stopping after one successful `recv()` can leave unread data in the socket while no new edge is necessarily generated for the remaining already-available data.

---

# Chapter 92 — Send Until `EAGAIN`

The same principle applies to output.

```text
while output exists:
    send()
       ↓
    success → remove sent bytes
       ↓
    continue
```

If:

```text
EAGAIN
```

occurs:

```text
stop writing
keep remaining output
wait for EPOLLOUT
```

This is the fundamental output-buffer loop.

---

# Chapter 93 — Handling `EINTR`

Even in the event-driven architecture:

```text
accept
recv
send
epoll_wait
```

can potentially be interrupted.

The project therefore used:

```cpp
if (errno == EINTR)
{
    continue;
}
```

where retrying is appropriate.

The distinction remains:

```text
EINTR
  =
operation interrupted

EAGAIN
  =
operation cannot make progress now
```

They require different reasoning.

---

# PART XVI — EDGE-TRIGGERED EPOLL

---

# Chapter 94 — Level-Triggered vs Edge-Triggered

The two modes can be understood conceptually.

## Level-triggered

If the condition remains true, the application can continue receiving notifications.

```text
data available
   ↓
event
   ↓
data still available
   ↓
event again
```

## Edge-triggered

The application is notified when the state changes.

```text
no data
   ↓
data arrives
   ↓
edge
   ↓
event
```

This places more responsibility on the application.

---

# Chapter 95 — `EPOLLET`

The flag:

```cpp
EPOLLET
```

requests edge-triggered behavior.

The project used combinations such as:

```cpp
EPOLLIN |
EPOLLRDHUP |
EPOLLET
```

This makes the event loop efficient but requires careful non-blocking code.

---

# Chapter 96 — Why Edge Triggering Changes the Programming Model

With level-triggered behavior, the application can sometimes process one portion of available work and rely on another readiness notification.

With edge-triggered behavior, the application should generally drain the resource:

```text
read
read
read
EAGAIN
```

The event means:

> Something changed.

The application must take responsibility for consuming the available work.

---

# Chapter 97 — Why `EPOLLET` + Blocking I/O Is Dangerous

Imagine:

```text
socket becomes readable
       ↓
recv()
       ↓
data received
       ↓
recv() again
       ↓
no more data
```

With a blocking socket, the second call can wait.

The event loop is now stuck.

Other clients cannot be processed.

Therefore:

```text
EPOLLET
+
blocking recv()
```

is a dangerous combination.

---

# Chapter 98 — `EPOLLET` + `O_NONBLOCK`

The safe conceptual combination is:

```text
EPOLLET
    +
O_NONBLOCK
    +
drain until EAGAIN
```

Now the event loop can determine when all currently available work has been consumed.

Example:

```text
recv()
  ↓
data
  ↓
recv()
  ↓
data
  ↓
recv()
  ↓
EAGAIN
  ↓
done for now
```

---

# Chapter 99 — Draining the Socket

The final event-driven read loop therefore follows:

```text
EPOLLIN
   ↓
recv()
   ↓
bytes > 0
   ↓
process
   ↓
recv() again
   ↓
...
   ↓
EAGAIN
   ↓
return to epoll_wait()
```

This is one of the most important implementation patterns in the project.

---

# PART XVII — WRITABLE EVENTS

---

# Chapter 100 — `EPOLLIN`

`EPOLLIN` tells the application that read-side progress is possible.

For the listening socket:

```text
EPOLLIN
   ↓
accept()
```

For a client socket:

```text
EPOLLIN
   ↓
recv()
```

It does not itself parse the bytes.

---

# Chapter 101 — `EPOLLOUT`

`EPOLLOUT` tells the application that write-side progress is possible.

The server uses it only when:

```text
outputBuffer != empty
```

Conceptually:

```text
message generated
       ↓
append to output buffer
       ↓
enable EPOLLOUT
       ↓
epoll_wait()
       ↓
EPOLLOUT
       ↓
send()
       ↓
partial or complete
```

---

# Chapter 102 — `EPOLLRDHUP`

`EPOLLRDHUP` can indicate that the peer has closed its writing side.

The important project lesson is not to blindly treat every event as an immediate full cleanup.

A readable event may accompany shutdown.

The application should still correctly interpret:

```cpp
recv() == 0
```

when confirming an orderly end-of-stream.

---

# Chapter 103 — `EPOLLERR`

`EPOLLERR` indicates an error condition associated with the descriptor.

The event loop must have a cleanup path:

```text
error
 ↓
remove from epoll
 ↓
close socket
 ↓
erase application state
```

---

# Chapter 104 — `EPOLLHUP`

`EPOLLHUP` indicates a hangup condition.

As with other exceptional events, the application must consider the possibility that readable data is also present.

A robust event loop should not accidentally discard valid data merely because a hangup notification is present.

---

# Chapter 105 — Combining Events

A client can be registered with:

```cpp
EPOLLIN |
EPOLLRDHUP |
EPOLLET
```

When output is pending:

```cpp
EPOLLIN |
EPOLLOUT |
EPOLLRDHUP |
EPOLLET
```

This means the server can simultaneously:

```text
read incoming data
+
write pending output
+
detect peer shutdown
```

within the same event-driven architecture.

---

# Chapter 106 — Why We Don't Keep `EPOLLOUT` Enabled Constantly

A writable TCP socket is often writable.

If `EPOLLOUT` is always monitored:

```text
epoll_wait()
   ↓
socket writable
   ↓
event
   ↓
nothing to send
   ↓
epoll_wait()
   ↓
socket writable
   ↓
event again
```

The server can wake repeatedly without useful work.

Therefore:

```text
output empty
    ↓
EPOLLOUT disabled

output non-empty
    ↓
EPOLLOUT enabled
```

This is a small but important event-loop optimization.

---

# PART XVIII — CURRENT EVENT-DRIVEN MENTAL MODEL

At this point the project has reached the core `epoll` architecture.

```text
                    epoll_wait()
                         │
               ┌─────────┴─────────┐
               │                   │
         server socket        client socket
               │                   │
            EPOLLIN          EPOLLIN/OUT
               │                   │
            accept              recv
               │                   │
          register FD          process data
                                   │
                               broadcast
                                   │
                          output buffers
                                   │
                              EPOLLOUT
                                   │
                                send()
                                   │
                         partial write?
                           │         │
                          yes       no
                           │         │
                     retain bytes   buffer empty
                           │         │
                        EAGAIN      disable
                           │       EPOLLOUT
                           │
                           └───────┘
```

This is the foundation for the later chapters on:

- final output-buffer architecture
- broadcast correctness
- backpressure
- safe client removal
- complete final `epoll` server
- interview-level comparison of `select`, `poll`, and `epoll`

---

# Progress Marker

## Completed in this bulk generation

```text
[x] Part I  — Project Foundations
[x] Part II — C++ Foundations Used in the Project
[x] Part III — Linux Socket Programming
[x] Part IV — TCP Fundamentals
[x] Part V — Building the First Server
[x] Part VI — Thread-Per-Client Server
[x] Part VII — Professionalizing the Architecture
[x] Part VIII — Logging and Configuration
[x] Part IX — Git and GitHub
[x] Part X — select()
[x] Part XI — poll()
[x] Part XII — Non-Blocking I/O
[x] Part XIII — Output Buffering
[x] Part XIV — epoll Fundamentals
[x] Part XV — Non-Blocking epoll
[x] Part XVI — Edge-Triggered epoll
[x] Part XVII — Writable Events
[x] Part XVIII — Current Event-Driven Mental Model
```

## Next unfinished part

```text
PART XIX — Broadcast Architecture
```

Next chapters:

```text
Chapter 113 — Connected vs Ready Clients
Chapter 114 — The Incorrect epoll_wait() Broadcast Idea
Chapter 115 — Correct Broadcast Architecture
```

After that:

```text
Part XX  — Backpressure
Part XXI — Client Lifecycle
Part XXII — Final Architecture
...
```


# PART XIX — BROADCAST ARCHITECTURE

---

# Chapter 113 — Connected Clients vs Ready Clients

This distinction is fundamental to the final `epoll` architecture.

The server may have:

```text
100 clients connected
```

while only:

```text
5 clients
```

are currently ready for some I/O operation.

These are two different sets.

### Connected clients

Represent application state:

```text
Who is currently connected?
```

### Ready events

Represent kernel readiness:

```text
Which registered descriptors need attention right now?
```

Therefore:

```text
connected clients
        ≠
events returned by epoll_wait()
```

This matters enormously for broadcasting.

If Client A sends:

```text
Hello
```

the server must not broadcast only to the descriptors that happened to appear in the current `epoll_wait()` result.

A client that is connected but currently not ready for reading may still need to receive the message.

---

# Chapter 114 — The Incorrect `epoll_wait()` Broadcast Idea

A tempting but incorrect design is:

```text
epoll_wait()
    ↓
5 ready events
    ↓
Client A sent a message
    ↓
send to those 5 events
```

This confuses:

```text
ready-to-read/write now
```

with:

```text
currently connected
```

Suppose:

```text
Clients:
A B C D E F G

Ready events:
A D
```

If A sends a message and the server broadcasts only to:

```text
A and D
```

then:

```text
B C E F G
```

may never receive it.

The event list is not the complete client list.

It is only the current readiness result.

---

# Chapter 115 — Correct Broadcast Architecture

The server needs separate application state representing connected clients.

Conceptually:

```text
                ChatServer
                    │
        ┌───────────┴───────────┐
        │                       │
 connected clients        epoll interest list
        │                       │
        │                  readiness events
        │
        └────── broadcast ─────┘
```

When Client A sends:

```text
Hello
```

the flow is:

```text
A
 ↓
EPOLLIN
 ↓
recv()
 ↓
application message
 ↓
iterate connected clients
 ↓
append message to each recipient's output buffer
 ↓
enable EPOLLOUT
 ↓
epoll_wait()
 ↓
EPOLLOUT
 ↓
send pending bytes
```

This cleanly separates:

```text
who should receive
```

from:

```text
who can currently send
```

---

# Chapter 116 — Why Broadcast Uses Output Buffers

Suppose:

```text
Client A = fast
Client B = medium
Client C = slow
```

A sends:

```text
large message
```

The server should not do:

```text
send(B)
send(C)
```

and assume both calls finish immediately.

Instead:

```text
A sends
  ↓
append to B output buffer
  ↓
append to C output buffer
  ↓
enable EPOLLOUT for B and C
```

Now the event loop controls when actual transmission occurs.

This means one slow client does not have to block the entire broadcast path.

---

# Chapter 117 — Broadcast Loop

Conceptually:

```cpp
for (auto& client : clients)
{
    if (client.fd == senderFd)
    {
        continue;
    }

    client.outputBuffer.append(
        buffer,
        bytesReceived
    );

    enable EPOLLOUT(client.fd);
}
```

The important operation is:

```text
queue first
send later
```

rather than:

```text
send immediately
```

This is a major architectural improvement.

---

# Chapter 118 — Why the Sender May Be Excluded

The project initially broadcast messages to all other clients.

Therefore:

```text
sender
  ↓
server
  ↓
everyone except sender
```

Whether the sender should also receive the message is an application decision.

For this chat server:

```text
sender does not receive its own broadcast
```

unless the application explicitly changes that behavior.

---

# Chapter 119 — Broadcast Complexity

Suppose there are:

```text
N connected clients
```

and one client sends a message.

A broadcast naturally requires visiting recipients:

```text
O(N)
```

because the server must potentially queue the message for every recipient.

This is different from readiness detection.

`epoll` improves how the server discovers ready descriptors, but it does not make a broadcast mathematically free.

Important distinction:

```text
event notification
    ≠
application broadcast work
```

---

# PART XX — BACKPRESSURE AND OUTPUT LIMITS

---

# Chapter 120 — The Slow Client Problem

Consider:

```text
Client A → sends very quickly
Client B → slow network
```

The server keeps receiving data from A.

Each message is added to B's output buffer:

```text
B output:
message 1
message 2
message 3
message 4
...
```

If B never catches up:

```text
buffer
  ↑
  │
  │ grows forever
  │
memory exhausted
```

This is an application-level backpressure problem.

---

# Chapter 121 — Maximum Output Buffer

The project introduced:

```cpp
const size_t MAX_OUTPUT_BUFFER = 1024 * 1024;
```

which represents a:

```text
1 MB
```

maximum pending output buffer.

Before appending:

```cpp
if (
    output.size() + bytesReceived
    > MAX_OUTPUT_BUFFER
)
{
    ...
}
```

the server checks whether the limit would be exceeded.

This prevents unlimited memory growth caused by a slow recipient.

---

# Chapter 122 — What Should Happen When the Limit Is Exceeded?

The project chose a simple policy:

```text
slow client exceeds limit
        ↓
remove client
```

This is one possible policy.

Other production systems might:

```text
drop old messages
drop new messages
disconnect client
apply rate limits
apply per-user quotas
```

The correct choice depends on the application.

For a learning chat server, disconnecting an unhealthy slow consumer is easy to reason about.

---

# Chapter 123 — Backpressure Is a System-Wide Concept

Backpressure can occur at several levels:

```text
Client
  ↓
application input
  ↓
server processing
  ↓
output queue
  ↓
kernel send buffer
  ↓
TCP
  ↓
network
  ↓
receiver
```

The server's output buffer is only one layer.

The important engineering principle is:

> Every unbounded queue is a potential memory-growth problem.

---

# Chapter 124 — Output Buffer Accounting

A useful invariant is:

```text
outputBuffer.size()
```

represents bytes still owned by the application and waiting to be written.

When:

```cpp
send()
```

returns:

```text
N
```

the server removes exactly N bytes.

Therefore:

```text
before send:
10000 bytes

send returns:
4096

after:
5904 bytes
```

This accounting must remain correct.

---

# Chapter 125 — Partial Write Example

Suppose:

```text
outputBuffer = 10 KB
```

The server calls:

```cpp
send(...)
```

and gets:

```text
4096
```

The remaining data is:

```text
10 KB - 4096 bytes
```

The server keeps those bytes.

It does not recreate the message.

It does not discard the remainder.

It simply advances the output buffer.

---

# Chapter 126 — `string::erase()` and Output Buffers

The implementation used:

```cpp
output.erase(0, bytesSent);
```

This removes the bytes that were successfully transmitted.

Conceptually:

```text
ABCDEFGH
  ↓ send 3
DEFGH
```

The application now knows that:

```text
DEFGH
```

is still pending.

---

# Chapter 127 — Why Output Buffer Design Matters

Without an output buffer:

```text
send()
  ↓
partial write
  ↓
message incomplete
```

The server has no reliable place to retain the unsent portion.

With a buffer:

```text
application message
       ↓
output buffer
       ↓
partial send
       ↓
remaining bytes stay
```

This turns partial writes from a correctness problem into normal event-loop behavior.

---

# PART XXI — CLIENT LIFECYCLE

---

# Chapter 128 — Connection Lifecycle

A client passes through a lifecycle:

```text
NOT CONNECTED
      ↓
CONNECTED
      ↓
REGISTERED WITH EPOLL
      ↓
READING / WRITING
      ↓
DISCONNECTING
      ↓
REMOVED
      ↓
CLOSED
```

Every transition should have clear ownership.

---

# Chapter 129 — Accepting a Client

The final event-driven sequence is:

```text
serverSocket
    ↓
EPOLLIN
    ↓
accept()
    ↓
clientSocket
    ↓
set O_NONBLOCK
    ↓
epoll_ctl(ADD)
    ↓
create client state
```

The client is now part of the server.

---

# Chapter 130 — Registering Client State

A client requires more than a file descriptor.

It may need:

```text
fd
username
input buffer
output buffer
authentication/handshake state
timestamps
application state
```

The final architecture should make this state explicit rather than scattering it across unrelated containers.

---

# Chapter 131 — Removing a Client

A complete removal sequence is:

```text
detect disconnect/error
        ↓
epoll_ctl(DEL)
        ↓
close(fd)
        ↓
erase application state
        ↓
optional notification
```

All relevant state must be removed.

---

# Chapter 132 — Why Cleanup Order Matters

Suppose the server closes the descriptor but forgets:

```text
outputBuffers[fd]
```

The application may retain memory for a client that no longer exists.

Or suppose it removes application state but leaves the descriptor registered with epoll.

The event loop may later receive an event associated with stale state.

Therefore cleanup should be centralized.

---

# Chapter 133 — `removeClient()`

The project introduced a helper:

```cpp
void removeClient(
    int epollFd,
    int clientSocket,
    unordered_map<int, string>& outputBuffers
)
{
    epoll_ctl(
        epollFd,
        EPOLL_CTL_DEL,
        clientSocket,
        nullptr
    );

    close(clientSocket);

    outputBuffers.erase(clientSocket);
}
```

The purpose is to avoid repeating cleanup logic in every error branch.

This is a strong refactoring because disconnects can happen from:

```text
recv()
send()
EPOLLERR
EPOLLHUP
application command
```

---

# Chapter 134 — The Double-Removal Problem

Once cleanup is centralized, another issue appears.

Suppose:

```text
EPOLLERR
```

causes:

```text
removeClient(fd)
```

and later the same event-processing path tries to use:

```text
outputBuffers[fd]
```

The state no longer exists.

Therefore after removal, event handling should generally:

```cpp
continue;
```

rather than continuing to process that descriptor.

---

# Chapter 135 — File Descriptor Reuse

Linux can reuse file descriptor numbers.

For example:

```text
client A → fd 7
client A disconnects
fd 7 closes

new client B → fd 7
```

The number:

```text
7
```

does not permanently identify a particular human client.

It identifies the current open file description associated with that descriptor number in the process.

This is another reason stale state must be removed promptly.

---

# Chapter 136 — Why Stale Events Matter

An event loop must always ask:

```text
Does this descriptor still have valid application state?
```

If an FD has been closed and reused, careless state handling can create extremely confusing bugs.

The safest approach is:

```text
centralized ownership
+
consistent registration
+
consistent cleanup
```

---

# PART XXII — FINAL EVENT-DRIVEN ARCHITECTURE

---

# Chapter 137 — Final Data Model

A clean final architecture can conceptually use:

```cpp
struct Client
{
    int fd;
    string username;
    string inputBuffer;
    string outputBuffer;
};
```

and a mapping:

```text
fd → Client
```

This consolidates per-connection state.

The exact container can vary.

The important property is:

> All state belonging to one connection should be easy to find and update.

---

# Chapter 138 — Final Startup Sequence

The final server startup is:

```text
socket()
   ↓
setsockopt(SO_REUSEADDR)
   ↓
setNonBlocking()
   ↓
bind()
   ↓
listen()
   ↓
epoll_create1()
   ↓
epoll_ctl(ADD serverSocket)
   ↓
event loop
```

This is the complete infrastructure setup.

---

# Chapter 139 — Final Event Loop

The high-level loop is:

```text
while running:

    epoll_wait()

    for each returned event:

        if server socket:
            accept until EAGAIN
            register clients

        else:

            if error:
                remove client
                continue

            if readable:
                recv until EAGAIN
                process messages
                queue broadcasts

            if writable:
                send pending output
                retain partial output

            if output empty:
                disable EPOLLOUT
```

This is the core of the final TCP chat server.

---

# Chapter 140 — Complete Read Path

The final read path is:

```text
EPOLLIN
   ↓
recv()
   ↓
bytes > 0
   ↓
append/process
   ↓
broadcast / command
   ↓
recv() again
   ↓
...
   ↓
EAGAIN
```

If:

```text
recv() == 0
```

then:

```text
client disconnected
```

and cleanup occurs.

---

# Chapter 141 — Complete Write Path

The final write path is:

```text
message queued
      ↓
output buffer non-empty
      ↓
enable EPOLLOUT
      ↓
epoll_wait()
      ↓
EPOLLOUT
      ↓
send()
      ↓
partial?
 ┌────┴────┐
yes        no
 ↓          ↓
retain      buffer empty
remaining      ↓
 ↓         disable EPOLLOUT
wait
```

This solves partial-write correctness.

---

# Chapter 142 — Complete Broadcast Path

The final broadcast path is:

```text
Client A
   ↓
EPOLLIN
   ↓
recv
   ↓
message
   ↓
iterate connected clients
   ↓
append to recipients' output buffers
   ↓
enable EPOLLOUT
   ↓
epoll_wait
   ↓
EPOLLOUT for clients that can currently send
   ↓
send pending bytes
```

Notice the separation:

```text
broadcast target selection
```

uses connected-client state.

```text
actual transmission
```

uses writable readiness.

---

# Chapter 143 — Why `epoll` Does Not Make Broadcasting O(1)

It is tempting to say:

> "epoll makes the server O(ready clients)."

That is only about readiness notification.

A broadcast to N clients still potentially touches:

```text
N recipients
```

because the server must queue the message for those recipients.

So:

```text
epoll readiness
```

can avoid scanning N sockets to discover readiness, but:

```text
application broadcast
```

may still be O(N).

This is an important interview distinction.

---

# Chapter 144 — Final Architecture Diagram

```text
                         ┌──────────────────────┐
                         │       Client A       │
                         └──────────┬───────────┘
                                    │
                                    │ TCP
                                    ▼
                         ┌──────────────────────┐
                         │                      │
                         │      TCP Server      │
                         │                      │
                         │  listening socket    │
                         │          │           │
                         │          ▼           │
                         │       epoll          │
                         │          │           │
                         │    ready events      │
                         │          │           │
                         │          ▼           │
                         │   connection state   │
                         │          │           │
                         │   ┌──────┼──────┐    │
                         │   ▼      ▼      ▼    │
                         │  read   command  write│
                         │   │       │       │   │
                         │   └───────┼───────┘   │
                         │           │           │
                         │      broadcast        │
                         │           │           │
                         │     output buffers    │
                         └───────────┬───────────┘
                                     │
                  ┌──────────────────┼──────────────────┐
                  ▼                  ▼                  ▼
             Client B           Client C           Client D
```

---

# Chapter 145 — What the Project Now Demonstrates

The TCP chat server now demonstrates knowledge of:

```text
C++
Linux
POSIX sockets
TCP
IPv4
socket lifecycle
bind/listen/accept
connect
send/recv
non-blocking I/O
fcntl
errno
EINTR
EAGAIN
EWOULDBLOCK
threads
mutexes
RAII
select
poll
epoll
level-triggered readiness
edge-triggered readiness
partial writes
output buffering
backpressure
broadcasting
client lifecycle
resource ownership
logging
configuration
Git
GitHub
```

This is much more valuable than a chat application that only happens to work.

The project demonstrates how the architecture evolved.

---

# PART XXIII — `select` vs `poll` vs `epoll`

---

# Chapter 146 — The Core Comparison

| Architecture | Main idea | Application work |
|---|---|---|
| Thread-per-client | one execution context per client | process each client's thread |
| `select()` | descriptor sets | inspect descriptor range |
| `poll()` | descriptor array | inspect `revents` entries |
| `epoll()` | kernel interest + ready events | process returned ready events |

The most important lesson is not:

```text
epoll good
select bad
```

It is:

> Each mechanism provides a different readiness and concurrency model with different trade-offs.

---

# Chapter 147 — Thread-Per-Client

Strengths:

```text
simple blocking code
easy mental model
straightforward client isolation
```

Weaknesses:

```text
many threads
memory overhead
scheduler overhead
synchronization
scalability challenges
```

It is excellent for learning and for workloads where the number of simultaneous connections is modest.

---

# Chapter 148 — `select()`

Strengths:

```text
widely understood
portable POSIX-style interface
simple readiness model
```

Weaknesses:

```text
descriptor-set limitations
application scanning
repeated set copying
less suitable for huge descriptor populations
```

It is an excellent first introduction to I/O multiplexing.

---

# Chapter 149 — `poll()`

Strengths:

```text
more flexible descriptor representation
no traditional FD_SETSIZE bitmap limit
simple events/revents model
```

Weaknesses:

```text
application still scans the descriptor array
large mostly-idle sets can require substantial checking
```

It is a natural progression from `select()`.

---

# Chapter 150 — `epoll()`

Strengths:

```text
kernel-managed interest list
ready events returned directly
works naturally with non-blocking I/O
supports level-triggered and edge-triggered modes
```

Weaknesses:

```text
Linux-specific API
more complicated programming model
edge-triggered mode requires careful draining
application must manage buffers and state correctly
```

This is why the project learned `select()` and `poll()` first instead of jumping directly to `epoll()`.

---

# Chapter 151 — The Interview Answer

If asked:

> Why did you move from threads to `select`, then `poll`, then `epoll`?

A strong answer is:

> "I wanted to understand the evolution of server concurrency rather than just use an event API. Thread-per-client gives each connection its own execution context, but that creates thread and synchronization costs. `select` and `poll` allow one event loop to monitor many descriptors, although the application still examines the monitored collection. `epoll` lets Linux maintain an interest list and return the descriptors that currently have relevant events, which is a better model for large numbers of mostly idle connections. I then combined `epoll` with non-blocking sockets, edge-triggered readiness, output buffers, and backpressure handling."

---

# PART XXIV — PRODUCTION-QUALITY REVIEW

---

# Chapter 152 — What Is Still Simplified?

Even the final learning server is not a complete production messaging platform.

Potential production concerns include:

```text
authentication
TLS
rate limiting
message persistence
structured logging
metrics
graceful shutdown
configuration validation
protocol framing
memory pooling
fair scheduling
connection limits
resource quotas
timeouts
load testing
security hardening
```

This is not a failure.

The project is a systems-programming learning project.

The important achievement is that the architecture now exposes the correct concepts.

---

# Chapter 153 — TCP Framing Is Still a Major Next Step

TCP is a byte stream.

Therefore a production protocol needs explicit framing.

Possible approaches:

```text
newline-delimited
length-prefixed
fixed-size header
binary protocol
```

For example:

```text
[length][payload]
```

could define a message boundary.

This is particularly important before building the Redis clone.

---

# Chapter 154 — Graceful Shutdown

A mature server should respond to a shutdown request by:

```text
stop accepting
        ↓
stop normal event processing
        ↓
flush or discard pending output according to policy
        ↓
close clients
        ↓
close epoll
        ↓
close listening socket
        ↓
exit
```

Signals such as `SIGINT` are relevant here.

The project has already encountered the concept of `EINTR`, so signal-aware shutdown is a natural next systems topic.

---

# Chapter 155 — Testing Strategy

The server should be tested beyond:

```text
one client sends hello
```

Useful tests include:

```text
multiple simultaneous clients
client disconnects
server receives empty input
large messages
partial writes
slow clients
rapid connect/disconnect
many clients
invalid commands
duplicate usernames
private messages
broadcast while clients disconnect
server shutdown
restart after shutdown
```

Testing should deliberately exercise failure paths.

---

# Chapter 156 — Stress Testing

A useful progression is:

```text
2 clients
   ↓
5 clients
   ↓
10 clients
   ↓
50 clients
   ↓
100 clients
   ↓
1000 clients
```

Measure:

```text
CPU
memory
latency
throughput
connections
output-buffer growth
disconnect rate
```

The goal is not merely to make the server survive.

The goal is to understand where its bottlenecks appear.

---

# Chapter 157 — Load Testing Questions

When performance degrades, ask:

```text
Is CPU saturated?
Is memory growing?
Are output buffers growing?
Is one slow client causing pressure?
Is logging expensive?
Is the broadcast O(N) work dominant?
Are system calls dominating?
Is the network saturated?
```

This turns performance debugging into a structured investigation.

---

# PART XXV — FINAL PROJECT CHECKLIST

---

# Chapter 158 — Networking Checklist

You should be able to explain without notes:

```text
socket()
bind()
listen()
accept()
connect()
send()
recv()
shutdown()
close()
```

and:

```text
sockaddr
sockaddr_in
htons
htonl
inet_pton
INADDR_ANY
```

---

# Chapter 159 — Linux Systems Checklist

You should understand:

```text
file descriptors
errno
perror
fcntl
O_NONBLOCK
EINTR
EAGAIN
EWOULDBLOCK
```

and why non-blocking I/O changes application control flow.

---

# Chapter 160 — Concurrency Checklist

You should explain:

```text
thread
join
detach
mutex
lock_guard
race condition
critical section
shared state
```

and why thread-per-client eventually becomes less attractive at large connection counts.

---

# Chapter 161 — I/O Multiplexing Checklist

You should be able to draw:

```text
thread-per-client
select
poll
epoll
```

and explain:

```text
what the kernel does
what the application does
what scales
what scans
what returns ready events
```

---

# Chapter 162 — Event-Driven Checklist

You should understand:

```text
non-blocking sockets
EPOLLIN
EPOLLOUT
EPOLLRDHUP
EPOLLERR
EPOLLHUP
EPOLLET
epoll_ctl
epoll_wait
```

and the rule:

```text
EPOLLET + O_NONBLOCK
        ↓
drain until EAGAIN
```

---

# Chapter 163 — Buffering Checklist

You should be able to explain:

```text
partial recv
partial send
input buffer
output buffer
backpressure
maximum output buffer
EPOLLOUT
```

and why:

```text
send() != guaranteed complete message transmission
```

---

# Chapter 164 — Architecture Checklist

The final architecture should be explainable as:

```text
socket setup
    ↓
epoll setup
    ↓
accept
    ↓
client state
    ↓
EPOLLIN
    ↓
recv
    ↓
application processing
    ↓
broadcast / command
    ↓
output buffers
    ↓
EPOLLOUT
    ↓
send
    ↓
partial write handling
    ↓
cleanup
```

If you can explain every arrow, you understand the project.

---

# PART XXVI — INTERVIEW PREPARATION

---

# Chapter 165 — Explain Your Project in 30 Seconds

A concise answer:

> "I built a TCP chat server in C++ on Linux using POSIX sockets. I started with a blocking sequential server, then implemented thread-per-client concurrency, and later rebuilt the connection handling using `select`, `poll`, and finally Linux `epoll`. The final architecture uses non-blocking sockets, edge-triggered epoll, per-client output buffers, partial-write handling, and backpressure limits. The project was mainly to understand how high-concurrency network servers work internally."

---

# Chapter 166 — Explain It in 2 Minutes

A longer answer should cover:

```text
1. socket lifecycle
2. first blocking server
3. thread-per-client
4. shared state + mutex
5. select
6. poll
7. non-blocking sockets
8. partial writes
9. output buffering
10. epoll
11. edge triggering
12. backpressure
13. cleanup
```

The story matters.

You are not merely saying:

> "I used epoll."

You are showing:

> "I understand why the architecture evolved toward epoll."

---

# Chapter 167 — High-Value Interview Questions

### Q1. Why is TCP called a byte stream?

Because TCP provides an ordered stream of bytes rather than preserving application-level message boundaries.

### Q2. Why can `recv()` return fewer bytes than expected?

Because TCP delivers a stream and only reports the bytes currently copied into the application's buffer.

### Q3. Why can `send()` send fewer bytes than requested?

Because the kernel may accept only part of the application buffer at that moment.

### Q4. Why use non-blocking sockets?

So an event-driven server does not become stuck waiting on one descriptor.

### Q5. What does `EAGAIN` mean?

For non-blocking I/O, it generally means the operation cannot make progress at that moment.

### Q6. Why drain until `EAGAIN` with edge-triggered epoll?

Because the edge indicates a state transition; the application is responsible for consuming the currently available work.

### Q7. Why not always monitor `EPOLLOUT`?

Because writable sockets are often continuously writable, which can cause unnecessary wakeups when there is no pending output.

### Q8. Does epoll make broadcast O(1)?

No. Broadcasting to N recipients can still require O(N) application work.

### Q9. Why do we need output buffers?

To handle partial writes and slow consumers without losing unsent data.

### Q10. What happens when `recv()` returns zero?

The peer has performed an orderly shutdown of its sending side; for a normal chat connection, the server should clean up the connection.

---

# Chapter 168 — The Most Important Interview Distinction

Remember this:

```text
READY
```

does not mean:

```text
CONNECTED
```

`epoll_wait()` returns:

```text
ready events
```

The server's client-state structure contains:

```text
connected clients
```

Broadcasting uses the latter.

Actual sending uses the former.

This distinction demonstrates that you understand event-driven architecture rather than simply memorizing epoll APIs.

---

# PART XXVII — PROJECT COMPLETION STATUS

At this point the book documents the complete conceptual journey:

```text
C++ foundations
      ↓
Linux sockets
      ↓
TCP
      ↓
blocking server
      ↓
client
      ↓
thread-per-client
      ↓
mutex / shared state
      ↓
application commands
      ↓
ChatServer architecture
      ↓
logging / configuration
      ↓
Git / GitHub
      ↓
select
      ↓
poll
      ↓
non-blocking I/O
      ↓
partial writes
      ↓
output buffering
      ↓
epoll
      ↓
edge-triggered epoll
      ↓
broadcast architecture
      ↓
backpressure
      ↓
client lifecycle
      ↓
final event-driven server
      ↓
interview preparation
```

This is the point where the TCP chat server stops being merely a project and becomes a systems-programming case study.

---

# PART XXVIII — WHAT TO BUILD NEXT

The next project should not immediately be another chat server.

The natural next step is:

```text
TCP Chat Server
       ↓
deeper protocol understanding
       ↓
Redis clone
```

The chat server has already taught:

```text
TCP
sockets
non-blocking I/O
epoll
buffers
event loops
client management
```

A Redis clone can now add:

```text
custom binary/text protocol
command parser
key-value data structures
expiration
RESP-style framing
persistence
concurrency
performance
memory management
```

The TCP chat server therefore becomes the networking foundation for the next systems project.

---

# FINAL MASTER-BOOK SUMMARY

The central lesson of the entire project is:

> A network server is not just `socket()`, `bind()`, `listen()`, and `accept()`. The difficult part is designing how the application manages many connections, readiness, partial I/O, buffers, errors, lifecycle, and shared state without allowing one slow or broken connection to stop the rest of the server.

The project evolved through increasingly capable models:

```text
blocking
   ↓
threads
   ↓
select
   ↓
poll
   ↓
non-blocking I/O
   ↓
epoll
   ↓
edge-triggered epoll
   ↓
buffered event-driven architecture
```

That evolution is the most valuable artifact of the project.

---

# END OF CURRENT BULK

## Continuation point

The book currently reaches:

```text
PART XXVIII — What to Build Next
```

The TCP chat server portion is now comprehensively documented through:

```text
final epoll architecture
broadcasting
backpressure
partial writes
client lifecycle
select vs poll vs epoll
production review
testing
interview preparation
```

The next major section can begin with:

```text
PART XXIX — Deep Systems Review
```

covering the project line-by-line from the Linux kernel perspective, system-call behavior, TCP buffers, file descriptors, epoll internals, performance reasoning, and Redis-clone preparation.


# PART XXIX — DEEP SYSTEMS REVIEW

---

# Chapter 167 — The File Descriptor Mental Model

At the application level, the server works with:

```cpp
int fd;
```

It is tempting to think:

```text
fd = socket
```

A more accurate mental model is:

```text
process
  ↓
file descriptor number
  ↓
kernel-managed open file state
  ↓
socket object
  ↓
TCP state / buffers / protocol state
```

The integer is only the handle used by the process to refer to the kernel object.

For example:

```text
fd 3 → listening socket
fd 4 → client A
fd 5 → client B
fd 6 → client C
```

The numbers are process-local references.

---

# Chapter 168 — Why File Descriptors Start Small

When a process begins, common descriptors are often:

```text
0 → stdin
1 → stdout
2 → stderr
```

A newly created socket may therefore receive:

```text
3
```

Then another:

```text
4
```

and so on.

This explains why server logs often show:

```text
Socket : 4
Socket : 5
Socket : 6
```

These numbers are not client IDs.

They are file descriptor numbers.

---

# Chapter 169 — File Descriptor Reuse

Suppose:

```text
Client A → fd 7
```

disconnects.

The server:

```cpp
close(7);
```

Later another client connects.

Linux may assign:

```text
Client B → fd 7
```

Therefore:

```text
fd 7
```

does not permanently mean Client A.

This is why application state must be tied to the current lifetime of the connection rather than assuming descriptor numbers are permanent identities.

---

# Chapter 170 — What `close()` Actually Means

When the server calls:

```cpp
close(clientSocket);
```

it releases that process's reference to the descriptor.

For a TCP socket, this participates in the TCP connection shutdown process.

It is therefore not merely:

```text
delete integer
```

It changes the kernel's ownership/reference state and eventually participates in TCP connection teardown.

---

# Chapter 171 — TCP Is a Byte Stream

Your chat application sends:

```text
"Hello"
```

but TCP does not know that `"Hello"` is a chat message.

TCP sees:

```text
H e l l o
```

as bytes in an ordered stream.

Therefore:

```text
send("Hello")
send("World")
```

does not guarantee that the receiver sees:

```text
"Hello"
"World"
```

as two `recv()` calls.

It might observe:

```text
"HelloWorld"
```

or:

```text
"He"
"lloWo"
"rld"
```

The byte order is preserved, but application message boundaries are not.

---

# Chapter 172 — Why the Chat Server Can Appear to Work Anyway

Human typing often produces small messages with enough timing separation that:

```text
send()
```

and:

```text
recv()
```

appear to correspond.

But that is not a protocol guarantee.

A serious protocol needs framing.

For example:

```text
[length][message]
```

If the length is:

```text
5
```

then the receiver knows the next five bytes form one application message.

---

# Chapter 173 — Input Buffer vs Output Buffer

The project emphasized output buffering, but a complete protocol eventually needs both.

### Input buffer

Stores bytes received but not yet forming a complete application message.

```text
TCP bytes
   ↓
inputBuffer
   ↓
parse
   ↓
complete message
```

### Output buffer

Stores application data that has not yet been completely transmitted.

```text
application message
   ↓
outputBuffer
   ↓
send()
   ↓
partial write
   ↓
remaining bytes
```

They solve opposite sides of the stream problem.

---

# Chapter 174 — The Four Important Buffers

A useful mental model is:

```text
Application
    │
    ├── input buffer
    │
    └── output buffer
          │
          ▼
      kernel socket buffers
          │
          ▼
         TCP
          │
          ▼
       network
```

There is therefore more buffering than the C++ `string` objects visible in your program.

---

# Chapter 175 — Why `recv()` Does Not Mean "Receive a Message"

The function:

```cpp
recv(fd, buffer, size, 0);
```

means approximately:

```text
copy currently available stream bytes
into my buffer, subject to the requested size
```

It does not mean:

```text
give me one application message
```

This distinction becomes essential when building a real protocol.

---

# Chapter 176 — Why `send()` Does Not Mean "Send a Message"

Similarly:

```cpp
send(fd, data, length, 0);
```

means:

```text
request that these bytes be transferred
```

It does not guarantee:

```text
all bytes reached the peer
```

or:

```text
the peer processed them
```

The return value tells the application how many bytes were accepted by the socket operation.

---

# Chapter 177 — Kernel Socket Buffers

The operating system maintains buffers associated with sockets.

Conceptually:

```text
application
   ↓ send()
kernel send buffer
   ↓
TCP
   ↓
network
```

and:

```text
network
   ↓
TCP
   ↓
kernel receive buffer
   ↓ recv()
application
```

This explains why `send()` and `recv()` do not directly represent physical network transmission and reception.

---

# Chapter 178 — What Non-Blocking Changes

For a blocking socket:

```text
recv()
   ↓
no data?
   ↓
wait
```

The calling thread can become blocked.

For a non-blocking socket:

```text
recv()
   ↓
no data?
   ↓
-1
errno = EAGAIN/EWOULDBLOCK
```

The application retains control and can return to the event loop.

This is the foundation of the event-driven architecture.

---

# Chapter 179 — Why `EAGAIN` Is Not a Fatal Error

Consider:

```cpp
ssize_t n = recv(fd, buffer, sizeof(buffer), 0);
```

If:

```text
n == -1
errno == EAGAIN
```

the correct interpretation is generally:

```text
there is currently no more work to read
```

not:

```text
the connection is broken
```

In edge-triggered mode this usually means:

```text
I drained the socket for now.
Return to epoll_wait().
```

---

# Chapter 180 — `EINTR` vs `EAGAIN`

These errors have very different meanings.

### `EINTR`

The operation was interrupted by a signal before completing.

Typical response:

```cpp
continue;
```

Retry the operation.

### `EAGAIN`

For non-blocking I/O, the operation cannot make progress right now.

Typical response:

```text
stop draining this descriptor
return to event loop
```

Remember:

```text
EINTR → retry
EAGAIN → wait for readiness again
```

---

# Chapter 181 — Why Edge-Triggered epoll Requires Discipline

With:

```cpp
EPOLLET
```

the kernel reports readiness based on transitions rather than repeatedly reporting the same level of readiness.

Therefore the application should generally:

```text
read until EAGAIN
```

and:

```text
write until EAGAIN
```

Forgetting this can cause the server to stop noticing work that remains available.

---

# Chapter 182 — Level-Triggered vs Edge-Triggered

### Level-triggered

Conceptually:

```text
data remains available
        ↓
epoll continues reporting readable
```

### Edge-triggered

Conceptually:

```text
not ready
   ↓
becomes ready
   ↓
event
```

The application must consume the available work correctly.

This is why edge-triggered programming is more demanding.

---

# Chapter 183 — Why Non-Blocking Is Paired With EPOLLET

Imagine an edge-triggered socket.

The application calls:

```cpp
recv()
```

and receives some data.

If the application then performs another blocking operation while more data is not currently available, the entire event loop can freeze.

Non-blocking I/O prevents this:

```text
recv()
   ↓
available bytes
   ↓
recv()
   ↓
EAGAIN
   ↓
stop
```

The event loop remains responsive.

---

# Chapter 184 — `epoll_ctl()` Is Registration, Not Waiting

These operations have different responsibilities.

### `epoll_ctl()`

Changes the interest list:

```text
ADD
MOD
DEL
```

### `epoll_wait()`

Waits for readiness:

```text
"Tell me which registered descriptors have events."
```

A common interview question is:

> What is the difference between `epoll_ctl()` and `epoll_wait()`?

Answer:

```text
epoll_ctl → configure what I care about
epoll_wait → retrieve what is ready
```

---

# Chapter 185 — The epoll Interest List

The server registers:

```text
server socket
client A
client B
client C
...
```

with epoll.

Conceptually:

```text
epoll instance
 ├── server socket → EPOLLIN
 ├── client A      → EPOLLIN
 ├── client B      → EPOLLIN | EPOLLRDHUP
 └── client C      → EPOLLIN | EPOLLOUT
```

The events associated with each descriptor can change.

For example, once a client's output buffer becomes non-empty:

```text
EPOLLOUT
```

is added.

---

# Chapter 186 — Why EPOLLOUT Should Be Dynamic

Sockets are often writable.

If the server always registers:

```text
EPOLLIN | EPOLLOUT
```

then writable events may be generated frequently even when there is nothing to send.

That creates unnecessary event processing.

Better:

```text
output buffer empty
    ↓
EPOLLIN only

output buffer non-empty
    ↓
EPOLLIN | EPOLLOUT
```

When the buffer drains:

```text
remove EPOLLOUT
```

This is an important event-loop optimization.

---

# Chapter 187 — The Meaning of "Writable"

`EPOLLOUT` does not mean:

```text
the client is waiting for a message
```

It means roughly:

```text
the socket can currently accept some output without blocking
```

Therefore the server still has to check:

```text
Do I actually have bytes to send?
```

The event only tells you that sending may make progress.

---

# Chapter 188 — The Meaning of "Readable"

`EPOLLIN` indicates that a read operation is likely to make progress.

For a TCP socket, this can include:

```text
incoming data
```

and conditions associated with connection closure.

Therefore the server should not blindly assume:

```text
EPOLLIN → bytes > 0
```

It must inspect the result of:

```cpp
recv()
```

---

# Chapter 189 — Why `recv() == 0` Matters More Than Guessing

For a normal TCP connection:

```cpp
recv() == 0
```

means the peer has performed an orderly shutdown of its sending side.

The server should then perform its application-level cleanup policy.

This is why a robust event loop handles:

```text
EPOLLIN
   ↓
recv()
   ↓
0
   ↓
disconnect
```

rather than relying only on hangup notifications.

---

# Chapter 190 — `EPOLLRDHUP`

`EPOLLRDHUP` can notify the application that the peer has closed its write side.

It is useful for detecting half-close conditions.

But the project intentionally treated:

```text
recv() == 0
```

as the final confirmation for ordinary connection cleanup.

This avoids confusing an event notification with the actual stream result.

---

# Chapter 191 — Half-Close

TCP supports independent directions.

Conceptually:

```text
Client → Server
Server → Client
```

A peer can shut down one direction.

For example:

```cpp
shutdown(fd, SHUT_WR);
```

means the local side will no longer send.

The peer may still be able to send data.

This is one reason TCP connection lifecycle is richer than simply:

```text
connected / disconnected
```

---

# Chapter 192 — `shutdown()` vs `close()`

`shutdown()` controls communication directions.

Examples:

```text
SHUT_RD
SHUT_WR
SHUT_RDWR
```

`close()` releases the file descriptor.

A production protocol can use `shutdown()` for graceful protocol-level behavior before final descriptor cleanup.

---

# PART XXX — SYSTEM CALL COST AND PERFORMANCE

---

# Chapter 193 — What Is a System Call?

Your server repeatedly interacts with the kernel through calls such as:

```text
socket()
bind()
listen()
accept()
epoll_ctl()
epoll_wait()
recv()
send()
close()
fcntl()
setsockopt()
```

These cross the user/kernel boundary.

They are necessary, but they are not free.

---

# Chapter 194 — Why Event Loops Reduce Unnecessary Work

A naive design might repeatedly ask about every client:

```text
client 1?
client 2?
client 3?
...
client 100000?
```

An event-driven mechanism allows the kernel to report relevant readiness.

Conceptually:

```text
100000 registered
       ↓
5 ready
       ↓
application handles 5
```

This reduces unnecessary user-space readiness scanning.

---

# Chapter 195 — epoll Does Not Eliminate Work

`epoll` does not magically make all server work proportional to only the number of active clients.

The application can still perform:

```text
broadcast O(N)
parsing
copying
logging
allocation
buffer management
authentication
database operations
```

Therefore a server can still be CPU-bound even with epoll.

---

# Chapter 196 — Copying Data

When a message is received:

```text
kernel receive buffer
       ↓
recv()
       ↓
user-space buffer
```

then broadcasting may perform additional copies:

```text
input buffer
       ↓
client B output buffer
client C output buffer
client D output buffer
...
```

A large broadcast can therefore create significant memory-copy work.

This is one reason high-performance messaging systems require careful data-path design.

---

# Chapter 197 — Logging Can Become a Bottleneck

The learning server prints messages:

```cpp
cout << ...
```

This is useful for debugging.

But under high load:

```text
100000 messages
       ↓
100000 log operations
       ↓
large I/O overhead
```

Logging can dominate the workload.

A production server would normally use:

```text
log levels
structured logs
buffered logging
sampling
asynchronous logging
```

depending on requirements.

---

# Chapter 198 — Memory Allocation

Operations such as:

```cpp
string::append()
string::erase()
unordered_map::operator[]
```

may involve allocation or memory movement.

This is acceptable for the learning server.

For extreme performance, an engineer may investigate:

```text
allocation frequency
buffer reuse
capacity growth
memory fragmentation
object lifetime
cache locality
```

Performance work should be measurement-driven rather than based on assumptions.

---

# Chapter 199 — `string::erase(0, n)` Consideration

The current output-buffer implementation uses:

```cpp
output.erase(0, bytesSent);
```

This is easy to understand.

However, removing bytes from the front of a string may require moving the remaining bytes.

If a buffer is large and many partial writes occur, this can become inefficient.

Possible later designs include:

```text
offset into string
deque
ring buffer
custom byte queue
```

The simple string approach is excellent for learning.

---

# Chapter 200 — A Better Output Buffer Model

Instead of physically erasing from the front:

```text
[already sent][pending data]
       ↑
     offset
```

the application can maintain:

```cpp
size_t outputOffset;
```

Then:

```text
send(data + outputOffset, remaining)
```

After a successful write:

```text
outputOffset += bytesSent
```

When all data is sent:

```text
clear buffer
offset = 0
```

This can avoid repeated front-erasure costs.

---

# Chapter 201 — Why You Should Not Optimize Too Early

The current project prioritizes:

```text
correctness
clarity
understanding
```

before:

```text
micro-optimization
```

A simpler:

```cpp
string
```

that is correct is better during learning than a complicated custom ring buffer whose behavior is not fully understood.

The correct engineering order is:

```text
correct
   ↓
measure
   ↓
identify bottleneck
   ↓
optimize
   ↓
measure again
```

---

# PART XXXI — REDIS-CLONE PREPARATION

---

# Chapter 202 — Why This Project Comes Before Redis

The TCP chat server taught you:

```text
How bytes enter the process
How bytes leave the process
How sockets become file descriptors
How many connections are multiplexed
How non-blocking I/O works
How event loops work
How partial writes are handled
How output queues work
```

These are exactly the foundations needed for a networked in-memory server.

---

# Chapter 203 — Redis Is More Than a Socket Server

A Redis clone requires multiple layers:

```text
TCP networking
      ↓
protocol parsing
      ↓
command dispatch
      ↓
data structures
      ↓
expiration
      ↓
responses
      ↓
persistence / additional features
```

Your chat server already gives you the networking layer.

---

# Chapter 204 — Chat Server vs Redis

### Chat server

```text
message
   ↓
broadcast
```

### Redis-like server

```text
command
   ↓
parse
   ↓
execute
   ↓
modify data
   ↓
construct response
   ↓
send response
```

The important shift is:

```text
communication
```

to:

```text
request/response protocol
```

---

# Chapter 205 — The Redis Event Loop Mental Model

The same fundamental loop applies:

```text
epoll_wait()
     ↓
client readable
     ↓
recv()
     ↓
parse request
     ↓
execute command
     ↓
append response
     ↓
EPOLLOUT
     ↓
send()
```

The networking machinery is therefore directly reusable conceptually.

---

# Chapter 206 — The Next Major Challenge: Protocol Framing

For the Redis clone, you should not simply assume:

```text
one recv() = one command
```

Instead:

```text
recv bytes
    ↓
input buffer
    ↓
parse complete request
    ↓
if incomplete:
    wait for more bytes
```

This is the next level of TCP programming.

---

# Chapter 207 — Incremental Parsing

Suppose the client sends:

```text
SET name Monarch
```

but TCP provides:

```text
SET na
```

first.

Then:

```text
me Monarch
```

later.

The parser should maintain:

```text
inputBuffer = "SET na"
```

then append:

```text
"me Monarch"
```

and parse the complete command.

This is why protocol parsing belongs above the TCP stream layer.

---

# Chapter 208 — Command Processing Architecture

A clean Redis-like architecture can eventually look like:

```text
             epoll
               │
               ▼
        connection manager
               │
               ▼
          input buffer
               │
               ▼
            parser
               │
               ▼
        command structure
               │
               ▼
        command dispatcher
          ┌────┼────┐
          ▼    ▼    ▼
         GET  SET  DEL
          │    │    │
          └────┼────┘
               ▼
          data store
               │
               ▼
         response buffer
               │
               ▼
             send
```

This is a natural continuation of the chat-server architecture.

---

# Chapter 209 — Systems Skills You Have Built

After completing the TCP chat server, your practical systems foundation includes:

```text
processes
threads
file descriptors
system calls
TCP sockets
blocking I/O
non-blocking I/O
I/O multiplexing
event loops
buffers
backpressure
connection lifecycle
error handling
Linux APIs
```

That is a substantial foundation for systems programming.

---

# PART XXXII — FINAL REVIEW QUESTIONS

---

# Chapter 210 — Architecture Questions

Before moving on, you should be able to answer these without looking at the book:

1. Why must the listening socket be registered with epoll?
2. Why are accepted client sockets also registered?
3. Why must sockets be non-blocking with edge-triggered epoll?
4. Why do we accept until `EAGAIN`?
5. Why do we receive until `EAGAIN`?
6. Why do we send until `EAGAIN`?
7. Why do we need output buffers?
8. Why should `EPOLLOUT` be enabled only when output is pending?
9. Why can't the epoll ready-event array represent all connected clients?
10. Why is broadcasting still O(N)?
11. What happens when `recv()` returns zero?
12. What is the difference between `EINTR` and `EAGAIN`?
13. What does `EPOLLRDHUP` represent?
14. What happens when a descriptor is closed and later reused?
15. Why do we call `epoll_ctl(DEL)` before closing a client?

---

# Chapter 211 — Deep Understanding Questions

You should also be able to explain:

```text
What is a file descriptor?
What is the kernel doing with a socket?
Where does received data live before recv()?
Where does outgoing data live before send()?
Why does TCP preserve order but not message boundaries?
Why can send() be partial?
Why can recv() be partial?
Why can one recv() return multiple application messages?
Why can one application message require multiple recv() calls?
```

These questions are more important than memorizing function signatures.

---

# Chapter 212 — Final Self-Test

Try explaining this without notes:

> "A client sends 50 KB. The server uses edge-triggered epoll and has a 1 KB receive buffer. Explain exactly what happens."

A strong answer should contain:

```text
EPOLLIN
   ↓
recv()
   ↓
up to 1 KB
   ↓
process bytes
   ↓
recv() again
   ↓
repeat
   ↓
eventually EAGAIN
   ↓
return to epoll_wait()
```

If the message belongs to a protocol:

```text
input buffer
   ↓
accumulate
   ↓
parse when complete
```

If broadcasting:

```text
append to recipient output buffers
   ↓
enable EPOLLOUT
```

---

# Chapter 213 — Final Self-Test: Partial Write

Now explain:

> "The output buffer contains 50 KB, but send() writes only 4 KB."

Correct reasoning:

```text
50 KB pending
     ↓
send()
     ↓
4 KB sent
     ↓
46 KB remains
     ↓
keep remaining 46 KB
     ↓
EPOLLOUT remains enabled
     ↓
wait for next writable event
     ↓
continue sending
```

Nothing is discarded.

---

# Chapter 214 — Final Self-Test: Slow Client

Explain:

> "Client A sends messages rapidly while Client B has a slow network."

Correct reasoning:

```text
A
 ↓
server
 ↓
B output buffer grows
 ↓
EPOLLOUT used when B can accept data
```

If:

```text
B output buffer > configured maximum
```

then the server applies its backpressure policy.

In this project:

```text
disconnect B
```

This protects server memory.

---

# Chapter 215 — Final Self-Test: 100,000 Connections

Suppose:

```text
100000 connected
50 readable
20 writable
```

With the event-driven architecture:

```text
epoll_wait()
      ↓
ready event list
      ↓
~70 current events to process
```

But remember:

```text
broadcasting
```

can still require visiting many connected clients.

Therefore:

```text
epoll reduces readiness-discovery overhead
```

but does not eliminate application work.

---

# Chapter 216 — The One-Sentence Mental Model

If you remember only one sentence from the entire project, remember:

> **The event loop waits for readiness, application state decides what the connection means, buffers preserve data that cannot be processed or transmitted immediately, and cleanup guarantees that every connection has a clear lifetime.**

That is the mental model behind the final server.

---

# PART XXXIII — PROJECT STATUS

## TCP Chat Server: COMPLETE

The project has now reached the point where it is reasonable to treat it as a completed systems-programming project.

The next work should be:

```text
Git cleanup
README
architecture diagram
benchmark/test results
GitHub push
LinkedIn post
resume bullet
```

Then move forward rather than endlessly modifying the same project.

---

# Chapter 217 — What to Put on GitHub

The repository should contain:

```text
TCP_Chat_Server/
├── src/
├── include/
├── CMakeLists.txt
├── README.md
├── .gitignore
└── LICENSE
```

If the current project is still organized differently, that is fine.

Do not reorganize merely for aesthetics if doing so risks breaking a working project.

---

# Chapter 218 — README Structure

A strong README should contain:

```text
# TCP Chat Server

## Overview

## Features

## Architecture

## Technologies

## Evolution

## Build

## Run

## Testing

## Design Decisions

## select vs poll vs epoll

## Backpressure

## Future Improvements
```

The most valuable section is:

```text
Design Decisions
```

because it demonstrates that you understand why the implementation looks the way it does.

---

# Chapter 219 — Resume Description

A concise resume version:

> Built a concurrent TCP chat server in C++ on Linux using POSIX sockets, evolving from blocking and thread-per-client designs to a non-blocking, edge-triggered `epoll` event loop with partial I/O handling, per-client output buffering, broadcast delivery, and backpressure protection.

This communicates both:

```text
what you built
```

and:

```text
what systems concepts you learned
```

---

# Chapter 220 — LinkedIn Description

A stronger project post can explain the journey:

```text
Built a TCP Chat Server from scratch in C++ on Linux.

Started with basic blocking sockets, then explored:

• thread-per-client
• mutex-based shared state
• select()
• poll()
• non-blocking sockets
• partial writes
• output buffering
• epoll()
• edge-triggered I/O
• backpressure

The main goal wasn't just to build a chat application.

It was to understand how network servers actually manage many connections and why high-concurrency servers use event-driven I/O.

Next: Redis clone.
```

This tells a much stronger story than:

```text
"I made a chat app."
```

---

# Chapter 221 — Final Project Philosophy

The most important achievement is not the final source file.

It is the progression:

```text
"I know how to call socket()."
```

to:

```text
"I understand how a Linux event-driven TCP server manages
connections, readiness, buffers, partial I/O, backpressure,
and lifecycle."
```

That is the systems-programming knowledge you can carry into future projects.

---

# END OF DEEP SYSTEMS REVIEW

## Current continuation point

The master book now reaches:

```text
PART XXXIII — PROJECT STATUS
Chapter 221
```

The next optional section is:

```text
PART XXXIV — Redis Clone From First Principles
```

which can begin the next major project using the networking concepts learned here.
