// Teensy 4.1: liest die Hardware, ruft App::step() 500-mal pro Sekunde auf und schreibt die Ausgänge.
// Bauen: pio run (siehe platformio.ini). Pinbelegung: docs/verdrahtung.md (muss mit PIN_* unten übereinstimmen).
//
// Motoren (Entwurf 5): 10 × RobStride RS04/RS03 im „Operation Control“-Modus (Typ 1, 29-Bit-CAN-ID), 1 Mbit/s, auf zwei Bussen:
// CAN1 Beine + Rumpf beugen (5), CAN3 Arme + Rumpf drehen (5). 500 Hz × 5 Motoren × (Befehl + Antwort) ≈ 70 % Buslast je Bus.
// Mitgehen (Rev. M2): je Takt t_ff, v_soll und Kd an den Motor; die Dämpfung auf v_soll rechnet der Motor selbst (ohne Busverzug).
// Manschetten (8 Glieder + 4 Rumpf): 3 × ADS131M04 an SPI0, 1000 Messungen/s, gemeinsamer Takt (8,192-MHz-Oszillator) und SYNC.
// Schlaufenköpfe (3) und Steigbügel (2): HX711 (langsam genügt).
// Lagesensor: BNO085 (I2C), liefert Kippgeschwindigkeit (Sturz) und Vorneigung des Rumpfs. Uhr: RTC des Teensy mit Knopfzelle. LED-Ring: 12 × WS2812. Watchdog: WDOG1 des i.MX RT1062 (direkt über Register, 0,5 s).
// Anzeige und Handy-Funk macht ein ESP32-C3 an Serial1: Der Teensy schickt ihm jede Sekunde eine Statuszeile.
// (Ein E-Paper-Update blockiert mehrere 100 ms und darf deshalb nicht im Regeltakt laufen.)
#ifdef ARDUINO
#include <Arduino.h>
#include <EEPROM.h>
#include <FlexCAN_T4.h>
#include <SPI.h>
#ifndef EXO_HOST_TEST
#include <Adafruit_NeoPixel.h>
#include <Adafruit_BNO08x.h>
#endif
#include "exo_app.h"
#include "drivers.h"
#include "settings.h"

using namespace exo;

// ---------- Pins (Teensy 4.1) ----------
constexpr int PIN_BTN_POWER = 2,              /* Serial1 zum ESP32-C3: 0 RX, 1 TX */ PIN_BTN_LOOP[NL] = {3, 4, 5};
constexpr int PIN_LOOP_PWM[NL] = {6, 7, 8}, PIN_LUMB_LOCK = 9;               // 9: Hubmagnet der Lendensperre (über MOSFET; 13 ist jetzt SPI-Takt)
constexpr int PIN_LOOP_CUR[NL] = {14, 15, 16}, PIN_VBAT = 17;                  // analog A0–A3; I2C: 18 SDA, 19 SCL
constexpr int PIN_LED_RING = 20, PIN_VIBE = 21;                                // CAN1: 22 TX, 23 RX
constexpr int PIN_LOOP_DIR[NL] = {24, 25, 26}, PIN_LOOP_END[NL] = {27, 28, 29};
constexpr int PIN_HEAT_LEGS = 42, PIN_HEAT_ARMS = 43;                          // Heizfolien der Gelenkkapseln (MOSFET; Lötpunkte unten, SD-Karte nicht benutzt)
constexpr int PIN_ESTOP_SENSE = 46, PIN_CONTACTOR = 47, PIN_STAND = 32, PIN_LIGHT = 33, PIN_HX_SCK = 34;   // 46/47: Lötpunkte unten; CAN3: 30 RX, 31 TX
// Zellen: 0/1 Oberschenkel L/R, 2/3 Oberarm, 4/5 Unterarm, 6/7/8 Schlaufe R1, R2, L1, 9/10 Fuß L/R, 11/12 Gurt L/R, 13/14 Rückenpolster L/R, 15/16 Unterschenkel L/R
// HX711 nur noch für Schlaufen und Steigbügel (−1 = Zelle hängt am ADS131M04)
constexpr int PIN_HX_DOUT[kCells] = {-1, -1, -1, -1, -1, -1, 36, 37, 40, 38, 39, -1, -1, -1, -1, -1, -1};
// ADS131M04: SPI0 (11 MOSI, 12 MISO, 13 SCK), Chipselect je Wandler, DRDY des ersten (alle laufen synchron), SYNC/RESET gemeinsam
constexpr int PIN_ADS_CS[3] = {10, 35, 41}, PIN_ADS_DRDY = 44, PIN_ADS_SYNC = 45;
constexpr int kAdsCell[3][4] = {{0, 1, 2, 3}, {4, 5, 11, 12}, {13, 14, 15, 16}};    // Zelle je Wandler-Kanal
constexpr float kFootContactN = 49.f;                                          // ≈ 5 kg auf dem Steigbügel = Standbein (am Prüfstand einstellen)

