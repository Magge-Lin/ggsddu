#!/bin/bash
sudo apt-get install cmake
mkdir -p linux_build
rm -rf ./build
ln -s ./linux_build ./build
cd linux_build
cmake ..
make -j4
sudo make install
