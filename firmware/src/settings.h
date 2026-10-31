// Einstellungen und Kalibrierung: im EEPROM gespeichert, mit Prüfsumme. Dazu ein kleiner Befehlsinterpreter für die
// USB-Konsole (Kalibrieren in der Werkstatt). Hardwareunabhängig und auf dem PC getestet.
#pragma once
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "drivers.h"

namespace exo {

struct Settings {
  uint32_t magic = 0x45584F33;   // "EXO3"
  uint16_t version = 1;
  uint8_t level = 1;             // Unterstützungsstufe 0/1/2
  uint8_t reserved = 0;
  CellCal cell[8];               // Wägezellen
  float joint0[6] = {};          // Motorwinkel bei gestreckter Grundstellung (Nullpunkt)
  float hours = 0.f;             // Betriebsstunden (für Wartungshinweise)
  uint32_t treesTotal = 0;
  uint32_t crc = 0;
};

inline uint32_t crc32(const uint8_t* d, size_t n) {
  uint32_t c = 0xFFFFFFFFu;
  for (size_t i = 0; i < n; i++) { c ^= d[i]; for (int k = 0; k < 8; k++) c = (c >> 1) ^ (0xEDB88320u & (0u - (c & 1u))); }
  return ~c;
}
inline void seal(Settings& s) { s.crc = crc32(reinterpret_cast<const uint8_t*>(&s), offsetof(Settings, crc)); }
inline bool valid(const Settings& s) {
  return s.magic == 0x45584F33 && s.version == 1 && s.crc == crc32(reinterpret_cast<const uint8_t*>(&s), offsetof(Settings, crc));
}

// Wartungshinweis aus den Betriebsstunden (Intervalle laut docs/wartung.md)
inline const char* maintenanceDue(float hours) {
  if (hours >= 200.f) return "Jahresinspektion fällig (200 h)";
  if (hours >= 40.f && std::fmod(hours, 40.f) < 1.f) return "Seile und Bowdenzüge prüfen (alle 40 h)";
  return nullptr;
}

/* ------------------------------------------------------------- Konsole ------ */
// Befehle (eine Zeile, Leerzeichen getrennt):
//   status                      Zustand, Akku, Stufe, Stunden
//   roh <zelle>                 Rohwert einer Wägezelle (0–7)
//   null <zelle>                aktuellen Rohwert als Nullpunkt übernehmen (Manschette unbelastet)
//   kal <zelle> <gramm>         mit angehängtem Gewicht kalibrieren
//   stufe <0|1|2>               Unterstützungsstufe
//   speichern                   ins EEPROM schreiben
//   werks                       Werkseinstellungen
struct ConsoleIO {
  int32_t (*rawCell)(int cell) = nullptr;   // liefert aktuellen Rohwert (gemittelt)
  void (*print)(const char*) = nullptr;
  bool (*save)(const Settings&) = nullptr;
};

inline bool runCommand(const char* line, Settings& s, const ConsoleIO& io) {
  char cmd[16] = {}; int a = -1; float b = 0.f;
  const int n = std::sscanf(line, "%15s %d %f", cmd, &a, &b);
  char out[96];
  auto say = [&](const char* t) { if (io.print) io.print(t); };
  if (n < 1) return false;
  if (!std::strcmp(cmd, "roh") && n >= 2 && a >= 0 && a < 8) { std::snprintf(out, sizeof out, "Zelle %d: %ld\n", a, long(io.rawCell(a))); say(out); return true; }
  if (!std::strcmp(cmd, "null") && n >= 2 && a >= 0 && a < 8) { s.cell[a].offset = float(io.rawCell(a)); std::snprintf(out, sizeof out, "Zelle %d genullt\n", a); say(out); return true; }
  if (!std::strcmp(cmd, "kal") && n >= 3 && a >= 0 && a < 8 && b > 50.f) {
    const float counts = float(io.rawCell(a)) - s.cell[a].offset;
    if (std::fabs(counts) < 100.f) { say("Fehler: kein Gewicht erkannt\n"); return false; }
    s.cell[a].nPerCount = (b / 1000.f * 9.81f) / counts;
    std::snprintf(out, sizeof out, "Zelle %d: %.6f N/Schritt\n", a, double(s.cell[a].nPerCount)); say(out); return true;
  }
  if (!std::strcmp(cmd, "stufe") && n >= 2 && a >= 0 && a <= 2) { s.level = uint8_t(a); say("Stufe gesetzt\n"); return true; }
  if (!std::strcmp(cmd, "speichern")) { seal(s); const bool ok = io.save && io.save(s); say(ok ? "gespeichert\n" : "Fehler beim Speichern\n"); return ok; }
  if (!std::strcmp(cmd, "werks")) { s = Settings(); say("Werkseinstellungen (noch nicht gespeichert)\n"); return true; }
  if (!std::strcmp(cmd, "status")) {
    std::snprintf(out, sizeof out, "Stufe %d, %.1f h, %lu Bäume gesamt\n", s.level, double(s.hours), (unsigned long)s.treesTotal); say(out);
    if (const char* m = maintenanceDue(s.hours)) { say(m); say("\n"); }
    return true;
  }
  say("Unbekannter Befehl\n");
  return false;
}

}  // namespace exo