static App app;
static Inputs in;
static Settings cfg;
static IntervalTimer ctlTimer;
static volatile bool tick = false;
static volatile uint32_t tickCount = 0;                                          // zählt der Zeitgeber: daran sieht die Schleife ausgelassene Takte
static const char* notice = "";                                                 // Hinweis der Steuerung für die Anzeige
static const char* sensorDetail = "";                                           // welcher Sensor zuletzt gestört war (USB-Konsole)

// ---------- Motoren ----------
static FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;
static FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> can3;
static const uint8_t kBus[NJ] = {1, 1, 1, 1, 3, 3, 3, 3, 1, 3};                  // Beine + Rumpf beugen an CAN1, Arme + Rumpf drehen an CAN3
static void canWrite(int i, const CAN_message_t& m) { if (kBus[i] == 1) can1.write(m); else can3.write(m); }
static const MotorLimits kLim[NJ] = {kRS04, kRS04, kRS04, kRS04, kRS04, kRS04, kRS03, kRS03, kRS04, kRS03};   // Hüfte, Knie, Schulter, Rumpf beugen: RS04; Ellbogen, Rumpf drehen: RS03
static const uint8_t kCanId[NJ] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};             // HipL, HipR, KneeL, KneeR, ShL, ShR, ElL, ElR, Rumpf beugen, Rumpf drehen
// Übersetzung Gelenk : Motor und Wirkungsgrad
static const Transmission kTr[NJ] = {{1.f, 0.97f}, {1.f, 0.97f}, {1.f, 0.95f}, {1.f, 0.95f}, {1.f, 0.97f}, {1.f, 0.97f}, {1.f, 0.95f}, {1.f, 0.95f}, {1.f, 0.95f}, {1.8f, 0.93f}};   // direkt bzw. Schubstange 1:1; Rumpf drehen über Kurbel 1 : 1,8
static MotorState motorState[NJ];
static uint32_t lastReplyMs[NJ];
static float powerOnTime = 0.f;                                                 // Zeit seit Schütz zu
static bool motorsOn = false;

static uint16_t zeroArm = 0;                                                    // Gelenke, deren Nullpunkt beim nächsten Einschalten eingemessen wird (Konsole gelenk0)
static void armJointZero(uint16_t mask) { zeroArm = mask; }
// Einschalten eines Motors: alte Fehler löschen, Bus-Wächter im Motor setzen (0,1 s ohne Befehl → der Motor wird von selbst
// stromlos; Werkseinstellung ist „aus“ und der Wert geht beim Ausschalten verloren), dann einschalten.
// Der Wächter ist die zweite Ebene neben dem Schütz: hängt der Teensy oder reißt das CAN-Kabel, hält der Motor sonst sein letztes Moment.
static void motorEnable(int i) {
  CAN_message_t m; m.flags.extended = 1; m.len = 8;
  m.id = packSpecial(kTypeStop, kCanId[i], m.buf); canWrite(i, m);
  m.id = packParamWrite(kCanId[i], kParamCanTimeout, kCanTimeoutTicks, m.buf); canWrite(i, m);
  m.id = packSpecial(kTypeEnable, kCanId[i], m.buf); canWrite(i, m);
}
static bool readMotors(JointInput* j) {
  CAN_message_t m;
  auto take = [&](uint8_t bus) {
    if (!m.flags.extended || m.len < 8) return;
    const uint8_t from = (m.id >> 8) & 0xFF;
    for (int i = 0; i < NJ; i++) if (kCanId[i] == from && kBus[i] == bus) { const MotorState s = unpackReply(m.id, m.buf, kLim[i]); if (s.valid) { motorState[i] = s; lastReplyMs[i] = millis(); } }
  };
  while (can1.read(m)) take(1);
  while (can3.read(m)) take(3);
  bool ok = true;
  for (int i = 0; i < NJ; i++) {
    // Winkel über den gespeicherten Nullpunkt, auf ±π gefaltet (der Motor meldet nach dem Einschalten 0…2π)
    j[i].q = ((zeroArm >> i) & 1u) ? 0.f : kTr[i].jointAngleFrom(motorState[i].pos, cfg.joint0[i]);
    j[i].qd = kTr[i].jointVelFrom(motorState[i].vel);
    j[i].motorTempC = motorState[i].tempC;
    if (!((cfg.jointMask >> i) & 1u)) continue;                                 // nur eingebaute Motoren prüfen
    if (millis() - lastReplyMs[i] > 50 || motorState[i].fault) ok = false;      // stumm oder Motorfehler (Übertemperatur, Überstrom, Encoder …)
    if (powerOnTime > 1.0f && motorState[i].mode != kMotorRun) ok = false;      // antwortet, ist aber nicht eingeschaltet (z. B. eigener Bus-Wächter)
  }
  return powerOnTime < 0.6f ? true : ok;          // Motoren brauchen nach dem Schütz einen Moment zum Hochfahren
}
static void motorSend(int id, float tauJoint, float vJoint, float kdJoint) {
  CAN_message_t m; m.flags.extended = 1; m.len = 8;
  m.id = packMitCommand(kCanId[id], kTr[id].motorTorqueFor(tauJoint), kTr[id].motorVelFor(vJoint), kTr[id].motorKdFor(kdJoint), kLim[id], m.buf);
  canWrite(id, m);
}

