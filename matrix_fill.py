#!/usr/bin/env python3
import time
from rgbmatrix import RGBMatrix, RGBMatrixOptions

opts = RGBMatrixOptions()
opts.rows = 32
opts.cols = 64
opts.chain_length = 1
opts.parallel = 1
opts.hardware_mapping = "regular"
opts.gpio_slowdown = 3
opts.brightness = 60

m = RGBMatrix(options=opts)

try:
    while True:
        m.Fill(255, 0, 0)   # red
        time.sleep(2)
        m.Fill(0, 255, 0)   # green
        time.sleep(2)
        m.Fill(0, 0, 255)   # blue
        time.sleep(2)
except KeyboardInterrupt:
    m.Clear()
