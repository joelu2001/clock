#include <chrono>
#include <csignal>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <thread>

#include "led-matrix.h"
#include "graphics.h"

using rgb_matrix::RGBMatrix;
using rgb_matrix::Font;
using rgb_matrix::Color;

static volatile bool running = true;
static void InterruptHandler(int) { running = false; }

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
  std::signal(SIGINT, InterruptHandler);
  std::signal(SIGTERM, InterruptHandler);

  RGBMatrix::Options options;
  rgb_matrix::RuntimeOptions runtime;

  // This makes your program understand -D0, --led-no-hardware-pulse, etc.
  // It also prints helpful usage on invalid flags.
  RGBMatrix *matrix = RGBMatrix::CreateFromFlags(&argc, &argv, &options, &runtime);
  if (matrix == nullptr) return 1;

  Font font;
  if (!font.LoadFont("fonts/7x13.bdf")) return 2;

  const Color white(255, 255, 255);
  const Color black(0, 0, 0);

  while (running) {
    matrix->Clear();

    const std::string t = NowHHMMSS();

    // y is baseline for the font. 7x13 fits well on 32px height.
    const int x = 2;
    const int y = 20;

    rgb_matrix::DrawText(matrix, font, x, y, white, &black, t.c_str());

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  matrix->Clear();
  delete matrix;
  return 0;
}
