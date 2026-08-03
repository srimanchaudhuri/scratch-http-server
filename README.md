# 🌐 scratch-http-server

> Building an HTTP server from scratch in C — one level at a time.

An educational project that progressively implements a fully-featured HTTP server using raw POSIX sockets, starting from the simplest possible TCP handshake and working up through parsing, routing, concurrency, and beyond.

---

## 📍 Roadmap

| Level | Focus | Status |
|-------|-------|--------|
| **1** | Raw TCP socket — client/server handshake | ✅ Done |
| **2** | HTTP response formatting (status line, headers, body) | ✅ Done |
| 3 | HTTP request parsing (method, path, headers) | 🔜 Planned |
| 4 | URL routing & basic GET handler | 🔜 Planned |
| 5 | Static file serving | 🔜 Planned |
| 6 | Basic concurrency (fork / threads) | 🔜 Planned |

---

## ✅ Level 2 — HTTP Response Formatting

The server now speaks HTTP. Instead of sending a raw string, it constructs a proper **HTTP/1.1 response** with status line, headers, and an HTML body — making it viewable in a real browser.

### What changed

- **New function** — `create_http_response(body, content_type, &out_size)`:
  - Dynamically builds a complete HTTP/1.1 response string
  - Sets `Content-Type` and `Content-Length` headers automatically
  - Uses `snprintf` for safe, measured string formatting
  - Returns a `malloc`'d buffer with the total size via an out-parameter

- **Server** (`src/server.c`):
  - Accepts a connection and reads the raw request (printing all headers to stdout)
  - Responds with a well-formed HTTP response containing `<h1>Hello World</h1>`
  - Browsers hitting `http://localhost:8080` now render the HTML page

### Concepts covered

- HTTP/1.1 response structure: `Status-Line \r\n Headers \r\n\r\n Body`
- `Content-Type` and `Content-Length` headers
- Dynamic string formatting with `snprintf(NULL, 0, ...)` to measure first, then write
- Heap allocation with `malloc` for variable-length responses

### Demo

```
# Terminal — start the server
$ bin/server

# Open http://localhost:8080 in a browser → renders "Hello World"
# Server stdout shows the full incoming request:
GET / HTTP/1.1
Host: localhost:8080
Connection: keep-alive
User-Agent: Mozilla/5.0 ...
Accept: text/html,application/xhtml+xml,...
...

HTTP message sent
```

---

## ✅ Level 1 — Raw TCP Socket

<details>
<summary>Click to expand</summary>

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

</details>

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
│   ├── server.c      # HTTP server (TCP + response formatting)
│   └── client.c      # TCP client
├── Makefile
└── README.md
```

---

## 📖 Goal

Each level builds on the last. By the end, this will be a working HTTP/1.1 server capable of serving static files with concurrent connections — written entirely from scratch in C with no external libraries.
