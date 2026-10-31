// Teensy 4.1: liest die Hardware, ruft App::step() 200-mal pro Sekunde auf und schreibt die Ausgänge.
// Bauen: pio run (siehe platformio.ini). Pinbelegung: docs/verdrahtung.md (muss mit PIN_* unten übereinstimmen).
//
// Motoren: CubeMars AK70-10 (oder baugleich) im MIT-Modus über CAN1, 1 Mbit/s. Wägezellen: 8 × HX711.
// Lagesensor: BNO085 (I2C). LED-Ring: 12 × WS2812. Watchdog: WDOG1 des i.MX RT1062 (direkt über Register, 0,5 s).
// Anzeige und Handy-Funk macht ein ESP32-C3 an Serial1: Der Teensy schickt ihm jede Sekunde eine Statuszeile.
// (Ein E-Paper-Update blockiert mehrere 100 ms und darf deshalb nicht im Regeltakt laufen.)
#ifdef ARDUINO
#include <Arduino.h>
#include <EEPROM.h>
#include <FlexCAN_T4.h>
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
constexpr int PIN_LOOP_PWM[NL] = {6, 7, 8};                                   // 10–13 frei (SPI) für Erweiterungen
constexpr int PIN_LOOP_CUR[NL] = {14, 15, 16}, PIN_VBAT = 17;                  // analog A0–A3; I2C: 18 SDA, 19 SCL
constexpr int PIN_LED_RING = 20, PIN_VIBE = 21;                                // CAN1: 22 TX, 23 RX
constexpr int PIN_LOOP_DIR[NL] = {24, 25, 26}, PIN_LOOP_END[NL] = {27, 28, 29};
constexpr int PIN_ESTOP_SENSE = 30, PIN_CONTACTOR = 31, PIN_STAND = 32, PIN_LIGHT = 33, PIN_HX_SCK = 34;
constexpr int PIN_HX_DOUT[8] = {9, 35, 36, 37, 38, 39, 40, 41};               // 0/1 Oberschenkel L/R, 2/3 Unterschenkel, 4/5 Oberarm, 6/7 Unterarm

static App app;
static Inputs in;
static Settings cfg;
static IntervalTimer ctlTimer;
static volatile bool tick = false;

// ---------- Motoren ----------
static FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can;
static const MotorLimits kLim;                                                  // AK70-10
static const uint8_t kCanId[NJ] = {1, 2, 3, 4, 5, 6};                          // LegL, LegR, ShL, ShR, ElL, ElR
static const Transmission kTr[NJ] = {{6.3f, 0.9f}, {6.3f, 0.9f}, {4.67f, 0.88f}, {4.67f, 0.88f}, {3.33f, 0.7f}, {3.33f, 0.7f}};
static MotorState motorState[NJ];
static uint32_t lastReplyMs[NJ];
static float powerOnTime = 0.f;                                                 // Zeit seit Schütz zu
static bool motorsOn = false;

static void motorsEnable(bool on) {
  for (int i = 0; i < NJ; i++) { CAN_message_t m; m.id = kCanId[i]; m.len = 8; packSpecial(on ? kEnterMotorMode : kExitMotorMode, m.buf); can.write(m); }
  motorsOn = on;
}
static bool readMotors(JointInput* j) {
  CAN_message_t m;
  while (can.read(m)) {
    if (m.len < 6) continue;
    const MotorState s = unpackReply(m.buf, kLim);
    for (int i = 0; i < NJ; i++) if (kCanId[i] == s.id) { motorState[i] = s; lastReplyMs[i] = millis(); }
  }
  bool ok = true;
  for (int i = 0; i < NJ; i++) {
    j[i].q = kTr[i].jointAngleFrom(motorState[i].pos - cfg.joint0[i]); j[i].qd = kTr[i].jointVelFrom(motorState[i].vel);
    if (((cfg.jointMask >> i) & 1u) && millis() - lastReplyMs[i] > 50) ok = false;   // nur eingebaute Motoren prüfen
  }
  return powerOnTime < 0.6f ? true : ok;          // Motoren brauchen nach dem Schütz einen Moment zum Hochfahren
}
static void motorSendTorque(int id, float tauJoint) {
  CAN_message_t m; m.id = kCanId[id]; m.len = 8;
  packTorqueCommand(kTr[id].motorTorqueFor(tauJoint), kLim, m.buf);
  can.write(m);
}

