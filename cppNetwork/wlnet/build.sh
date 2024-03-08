#!/bin/bash
mkdir -p build_src
cd build_src
cmake ..
make -j4
sudo make install
