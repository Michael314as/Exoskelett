// Baut main_teensy.cpp mit Platzhaltern für die Arduino-Bibliotheken (test/stubs) und lässt 1000 Regeltakte laufen.
// Prüft so auf dem PC, dass der Teensy-Code kompiliert und durchläuft.
#include "main_teensy.cpp"
float readTrunkTiltRate(bool* still) { *still = true; return 0; }
void showLed(exo::Led, int) {}
void showDisplay(const exo::Outputs&, float) {}
int main() { setup(); for (int i = 0; i < 1000; i++) { tick = true; loop(); } return 0; }
