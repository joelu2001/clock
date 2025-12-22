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

void Line(rgb_matrix::FrameCanvas* c,
              int x1, int y1, int x2, int y2, bool alt, bool vert,
              const Color& color_a, const Color& color_b) {
  if (alt) {
    bool next = true;
    if (vert) {
      for (int yy = y1; yy < y2; ++yy) {
        if (next) {
          c->SetPixel(x1, yy, color_a.r, color_a.g, color_a.b);
          next = false;
        } else {
          c->SetPixel(x1, yy, color_b.r, color_b.g, color_b.b);
          next = true;
        }
      }
    } else {
      for (int xx = x1; xx < x2; ++xx) {
        if (next) {
          c->SetPixel(xx, y1, color_a.r, color_a.g, color_a.b);
          next = false;
        } else {
          c->SetPixel(xx, y1, color_b.r, color_b.g, color_b.b);
          next = true;
        }
      }
    }
  } else {
    if (vert) {
      for (int yy = y1; yy < y2; ++yy) {
        c->SetPixel(x1, yy, color_a.r, color_a.g, color_a.b);
      }
    } else {
      for (int xx = x1; xx < x2; ++xx) {
        c->SetPixel(xx, y1, color_a.r, color_a.g, color_a.b);
      }
    }
  }
}

void FillRect(rgb_matrix::FrameCanvas* c,
              const Color& color_a, const Color& color_b) {
  Line(c, 0, 0, 64, 0, true, false, color_a, color_b); // top
  Line(c, 0, 1, 0, 31, true, true, color_b, color_a); // left
  Line(c, 0, 31, 63, 31, true, false, color_b, color_a); // bottom
  Line(c, 63, 1, 63, 32, true, true, color_a, color_b); // right
}

void setPixelHelper(rgb_matrix::FrameCanvas* c, const Color& color, int x, int y, int xx, int yy) {
  c->SetPixel(x+xx, y-yy, color.r, color.g, color.b);
}

void SnowMan(rgb_matrix::FrameCanvas* c, int x, int y, int ver,
              const Color& white, const Color& grey, const Color& brown, const Color& red, const Color& orange) {
      setPixelHelper(c, grey, x, y, 4, 0);
      setPixelHelper(c, white, x, y, 5, 0);
      setPixelHelper(c, white, x, y, 6, 0);
      setPixelHelper(c, grey, x, y, 7, 0);
      setPixelHelper(c, grey, x, y, 3, 1);
      setPixelHelper(c, white, x, y, 4, 1);
      setPixelHelper(c, white, x, y, 5, 1);
      setPixelHelper(c, white, x, y, 7, 1);
      setPixelHelper(c, white, x, y, 8, 1);
      setPixelHelper(c, white, x, y, 3, 2);
      setPixelHelper(c, white, x, y, 3, 2);
      setPixelHelper(c, white, x, y, 4, 2);
      setPixelHelper(c, white, x, y, 5, 2);
      setPixelHelper(c, white, x, y, 6, 2);
      setPixelHelper(c, white, x, y, 7, 2);
      setPixelHelper(c, white, x, y, 8, 2);
      setPixelHelper(c, white, x, y, 4, 3);
      setPixelHelper(c, white, x, y, 5, 3);
      setPixelHelper(c, white, x, y, 7, 3);
      setPixelHelper(c, brown, x, y, 8, 3);
      setPixelHelper(c, red, x, y, 3, 4);
      setPixelHelper(c, red, x, y, 4, 4);
      setPixelHelper(c, red, x, y, 5, 4);
      setPixelHelper(c, red, x, y, 6, 4);
      setPixelHelper(c, red, x, y, 7, 4);
      setPixelHelper(c, white, x, y, 4, 5);
      setPixelHelper(c, white, x, y, 5, 5);
      setPixelHelper(c, white, x, y, 6, 5);
      setPixelHelper(c, white, x, y, 7, 5);
      setPixelHelper(c, white, x, y, 4, 6);
      setPixelHelper(c, white, x, y, 5, 6);
      setPixelHelper(c, orange, x, y, 6, 6);
      setPixelHelper(c, orange, x, y, 7, 6);
      setPixelHelper(c, orange, x, y, 8, 6);
      setPixelHelper(c, white, x, y, 4, 7);
      setPixelHelper(c, white, x, y, 6, 7);
      setPixelHelper(c, white, x, y, 8, 7);
      setPixelHelper(c, white, x, y, 5, 8);
      setPixelHelper(c, white, x, y, 6, 8);
      setPixelHelper(c, white, x, y, 7, 8);
      setPixelHelper(c, red, x, y, 6, 10);
    if (ver == 0) {
      setPixelHelper(c, brown, x, y, 1, 1);
      setPixelHelper(c, brown, x, y, 2, 2);
      setPixelHelper(c, brown, x, y, 10, 1);
      setPixelHelper(c, brown, x, y, 9, 2);
      setPixelHelper(c, brown, x, y, 8, 3);
      setPixelHelper(c, red, x, y, 2, 3);
      setPixelHelper(c, red, x, y, 3, 3);
      setPixelHelper(c, red, x, y, 2, 5);
      setPixelHelper(c, red, x, y, 3, 9);
      setPixelHelper(c, red, x, y, 4, 10);
    } else {
      setPixelHelper(c, brown, x, y, 3, 3);
      setPixelHelper(c, brown, x, y, 2, 4);
      setPixelHelper(c, brown, x, y, 1, 5);
      setPixelHelper(c, brown, x, y, 0, 6);
      setPixelHelper(c, brown, x, y, 9, 4);
      setPixelHelper(c, brown, x, y, 10, 5);
      setPixelHelper(c, brown, x, y, 11, 6);
      setPixelHelper(c, red, x, y, 3, 5);
      setPixelHelper(c, red, x, y, 2, 5);
      setPixelHelper(c, red, x, y, 2, 6);
      setPixelHelper(c, red, x, y, 1, 7);
      setPixelHelper(c, red, x, y, 6, 10);
      setPixelHelper(c, red, x, y, 7, 10);
    }
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
  const Color orange(255, 165, 0);
  const Color grey(128, 128, 128);
  const Color brown(139, 69, 19);

  const int x_text = 5;
  const int y_text = 20;

  bool state = false;
  auto last_switch = std::chrono::steady_clock::now();

  while (running) {
    auto now = std::chrono::steady_clock::now();

    if (now - last_switch >= std::chrono::seconds(1)) {
      state = !state;
      last_switch = now;
    }

    offscreen->Clear();
    offscreen->SetBrightness(30);

    offscreen->SetPixel(10, 10, green.r, green.g, green.b);

    if (state) {
      FillRect(offscreen, red, white);
      SnowMan(offscreen, 31, 30, 0, white, grey, brown, red, orange);
      //rgb_matrix::DrawText(offscreen, font, x_text, y_text, white, nullptr, "God Jul!");
    } else {
      FillRect(offscreen, white, red);
      SnowMan(offscreen, 31, 25, 1, white, grey, brown, red, orange);
      //rgb_matrix::DrawText(offscreen, font, x_text, y_text, red, nullptr, "God Jul!");
    }

    offscreen = matrix->SwapOnVSync(offscreen);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }


  offscreen->Clear();
  delete matrix;
  return 0;
}