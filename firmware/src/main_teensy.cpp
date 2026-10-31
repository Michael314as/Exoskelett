// Teensy 4.1: liest die Hardware, ruft App::step() 200-mal pro Sekunde auf und schreibt die Ausgänge.
// Bibliotheken (PlatformIO/Arduino): FlexCAN_T4 (CAN zu den Motoren), HX711 oder SparkFun NAU7802 (Wägezellen),
// Adafruit_BNO08x (IMU), Adafruit_NeoPixel (LED-Ring), GxEPD2 (E-Paper).
//
// Motoren: CubeMars AK70-10 (oder baugleich) im MIT-Modus über CAN, 1 Mbit/s. Grenzen und Übersetzungen siehe drivers.h
// bzw. kTr unten. Noch offen: IMU, LED-Ring, E-Paper (Standardbibliotheken, je wenige Zeilen).
#ifdef ARDUINO
#include <Arduino.h>
#include "exo_app.h"

using namespace exo;

static App app;
static Inputs in;
static IntervalTimer ctlTimer;
static volatile bool tick = false;

// ---------- Pins (Teensy 4.1) ----------
constexpr int PIN_BTN_POWER = 2, PIN_BTN_LOOP[NL] = {3, 4, 5};
constexpr int PIN_LOOP_PWM[NL] = {6, 7, 8}, PIN_LOOP_DIR[NL] = {9, 10, 11}, PIN_LOOP_CUR[NL] = {A0, A1, A2}, PIN_LOOP_END[NL] = {24, 25, 26};
constexpr int PIN_ESTOP_SENSE = 27, PIN_CONTACTOR = 28, PIN_LIGHT = 29, PIN_VIBE = 30, PIN_STAND = 31, PIN_VBAT = A3;

// ---------- Hardware-Anbindung ----------
#include <FlexCAN_T4.h>
#include "drivers.h"
static FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can;
static const MotorLimits kLim;                                    // AK70-10
static const uint8_t kCanId[NJ] = {1, 2, 3, 4, 5, 6};             // LegL, LegR, ShL, ShR, ElL, ElR
static const Transmission kTr[NJ] = {{6.3f, 0.9f}, {6.3f, 0.9f}, {4.67f, 0.88f}, {4.67f, 0.88f}, {3.33f, 0.7f}, {3.33f, 0.7f}};
static MotorState motorState[NJ];
static uint32_t lastReplyMs[NJ];

// Wägezellen: 8 × HX711, gemeinsamer Takt, je ein Datenpin
constexpr int PIN_HX_SCK = 32, PIN_HX_DOUT[8] = {33, 34, 35, 36, 37, 38, 39, 40};
// Zuordnung: 0/1 Oberschenkel L/R, 2/3 Unterschenkel L/R, 4/5 Oberarm L/R, 6/7 Unterarm L/R
static CellCal cal[8];
static float cellN[8];

bool readCuffs(JointInput* j) {
  bool ok = true;
  for (int c = 0; c < 8; c++) {
    if (digitalRead(PIN_HX_DOUT[c]) == HIGH) continue;              // noch kein neuer Wert: alten behalten (80 Hz)
    const int32_t raw = hx711ReadRaw([c] { digitalWrite(PIN_HX_SCK, HIGH); delayMicroseconds(1); const bool b = digitalRead(PIN_HX_DOUT[c]);
                                           digitalWrite(PIN_HX_SCK, LOW); delayMicroseconds(1); return b; });
    if (!cal[c].plausible(raw)) { ok = false; continue; }
    cellN[c] = cal[c].toNewton(raw);
  }
  // Beine: ein Seil für Hüfte und Knie → der stärkere Streckwunsch zählt
  j[LegL].cuffN = fmaxf(cellN[0], cellN[2]); j[LegR].cuffN = fmaxf(cellN[1], cellN[3]);
  j[ShL].cuffN = cellN[4]; j[ShR].cuffN = cellN[5]; j[ElL].cuffN = cellN[6]; j[ElR].cuffN = cellN[7];
  return ok;
}

bool readMotors(JointInput* j) {
  CAN_message_t m;
  while (can.read(m)) {
    if (m.len < 6) continue;
    const MotorState s = unpackReply(m.buf, kLim);
    for (int i = 0; i < NJ; i++) if (kCanId[i] == s.id) { motorState[i] = s; lastReplyMs[i] = millis(); }
  }
  bool ok = true;
  for (int i = 0; i < NJ; i++) {
    j[i].q = kTr[i].jointAngleFrom(motorState[i].pos); j[i].qd = kTr[i].jointVelFrom(motorState[i].vel);
    if (millis() - lastReplyMs[i] > 50) ok = false;                 // Motor antwortet nicht
  }
  return ok;
}

