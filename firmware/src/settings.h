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

constexpr int kCells = 17;

constexpr uint16_t kSettingsVersion = 7;
constexpr uint32_t kAllCells = (1u << kCells) - 1u;
// Messbereich je Zelle [N]: darüber gilt der Sensor als gestört. Manschetten und Rumpf: Biegebalken 20 kg, Puffer liegen bei ≈ 150 N an.
// Schlaufenköpfe: 50-kg-Zelle, ein schwerer Baum zieht 300–500 N. Steigbügel: 4 × 50 kg, Mensch + Exo + Baum auf einem Bein ≈ 1500 N,
// beim Auftreten mehr. Mit einheitlich 150 N (bis Version 6) hätte schon das Stehen auf einem Bein einen Sensorfehler ausgelöst.
inline float defaultCellLimitN(int c) { return c >= 6 && c <= 8 ? 650.f : (c == 9 || c == 10) ? 2600.f : 150.f; }
// Welche Zelle ein Gelenk für seine Absicht braucht (Prüfstand: nur diese muss angeschlossen sein)
inline uint32_t cuffCellsOfJoint(int g) {
  static const uint32_t m[10] = {1u << 0, 1u << 1, 1u << 15, 1u << 16, 1u << 2, 1u << 3, 1u << 4, 1u << 5, 0x7800u, 0x7800u};
  return g >= 0 && g < 10 ? m[g] : 0u;
}

struct Settings {
  uint32_t magic = 0x45584F33;   // "EXO3"
  uint16_t version = kSettingsVersion;   // 7: Messbereich je Zelle, Zellenmaske; 6: Manschetten M2 (17 Wägezellen); 5: Rumpf
  uint8_t level = 1;             // Unterstützungsstufe 0/1/2
  uint16_t jointMask = 0x3FF;    // aktive Gelenke (Bit = JointId, 10 Gelenke); Prüfstand: nur Ellbogen rechts
  uint32_t cellMask = kAllCells; // angeschlossene Wägezellen (Bit = Zelle); nicht angeschlossene zählen als 0 N und werden nicht überwacht
  Settings() { for (int c = 0; c < kCells; c++) cell[c].limitN = defaultCellLimitN(c); }
  CellCal cell[kCells];          // Wägezellen: 0/1 Oberschenkel L/R, 2/3 Oberarm, 4/5 Unterarm, 6/7/8 Schlaufen R1, R2, L1, 9/10 Fuß L/R, 11/12 Schultergurt L/R, 13/14 Rückenpolster L/R, 15/16 Unterschenkel L/R
  float joint0[10] = {};         // Motorwinkel bei gestreckter Grundstellung (Nullpunkt), je Gelenk
  float hours = 0.f;             // Betriebsstunden (für Wartungshinweise)
  uint32_t crc = 0;
};

inline uint32_t crc32(const uint8_t* d, size_t n) {
  uint32_t c = 0xFFFFFFFFu;
  for (size_t i = 0; i < n; i++) { c ^= d[i]; for (int k = 0; k < 8; k++) c = (c >> 1) ^ (0xEDB88320u & (0u - (c & 1u))); }
  return ~c;
}
inline void seal(Settings& s) { s.crc = crc32(reinterpret_cast<const uint8_t*>(&s), offsetof(Settings, crc)); }
inline bool valid(const Settings& s) {
  return s.magic == 0x45584F33 && s.version == kSettingsVersion && s.crc == crc32(reinterpret_cast<const uint8_t*>(&s), offsetof(Settings, crc));
}

// Wartungshinweis aus den Betriebsstunden (Intervalle laut docs/wartung.md)
inline const char* maintenanceDue(float hours) {
  if (hours >= 200.f && std::fmod(hours, 200.f) < 1.f) return "Inspektion fällig (alle 200 h)";
  if (hours >= 100.f && std::fmod(hours, 100.f) < 1.f) return "Wägezellen prüfen und nachkalibrieren (alle 100 h)";
  if (hours >= 40.f && std::fmod(hours, 40.f) < 1.f) return "Schubstangen, Gelenkköpfe, Schrauben prüfen (alle 40 h)";
  return nullptr;
}

/* ------------------------------------------------------------- Konsole ------ */
// Befehle (eine Zeile, Leerzeichen getrennt):
//   status                      Zustand, Akku, Stufe, Stunden
//   roh <zelle>                 Rohwert einer Wägezelle (0–16)
//   null <zelle>                aktuellen Rohwert als Nullpunkt übernehmen (Manschette unbelastet)
//   kal <zelle> <gramm>         mit angehängtem Gewicht kalibrieren
//   stufe <0|1|2>               Unterstützungsstufe
//   pruefstand [gelenk] / voll  nur ein Gelenk und seine Manschetten-Zelle aktiv (ohne Angabe 7 = Ellbogen rechts) / alles aktiv
//   zelle <zelle> <0|1>         Wägezelle ab-/anmelden (Prüfstand: z. B. Schlaufenkopf dazunehmen); abgemeldet = 0 N, nicht überwacht
//   gelenk0 <gelenk|alle>       Gelenk-Nullpunkt einmessen: Befehl geben, Exo in gestreckter Grundstellung einschalten, 2 s ruhig
//                               stehen; der Motorwinkel wird beim Wechsel auf AKTIV übernommen und beim Ausschalten gespeichert
//   log <0|1> [gelenk]          Messdaten über USB, 500/s: t, q, qd, Kraft, t_ff, v_soll, Kd, Gesamtmoment,
//                               Motormoment (Rückmeldung), Wächter, Schwinganteil – Gelenk 0–9, ohne Angabe 7 (Ellbogen rechts)
//   uhr <hh> <mm>               Uhrzeit für die Anzeige stellen (läuft mit Knopfzelle weiter)
//   speichern                   ins EEPROM schreiben
//   werks                       Werkseinstellungen
struct ConsoleIO {
  int32_t (*rawCell)(int cell) = nullptr;   // liefert aktuellen Rohwert (gemittelt)
  void (*print)(const char*) = nullptr;
  bool (*save)(const Settings&) = nullptr;
  void (*armJointZero)(uint16_t mask) = nullptr;   // Gelenk-Nullpunkte beim nächsten Einschalten übernehmen
  void (*setLog)(bool on, int joint) = nullptr;
  void (*setClock)(int h, int m) = nullptr;
};

