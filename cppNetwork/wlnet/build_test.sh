#!/bin/bash
mkdir -p build_test
cd build_test
cmake ..
make -j4
sudo make install
