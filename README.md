# TCP Chat Server

A TCP chat server built from scratch in **C++ on Linux using POSIX sockets**.

This project started as a basic TCP server and evolved into an **event-driven, non-blocking server using Linux `epoll`**. The main purpose is to understand how network servers work internally rather than simply building a chat application.

---

## 🚀 Project Evolution

The server was built incrementally:

```text
Basic TCP Server
      ↓
Multiple Clients
      ↓
Thread-per-Client
      ↓
Shared State + Mutex
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
Edge-Triggered epoll
      ↓
EPOLLOUT
      ↓
Backpressure
      ↓
Event-Driven Server