// ---------- Wägezellen ----------
static float cellN[kCells];
static int32_t cellRaw[kCells];
static int32_t readCellRaw(int c) {
  return hx711ReadRaw([c] { digitalWrite(PIN_HX_SCK, HIGH); delayMicroseconds(1); const bool b = digitalRead(PIN_HX_DOUT[c]);
                            digitalWrite(PIN_HX_SCK, LOW); delayMicroseconds(1); return b; });
}
// ADS131M04: einen Rahmen je Wandler lesen (18 Bytes, 25 MHz SPI ≈ 6 µs je Wandler)
static const SPISettings kAdsSpi(25000000, MSBFIRST, SPI_MODE1);
static void adsTransfer(int a, uint8_t* buf) {
  SPI.beginTransaction(kAdsSpi); digitalWrite(PIN_ADS_CS[a], LOW);
  SPI.transfer(buf, 18);
  digitalWrite(PIN_ADS_CS[a], HIGH); SPI.endTransaction();
}
static void adsConfigure(int a);
static void adsBegin() {
  SPI.begin();
  for (int a = 0; a < 3; a++) { pinMode(PIN_ADS_CS[a], OUTPUT); digitalWrite(PIN_ADS_CS[a], HIGH); }
  pinMode(PIN_ADS_DRDY, INPUT); pinMode(PIN_ADS_SYNC, OUTPUT); digitalWrite(PIN_ADS_SYNC, HIGH);
  for (int a = 0; a < 3; a++) adsConfigure(a);
  digitalWrite(PIN_ADS_SYNC, LOW); delayMicroseconds(2); digitalWrite(PIN_ADS_SYNC, HIGH);   // alle drei gleichzeitig starten
  uint8_t b[18];
  for (int a = 0; a < 3; a++) { for (int i = 0; i < 18; i++) b[i] = 0; adsTransfer(a, b); }   // einmal lesen: löscht die Neustart-Meldung im STATUS
}
static uint32_t lastAdsMs = 0, lastHxMs[kCells] = {}, adsCrcErrors = 0, adsResets = 0;
static void adsConfigure(int a) { uint8_t b[18]; adsWregFrame(kAdsRegClock, kAdsClock1k, b); adsTransfer(a, b); adsWregFrame(kAdsRegGain1, kAdsGain128, b); adsTransfer(a, b); }
// Liest alle drei Wandler. Jeder Rahmen wird mit seiner Prüfsumme geprüft. Je Wandler zwei Rahmen: Der Baustein puffert zwei Messungen,
// und weil der Regeltakt (500/s) nur jede zweite abholt, käme sonst immer die ältere (1 ms mehr Verzug). Meldet ein Wandler einen
// Neustart (z. B. nach einem Spannungseinbruch), wird er neu eingestellt; sonst liefe er mit Verstärkung 1 statt 128 weiter.
static bool adsRead() {                                                         // true = neuer, gültiger Satz von allen drei Wandlern
  if (digitalRead(PIN_ADS_DRDY) == HIGH) return false;                          // DRDY aktiv tief
  bool all = true;
  for (int a = 0; a < 3; a++) {
    int32_t ch[4] = {}; bool got = false;
    for (int pass = 0; pass < 2; pass++) {
      uint8_t b[18] = {}; int32_t c2[4];
      adsTransfer(a, b);
      if (!adsFrameCrcOk(b)) { adsCrcErrors++; break; }
      const uint16_t st = adsParseFrame(b, c2);
      if (adsStatusReset(st)) { adsResets++; adsConfigure(a); got = false; break; }
      if (pass == 1 && !adsStatusDataReady(st)) break;                          // keine zweite Messung im Puffer: die erste gilt
      for (int k = 0; k < 4; k++) ch[k] = c2[k];
      got = true;
    }
    if (!got) { all = false; continue; }
    for (int k = 0; k < 4; k++) if (kAdsCell[a][k] >= 0) cellRaw[kAdsCell[a][k]] = ch[k];
  }
  if (all) lastAdsMs = millis();
  return all;
}
// Sensoren lesen und überwachen. Als gestört gilt eine angemeldete Zelle, wenn sie am Anschlag ist ODER keine frischen Werte mehr
// kommen (Wandler stumm, Kabel ab, Prüfsumme falsch). Ein eingefrorener Wert ist beim Mitgehen der gefährlichste Fehler: Die letzte
// Kraft bliebe stehen und das Gelenk liefe mit ihrer Sollgeschwindigkeit weiter.
constexpr uint32_t kAdsStaleMs = 20, kHxStaleMs = 300;                          // ADS131M04: 1000/s; HX711: 80/s (Brücke RATE gesetzt), 10/s ab Werk
static bool readCuffs(JointInput* j) {
  bool ok = true;
  adsRead();
  const uint32_t now = millis();
  for (int c = 0; c < kCells; c++) {
    if (!((cfg.cellMask >> c) & 1u)) { cellN[c] = 0.f; lastHxMs[c] = now; continue; }   // nicht angeschlossen (Prüfstand)
    if (PIN_HX_DOUT[c] < 0) {                                                   // ADS131M04-Zelle: Wert liegt schon in cellRaw
      if (now - lastAdsMs > kAdsStaleMs) { ok = false; sensorDetail = "Manschetten-Messwandler liefert nichts (Kabel, Prüfsumme)"; continue; }
      if (!cfg.cell[c].plausible(cellRaw[c])) { ok = false; sensorDetail = "Manschetten-Zelle am Anschlag"; continue; }
      cellN[c] = cfg.cell[c].toNewton(cellRaw[c]); continue;
    }
    if (digitalRead(PIN_HX_DOUT[c]) == HIGH) {                                  // noch kein neuer Wert (HX711)
      if (now - lastHxMs[c] > kHxStaleMs) { ok = false; sensorDetail = "HX711 (Schlaufe/Steigbügel) liefert nichts"; }
      continue;
    }
    cellRaw[c] = readCellRaw(c); lastHxMs[c] = now;
    if (!cfg.cell[c].plausible(cellRaw[c])) { ok = false; sensorDetail = "Schlaufen-/Fußzelle am Anschlag"; continue; }
    cellN[c] = cfg.cell[c].toNewton(cellRaw[c]);
  }
  j[HipL].cuffN = cellN[0]; j[HipR].cuffN = cellN[1];
  j[KneeL].cuffN = cellN[15]; j[KneeR].cuffN = cellN[16];                       // Schienbein-Manschetten (M2); dazu die Kopplung vom Oberschenkel
  j[ShL].cuffN = cellN[2]; j[ShR].cuffN = cellN[3]; j[ElL].cuffN = cellN[4]; j[ElR].cuffN = cellN[5];
  for (int i = 0; i < NL; i++) in.loopN[i] = fmaxf(0.f, cellN[6 + i]);
  // Rumpf: Gurte ziehen (11/12) = vorbeugen, ins Rückenpolster drücken (13/14) = aufrichten; Drehen aus dem Unterschied rechts/links
  j[Lumb].cuffN = (cellN[11] + cellN[12]) - (cellN[13] + cellN[14]);
  j[Twist].cuffN = 0.5f * ((cellN[12] - cellN[11]) + (cellN[13] - cellN[14]));
  return ok;
}

