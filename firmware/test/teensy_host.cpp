// Baut main_teensy.cpp mit Platzhaltern für die Arduino-Bibliotheken (test/stubs) und lässt 1000 Regeltakte laufen.
// Prüft so auf dem PC, dass der Teensy-Code kompiliert und durchläuft.
#include "main_teensy.cpp"
float readTrunkTiltRate(bool* still) { *still = true; return 0; }
void showLed(exo::Led, int) {}
int main() {
  setup();
  for (int i = 0; i < 1000; i++) { tick = true; loop(); }
  // Statuszeile an den ESP32-C3 muss das vereinbarte Format haben
  const char* st = Serial1.lastLine;
  if (std::strncmp(st, "S,AUS,", 6) != 0) { std::printf("Statuszeile falsch: %s\n", st); return 1; }
  std::printf("Statuszeile: %s", st);
  return 0;
}
