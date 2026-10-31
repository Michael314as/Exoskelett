// Treiber: Motor-Protokoll (MIT-Modus, CubeMars AK70-10 und baugleiche Robotergelenk-Motoren) und Wägezellen-Verstärker HX711.
// Hardwareunabhängig geschrieben: Das Verpacken/Entpacken der CAN-Daten und das Einlesen der Bits ist hier,
// das eigentliche Senden/Empfangen (FlexCAN_T4, digitalWrite) in main_teensy.cpp. Dadurch auf dem PC testbar.
#pragma once
#include <cstdint>
#include <cmath>
#include "exo_core.h"

namespace exo {

/* ----------------------------------------------------------- MIT-Protokoll ----- */
// Grenzen laut Datenblatt AK70-10 (bei anderem Motor hier anpassen)
struct MotorLimits { float pMax = 12.5f, vMax = 50.f, tMax = 25.f, kpMax = 500.f, kdMax = 5.f; };

inline uint32_t floatToUint(float x, float lo, float hi, int bits) {
  x = clampf(x, lo, hi);
  return uint32_t(std::lround((x - lo) * float((1u << bits) - 1) / (hi - lo)));
}
inline float uintToFloat(uint32_t v, float lo, float hi, int bits) { return float(v) * (hi - lo) / float((1u << bits) - 1) + lo; }

// Befehl: reines Moment (kp = kd = 0). 8 Byte: p16 | v12 kp12 | kd12 t12
inline void packTorqueCommand(float torqueNm, const MotorLimits& L, uint8_t out[8]) {
  const uint32_t p = floatToUint(0, -L.pMax, L.pMax, 16), v = floatToUint(0, -L.vMax, L.vMax, 12);
  const uint32_t kp = floatToUint(0, 0, L.kpMax, 12), kd = floatToUint(0, 0, L.kdMax, 12), t = floatToUint(torqueNm, -L.tMax, L.tMax, 12);
  out[0] = p >> 8; out[1] = p & 0xFF; out[2] = v >> 4; out[3] = ((v & 0xF) << 4) | (kp >> 8);
  out[4] = kp & 0xFF; out[5] = kd >> 4; out[6] = ((kd & 0xF) << 4) | (t >> 8); out[7] = t & 0xFF;
}
// Antwort des Motors: id | p16 | v12 | i12 (Moment)
struct MotorState { uint8_t id = 0; float pos = 0, vel = 0, torque = 0; };
inline MotorState unpackReply(const uint8_t in[6], const MotorLimits& L) {
  MotorState s; s.id = in[0];
  const uint32_t p = (uint32_t(in[1]) << 8) | in[2], v = (uint32_t(in[3]) << 4) | (in[4] >> 4), i = (uint32_t(in[4] & 0xF) << 8) | in[5];
  s.pos = uintToFloat(p, -L.pMax, L.pMax, 16); s.vel = uintToFloat(v, -L.vMax, L.vMax, 12); s.torque = uintToFloat(i, -L.tMax, L.tMax, 12);
  return s;
}
// Sonderbefehle: Motor einschalten / ausschalten / Nullpunkt
inline void packSpecial(uint8_t last, uint8_t out[8]) { for (int i = 0; i < 7; i++) out[i] = 0xFF; out[7] = last; }
constexpr uint8_t kEnterMotorMode = 0xFC, kExitMotorMode = 0xFD, kSetZero = 0xFE;

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
