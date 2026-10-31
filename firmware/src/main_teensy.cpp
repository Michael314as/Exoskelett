// Teensy 4.1: liest die Hardware, ruft App::step() 200-mal pro Sekunde auf und schreibt die Ausgänge.
// Bibliotheken (PlatformIO/Arduino): FlexCAN_T4 (CAN zu den Motoren), HX711 oder SparkFun NAU7802 (Wägezellen),
// Adafruit_BNO08x (IMU), Adafruit_NeoPixel (LED-Ring), GxEPD2 (E-Paper).
//
// Die Motor-Kommandos hängen vom gewählten Motortyp ab (MIT-Protokoll bei CubeMars/MyActuator-Klasse).
// Unten steht dafür motorSendTorque(); sie ist der einzige Teil, der zum Motor-Datenblatt passen muss.
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

// ---------- Hardware-Anbindung (je nach Motor/Sensor ausfüllen) ----------
bool readCuffs(JointInput* j);                       // 8 Wägezellen → 6 Gelenkkräfte (Beine: max(Oberschenkel, Unterschenkel))
bool readMotors(JointInput* j);                      // Winkel + Geschwindigkeit über CAN; false = Motor fehlt
void motorSendTorque(int id, float tauJoint);        // Gelenkmoment → Motormoment (÷ Übersetzung) → CAN
float readTrunkTiltRate(bool* still);                // IMU
void showLed(Led led, int count);
void showDisplay(const Outputs& o, float batteryV);

void setup() {
  pinMode(PIN_BTN_POWER, INPUT_PULLUP);
  for (int i = 0; i < NL; i++) { pinMode(PIN_BTN_LOOP[i], INPUT_PULLUP); pinMode(PIN_LOOP_PWM[i], OUTPUT); pinMode(PIN_LOOP_DIR[i], OUTPUT); pinMode(PIN_LOOP_END[i], INPUT_PULLUP); }
  pinMode(PIN_ESTOP_SENSE, INPUT_PULLUP); pinMode(PIN_CONTACTOR, OUTPUT); pinMode(PIN_LIGHT, OUTPUT); pinMode(PIN_VIBE, OUTPUT); pinMode(PIN_STAND, INPUT_PULLUP);
  digitalWrite(PIN_CONTACTOR, LOW);                  // Schütz erst ziehen, wenn alles geprüft ist
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
