#!/usr/bin/env bash

#run this once: chmod +x update.sh

cd /home/joel/clock
git pull
scp /home/joel/clock/clock.cc /home/joel/rpi-rgb-led-matrix/
cd /home/joel/rpi-rgb-led-matrix/
g++ -O2 -std=c++17 clock.cc -o clock -Iinclude -Llib -lrgbmatrix -lpthread
/home/joel/rpi-rgb-led-matrix/clock -D0 --led-rgb-sequence=RGB --led-no-hardware-pulse --led-cols=64 --led-ro>
