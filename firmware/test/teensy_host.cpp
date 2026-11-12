// Baut main_teensy.cpp mit Platzhaltern für die Arduino-Bibliotheken (test/stubs) und spielt die Hardware nach:
// 10 Motoren am CAN (antworten wie ein RobStride), 3 × ADS131M04 an SPI (Rahmen mit Prüfsumme), Taster, Not-Aus, Akku.
// Geprüft wird der Teil der Sicherheit, der nur in main_teensy.cpp steckt: Einschaltfolge der Motoren, Bus-Wächter,
// eingefrorene und stumme Sensoren, stumme oder ausgeschaltete Motoren, Winkel nach dem Einschalten (0…2π).
#include "main_teensy.cpp"
#include <deque>

static float imuRate = 0.f;
float readTrunkTiltRate(bool* still) { *still = true; return imuRate; }
void showLed(exo::Led, int) {}

struct FakeMotor { bool alive = true, run = false, timeoutSet = false, refuseEnable = false; float pos = 0.f; int enables = 0; float lastTau = 0.f; };
static FakeMotor fm[NJ];
static std::deque<CAN_message_t> rxq[4];
static int idxOf(uint8_t canId) { for (int i = 0; i < NJ; i++) if (kCanId[i] == canId) return i; return -1; }
static void canWriteStub(int bus, const CAN_message_t& m) {
  const int i = idxOf(m.id & 0xFF), type = (m.id >> 24) & 0x1F;
  if (i < 0 || kBus[i] != bus || !fm[i].alive) return;
  if (type == kTypeStop) fm[i].run = false;
  if (type == kTypeWriteParam && m.buf[0] == 0x28 && m.buf[1] == 0x70) fm[i].timeoutSet = (m.buf[4] | (m.buf[5] << 8)) == int(kCanTimeoutTicks);
  if (type == kTypeEnable) { fm[i].enables++; if (!fm[i].refuseEnable) fm[i].run = true; }
  if (type == kTypeControl) fm[i].lastTau = uintToFloat((m.id >> 8) & 0xFFFF, -kLim[i].tMax, kLim[i].tMax, 16);
  if (type != kTypeControl && type != kTypeEnable && type != kTypeStop) return;
  CAN_message_t r; r.flags.extended = true; r.len = 8;                       // Rückmeldung Typ 2
  const uint32_t p = floatToUint(fm[i].pos, -kLim[i].pMax, kLim[i].pMax, 16), v = floatToUint(0, -kLim[i].vMax, kLim[i].vMax, 16), t = floatToUint(0, -kLim[i].tMax, kLim[i].tMax, 16);
  const uint8_t d[8] = {uint8_t(p >> 8), uint8_t(p), uint8_t(v >> 8), uint8_t(v), uint8_t(t >> 8), uint8_t(t), 0, 200};
  memcpy(r.buf, d, 8);
  r.id = (uint32_t(kTypeFeedback) << 24) | (uint32_t(fm[i].run ? kMotorRun : kMotorReset) << 22) | (uint32_t(kCanId[i]) << 8) | kHostId;
  rxq[bus].push_back(r);
}
static int canReadStub(int bus, CAN_message_t& m) { if (rxq[bus].empty()) return 0; m = rxq[bus].front(); rxq[bus].pop_front(); return 1; }

static bool adsCorrupt = false, adsResetOnce = false; static int32_t adsValue = 0; static int adsWregs = 0;
static void spiStub(uint8_t* b, size_t n) {
  if (n != 18) return;
  const bool wreg = (b[0] & 0xE0) == 0x60;
  for (size_t i = 0; i < n; i++) b[i] = 0;
  if (wreg) { adsWregs++; return; }
  b[0] = adsResetOnce ? 0x05 : 0x01; b[1] = 0x0F; adsResetOnce = false;       // STATUS: 24 Bit, DRDY0…3; 0x05.. = Neustart gemeldet
  for (int k = 0; k < 4; k++) { b[3 + 3 * k] = (adsValue >> 16) & 0xFF; b[4 + 3 * k] = (adsValue >> 8) & 0xFF; b[5 + 3 * k] = adsValue & 0xFF; }
  const uint16_t c = crc16Ccitt(b, 15); b[15] = c >> 8; b[16] = c & 0xFF;
  if (adsCorrupt) b[4] ^= 0x10;
}

