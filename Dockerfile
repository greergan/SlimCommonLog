FROM ubuntu:26.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    rpm \
    git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
COPY CMakeLists.txt .
COPY Makefile .