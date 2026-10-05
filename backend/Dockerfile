FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

WORKDIR /backend

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

COPY .CMakeLists.txt .
COPY .env .

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release

COPY include ./include
COPY src ./src
COPY scripts ./scripts

RUN cmake --build build --config Release -j$(nproc)

FROM debian:bookworm-slim
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    ca-certificates \
    libstdc++6 \
    && rm -rf var/lib/apt/lists/*

COPY --from=builder /backend/build/backend .
COPY --from=builder /backend/scripts ./scripts
COPY gtfs ./gtfs

EXPOSE 18080

CMD ["./backend"]
