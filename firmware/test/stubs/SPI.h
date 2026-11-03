// Platzhalter nur für den PC-Test (test/teensy_host.cpp), nicht für den echten Teensy.
#pragma once
#include <cstdint>
#include <cstddef>
#define MSBFIRST 1
#define SPI_MODE1 1
struct SPISettings { SPISettings(uint32_t, int, int) {} };
struct SPIClass { void begin() {} void beginTransaction(const SPISettings&) {} void endTransaction() {} void transfer(uint8_t*, size_t) {} };
inline SPIClass SPI;