inline bool runCommand(const char* line, Settings& s, const ConsoleIO& io) {
  char cmd[16] = {}; int a = -1; float b = 0.f;
  const int n = std::sscanf(line, "%15s %d %f", cmd, &a, &b);
  char out[96];
  auto say = [&](const char* t) { if (io.print) io.print(t); };
  if (n < 1) return false;
  if (!std::strcmp(cmd, "roh") && n >= 2 && a >= 0 && a < kCells) { std::snprintf(out, sizeof out, "Zelle %d: %ld\n", a, long(io.rawCell(a))); say(out); return true; }
  if (!std::strcmp(cmd, "null") && n >= 2 && a >= 0 && a < kCells) { s.cell[a].offset = float(io.rawCell(a)); std::snprintf(out, sizeof out, "Zelle %d genullt\n", a); say(out); return true; }
  if (!std::strcmp(cmd, "kal") && n >= 3 && a >= 0 && a < kCells && b > 50.f) {
    const float counts = float(io.rawCell(a)) - s.cell[a].offset;
    if (std::fabs(counts) < 100.f) { say("Fehler: kein Gewicht erkannt\n"); return false; }
    s.cell[a].nPerCount = (b / 1000.f * 9.81f) / counts;
    std::snprintf(out, sizeof out, "Zelle %d: %.6f N/Schritt\n", a, double(s.cell[a].nPerCount)); say(out); return true;
  }
  if (!std::strcmp(cmd, "pruefstand")) {
    const int g = n >= 2 ? a : 7;
    if (g < 0 || g > 9) { say("Fehler: Gelenk 0–9\n"); return false; }
    s.jointMask = uint16_t(1u << g); s.cellMask = cuffCellsOfJoint(g);
    std::snprintf(out, sizeof out, "Prüfstand: nur Gelenk %d aktiv (Motor-ID %d), Zellenmaske 0x%05lX\n", g, g + 1, (unsigned long)s.cellMask); say(out); return true;
  }
  if (!std::strcmp(cmd, "voll")) { s.jointMask = 0x3FF; s.cellMask = kAllCells; say("alle 10 Gelenke und 17 Zellen aktiv\n"); return true; }
  if (!std::strcmp(cmd, "zelle") && n >= 3 && a >= 0 && a < kCells && (b == 0.f || b == 1.f)) {
    if (b == 1.f) s.cellMask |= (1u << a); else s.cellMask &= ~(1u << a);
    std::snprintf(out, sizeof out, "Zelle %d %s, Zellenmaske 0x%05lX\n", a, b == 1.f ? "angemeldet" : "abgemeldet", (unsigned long)s.cellMask); say(out); return true;
  }
  if (!std::strcmp(cmd, "gelenk0")) {
    char what[8] = {}; std::sscanf(line, "%*s %7s", what);
    const bool all = !std::strcmp(what, "alle");
    if (!all && (n < 2 || a < 0 || a > 9)) { say("Fehler: gelenk0 <0–9|alle>\n"); return false; }
    const uint16_t m = all ? s.jointMask : uint16_t(1u << a);
    if (io.armJointZero) io.armJointZero(m);
    say("Nullpunkt vorgemerkt: Gelenk in gestreckte Grundstellung, Exo einschalten, 2 s ruhig stehen, ausschalten\n"); return true;
  }
  if (!std::strcmp(cmd, "log") && n >= 2 && (a == 0 || a == 1)) {
    const int g = n >= 3 ? int(b) : 7;
    if (g < 0 || g > 9) { say("Fehler: Gelenk 0–9\n"); return false; }
    if (io.setLog) io.setLog(a == 1, g);
    say(a ? "Log an: t_us,q,qd,kraft_N,moment_Nm,v_soll,kd,gesamt_Nm,motor_Nm,waechter,schwing_N\n" : "Log aus\n"); return true;
  }
  if (!std::strcmp(cmd, "stufe") && n >= 2 && a >= 0 && a <= 2) { s.level = uint8_t(a); say("Stufe gesetzt\n"); return true; }
  if (!std::strcmp(cmd, "uhr") && n >= 3 && a >= 0 && a < 24 && b >= 0.f && b < 60.f) { if (io.setClock) io.setClock(a, int(b)); say("Uhr gestellt\n"); return true; }
  if (!std::strcmp(cmd, "speichern")) { seal(s); const bool ok = io.save && io.save(s); say(ok ? "gespeichert\n" : "Fehler beim Speichern\n"); return ok; }
  if (!std::strcmp(cmd, "werks")) { s = Settings(); say("Werkseinstellungen (noch nicht gespeichert)\n"); return true; }
  if (!std::strcmp(cmd, "status")) {
    std::snprintf(out, sizeof out, "Stufe %d, %.1f Betriebsstunden\n", s.level, double(s.hours)); say(out);
    if (const char* m = maintenanceDue(s.hours)) { say(m); say("\n"); }
    return true;
  }
  say("Unbekannter Befehl\n");
  return false;
}

}  // namespace exo
