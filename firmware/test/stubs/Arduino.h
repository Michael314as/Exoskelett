// Platzhalter nur für den PC-Test (test/teensy_host.cpp), nicht für den echten Teensy.
#pragma once
#include <cstdint>
#include <cmath>
#define HIGH 1
#define LOW 0
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
enum { A0 = 14, A1, A2, A3 };
inline void pinMode(int, int) {}
inline void digitalWrite(int, int) {}
inline int digitalRead(int) { return 0; }
inline int analogRead(int) { return 512; }
inline void analogWrite(int, int) {}
inline void delayMicroseconds(int) {}
inline uint32_t millis() { return 0; }
struct IntervalTimer { template <class F> bool begin(F, int) { return true; } };
#include <cstdio>
#include <cstring>
#define constrain(x, a, b) ((x) < (a) ? (a) : ((x) > (b) ? (b) : (x)))
struct SerialStub {
  void begin(long) {}
  int available() { return 0; }
  int read() { return -1; }
  int availableForWrite() { return 64; }
  size_t write(const char* s) { std::snprintf(lastLine, sizeof lastLine, "%s", s); return strlen(s); }
  size_t print(const char* s) { return write(s); }
  char lastLine[128] = {};
};
inline SerialStub Serial, Serial1;
