#!/usr/bin/env python3
import time
from datetime import datetime

from rgbmatrix import RGBMatrix, RGBMatrixOptions
from PIL import Image, ImageDraw, ImageFont


def setup_matrix():
    options = RGBMatrixOptions()
    options.rows = 32
    options.cols = 64
    options.chain_length = 1
    options.parallel = 1
    options.hardware_mapping = "regular"

    # Important for Pi Zero 2 W
    options.gpio_slowdown = 3
    options.brightness = 60

    return RGBMatrix(options=options)


def main():
    matrix = setup_matrix()

    # Create an image buffer
    width = matrix.width
    height = matrix.height
    image = Image.new("RGB", (width, height))
    draw = ImageDraw.Draw(image)

    # Load a font (this one exists on Raspberry Pi OS)
    font = ImageFont.truetype(
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
        18
    )

    try:
        while True:
            now = datetime.now()
            timestr = now.strftime("%H:%M:%S")

            # Clear image
            draw.rectangle((0, 0, width, height), fill=(0, 0, 0))

            # Center the text
            text_width = draw.textlength(timestr, font=font)
            x = (width - text_width) // 2
            y = (height - 18) // 2

            # Draw time
            draw.text((x, y), timestr, fill=(255, 255, 255), font=font)

            # Push to matrix
            matrix.SetImage(image)

            time.sleep(0.2)

    except KeyboardInterrupt:
        matrix.Clear()


if __name__ == "__main__":
    main()
