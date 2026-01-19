#include <chrono>
#include <csignal>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <thread>
#include <iomanip>

#include "led-matrix.h"
#include "graphics.h"

using rgb_matrix::RGBMatrix;
using rgb_matrix::Font;
using rgb_matrix::Color;

static volatile bool running = true;
static volatile bool birthday = false;
static void InterruptHandler(int) { running = false; }

static std::string NowHHMMSS() {
  std::time_t t = std::time(nullptr);
  std::tm tm = *std::localtime(&t);

  std::ostringstream oss;
  oss << std::setfill('0')
      << std::setw(2) << tm.tm_hour << ":"
      << std::setw(2) << tm.tm_min << ":"
      << std::setw(2) << tm.tm_sec;

  return oss.str();
}

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

static bool IsSpecificDate(int target_month, int target_day) {
  std::time_t t = std::time(nullptr);
  std::tm tm = *std::localtime(&t);
  
  int current_month = tm.tm_mon + 1; 
  int current_day = tm.tm_mday;      
  
  return (current_month == target_month && current_day == target_day);
}

static double CalculateProgress(std::time_t start, std::time_t now, std::time_t target) {
  if (now <= start) return 0.0;
  if (now >= target) return 1.0;
  
  double total_duration = difftime(target, start);
  double elapsed = difftime(now, start);
  
  return elapsed / total_duration;
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
  Font biggest_font;
  if (!font.LoadFont("fonts/7x13.bdf")) return 2;
  if (!small_font.LoadFont("fonts/6x10.bdf")) return 3;
  if (!big_font.LoadFont("fonts/10x20.bdf")) return 4;
  if (!biggest_font.LoadFont("fonts/texgyre-27.bdf")) return 5;

  const Color white (255, 255, 255);
  const Color black (0,   0,   0); 
  const Color red   (255, 0,   0); 
  const Color green (0,   0,   255);  
  const Color blue  (0,   255, 0);    
  const Color orange(255, 0,   165);
  const Color grey  (128, 128, 128);
  const Color brown (139, 19,  69);
 
  while (running) {
    offscreen->Clear();
    offscreen->SetBrightness(70);

    const int start_year = 2025;
    const int start_day = 258; 
    const int start_hour = 0;

    const int target_year = 2030;
    const int target_day = 258;
    const int target_hour = 0;
    
    const int target_year = 2025;
    const int target_day = 19;
    const int target_hour = 0;

    std::time_t now = std::time(nullptr);
    std::time_t target = MakeTargetTime(target_year, target_day, target_hour);
    
    std::time_t start = MakeTargetTime(start_year, start_day, start_hour);
    double progress = CalculateProgress(start, now, target);

    long days_left = DaysLeft(now, target);

    std::ostringstream oss;
    oss << std::setw(4) << std::setfill('0') << days_left;
    const std::string days_text = oss.str();

    //Code for centering text
    int text_width = 0;
    for (char c : days_text) {
        text_width += biggest_font.CharacterWidth(c);
    }
    int canvas_width = offscreen->width();

    const int bar_height = 2;           
    const int bar_y = offscreen->height() - bar_height - 1;  
    const int bar_width = offscreen->width(); 

    // Draw filled portion with gradient
    int filled_width = static_cast<int>(progress * bar_width);
    for (int y = bar_y; y < bar_y + bar_height; y++) {
        for (int x = 0; x < filled_width; x++) {
            float local_progress = (float)x / (float)bar_width;
            int r = 255 * local_progress;
            int g = 0;
            int b = 255 * (1 - local_progress);
            offscreen->SetPixel(x, y, r, g, b);
        }
    }

    const int x_text = 6;
    int x_days = (canvas_width - text_width) / 2;
    const int y_days = 26;

    std::string birthday_text = "Glad Födelsedag!";

    int birthday_width = 0;
    for (char c : birthday_text) {
        birthday_width += big_font.CharacterWidth(c);
    }
    int x_birthday = (canvas_width - birthday_width) / 2;
    const int y_birthday = 20;

    birthday = IsSpecificDate(9, 9);
    
    if (birthday) {
        std::string line1 = "Glad";
        std::string line2 = "Födelsedag!";
        
        int line1_width = 0;
        for (char c : line1) {
            line1_width += font.CharacterWidth(c);
        }
        int x_line1 = (canvas_width - line1_width) / 2;
        
        int line2_width = 0;
        for (char c : line2) {
            line2_width += font.CharacterWidth(c);
        }
        int x_line2 = (canvas_width - line2_width) / 2 + 11;
        
        const int y_line1 = 13;
        const int y_line2 = 26;
        
        rgb_matrix::DrawText(offscreen, font, x_line1, y_line1, white, nullptr, line1.c_str());
        rgb_matrix::DrawText(offscreen, small_font, x_line2, y_line2, white, nullptr, line2.c_str());
    } else if (days_left <= 0) {
      std::string line = "THE END";
      int line_width = 0;
        for (char c : line) {
            line_width += font.CharacterWidth(c);
        }
        int x_line = (canvas_width - line_width) / 2;
        const int y_line = 26;
        rgb_matrix::DrawText(offscreen, font, x_line, y_line, white, nullptr, line.c_str());
    } else {
      rgb_matrix::DrawText(offscreen, biggest_font, x_days, y_days, white, nullptr, days_text.c_str());
    }

    offscreen = matrix->SwapOnVSync(offscreen);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  }

  offscreen->Clear();
  delete matrix;
  return 0;
}
