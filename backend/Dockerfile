FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

WORKDIR /backend

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    ca-certificates \
    libssl-dev \
    libpq-dev \
    && rm -rf /var/lib/apt/lists/*

#delteted scrips here
COPY CMakeLists.txt .
COPY include ./include
COPY src ./src

#everything is just one run now
RUN cmake -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build -j$(nproc)

#chnaged the base image so its for sure safe
FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive

#added the libpq5 for postgres
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates libstdc++6 libpq5 \
    && rm -rf var/lib/apt/lists/*

COPY --from=builder /backend/build/backend .
#i dont have the scrips and gtfs here anymore
EXPOSE 18080

CMD ["./backend"]
