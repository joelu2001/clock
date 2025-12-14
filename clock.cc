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

  Font font;
  Font small_font;
  Font big_font;
  if (!font.LoadFont("fonts/7x13.bdf")) return 2;
  if (!small_font.LoadFont("fonts/6x10.bdf")) return 3;
  if (!big_font.LoadFont("fonts/10x20.bdf")) return 4;

  const Color white(255, 255, 255);
  const Color black(0, 0, 0);

  while (running) {
    matrix->Clear();

    const int target_year = 2031;
    const int target_day = 258;
    const int target_hour = 0;

    std::time_t now = std::time(nullptr);
    std::time_t target = MakeTargetTime(target_year, target_day, target_hour);

    long days_left = DaysLeft(now, target);

    std::ostringstream oss;
    oss << days_left;
    const std::string days_text = oss.str();

    // Title
    rgb_matrix::DrawText(
        matrix,
        small_font,
        2, 10,
        white,
        nullptr,
        "Days left"
    );

    // Big number
    rgb_matrix::DrawText(
        matrix,
        big_font,
        2, 28,
        white,
        nullptr,
        days_text.c_str()
    );


    // y is baseline for the font. 7x13 fits well on 32px height.
    // const int x = 2;
    // const int y = 20;


    // rgb_matrix::DrawText(matrix, font, x, y, white, &black, t.c_str());

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  matrix->Clear();
  delete matrix;
  return 0;
}
