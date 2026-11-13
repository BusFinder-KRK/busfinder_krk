FROM ubuntu:26.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

WORKDIR /backend

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    ca-certificates \
    python3 \
    libssl-dev \
    libpq-dev \
    && rm -rf /var/lib/apt/lists/*

COPY CMakeLists.txt .
COPY include ./include
COPY src ./src

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build -j$(nproc)

FROM ubuntu:26.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates libstdc++6 libpq5 tzdata \
    && rm -rf var/lib/apt/lists/*
WORKDIR /backend
COPY --from=builder /backend/build/backend .

EXPOSE 18080

CMD ["./backend"]