// ---------- Lagesensor, LED-Ring, Anzeige, Watchdog ----------
#ifndef EXO_HOST_TEST
static Adafruit_BNO08x imu(-1);
static Adafruit_NeoPixel ring(12, PIN_LED_RING, NEO_GRB + NEO_KHZ800);
static bool imuOk = false;
static uint32_t imuLastMs = 0;
static void imuReports() { imuOk = imu.enableReport(SH2_GYROSCOPE_CALIBRATED, 5000) && imu.enableReport(SH2_GAME_ROTATION_VECTOR, 10000); }
// Lagesensor lebt: Er liefert 200 Werte/s. Bleibt er stumm, stünden Vorneigung (Hüft-Vorsteuerung) und Sturzerkennung auf altem Stand.
static bool imuAlive() { return imuOk && millis() - imuLastMs < 100; }

static float trunkPitch = 0.f;                                                  // Vorneigung aus dem Rotationsvektor
static float readTrunkTiltRate(bool* still) {
  static float rate = 0.f, quiet = 0.f;
  sh2_SensorValue_t v;
  if (imuOk && imu.wasReset()) imuReports();                                    // der BNO085 startet gelegentlich neu und vergisst dann seine Berichte
  while (imuOk && imu.getSensorEvent(&v)) {
    imuLastMs = millis();
    if (v.sensorId == SH2_GAME_ROTATION_VECTOR) {
      const float qr = v.un.gameRotationVector.real, qi = v.un.gameRotationVector.i, qj = v.un.gameRotationVector.j, qk = v.un.gameRotationVector.k;
      trunkPitch = atan2f(2.f * (qr * qk + qi * qj), 1.f - 2.f * (qj * qj + qk * qk));   // Drehung um die Querachse (z), wie bei der Kippgeschwindigkeit
      continue;
    }
    if (v.sensorId != SH2_GYROSCOPE_CALIBRATED) continue;
    rate = v.un.gyroscope.z;                                                    // Achse quer zur Laufrichtung (Einbaulage: Platine senkrecht am Rücken)
    const float mag = sqrtf(v.un.gyroscope.x * v.un.gyroscope.x + v.un.gyroscope.y * v.un.gyroscope.y + rate * rate);
    quiet = mag < 0.08f ? quiet + 0.005f : 0.f;
  }
  *still = quiet > 0.3f;
  return rate;
}
static void showLed(Led led, int count) {
  static uint32_t t0 = millis();
  uint32_t col = 0;
  switch (led) {
    case Led::Green: col = ring.Color(0, 160, 40); break;
    case Led::Blue: col = ring.Color(20, 60, 200); break;
    case Led::Yellow: col = ring.Color(200, 140, 0); break;
    case Led::Red: col = ring.Color(220, 0, 0); break;
    case Led::WhitePulse: { const uint8_t w = uint8_t(60 + 60 * sinf((millis() - t0) / 250.f)); col = ring.Color(w, w, w); break; }
    default: break;
  }
  for (int i = 0; i < 12; i++) ring.setPixelColor(i, (led == Led::Red || led == Led::WhitePulse || i < count) ? col : ring.Color(2, 2, 2));
  ring.show();
}
static void peripheralsBegin() {
  Wire.begin(); Wire.setClock(400000);                                          // 400 kHz: ein Bericht (≈ 20 Bytes) dauert sonst länger als ein Regeltakt
  imuOk = imu.begin_I2C(); if (imuOk) imuReports();
  ring.begin(); ring.setBrightness(80); ring.show();
  // Hardware-Watchdog: 0,5 s ohne Lebenszeichen → Neustart; dabei fällt das Schütz ab (Pin wird beim Reset Eingang)
  CCM_CCGR3 |= CCM_CCGR3_WDOG1(CCM_CCGR_ON);
  WDOG1_WMCR = 0;                                                               // Power-Down-Zähler aus
  WDOG1_WCR = WDOG_WCR_WT(0) | WDOG_WCR_WDE | WDOG_WCR_WDA | WDOG_WCR_SRS;      // 0,5 s, aktiv
}
static void watchdogFeed() { WDOG1_WSR = 0x5555; WDOG1_WSR = 0xAAAA; }
#else
float readTrunkTiltRate(bool* still);
static bool imuAlive() { return true; }
static float trunkPitch = 0.f;
static unsigned long rtcNow = 14 * 3600 + 32 * 60;
static unsigned long rtc_get() { return rtcNow; }
static void rtc_set(unsigned long t) { rtcNow = t; }
void showLed(Led led, int count);
static void peripheralsBegin() {}
static void watchdogFeed() {}
#endif

