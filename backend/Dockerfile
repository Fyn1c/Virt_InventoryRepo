FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libboost-dev \
    nlohmann-json3-dev \
    libpq-dev \
    libpqxx-dev \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . ./


RUN rm -rf build && \
    cmake -B build -S . && \
    cmake --build build 

FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive


RUN apt-get update && apt-get install -y \
    libpq5 \
    libssl3 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app


COPY --from=builder /app/build/backend-service ./backend-service

ENV PORT=8080
EXPOSE 8080

CMD ["./backend-service"]