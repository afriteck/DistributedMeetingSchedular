#!/bin/bash
set -x

git submodule update --init --force

# Install required dependencies for libical
sudo apt-get install cmake perl

# Build libical
rm -rf libical/build
mkdir libical/build
cd libical/build
cmake ..
make
sudo make install
sudo ldconfig
