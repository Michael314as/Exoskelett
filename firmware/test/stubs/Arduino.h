// Platzhalter nur für den PC-Test (test/teensy_host.cpp), nicht für den echten Teensy.
// Pins, Zeit und Analogwerte sind von außen setzbar, damit der Test Einschalten, Sensor- und Motorausfall durchspielen kann.
#pragma once
#include <cstdint>
#include <cmath>
#define HIGH 1
#define LOW 0
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
enum { A0 = 14, A1, A2, A3 };
inline int stubPin[64] = {};        // Eingänge: was digitalRead liefert
inline int stubOut[64] = {};        // Ausgänge: was zuletzt geschrieben wurde
inline int stubAnalog[64] = {};     // analogRead je Pin (0 = Vorgabe 512)
inline uint32_t stubMillis = 0;
inline void pinMode(int, int) {}
inline void digitalWrite(int p, int v) { if (p >= 0 && p < 64) stubOut[p] = v; }
inline int digitalRead(int p) { return p >= 0 && p < 64 ? stubPin[p] : 0; }
inline int analogRead(int p) { return p >= 0 && p < 64 && stubAnalog[p] ? stubAnalog[p] : 512; }
inline void analogWrite(int p, int v) { if (p >= 0 && p < 64) stubOut[p] = v; }
inline void delayMicroseconds(int) {}
inline uint32_t millis() { return stubMillis; }
inline uint32_t micros() { return stubMillis * 1000u; }
struct IntervalTimer { template <class F> bool begin(F, int) { return true; } };
#include <cstdio>
#include <cstring>
#define constrain(x, a, b) ((x) < (a) ? (a) : ((x) > (b) ? (b) : (x)))
struct SerialStub {
  void begin(long) {}
  int available() { return 0; }
  int read() { return -1; }
  int availableForWrite() { return 256; }
  size_t write(const char* s) { std::snprintf(lastLine, sizeof lastLine, "%s", s); return strlen(s); }
  size_t print(const char* s) { return write(s); }
  char lastLine[160] = {};
};
inline SerialStub Serial, Serial1;
