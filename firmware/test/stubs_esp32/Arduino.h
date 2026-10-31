// Platzhalter für den ESP32-Arduino-Kern: nur was esp32_display_ble.ino benutzt. Für den PC-Testbuild.
#pragma once
#include <cstdint>
#include <cstring>
#include <string>
#define SERIAL_8N1 0x800001c
struct HostSerial1 {
  std::string in; size_t pos = 0; long baud = 0; int rx = -1, tx = -1;
  void begin(long b, uint32_t, int r, int t) { baud = b; rx = r; tx = t; }
  int available() { return int(in.size() - pos); }
  int read() { return pos < in.size() ? (unsigned char)in[pos++] : -1; }
};
inline HostSerial1 Serial1;
