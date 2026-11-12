// Platzhalter nur für den PC-Test (test/teensy_host.cpp), nicht für den echten Teensy.
#pragma once
#include <cstdint>
#include <cstddef>
#define MSBFIRST 1
#define SPI_MODE1 1
struct SPISettings { SPISettings(uint32_t, int, int) {} };
inline void (*stubSpiTransfer)(uint8_t*, size_t) = nullptr;   // der Test spielt hier den ADS131M04
struct SPIClass { void begin() {} void beginTransaction(const SPISettings&) {} void endTransaction() {} void transfer(uint8_t* b, size_t n) { if (stubSpiTransfer) stubSpiTransfer(b, n); } };
inline SPIClass SPI;