// ---------- Statuszeile an den ESP32-C3 (Display, Handy) ----------
// Format: S,<Modus>,<Akku %>,<Stufe 1–3>,<Restzeit min>,<hh:mm>,<Stunden>,<Grund>\n
// Restzeit: Akkustand mal typische Laufzeit (≈ 2 h 45 min bei voller Ladung, am echten Exo nachmessen).
static void sendStatus(const Outputs&, float batteryV) {
  static const char* kMode[] = {"AUS", "NULLEN", "AKTIV", "AUSLAUF", "FEHLER"};
  const int pct = int(constrain((batteryV - 30.f) / 11.f * 100.f, 0.f, 100.f));
  const unsigned long t = rtc_get() % 86400UL;
  char b[96];
  const char* why = app.sup.reason && app.sup.reason[0] ? app.sup.reason : notice;   // Fehlergrund hat Vorrang vor Hinweisen („Akku schwach“, „Motor warm“ …)
  snprintf(b, sizeof b, "S,%s,%d,%d,%d,%02lu:%02lu,%.1f,%.39s\n", kMode[int(app.sup.mode)], pct, cfg.level + 1, pct * 165 / 100, t / 3600, (t / 60) % 60, double(cfg.hours), why);
  if (Serial1.availableForWrite() >= int(strlen(b))) Serial1.write(b);          // nie warten: lieber eine Zeile auslassen
}

