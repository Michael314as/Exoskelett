// Treiber: Motor-Protokoll (RobStride RS03/RS04/RS06, CAN 2.0 mit 29-Bit-ID, „Operation Control“-Modus) und Wägezellen-Verstärker HX711.
// Hardwareunabhängig geschrieben: Das Verpacken/Entpacken der CAN-Daten und das Einlesen der Bits ist hier,
// das eigentliche Senden/Empfangen (FlexCAN_T4, digitalWrite) in main_teensy.cpp. Dadurch auf dem PC testbar.
#pragma once
#include <cstdint>
#include <cmath>
#include "exo_core.h"

namespace exo {

/* ------------------------------------------------- RobStride-Protokoll (RS06) ----- */
// Quelle: RS06 User Manual (github.com/RobStride/Product_Information), Abschnitt 4.1. CAN 2.0, 1 Mbit/s, erweiterte 29-Bit-ID:
//   Bit 28–24 Befehlstyp, Bit 23–8 Daten (Typ 1: Moment, sonst Host-ID), Bit 7–0 Ziel-ID. Daten High-Byte zuerst.
// Typ 1 Betrieb: t_ref = Kd·(v_soll − v) + Kp·(p_soll − p) + t_ff. Hier reines Moment: Kp = Kd = 0.
struct MotorLimits { float pMax = 12.57f, vMax = 50.f, tMax = 36.f, kpMax = 5000.f, kdMax = 100.f; };   // RS06: ±4π, ±50 rad/s, ±36 Nm
// Wertebereiche je Modell laut Handbuch (Kommunikationstyp 1/2): Kodierung hängt vom Modell ab, deshalb je Motor die richtigen nehmen
constexpr MotorLimits kRS04{12.57f, 15.f, 120.f, 5000.f, 100.f};   // RS04: ±15 rad/s, ±120 Nm
constexpr MotorLimits kRS03{12.57f, 20.f, 60.f, 5000.f, 100.f};    // RS03: ±20 rad/s, ±60 Nm
constexpr MotorLimits kRS06{12.57f, 50.f, 36.f, 5000.f, 100.f};    // RS06: ±50 rad/s, ±36 Nm
constexpr uint8_t kHostId = 0xFD;                                  // eigene ID am Bus
enum : uint8_t { kTypeGetId = 0, kTypeControl = 1, kTypeFeedback = 2, kTypeEnable = 3, kTypeStop = 4, kTypeSetZero = 6 };

inline uint32_t floatToUint(float x, float lo, float hi, int bits) {
  x = clampf(x, lo, hi);
  return uint32_t(std::lround((x - lo) * float((1u << bits) - 1) / (hi - lo)));
}
inline float uintToFloat(uint32_t v, float lo, float hi, int bits) { return float(v) * (hi - lo) / float((1u << bits) - 1) + lo; }

// Befehl Typ 1 (reines Moment): Moment steckt in der ID, die 8 Datenbytes tragen p, v, Kp, Kd (alle 0 bzw. Mitte).
inline uint32_t packTorqueCommand(uint8_t motorId, float torqueNm, const MotorLimits& L, uint8_t out[8]) {
  const uint32_t t = floatToUint(torqueNm, -L.tMax, L.tMax, 16), p = floatToUint(0, -L.pMax, L.pMax, 16), v = floatToUint(0, -L.vMax, L.vMax, 16);
  out[0] = p >> 8; out[1] = p & 0xFF; out[2] = v >> 8; out[3] = v & 0xFF; out[4] = out[5] = out[6] = out[7] = 0;   // Kp = Kd = 0
  return (uint32_t(kTypeControl) << 24) | (t << 8) | motorId;
}
// Sonderbefehle Typ 3 (einschalten), 4 (stoppen; Byte 0 = 1 löscht Fehler), 6 (Nullpunkt setzen, Byte 0 = 1)
inline uint32_t packSpecial(uint8_t type, uint8_t motorId, uint8_t out[8]) {
  for (int i = 0; i < 8; i++) out[i] = 0;
  if (type == kTypeStop || type == kTypeSetZero) out[0] = 1;
  return (uint32_t(type) << 24) | (uint32_t(kHostId) << 8) | motorId;
}
// Rückmeldung Typ 2: ID-Bit 15–8 Motor-ID, Bit 21–16 Fehler, Bit 23–22 Modus; Daten p16 v16 t16 Temperatur·10
struct MotorState { uint8_t id = 0, fault = 0; float pos = 0, vel = 0, torque = 0, tempC = 0; bool valid = false; };
inline MotorState unpackReply(uint32_t canId, const uint8_t in[8], const MotorLimits& L) {
  MotorState s;
  if (((canId >> 24) & 0x1F) != kTypeFeedback) return s;
  s.id = (canId >> 8) & 0xFF; s.fault = (canId >> 16) & 0x3F; s.valid = true;
  s.pos = uintToFloat((uint32_t(in[0]) << 8) | in[1], -L.pMax, L.pMax, 16);
  s.vel = uintToFloat((uint32_t(in[2]) << 8) | in[3], -L.vMax, L.vMax, 16);
  s.torque = uintToFloat((uint32_t(in[4]) << 8) | in[5], -L.tMax, L.tMax, 16);
  s.tempC = float(int16_t((uint16_t(in[6]) << 8) | in[7])) / 10.f;
  return s;
}

// Gelenk ↔ Motor über die Seilübersetzung (Trommel ↔ Scheibe/Hebel) und den Wirkungsgrad
struct Transmission {
  float ratio = 4.67f;   // Gelenkweg je Motorweg, z. B. Scheibe 7 cm / Trommel 1,5 cm
  float eta = 0.88f;     // Reibung im Seilweg
  float motorTorqueFor(float jointTorque) const { return jointTorque / (ratio * eta); }
  float jointAngleFrom(float motorAngle) const { return motorAngle / ratio; }
  float jointVelFrom(float motorVel) const { return motorVel / ratio; }
};

/* ---------------------------------------------------------------- HX711 -------- */
// 24-Bit-Wert, Zweierkomplement, MSB zuerst. readBit() liefert das nächste Datenbit (nach Takt-Puls).
template <class ReadBit>
inline int32_t hx711ReadRaw(ReadBit readBit, int extraPulses = 1) {   // 1 Zusatzpuls = Kanal A, Verstärkung 128
  uint32_t v = 0;
  for (int i = 0; i < 24; i++) v = (v << 1) | (readBit() ? 1u : 0u);
  for (int i = 0; i < extraPulses; i++) readBit();
  if (v & 0x800000u) v |= 0xFF000000u;   // Vorzeichen erweitern
  return int32_t(v);
}
// Kalibrierung: Rohwert → Newton, mit Nullpunkt aus dem Nullen und Plausibilitätsprüfung
struct CellCal {
  float offset = 0.f, nPerCount = 1.f / 4200.f;   // aus Kalibrierung mit Gewicht
  float limitN = 150.f;                            // darüber: Überlast-Puffer liegen an → Sensor „am Anschlag“
  float toNewton(int32_t raw) const { return (float(raw) - offset) * nPerCount; }
  bool plausible(int32_t raw) const { return raw != 0x7FFFFF && raw != int32_t(0xFF800000) && std::fabs(toNewton(raw)) < limitN; }
};

}  // namespace exo
