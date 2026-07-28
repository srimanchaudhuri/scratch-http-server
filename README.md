# 🌐 scratch-http-server

> Building an HTTP server from scratch in C — one level at a time.

An educational project that progressively implements a fully-featured HTTP server using raw POSIX sockets, starting from the simplest possible TCP handshake and working up through parsing, routing, concurrency, and beyond.

---

## 📍 Roadmap

| Level | Focus | Status |
|-------|-------|--------|
| **1** | Raw TCP socket — client/server handshake | ✅ Done |
| 2 | HTTP request parsing (method, path, headers) | 🔜 Planned |
| 3 | HTTP response formatting (status line, headers, body) | 🔜 Planned |
| 4 | URL routing & basic GET handler | 🔜 Planned |
| 5 | Static file serving | 🔜 Planned |
| 6 | Basic concurrency (fork / threads) | 🔜 Planned |

---

## ✅ Level 1 — Raw TCP Socket

The foundation: a minimal TCP server and client communicating over `localhost:8080` using POSIX sockets. No HTTP yet — just raw bytes over a stream socket.

### What it does

- **Server** (`src/server.c`):
  - Creates a TCP socket with `socket()`
  - Binds to `0.0.0.0:8080` and listens for one connection
  - Accepts the connection, reads the client's message, and replies with `"Hello from server"`

- **Client** (`src/client.c`):
  - Creates a TCP socket and connects to `127.0.0.1:8080`
  - Sends `"Hello, From Client"` and prints the server's reply

### Concepts covered

- `socket()`, `bind()`, `listen()`, `accept()`, `connect()`
- `send()` / `read()` over a stream socket
- `struct sockaddr_in` and address families (`AF_INET`)
- `inet_pton()` for address conversion
- `htons()` for byte-order conversion

### Demo

```
# Terminal 1
$ bin/server
Hello, From Client       ← message received from client
Hello message sent

# Terminal 2
$ bin/client
Hello sent               ← message sent to server
Hello from server        ← reply received
```

---

## 🛠️ Building

Requires `gcc` and `make`.

```bash
make          # builds bin/server and bin/client
make clean    # removes the bin/ directory
```

Binaries are placed in the `bin/` directory (git-ignored).

---

## 📁 Structure

```
scratch-http-server/
├── src/
│   ├── server.c      # TCP server
│   └── client.c      # TCP client
├── Makefile
└── README.md
```

---

## 📖 Goal

Each level builds on the last. By the end, this will be a working HTTP/1.1 server capable of serving static files with concurrent connections — written entirely from scratch in C with no external libraries.
