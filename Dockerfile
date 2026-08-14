FROM ubuntu:rolling AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build

COPY ./src/server.c ./src/server.c

RUN gcc -o server ./src/server.c -lpthread

FROM ubuntu:rolling

WORKDIR /app

COPY --from=builder /build/server /app/server

COPY ./index.html /index.html
COPY ./greeting.html /greeting.html

CMD ["/app/server"]