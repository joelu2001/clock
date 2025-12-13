#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <thread>

#include "led-matrix.h"
#include "graphics.h"

using rgb_matrix::RGBMatrix;
using rgb_matrix::Font;
using rgb_matrix::Color;

static std::string NowHHMMSS() {
  std::time_t t = std::time(nullptr);
  std::tm tm = *std::localtime(&t);
  std::ostringstream oss;
  oss << std::setfill('0')
      << std::setw(2) << tm.tm_hour << ":"
      << std::setw(2) << tm.tm_min  << ":"
      << std::setw(2) << tm.tm_sec;
  return oss.str();
}

int main(int argc, char *argv[]) {
  RGBMatrix::Options options;
  rgb_matrix::RuntimeOptions runtime;

  // Match your known-good panel setup
  options.rows = 32;
  options.cols = 64;
  options.chain_length = 1;
  options.parallel = 1;
  options.brightness = 60;

  // This matches your working "--led-no-hardware-pulse"
  runtime.disable_hardware_pulsing = true;

  // Create matrix
  RGBMatrix *matrix = RGBMatrix::CreateFromOptions(options, runtime);
  if (!matrix) return 1;

  Font font;
  // Use a font bundled with the repo
  if (!font.LoadFont("fonts/7x13.bdf")) return 2;

  Color white(255, 255, 255);
  Color black(0, 0, 0);

  while (true) {
    matrix->Clear();

    std::string t = NowHHMMSS();

    // Draw at a nice spot for 64x32 with 7x13 font
    int x = 2;
    int y = 20;  // baseline

    rgb_matrix::DrawText(matrix, font, x, y, white, &black, t.c_str());

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  delete matrix;
  return 0;
}
