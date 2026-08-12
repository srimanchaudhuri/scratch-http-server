# 🌐 scratch-http-server

> Building an HTTP server from scratch in C — one level at a time.

A fully-featured HTTP server built from the ground up using raw POSIX sockets — no frameworks, no libraries. Progresses through 6 levels: TCP sockets → HTTP formatting → request parsing → URL routing → static file serving → multi-threaded concurrency.

---

## 📍 Roadmap

| Level | Focus | Status |
|-------|-------|--------|
| **1** | Raw TCP socket — client/server handshake | ✅ Done |
| **2** | HTTP response formatting (status line, headers, body) | ✅ Done |
| **3** | HTTP request parsing (method, path, headers) | ✅ Done |
| **4** | URL routing & dynamic GET handler | ✅ Done |
| **5** | Static file serving | ✅ Done |
| **6** | Multi-threaded concurrency (pthreads) | ✅ Done |

---

## ✅ Level 6 — Multi-Threaded Concurrency

The server now handles **multiple clients simultaneously** using POSIX threads. Each incoming connection spawns a dedicated thread, so one slow request no longer blocks every other client.

### What changed

- **New thread function** — `send_response(void *arg)`:
  - Receives a heap-allocated socket fd as its argument
  - Calls `pthread_detach(pthread_self())` so resources are freed automatically on exit
  - Handles the full lifecycle: read request → parse route → serve response → close & free
  - Added NULL checks for template file reads with proper cleanup on failure

- **Main loop** (`main()`):
  - Allocates the client socket on the heap (`malloc(sizeof(int))`) so each thread owns its own copy
  - Spawns a new thread per connection with `pthread_create`
  - Main thread immediately loops back to `accept` the next connection
  - Moved `buffer`, `valread`, `type`, and `allocated_size` out of `main` and into the thread function

### Concepts covered

- `pthread_create` for spawning threads
- `pthread_detach` for fire-and-forget thread management
- Heap-allocating arguments to avoid data races between threads
- Thread-local stack variables (each thread gets its own `buffer`, `route`, etc.)
- Resource cleanup in threaded code — every path frees the socket and allocated memory

### Demo

```
# Terminal — start the server
$ bin/server

# Open multiple browser tabs simultaneously:
# http://localhost:8080/       → landing page
# http://localhost:8080/alice  → "Hello alice"
# http://localhost:8080/bob    → "Hello bob"
# All served concurrently — no blocking!

# Server stdout (interleaved):
/
Entered default route
/alice
Entered alice route
/bob
Entered bob route
```

---

## ✅ Level 5 — Static File Serving

<details>
<summary>Click to expand</summary>

The server now reads real HTML files from disk instead of hardcoding HTML strings in C. The `/` route serves `index.html` — a fully styled landing page for the project.

### What changed

- **New function** — `read_file(filepath)`:
  - Opens any file with `fopen` in binary mode
  - Measures file size with `fseek` / `ftell`, then `rewind`s
  - Allocates a buffer with `malloc(file_size + 1)` and reads the entire file with `fread`
  - Null-terminates the string and returns it (caller frees)

- **Template engine** — `str_replace(source, placeholder, replacement)`:
  - Replaces all `{{NAME}}` placeholders in `greeting.html` with the route name

### Concepts covered

- File I/O with `fopen`, `fseek`, `ftell`, `rewind`, `fread`, `fclose`
- Binary vs text mode (`"rb"`)
- Measuring file size before allocating memory
- Separating content (HTML) from code (C)

</details>

---

## ✅ Level 4 — URL Routing & Dynamic GET Handler

<details>
<summary>Click to expand</summary>

The server now routes requests based on the URL path and generates **dynamic HTML responses**. Visit `/sriman` and the page greets you by name.

### What changed

- **Route dispatch** — `strcmp` on the parsed path:
  - `/` → responds with a static `"Hello World"` page
  - `/<name>` → responds with `"Hello <name>"` (dynamic body built at runtime)

- **Persistent server** — `while(1)` event loop:
  - Accepts connections in a loop instead of exiting after one request
  - Properly `free()`s every allocated response buffer before closing each connection

- **Port reuse** — enabled `setsockopt(SO_REUSEADDR)` so the server can restart immediately without `Address already in use` errors

### Concepts covered

- Basic URL routing with `strcmp`
- Dynamic HTML generation with `snprintf` + `malloc`
- Iterative server event loop (`while(1)` + `accept`)
- `SO_REUSEADDR` socket option for fast restarts
- Heap hygiene — `free()` after every `malloc`

</details>

---

## ✅ Level 3 — HTTP Request Parsing

<details>
<summary>Click to expand</summary>

The server now reads and parses the incoming HTTP request line to extract the URL path, rather than treating the request as an opaque blob.

### What changed

- **Request-line parsing** with `sscanf`:
  - Extracts the full path (e.g. `/cat`) from `GET /cat HTTP/1.1`
  - Also extracts the path segment without the leading `/` for use in dynamic content

### Concepts covered

- HTTP request-line format: `METHOD PATH VERSION`
- `sscanf` with `%*s` (skip) and `%49s` (bounded capture)
- Separating raw path (`/cat`) from the name segment (`cat`)

</details>

---

## ✅ Level 2 — HTTP Response Formatting

<details>
<summary>Click to expand</summary>

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

</details>

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
│   ├── server.c      # HTTP server (routing, parsing, file serving)
│   └── client.c      # TCP client
├── index.html            # Landing page served at /
├── Makefile
└── README.md
```

---

## 📖 Goal

Each level builds on the last. By the end, this will be a working HTTP/1.1 server capable of serving static files with concurrent connections — written entirely from scratch in C with no external libraries.