void motorSendTorque(int id, float tauJoint) {
  CAN_message_t m; m.id = kCanId[id]; m.len = 8;
  packTorqueCommand(kTr[id].motorTorqueFor(tauJoint), kLim, m.buf);
  can.write(m);
}

float readTrunkTiltRate(bool* still);                              // IMU (BNO08x): Drehrate um die Querachse
void showLed(Led led, int count);                                  // NeoPixel-Ring
void showDisplay(const Outputs& o, float batteryV);                // E-Paper

static void motorsEnable(bool on) {
  for (int i = 0; i < NJ; i++) { CAN_message_t m; m.id = kCanId[i]; m.len = 8; packSpecial(on ? kEnterMotorMode : kExitMotorMode, m.buf); can.write(m); }
}

void setup() {
  pinMode(PIN_BTN_POWER, INPUT_PULLUP);
  for (int i = 0; i < NL; i++) { pinMode(PIN_BTN_LOOP[i], INPUT_PULLUP); pinMode(PIN_LOOP_PWM[i], OUTPUT); pinMode(PIN_LOOP_DIR[i], OUTPUT); pinMode(PIN_LOOP_END[i], INPUT_PULLUP); }
  pinMode(PIN_ESTOP_SENSE, INPUT_PULLUP); pinMode(PIN_CONTACTOR, OUTPUT); pinMode(PIN_LIGHT, OUTPUT); pinMode(PIN_VIBE, OUTPUT); pinMode(PIN_STAND, INPUT_PULLUP);
  digitalWrite(PIN_CONTACTOR, LOW);                  // Schütz erst ziehen, wenn alles geprüft ist
  pinMode(PIN_HX_SCK, OUTPUT); for (int c = 0; c < 8; c++) pinMode(PIN_HX_DOUT[c], INPUT);
  can.begin(); can.setBaudRate(1000000);
  motorsEnable(true);
  ctlTimer.begin([] { tick = true; }, 5000);         // 200 Hz
  // Hardware-Watchdog: hängt loop() länger als 50 ms, startet der Teensy neu und das Schütz fällt ab.
}

void loop() {
  if (!tick) return;
  tick = false;
  const float dt = 0.005f;
  in.btnPower = !digitalRead(PIN_BTN_POWER);
  for (int i = 0; i < NL; i++) {
    in.btnLoop[i] = !digitalRead(PIN_BTN_LOOP[i]);
    in.loopCurrentA[i] = analogRead(PIN_LOOP_CUR[i]) * (3.3f / 1023.f) / 0.5f;   // Stromsensor 0,5 V/A
    in.loopOpenSwitch[i] = !digitalRead(PIN_LOOP_END[i]);
  }
  in.safety.sensorsOk = readCuffs(in.joint);
  in.safety.canOk = readMotors(in.joint);
  in.safety.estopOk = digitalRead(PIN_ESTOP_SENSE) == LOW;
  in.safety.parkStandDown = !digitalRead(PIN_STAND);
  in.safety.batteryV = analogRead(PIN_VBAT) * (3.3f / 1023.f) * 15.f;           // Spannungsteiler 1:15
  in.safety.trunkTiltRate = readTrunkTiltRate(&in.standingStill);

  const Outputs o = app.step(in, dt);

  digitalWrite(PIN_CONTACTOR, app.sup.scale > 0.f || app.sup.mode == Mode::Zeroing ? HIGH : LOW);
  for (int i = 0; i < NJ; i++) motorSendTorque(i, o.tau[i]);
  for (int i = 0; i < NL; i++) { digitalWrite(PIN_LOOP_DIR[i], o.winch[i] >= 0.f); analogWrite(PIN_LOOP_PWM[i], int(fabsf(o.winch[i]) * 255)); }
  digitalWrite(PIN_LIGHT, o.workLight); digitalWrite(PIN_VIBE, o.vibrate);
  static int div = 0;
  if (++div >= 10) { div = 0; showLed(o.led, o.ledCount); }                    // 20 Hz
  static int div2 = 0;
  if (++div2 >= 400) { div2 = 0; showDisplay(o, in.safety.batteryV); }        // alle 2 s (E-Paper ist langsam)
}
#endif
