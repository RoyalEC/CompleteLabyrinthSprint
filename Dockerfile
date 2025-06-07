# Use Ubuntu 22.04 base image
FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libboost-all-dev \
    libssl-dev \
    libwebsocketpp-dev \
    libasio-dev \
    nlohmann-json3-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source code
COPY backend/ ./backend

# Build the project
RUN cmake -S backend -B build && cmake --build build

# Expose the game server port
EXPOSE 9002

# Run the server
CMD ["./build/LabyrinthSprint"]


