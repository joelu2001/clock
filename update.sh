#!/usr/bin/env bash

#run this once: chmod +x update.sh

cd clock
git pull
cd ..
scp clock/clock.cc rpi-rgb-led-matrix/
cd rpi-rgb-led-matrix/
g++ -O2 -std=c++17 clock.cc -o clock -Iinclude -Llib -lrgbmatrix -lpthread
sudo reboot
