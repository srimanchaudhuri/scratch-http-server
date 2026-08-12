# 🌐 scratch-http-server

> Building an HTTP server from scratch in C — one level at a time.

An educational project that progressively implements a fully-featured HTTP server using raw POSIX sockets, starting from the simplest possible TCP handshake and working up through parsing, routing, concurrency, and beyond.

---

## 📍 Roadmap

| Level | Focus | Status |
|-------|-------|--------|
| **1** | Raw TCP socket — client/server handshake | ✅ Done |
| **2** | HTTP response formatting (status line, headers, body) | ✅ Done |
| **3** | HTTP request parsing (method, path, headers) | ✅ Done |
| **4** | URL routing & dynamic GET handler | ✅ Done |
| **5** | Static file serving | ✅ Done |
| 6 | Basic concurrency (fork / threads) | 🔜 Planned |

---

## ✅ Level 5 — Static File Serving

The server now reads real HTML files from disk instead of hardcoding HTML strings in C. The `/` route serves `index.html` — a fully styled landing page for the project.

### What changed

- **New function** — `get_http_content()`:
  - Opens `../index.html` with `fopen` in binary mode
  - Measures file size with `fseek` / `ftell`, then `rewind`s
  - Allocates a buffer with `malloc(file_size + 1)` and reads the entire file with `fread`
  - Null-terminates the string and returns it (caller frees)

- **Server** (`src/server.c`):
  - The `/` route now calls `get_http_content()` instead of using a hardcoded string
  - Dynamic routes (`/<name>`) still generate HTML at runtime

- **Landing page** (`index.html`):
  - Self-contained HTML + CSS with a dark minimalistic design
  - Sections: hero, roadmap, features grid, architecture diagram, quick start
  - No external dependencies — served directly by the C server

### Concepts covered

- File I/O with `fopen`, `fseek`, `ftell`, `rewind`, `fread`, `fclose`
- Binary vs text mode (`"rb"`)
- Measuring file size before allocating memory
- Null-terminating file content for use as a C string
- Separating content (HTML) from code (C) — the first step toward a real web server

### Demo

```
# Terminal — start the server
$ bin/server

# Browser: http://localhost:8080/
# → serves the styled index.html landing page from disk

# Browser: http://localhost:8080/sriman
# → still returns dynamic "Hello sriman" HTML

# Server stdout:
/
Entered default route
/sriman
Entered sriman route
```

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
