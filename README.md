# SlimLog

A small modern c++ logger

---

## Features

- **this is a place holder** — logs

---

## Build & Install

This project uses a Makefile that delegates to CMake.

### Build environment

Builds should be performed either:

- on the native target platform, or  
- inside a fully functional container matching the target system (e.g. Debian for `.deb`, Fedora/RHEL for `.rpm`)

## Docker Build Environment

A Dockerfile can be used to provide a consistent build environment for generating both `.deb` and `.rpm` packages.

### Example Dockerfile

```dockerfile
FROM ubuntu:26.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    rpm \
    git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
COPY . .

```

Build container
```
docker build -t packager:latest .
```

Build packages
```
docker run -it --rm -v $PWD:/dist packager:latest bash -c "cd /workspace && make packages LOCAL_SRC=OFF DIST_DIR=/dist"
```

### Build

``` bash
make build
```

### Install

``` bash
make install
```

Default install prefix: - /usr/local

Custom prefix:

``` bash
make install INSTALL_PREFIX=/your/path
```
