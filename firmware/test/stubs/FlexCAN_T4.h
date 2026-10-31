// Platzhalter nur für den PC-Test (test/teensy_host.cpp), nicht für den echten Teensy.
#pragma once
#include <cstdint>
struct CAN_message_t { uint32_t id = 0; uint8_t len = 0; uint8_t buf[8] = {}; };
enum CAN_DEV_TABLE { CAN1 }; enum RXQ { RX_SIZE_256 }; enum TXQ { TX_SIZE_16 };
template <CAN_DEV_TABLE B, RXQ R, TXQ T> struct FlexCAN_T4 { void begin() {} void setBaudRate(uint32_t) {} int read(CAN_message_t&) { return 0; } int write(const CAN_message_t&) { return 1; } };
