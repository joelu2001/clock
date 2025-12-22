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

static std::string NowYYYYDDDHH() {
  std::time_t t = std::time(nullptr);
  std::tm tm = *std::localtime(&t);

  std::ostringstream oss;
  oss << std::setfill('0')
      << std::setw(4) << (tm.tm_year + 1900) << ":"
      << std::setw(3) << (tm.tm_yday + 1) << ":"   // make it 1–366
      << std::setw(2) << tm.tm_hour;

  return oss.str();
}

static std::time_t MakeTargetTime(
    int year,
    int day_of_year,   // 1–366
    int hour
) {
  std::tm tm{};
  tm.tm_year = year - 1900;
  tm.tm_mon  = 0;      // January
  tm.tm_mday = 1;      // Jan 1
  tm.tm_hour = hour;
  tm.tm_min  = 0;
  tm.tm_sec  = 0;

  std::time_t t = std::mktime(&tm);

  // Add (day_of_year - 1) days
  t += (day_of_year - 1) * 24 * 3600;

  return t;
}

static long DaysLeft(std::time_t now, std::time_t target) {
  if (target <= now)
    return 0;

  long diff_seconds = static_cast<long>(difftime(target, now));

  return diff_seconds / (24 * 3600);
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
  rgb_matrix::FrameCanvas *offscreen = matrix->CreateFrameCanvas();

  Font font;
  Font small_font;
  Font big_font;
  if (!font.LoadFont("fonts/7x13.bdf")) return 2;
  if (!small_font.LoadFont("fonts/6x10.bdf")) return 3;
  if (!big_font.LoadFont("fonts/10x20.bdf")) return 4;

  const Color white(255, 255, 255);
  const Color black(0, 0, 0);
  const Color red(255, 0, 0);
  const Color green(0, 255, 0);
  const Color blue(0, 0, 255);

  bool state = false;
  auto last_switch = std::chrono::steady_clock::now();

  while (running) {
    auto now = std::chrono::steady_clock::now();

    if (now - last_switch >= std::chrono::seconds(1)) {
      state = !state;
      last_switch = now;
    }

    offscreen->Clear();

    if (state) {
      FillRect(offscreen, 0, 0, 63, 31, Color(255, 0, 0));
    } else {
      FillRect(offscreen, 0, 0, 63, 31, Color(0, 0, 255));
    }

    offscreen = matrix->SwapOnVSync(offscreen);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }


  offscreen->Clear();
  delete matrix;
  return 0;
}

void FillRect(rgb_matrix::FrameCanvas* c,
              int x, int y, int w, int h,
              const Color& color) {
  for (int yy = y; yy < y + h; ++yy) {
    for (int xx = x; xx < x + w; ++xx) {
      c->SetPixel(xx, yy, color);
    }
  }
}