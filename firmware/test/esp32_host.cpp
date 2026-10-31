// Baut den ESP32-C3-Sketch (esp32/esp32_display_ble.ino) mit Platzhaltern auf dem PC und prüft sein Verhalten:
// Bluetooth meldet sich an, jede gültige Statuszeile geht ans Handy, das E-Paper zeichnet nur bei Änderung,
// kaputte Zeilen werden verworfen, überlange Zeilen laufen nicht über.
// Ersetzt nicht den echten ESP32-Build, findet aber Tipp- und Logikfehler vorher.
#include <Arduino.h>
#include <cstdio>
#include "../esp32/esp32_display_ble.ino"

static int fails = 0;
static void check(bool ok, const char* what) { std::printf("%s %s\n", ok ? "  ok  " : "  FEHLER", what); if (!ok) ++fails; }
static void feed(const std::string& s) { Serial1.in += s; loop(); }

int main() {
  std::printf("ESP32-C3-Sketch (PC-Lauf mit Platzhaltern)\n");
  setup();
  check(Serial1.baud == 115200 && Serial1.rx == 20, "Statuszeile: 115200 Baud, RX an GPIO 20 (wie verdrahtung.md)");
  check(epd.inited, "E-Paper initialisiert");
  check(NimBLEDevice::name == "Christbaum-Exo" && NimBLEDevice::adv.running, "Bluetooth wirbt als „Christbaum-Exo“");
  check(chr && (chr->props & NIMBLE_PROPERTY::NOTIFY), "Kennwert mit Benachrichtigung");

  feed("S,AKTIV,82,2,37,1290,41.5,\n");
  check(chr->sent.size() == 1 && chr->sent[0] == "S,AKTIV,82,2,37,1290,41.5,", "gültige Zeile geht 1:1 ans Handy");
  check(epd.draws == 1, "E-Paper zeichnet beim ersten Status");
  check(epd.text.find("AKTIV") != std::string::npos && epd.text.find("82") != std::string::npos && epd.text.find("37") != std::string::npos,
        "Anzeige zeigt Modus, Akku und Bäume");
  check(epd.lastBarWidth == 146 * 82 / 100, "Akkubalken passt zum Ladestand");

  feed("S,AKTIV,82,2,37,1290,41.6,\n");
  check(chr->sent.size() == 2 && epd.draws == 1, "nur Betriebsstunden geändert: Handy ja, E-Paper nicht neu");

  feed("S,AKTIV,82,2,38,1291,41.6,\n");
  check(epd.draws == 2, "Baum dazu: E-Paper zeichnet neu");

  feed("Mull\nS,AKTIV,82\nS,AKTIV,150,2,1,1,1,\n");
  check(chr->sent.size() == 3 && epd.draws == 2, "kaputte und fremde Zeilen werden verworfen");

  feed("S,FEHL");                                   // Zeile kommt in zwei Stücken
  feed("ER,40,1,38,1291,41.7,Zelle 3\n");
  check(chr->sent.size() == 4 && epd.draws == 3 && epd.text.find("Zelle 3") != std::string::npos, "geteilte Zeile mit Fehlergrund");

  feed("S,FEHLER,40,1,38,1291,41.7,Zelle 5\n");
  check(epd.draws == 4 && epd.text.find("Zelle 5") != std::string::npos, "anderer Fehlergrund bei gleichem Modus: E-Paper zeichnet neu");

  feed(std::string(500, 'x') + "\nS,AKTIV,80,2,38,1291,41.8,\n");
  check(chr->sent.size() == 6 && chr->sent.back() == "S,AKTIV,80,2,38,1291,41.8,", "überlange Zeile läuft nicht über, nächste Zeile stimmt");

  std::printf(fails ? "ESP32-SKETCH: %d FEHLER\n" : "ESP32-Sketch: kompiliert und verhält sich richtig (mit Platzhaltern)\n", fails);
  return fails ? 1 : 0;
}
