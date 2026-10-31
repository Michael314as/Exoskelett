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
