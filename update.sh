#!/usr/bin/env bash

#run this once: chmod +x update.sh

for i in {1..4}; do
    if ping -c 1 -W 2 8.8.8.8 &> /dev/null; then
        cd /home/joel/clock
        
        if git pull; then
            scp /home/joel/clock/clock.cc /home/joel/rpi-rgb-led-matrix/
            cd /home/joel/rpi-rgb-led-matrix/
            g++ -O2 -std=c++17 clock.cc -o clock -Iinclude -Llib -lrgbmatrix -lpthread
        fi
        break
    fi
    sleep 5
done
