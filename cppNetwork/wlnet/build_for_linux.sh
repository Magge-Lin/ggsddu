#!/bin/bash
sudo apt-get install cmake
mkdir -p build
cd build
cmake ..
make -j4
sudo make install
