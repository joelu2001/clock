#!/usr/bin/env bash

#run this once: chmod +x update.sh

git pull
cd ..
scp clock/clock.cc rpi-rgb-led-matrix/
cd rpi-rgb-led-matrix/
g++ -O2 -std=c++17 clock.cc -o clock -Iinclude -Llib -lrgbmatrix -lpthread
./clock -D0 --led-no-hardware-pulse --led-cols=64 --led-rows=32
