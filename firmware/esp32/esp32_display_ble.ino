// ESP32-C3: zeigt den Status des Exos auf dem E-Paper und sendet ihn per Bluetooth (BLE) ans Handy.
// Bauen mit Arduino-IDE oder PlatformIO (Board „ESP32C3 Dev Module“). Bibliotheken: GxEPD2, NimBLE-Arduino.
// Verbindung: Teensy Pin 1 (TX) → ESP32-C3 GPIO 20 (RX), Masse verbinden. Kein Rückkanal nötig.
// Hinweis: Mit dem echten ESP32-Compiler ist diese Datei noch nicht gebaut worden. Auf dem PC wird sie mit
// Platzhaltern für Kern, GxEPD2 und NimBLE kompiliert und getestet (test/esp32_host.cpp, Teil von make test).
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
    epd.fillScreen(GxEPD_WHITE); epd.setTextColor(GxEPD_BLACK); epd.setTextSize(2);
    epd.setCursor(6, 10); epd.print(s.mode);
    epd.drawRect(6, 40, 150, 24, GxEPD_BLACK); epd.fillRect(8, 42, 146 * s.batteryPct / 100, 20, GxEPD_BLACK);
    epd.setCursor(6, 76); epd.print(s.batteryPct); epd.print(" %  Stufe "); epd.print(s.level);
    epd.setCursor(6, 106); epd.print("Baeume "); epd.print(s.treesToday);
    epd.setTextSize(1); epd.setCursor(6, 140); epd.print(s.reason[0] ? s.reason : "");
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
      const bool changed = std::strcmp(s.mode, last.mode) || s.batteryPct != last.batteryPct || s.level != last.level || s.treesToday != last.treesToday
                         || std::strcmp(s.reason, last.reason);
      if (changed) { draw(s); last = s; }                        // E-Paper nur bei Änderung (schont und spart Strom)
    } else if (n < sizeof line - 1) line[n++] = c;
  }
}