// ---------- Messdaten-Log (Prüfstand): Grundlage für tools/auswertung.py ----------
// Spalten: t_us,q,qd,kraft_N,moment_Nm (t_ff),v_soll,kd,gesamt_Nm (t_ff + Kd·(v_soll − v)),motor_Nm (Rückmeldung, aufs Gelenk
// umgerechnet),waechter (1 = volles Mitgehen),schwing_N (Anteil > 6 Hz). Die ersten 5 Spalten wie bis Rev. T2.
static bool logOn = false;
static int logJoint = ElR;
static void setLog(bool on, int joint) { logOn = on; logJoint = joint; }
static void logLine(const Outputs& o) {
  const int i = logJoint;
  const float motorNm = motorState[i].torque * kTr[i].ratio * kTr[i].eta;
  char b[160];
  snprintf(b, sizeof b, "%lu,%.5f,%.4f,%.2f,%.3f,%.3f,%.3f,%.3f,%.3f,%.2f,%.2f\n", (unsigned long)micros(), double(in.joint[i].q), double(in.joint[i].qd),
           double(in.joint[i].cuffN), double(o.tau[i]), double(o.vDes[i]), double(o.kd[i]), double(o.tauTotal[i]), double(motorNm),
           double(o.guard[i]), double(app.joint[i].oscN));
  if (Serial.availableForWrite() >= int(strlen(b))) Serial.write(b);          // nie blockieren
}

// ---------- Einstellungen und Konsole ----------
static bool saveSettings(const Settings& s) { EEPROM.put(0, s); Settings chk; EEPROM.get(0, chk); return valid(chk); }
static void consolePrint(const char* t) { Serial.print(t); }
static void setClock(int h, int m) { const unsigned long day = rtc_get() / 86400UL * 86400UL; rtc_set(day + h * 3600UL + m * 60UL); }
static int32_t consoleRaw(int c) {
  int64_t sum = 0;
  for (int i = 0; i < 16; i++) {
    if (PIN_HX_DOUT[c] < 0) { while (!adsRead()) watchdogFeed(); sum += cellRaw[c]; continue; }
    while (digitalRead(PIN_HX_DOUT[c]) == HIGH) watchdogFeed();
    sum += readCellRaw(c);
  }
  return int32_t(sum / 16);
}
static void pollConsole() {
  static char line[48]; static size_t n = 0;
  while (Serial.available()) {
    const char ch = char(Serial.read());
    if (ch == '\n' || ch == '\r') {
      if (n) { line[n] = 0; if (app.sup.mode == Mode::Off) { ConsoleIO io; io.rawCell = consoleRaw; io.print = consolePrint; io.save = saveSettings; io.armJointZero = armJointZero; io.setLog = setLog; io.setClock = setClock; runCommand(line, cfg, io); app.setLevel(cfg.level); app.jointMask = cfg.jointMask; }
               else Serial.print("Konsole nur, wenn das Exo aus ist\n"); }
      n = 0;
    } else if (n < sizeof line - 1) line[n++] = ch;
  }
}

