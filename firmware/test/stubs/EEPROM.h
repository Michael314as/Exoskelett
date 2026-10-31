// Platzhalter nur für den PC-Test (test/teensy_host.cpp): EEPROM im Speicher.
#pragma once
#include <cstring>
struct EEPROMStub {
  unsigned char mem[4284] = {};
  template <class T> void get(int a, T& t) { std::memcpy(&t, mem + a, sizeof(T)); }
  template <class T> void put(int a, const T& t) { std::memcpy(mem + a, &t, sizeof(T)); }
};
inline EEPROMStub EEPROM;