// ---------- Wägezellen ----------
static float cellN[8];
static int32_t cellRaw[8];
static int32_t readCellRaw(int c) {
  return hx711ReadRaw([c] { digitalWrite(PIN_HX_SCK, HIGH); delayMicroseconds(1); const bool b = digitalRead(PIN_HX_DOUT[c]);
                            digitalWrite(PIN_HX_SCK, LOW); delayMicroseconds(1); return b; });
}
static bool readCuffs(JointInput* j) {
  bool ok = true;
  for (int c = 0; c < 8; c++) {
    if (digitalRead(PIN_HX_DOUT[c]) == HIGH) continue;                          // noch kein neuer Wert (80 Hz)
    cellRaw[c] = readCellRaw(c);
    if (!cfg.cell[c].plausible(cellRaw[c])) { ok = false; continue; }
    cellN[c] = cfg.cell[c].toNewton(cellRaw[c]);
  }
  j[LegL].cuffN = fmaxf(cellN[0], cellN[2]); j[LegR].cuffN = fmaxf(cellN[1], cellN[3]);   // ein Seil: stärkerer Streckwunsch
  j[ShL].cuffN = cellN[4]; j[ShR].cuffN = cellN[5]; j[ElL].cuffN = cellN[6]; j[ElR].cuffN = cellN[7];
  return ok;
}

// ---------- Lagesensor, LED-Ring, Anzeige, Watchdog ----------
#ifndef EXO_HOST_TEST
static Adafruit_BNO08x imu(-1);
static Adafruit_NeoPixel ring(12, PIN_LED_RING, NEO_GRB + NEO_KHZ800);
static bool imuOk = false;