void setup() {
  pinMode(PIN_BTN_POWER, INPUT_PULLUP);
  for (int i = 0; i < NL; i++) { pinMode(PIN_BTN_LOOP[i], INPUT_PULLUP); pinMode(PIN_LOOP_PWM[i], OUTPUT); pinMode(PIN_LOOP_DIR[i], OUTPUT); pinMode(PIN_LOOP_END[i], INPUT_PULLUP); }
  pinMode(PIN_ESTOP_SENSE, INPUT_PULLUP); pinMode(PIN_CONTACTOR, OUTPUT); pinMode(PIN_LIGHT, OUTPUT); pinMode(PIN_LUMB_LOCK, OUTPUT); pinMode(PIN_HEAT_LEGS, OUTPUT); pinMode(PIN_HEAT_ARMS, OUTPUT); pinMode(PIN_VIBE, OUTPUT); pinMode(PIN_STAND, INPUT_PULLUP);
  digitalWrite(PIN_CONTACTOR, LOW);                                             // Schütz erst mit dem Einschalten
  pinMode(PIN_HX_SCK, OUTPUT); for (int c = 0; c < kCells; c++) if (PIN_HX_DOUT[c] >= 0) pinMode(PIN_HX_DOUT[c], INPUT);
  adsBegin();
  Serial.begin(115200); Serial1.begin(115200);
  EEPROM.get(0, cfg);
  if (!valid(cfg)) cfg = Settings();                                            // ungültig oder neu: Werkseinstellungen
  app.setLevel(cfg.level); app.jointMask = cfg.jointMask;
  can1.begin(); can1.setBaudRate(1000000); can3.begin(); can3.setBaudRate(1000000);
  peripheralsBegin();
  ctlTimer.begin([] { tick = true; tickCount++; }, 2000);                                    // 500 Hz (Mitgehen braucht ≤ 2 ms Takt, Studie tools/transparenz.cpp)
}

