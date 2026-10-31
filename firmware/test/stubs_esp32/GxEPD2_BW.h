// Platzhalter für GxEPD2 (E-Paper): zählt Neuzeichnungen und merkt sich den Text. Für den PC-Testbuild.
#pragma once
#include <cstdint>
#include <string>
#define GxEPD_WHITE 0xFFFF
#define GxEPD_BLACK 0x0000
struct GxEPD2_154_D67 {
  static constexpr int WIDTH = 200, HEIGHT = 200;
  int cs, dc, rst, busy;
  GxEPD2_154_D67(int c, int d, int r, int b) : cs(c), dc(d), rst(r), busy(b) {}
};
template <class Panel, int PageHeight>
struct GxEPD2_BW {
  Panel panel; int pages = 0, draws = 0, rotation = 0; bool inited = false; std::string text;
  explicit GxEPD2_BW(Panel p) : panel(p) {}
  void init(int) { inited = true; }
  void setRotation(int r) { rotation = r; }
  int width() const { return Panel::WIDTH; }
  int height() const { return Panel::HEIGHT; }
  void setPartialWindow(int, int, int, int) {}
  void firstPage() { pages = 0; ++draws; text.clear(); }
  bool nextPage() { return ++pages < 1; }
  void fillScreen(uint16_t) {}
  void setTextColor(uint16_t) {}
  void setTextSize(int) {}
  void setCursor(int, int) { text += '|'; }
  void drawRect(int, int, int, int, uint16_t) {}
  void fillRect(int, int, int w, int, uint16_t) { lastBarWidth = w; }
  void print(const char* s) { text += s; }
  void print(int v) { text += std::to_string(v); }
  int lastBarWidth = -1;
};
