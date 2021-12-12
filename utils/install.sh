#!/bin/sh

# Install build tools
apt install build-essential make flex bison -y

# Install CMAKE
if ! [ -x "$(command -v cmake)" ]; then
    wget https://github.com/Kitware/CMake/releases/download/v3.22.1/cmake-3.22.1.tar.gz
    tar -zxvf cmake-3.22.1.tar.gz
    cd cmake-3.22.1
    ./bootstrap -- -DCMAKE_USE_OPENSSL=OFF
    make
    make install
    rm -rf cmake-3.22.1/
    rm cmake-3.22.1.tar.gz
fi
