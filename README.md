# Multithreaded TCP Chat Server

A multithreaded TCP chat server built from scratch in **C++ on Linux using POSIX sockets**.

This version of the project explores how a server can handle multiple clients concurrently using a **thread-per-client architecture**.

---

## Overview

The initial TCP server could communicate with a client, but handling multiple clients required a different architecture.

The server was redesigned so that each connected client is handled by its own thread:

```text
                    TCP Server
                        │
                    accept()
                        │
              ┌─────────┼─────────┐
              ↓         ↓         ↓
          Client A   Client B   Client C
              │         │         │
          Thread A   Thread B   Thread C