static int fails = 0;
static void check(bool ok, const char* what) { std::printf("  [%s] %s\n", ok ? "OK " : "FEHLER", what); if (!ok) fails++; }
static void run(float sec) { for (int i = 0; i < int(sec / 0.002f + 0.5f); i++) { stubMillis += 2; tick = true; loop(); } }
static void powerOn() { stubPin[PIN_BTN_POWER] = 0; run(0.15f); stubPin[PIN_BTN_POWER] = 1; run(6.f); }
static void reset() {   // Fehler quittieren: lang drücken → AUS; alles wieder heil
  for (auto& m : fm) { m.alive = true; m.refuseEnable = false; }
  adsCorrupt = false; stubPin[PIN_ADS_DRDY] = 0; stubPin[PIN_ESTOP_SENSE] = 0; imuRate = 0.f;
  stubPin[PIN_BTN_POWER] = 0; run(1.2f); stubPin[PIN_BTN_POWER] = 1; run(2.f);
}

int main() {
  stubCanRead = canReadStub; stubCanWrite = canWriteStub; stubSpiTransfer = spiStub;
  for (int p = 0; p < 64; p++) stubPin[p] = 1;                                  // Taster offen (Pullup), Parkstütze oben
  stubPin[PIN_ESTOP_SENSE] = 0; stubPin[PIN_ADS_DRDY] = 0;                      // Not-Aus-Kreis zu, Wandler hat Daten
  for (int c = 0; c < kCells; c++) if (PIN_HX_DOUT[c] >= 0) stubPin[PIN_HX_DOUT[c]] = 0;   // HX711 bereit, liefert 0
  stubAnalog[PIN_VBAT] = 744;                                                   // 36 V
  setup();
  cfg = Settings(); app.jointMask = cfg.jointMask;
  std::printf("Teensy-Code mit nachgespielter Hardware\n");
  run(2.f);
  const char* st = Serial1.lastLine;
  check(std::strncmp(st, "S,AUS,", 6) == 0, "Statuszeile im vereinbarten Format");
  std::printf("     %s", st);

  // Motor steht knapp unter seinem Nullpunkt: nach dem Einschalten meldet er 2π − 0,2 statt −0,2
  fm[HipL].pos = 6.2832f - 0.2f;
  powerOn();
  check(app.sup.mode == Mode::Active && stubOut[PIN_CONTACTOR] == HIGH, "Einschalten: Schütz zu, nach dem Nullen aktiv");
  bool allRun = true, allTimeout = true; for (auto& m : fm) { allRun &= m.run; allTimeout &= m.timeoutSet; }
  check(allRun && allTimeout, "jeder Motor ist eingeschaltet und hat seinen Bus-Wächter (0,1 s) bekommen");
  check(std::fabs(in.joint[HipL].q + 0.2f) < 0.01f, "Motorwinkel 2π − 0,2 wird als −0,2 rad gelesen (kein Sprung um eine Umdrehung)");

  // Messwandler friert ein (DRDY kommt nicht mehr): Unterstützung sofort aus, Beine gedämpft, dann Schütz auf
  stubPin[PIN_ADS_DRDY] = 1; run(0.05f);
  check(app.sup.mode == Mode::SoftOff && app.sup.scale == 0.f && stubOut[PIN_CONTACTOR] == HIGH, "Manschetten-Wandler stumm: nach 20 ms Unterstützung aus, Schütz bleibt fürs gedämpfte Absinken zu");
  run(1.6f);
  check(app.sup.mode == Mode::Fault && stubOut[PIN_CONTACTOR] == LOW && !std::strcmp(app.sup.reason, "Sensor-Manschette gestört"), "nach 1,5 s: Fehler, Schütz offen, Grund in der Statuszeile");
  reset(); powerOn();
  check(app.sup.mode == Mode::Active, "nach dem Quittieren wieder aktiv");

  const int w0 = adsWregs; adsResetOnce = true; run(0.01f);
  check(adsWregs - w0 == 2 && app.sup.mode == Mode::Active, "Wandler meldet Neustart: Takt und Verstärkung werden neu gesetzt, kein Fehler");

  adsCorrupt = true; run(0.05f);
  check(app.sup.mode == Mode::SoftOff, "falsche Prüfsumme vom Wandler (gestörtes Kabel) wird wie ein Ausfall behandelt");
  run(1.6f); reset(); powerOn();

  // Ein Motor verstummt (Kabel ab): hart aus
  fm[KneeR].alive = false; run(0.1f);
  check(app.sup.mode == Mode::Fault && stubOut[PIN_CONTACTOR] == LOW && !std::strcmp(app.sup.reason, "Motor meldet sich nicht"), "Motor antwortet 50 ms nicht: sofort stromlos, Schütz offen");
  reset(); powerOn();

  // Ein Motor antwortet, ist aber nicht (mehr) eingeschaltet, z. B. weil sein eigener Bus-Wächter ausgelöst hat
  fm[ShL].run = false; run(0.02f);
  check(app.sup.mode == Mode::Fault, "Motor im Zustand „Reset“ trotz Befehlen: Fehler statt stiller Ausfall eines Gelenks");
  reset();
  fm[ElR].refuseEnable = true; const int e0 = fm[ElR].enables; powerOn();
  check(fm[ElR].enables - e0 >= 3 && app.sup.mode == Mode::Fault, "Motor nimmt das Einschalten nicht an: mehrere Versuche, dann Fehler");
  reset(); powerOn();

  // Not-Aus: hart, Schütz offen
  stubPin[PIN_ESTOP_SENSE] = 1; run(0.004f);
  check(app.sup.mode == Mode::Fault && stubOut[PIN_CONTACTOR] == LOW, "Not-Aus: im selben Takt Fehler, Schütz offen");
  reset(); powerOn();

  // Akku bricht unter Last ein: erst weniger Kraft, bei „leer“ weich aus
  stubAnalog[PIN_VBAT] = 651; run(0.5f);                                        // 31,5 V
  check(app.sup.mode == Mode::Active && app.sup.powerLimit() < 0.75f, "Akku 31,5 V unter Last: Exo bleibt an, Momentgrenzen gehen zurück");
  stubAnalog[PIN_VBAT] = 600; run(4.f);                                         // 29 V
  check(app.sup.mode != Mode::Active && !std::strcmp(app.sup.reason, "Akku leer"), "Akku 29 V über mehrere Sekunden: weich aus mit Grund „Akku leer“");
  stubAnalog[PIN_VBAT] = 744; run(3.f); reset();

  // Prüfstand: ein Gelenk, eine Zelle; die anderen Wandler und Motoren fehlen
  ConsoleIO io; io.print = [](const char*) {};
  runCommand("pruefstand 7", cfg, io); app.jointMask = cfg.jointMask;
  for (int i = 0; i < NJ; i++) fm[i].alive = i == ElR;
  for (int c = 0; c < kCells; c++) if (PIN_HX_DOUT[c] >= 0) stubPin[PIN_HX_DOUT[c]] = 1;   // keine HX711 angeschlossen
  powerOn();
  check(app.sup.mode == Mode::Active, "Prüfstand mit einem Motor und einer Manschette: läuft, fehlende Zellen stören nicht");
  runCommand("voll", cfg, io);

  std::printf("\n%s (%d Fehler)\n", fails ? "Teensy-Code: NICHT BESTANDEN" : "Teensy-Code: kompiliert und verhält sich richtig (mit nachgespielter Hardware)", fails);
  return fails ? 1 : 0;
}
