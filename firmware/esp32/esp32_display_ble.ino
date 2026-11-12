// ESP32-C3: E-Paper am rechten Oberarm (Uhrzeit, Akku mit Restzeit, Stufe, Fehler) und Bluetooth (BLE) zum Handy.
// Bauen mit Arduino-IDE oder PlatformIO (Board „ESP32C3 Dev Module“). Bibliotheken: GxEPD2, NimBLE-Arduino.
// Verbindung: Teensy Pin 1 (TX) → ESP32-C3 GPIO 20 (RX), Masse verbinden. Kein Rückkanal nötig.
// Wird im GitHub-Workflow mit dem echten ESP32-Compiler gebaut (Job „esp32“) und auf dem PC mit Platzhaltern für
// Kern, GxEPD2 und NimBLE getestet (test/esp32_host.cpp, Teil von make test). Auf einem Board noch nicht gelaufen.
#include <GxEPD2_BW.h>
#include <NimBLEDevice.h>
#include "status.h"

GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> epd(GxEPD2_154_D67(/*CS*/ 7, /*DC*/ 4, /*RST*/ 5, /*BUSY*/ 6));
static NimBLECharacteristic* chr = nullptr;
static ExoStatus last;
static char line[128]; static size_t n = 0;

static void draw(const ExoStatus& s) {
  epd.setPartialWindow(0, 0, epd.width(), epd.height());
  epd.firstPage();
  do {
    epd.fillScreen(GxEPD_WHITE); epd.setTextColor(GxEPD_BLACK);
    epd.setTextSize(5); epd.setCursor(16, 10); epd.print(s.clock);                       // groß: Uhrzeit
    epd.drawRect(6, 70, 150, 24, GxEPD_BLACK); epd.fillRect(8, 72, 146 * s.batteryPct / 100, 20, GxEPD_BLACK);
    epd.setTextSize(2); epd.setCursor(162, 74); epd.print(s.batteryPct); epd.print("%");
    epd.setCursor(6, 106); epd.print("Rest "); epd.print(s.restMin / 60); epd.print(":"); if (s.restMin % 60 < 10) epd.print("0"); epd.print(s.restMin % 60);
    epd.print("  Stufe "); epd.print(s.level);
    epd.drawLine(0, 134, 200, 134, GxEPD_BLACK);
    epd.setCursor(6, 150); epd.print(s.reason[0] ? s.reason : s.mode);                  // unten: Fehler im Klartext, sonst Modus
  } while (epd.nextPage());
}

void setup() {
  Serial1.begin(115200, SERIAL_8N1, /*RX*/ 20, /*TX*/ 21);
  epd.init(0); epd.setRotation(1);
  NimBLEDevice::init("Christbaum-Exo");
  NimBLEServer* srv = NimBLEDevice::createServer();
  NimBLEService* svc = srv->createService("6e400001-b5a3-f393-e0a9-e50e24dcca9e");
  chr = svc->createCharacteristic("6e400003-b5a3-f393-e0a9-e50e24dcca9e", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
  svc->start(); NimBLEDevice::getAdvertising()->addServiceUUID(svc->getUUID()); NimBLEDevice::getAdvertising()->start();
}

void loop() {
  while (Serial1.available()) {
    const char c = char(Serial1.read());
    if (c == '\n') {
      line[n] = 0; n = 0;
      const ExoStatus s = parseStatus(line);
      if (!s.valid) continue;
      chr->setValue(line); chr->notify();                       // Handy bekommt die Zeile 1:1
      const bool changed = std::strcmp(s.mode, last.mode) || s.batteryPct != last.batteryPct || s.level != last.level || std::strcmp(s.clock, last.clock)
                         || std::strcmp(s.reason, last.reason);
      if (changed) { draw(s); last = s; }                        // E-Paper nur bei Änderung (schont und spart Strom)
    } else if (n < sizeof line - 1) line[n++] = c;
  }
}