static float readTrunkTiltRate(bool* still) {
  static float rate = 0.f, quiet = 0.f;
  sh2_SensorValue_t v;
  while (imuOk && imu.getSensorEvent(&v)) {
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
  Wire.begin();
  imuOk = imu.begin_I2C() && imu.enableReport(SH2_GYROSCOPE_CALIBRATED, 5000);
  ring.begin(); ring.setBrightness(80); ring.show();
  // Hardware-Watchdog: 0,5 s ohne Lebenszeichen → Neustart; dabei fällt das Schütz ab (Pin wird beim Reset Eingang)
  CCM_CCGR3 |= CCM_CCGR3_WDOG1(CCM_CCGR_ON);
  WDOG1_WMCR = 0;                                                               // Power-Down-Zähler aus
  WDOG1_WCR = WDOG_WCR_WT(0) | WDOG_WCR_WDE | WDOG_WCR_WDA | WDOG_WCR_SRS;      // 0,5 s, aktiv
}
static void watchdogFeed() { WDOG1_WSR = 0x5555; WDOG1_WSR = 0xAAAA; }
#else
float readTrunkTiltRate(bool* still);
void showLed(Led led, int count);
static void peripheralsBegin() {}
static void watchdogFeed() {}
#endif

// ---------- Statuszeile an den ESP32-C3 (Display, Handy) ----------
// Format: S,<Modus>,<Akku %>,<Stufe 1–3>,<Bäume heute>,<Bäume gesamt>,<Stunden>,<Grund>\n
static void sendStatus(const Outputs& o, float batteryV) {
  static const char* kMode[] = {"AUS", "NULLEN", "AKTIV", "AUSLAUF", "FEHLER"};
  const int pct = int(constrain((batteryV - 30.f) / 11.f * 100.f, 0.f, 100.f));
  char b[96];
  snprintf(b, sizeof b, "S,%s,%d,%d,%d,%lu,%.1f,%s\n", kMode[int(app.sup.mode)], pct, cfg.level + 1, o.trees, (unsigned long)cfg.treesTotal, double(cfg.hours), app.sup.reason);
  if (Serial1.availableForWrite() >= int(strlen(b))) Serial1.write(b);          // nie warten: lieber eine Zeile auslassen
}

// ---------- Messdaten-Log (Prüfstand): Grundlage für tools/auswertung.py ----------
static bool logOn = false;
static void setLog(bool on) { logOn = on; }
static void logLine(const Outputs& o) {
  char b[80];
  snprintf(b, sizeof b, "%lu,%.5f,%.4f,%.2f,%.3f\n", (unsigned long)micros(), double(in.joint[ElR].q), double(in.joint[ElR].qd), double(in.joint[ElR].cuffN), double(o.tau[ElR]));
  if (Serial.availableForWrite() >= int(strlen(b))) Serial.write(b);          // nie blockieren
}

// ---------- Einstellungen und Konsole ----------
static bool saveSettings(const Settings& s) { EEPROM.put(0, s); Settings chk; EEPROM.get(0, chk); return valid(chk); }
static void consolePrint(const char* t) { Serial.print(t); }
static int32_t consoleRaw(int c) { int64_t sum = 0; for (int i = 0; i < 16; i++) { while (digitalRead(PIN_HX_DOUT[c]) == HIGH) watchdogFeed(); sum += readCellRaw(c); } return int32_t(sum / 16); }
static void pollConsole() {
  static char line[48]; static size_t n = 0;
  while (Serial.available()) {
    const char ch = char(Serial.read());
    if (ch == '\n' || ch == '\r') {
      if (n) { line[n] = 0; if (app.sup.mode == Mode::Off) { ConsoleIO io; io.rawCell = consoleRaw; io.print = consolePrint; io.save = saveSettings; io.setLog = setLog; runCommand(line, cfg, io); app.setLevel(cfg.level); app.jointMask = cfg.jointMask; }
               else Serial.print("Konsole nur, wenn das Exo aus ist\n"); }
      n = 0;
    } else if (n < sizeof line - 1) line[n++] = ch;
  }
}

void setup() {
  pinMode(PIN_BTN_POWER, INPUT_PULLUP);
  for (int i = 0; i < NL; i++) { pinMode(PIN_BTN_LOOP[i], INPUT_PULLUP); pinMode(PIN_LOOP_PWM[i], OUTPUT); pinMode(PIN_LOOP_DIR[i], OUTPUT); pinMode(PIN_LOOP_END[i], INPUT_PULLUP); }
  pinMode(PIN_ESTOP_SENSE, INPUT_PULLUP); pinMode(PIN_CONTACTOR, OUTPUT); pinMode(PIN_LIGHT, OUTPUT); pinMode(PIN_VIBE, OUTPUT); pinMode(PIN_STAND, INPUT_PULLUP);
  digitalWrite(PIN_CONTACTOR, LOW);                                             // Schütz erst mit dem Einschalten
  pinMode(PIN_HX_SCK, OUTPUT); for (int c = 0; c < 8; c++) pinMode(PIN_HX_DOUT[c], INPUT);
  Serial.begin(115200); Serial1.begin(115200);
  EEPROM.get(0, cfg);
  if (!valid(cfg)) cfg = Settings();                                            // ungültig oder neu: Werkseinstellungen
  app.setLevel(cfg.level); app.jointMask = cfg.jointMask;
  can.begin(); can.setBaudRate(1000000);
  peripheralsBegin();
  ctlTimer.begin([] { tick = true; }, 5000);                                    // 200 Hz
}

void loop() {
  if (!tick) return;
  tick = false;
  watchdogFeed();
  const float dt = 0.005f;
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

  const Outputs o = app.step(in, dt);

  // Schütz: an, solange das Exo nullt oder aktiv ist; Motoren erst nach dem Hochfahren einschalten
  const bool power = app.sup.mode == Mode::Zeroing || app.sup.mode == Mode::Active || app.sup.mode == Mode::SoftOff;
  digitalWrite(PIN_CONTACTOR, power ? HIGH : LOW);
  powerOnTime = power ? powerOnTime + dt : 0.f;
  if (power && !motorsOn && powerOnTime > 0.3f) motorsEnable(true);
  if (!power && motorsOn) motorsOn = false;                                     // ohne Strom sind sie ohnehin aus
  if (motorsOn) for (int i = 0; i < NJ; i++) if ((cfg.jointMask >> i) & 1u) motorSendTorque(i, o.tau[i]);
  if (logOn) logLine(o);
  for (int i = 0; i < NL; i++) { digitalWrite(PIN_LOOP_DIR[i], o.winch[i] >= 0.f); analogWrite(PIN_LOOP_PWM[i], int(fabsf(o.winch[i]) * 255)); }
  digitalWrite(PIN_LIGHT, o.workLight); digitalWrite(PIN_VIBE, o.vibrate);

  // Betriebsstunden und Bäume: gespeichert wird nur, wenn das Exo aus ist (Flash-Schreiben kann den Regeltakt stören)
  static bool dirty = false; static int treesSeen = 0;
  if (app.sup.mode == Mode::Active) { cfg.hours += dt / 3600.f; dirty = true; }
  if (o.trees > treesSeen) { cfg.treesTotal += uint32_t(o.trees - treesSeen); treesSeen = o.trees; dirty = true; }
  if (dirty && app.sup.mode == Mode::Off) { dirty = false; seal(cfg); saveSettings(cfg); }

  static int div = 0;
  if (++div >= 10) { div = 0; showLed(o.led, o.ledCount); }                    // 20 Hz
  static int div2 = 0;
  if (++div2 >= 200) { div2 = 0; sendStatus(o, in.safety.batteryV); }        // 1 Hz an Display und Handy
  pollConsole();
}
#endif
