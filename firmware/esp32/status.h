// Statuszeile vom Teensy einlesen (Format siehe main_teensy.cpp: sendStatus). Portabel, auf dem PC getestet.
#pragma once
#include <cstdlib>
#include <cstring>

struct ExoStatus {
  char mode[10] = "?";
  int batteryPct = 0, level = 0, treesToday = 0;
  unsigned long treesTotal = 0;
  float hours = 0.f;
  char reason[40] = "";
  bool valid = false;
};

// "S,AKTIV,82,2,37,1290,41.5,\n" → ExoStatus. Unvollständige oder fremde Zeilen ergeben valid = false.
inline ExoStatus parseStatus(const char* line) {
  ExoStatus s;
  if (!line || line[0] != 'S' || line[1] != ',') return s;
  char buf[128]; std::strncpy(buf, line + 2, sizeof buf - 1); buf[sizeof buf - 1] = 0;
  if (char* nl = std::strpbrk(buf, "\r\n")) *nl = 0;
  char* f[7] = {}; int n = 0; char* p = buf;
  while (n < 7) { f[n++] = p; char* c = std::strchr(p, ','); if (!c) break; *c = 0; p = c + 1; }
  if (n < 6) return s;
  std::strncpy(s.mode, f[0], sizeof s.mode - 1);
  s.batteryPct = std::atoi(f[1]); s.level = std::atoi(f[2]); s.treesToday = std::atoi(f[3]);
  s.treesTotal = std::strtoul(f[4], nullptr, 10); s.hours = float(std::atof(f[5]));
  if (n == 7) std::strncpy(s.reason, f[6], sizeof s.reason - 1);
  s.valid = s.batteryPct >= 0 && s.batteryPct <= 100 && s.level >= 1 && s.level <= 3;
  return s;
}