void loop() {
  if (!tick) return;
  tick = false;
  watchdogFeed();
  // Regeltakt überwachen: Dauert ein Durchlauf länger als 2 ms, fehlen Takte. Die Zeit läuft dann mit dem echten Abstand weiter;
  // fehlen in einer Sekunde mehr als 100 von 500 (unter 400 Hz ist das Mitgehen nicht mehr nachgewiesen stabil), ist das ein Fehler.
  static uint32_t seenTick = 0, missedWin = 0, winTicks = 0; static bool timingOk = true;
  const uint32_t tc = tickCount, missed = tc - seenTick > 1u ? tc - seenTick - 1u : 0u; seenTick = tc;
  missedWin += missed;
  if (++winTicks >= 500) {
    timingOk = missedWin <= 100;
    if (missedWin && Serial.availableForWrite() >= 48) { char b[48]; snprintf(b, sizeof b, "Regeltakt: %lu Takte ausgelassen\n", (unsigned long)missedWin); Serial.write(b); }
    missedWin = 0; winTicks = 0;
  }
  const float dt = 0.002f * float(1u + (missed > 4u ? 4u : missed));
  in.btnPower = !digitalRead(PIN_BTN_POWER);
  for (int i = 0; i < NL; i++) {
    in.btnLoop[i] = !digitalRead(PIN_BTN_LOOP[i]);
    in.loopCurrentA[i] = analogRead(PIN_LOOP_CUR[i]) * (3.3f / 1023.f) / 0.5f;  // Stromsensor 0,5 V/A
    in.loopOpenSwitch[i] = !digitalRead(PIN_LOOP_END[i]);
  }
  in.safety.sensorsOk = readCuffs(in.joint);
  in.safety.canOk = readMotors(in.joint);
  in.safety.estopOk = digitalRead(PIN_ESTOP_SENSE) == LOW;
  in.safety.parkStandDown = !digitalRead(PIN_STAND);
  in.safety.batteryV = analogRead(PIN_VBAT) * (3.3f / 1023.f) * 15.f;          // Spannungsteiler 1:15
  in.safety.trunkTiltRate = readTrunkTiltRate(&in.standingStill);
  in.safety.imuOk = imuAlive();
  in.safety.timingOk = timingOk;
  in.trunkPitch = trunkPitch;
  for (int k = 0; k < 2; k++) in.footContact[k] = cellN[9 + k] > kFootContactN;   // 4 Halbbrücken-Zellen je Steigbügel als Vollbrücke an einem HX711

  const Mode before = app.sup.mode;
  const Outputs o = app.step(in, dt);
  notice = o.notice;
  static const char* lastReason = "";
  if (app.sup.reason != lastReason) {                                           // Fehlergrund mit Einzelheit auf die USB-Konsole
    lastReason = app.sup.reason;
    if (lastReason[0] && Serial.availableForWrite() >= 120) { char b[120]; snprintf(b, sizeof b, "%s (%s; CRC-Fehler %lu, Wandler-Neustarts %lu)\n", lastReason, sensorDetail, (unsigned long)adsCrcErrors, (unsigned long)adsResets); Serial.write(b); }
  }
  static bool dirty = false;
  // Gelenk-Nullpunkte einmessen (Konsole gelenk0): beim Wechsel NULLEN → AKTIV den Motorwinkel übernehmen, dann gleich wieder aus
  if (zeroArm && before == Mode::Zeroing && app.sup.mode == Mode::Active) {
    for (int i = 0; i < NJ; i++) if ((zeroArm >> i) & 1u) cfg.joint0[i] = motorState[i].pos;
    zeroArm = 0; dirty = true; app.sup.powerButton(Press::Long);
    Serial.print("Gelenk-Nullpunkte übernommen; das Exo schaltet aus und speichert\n");
  }

  // Schütz: an, solange das Exo nullt, aktiv ist oder ausläuft (auch beim gedämpften Absinken nach einem weichen Fehler).
  // Motoren nach dem Hochfahren einzeln einschalten (ein Motor je Takt, damit der Sendepuffer nicht überläuft); wer danach
  // nicht im Betriebszustand ist, bekommt alle 0,1 s einen neuen Versuch. Ab 1 s gilt das als Fehler (readMotors).
  const bool power = app.sup.mode == Mode::Zeroing || app.sup.mode == Mode::Active || app.sup.mode == Mode::SoftOff;
  digitalWrite(PIN_CONTACTOR, power ? HIGH : LOW);
  powerOnTime = power ? powerOnTime + dt : 0.f;
  static int enableNext = 0; static float retryT = 0.f;
  if (!power) { motorsOn = false; enableNext = 0; retryT = 0.f; }               // ohne Strom sind sie ohnehin aus
  else if (powerOnTime > 0.3f && enableNext < NJ) {
    if ((cfg.jointMask >> enableNext) & 1u) motorEnable(enableNext);
    if (++enableNext >= NJ) motorsOn = true;
  } else if (motorsOn && powerOnTime < 1.0f && (retryT += dt) >= 0.1f) {
    retryT = 0.f;
    for (int i = 0; i < NJ; i++) if (((cfg.jointMask >> i) & 1u) && motorState[i].mode != kMotorRun) { motorEnable(i); break; }
  }
  if (motorsOn) for (int i = 0; i < NJ; i++) if ((cfg.jointMask >> i) & 1u) motorSend(i, o.tau[i], o.vDes[i], o.kd[i]);
  if (logOn) logLine(o);
  for (int i = 0; i < NL; i++) { digitalWrite(PIN_LOOP_DIR[i], o.winch[i] >= 0.f); analogWrite(PIN_LOOP_PWM[i], int(fabsf(o.winch[i]) * 255)); }
  digitalWrite(PIN_LIGHT, o.workLight); digitalWrite(PIN_VIBE, o.vibrate); analogWrite(PIN_LUMB_LOCK, (int)(o.lumbarDuty * 255.f + 0.5f));
  digitalWrite(PIN_HEAT_LEGS, o.heaterLegs); digitalWrite(PIN_HEAT_ARMS, o.heaterArms);

  // Betriebsstunden: gespeichert wird nur, wenn das Exo aus ist (Flash-Schreiben kann den Regeltakt stören)
  if (app.sup.mode == Mode::Active) { cfg.hours += dt / 3600.f; dirty = true; }
  if (dirty && (app.sup.mode == Mode::Off || app.sup.mode == Mode::Fault)) { dirty = false; seal(cfg); saveSettings(cfg); }

  static int div = 0;
  if (++div >= 25) { div = 0; showLed(o.led, o.ledCount); }                    // 20 Hz
  static int div2 = 0;
  if (++div2 >= 500) { div2 = 0; sendStatus(o, in.safety.batteryV); }        // 1 Hz an Display und Handy
  pollConsole();
}
#endif
