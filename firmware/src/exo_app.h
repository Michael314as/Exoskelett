// Gesamtsteuerung: verbindet 6 Gelenke, 3 Schlaufenwinden, Taster, Sicherheit, LED-Ring und Baumzähler.
// Eingänge und Ausgänge sind reine Daten; die Hardware liest/schreibt main_teensy.cpp.
#pragma once
#include "exo_core.h"

namespace exo {

enum JointId : uint8_t { LegL, LegR, ShL, ShR, ElL, ElR, NJ };
enum LoopId : uint8_t { LoopR1, LoopR2, LoopL1, NL };

struct Inputs {
  JointInput joint[NJ];
  bool btnPower = false;               // Ein/Aus-Taster am Schultergurt
  bool btnLoop[NL] = {};               // Daumentaster (rechts 2, links 1)
  float loopCurrentA[NL] = {};
  bool loopOpenSwitch[NL] = {};
  SafetyInput safety;
  bool standingStill = true;           // IMU: ruhig (zum Nullen)
};

struct Outputs {
  float tau[NJ] = {};                  // Moment je Gelenk [Nm] → Motor über CAN
  float winch[NL] = {};                // −1…+1
  Led led = Led::Off;
  int ledCount = 0;                    // leuchtende LEDs (Akku)
  bool workLight = false;
  bool vibrate = false;                // kurzer Impuls bei Schlaufe zu/auf
  int trees = 0;
};

inline JointConfig defaultJoint(JointId id) {
  JointConfig c;
  switch (id) {
    case LegL: case LegR:
      c.name = "Bein"; c.gain = 1.0f; c.rCuff = 0.2f; c.pullOnly = true; c.tauMin = 0.f; c.tauMax = 140.f;
      c.qMin = -0.3f; c.qMax = 2.0f; c.slew = 600.f; break;
    case ShL: case ShR:
      c.name = "Schulter"; c.gain = 1.0f; c.rCuff = 0.17f; c.tauMax = 100.f; c.tauMin = -100.f;
      c.qMin = -0.4f; c.qMax = 2.6f; c.gravCoef = 2.4f; break;              // Exo-Arm ~1,4 kg
    default:
      c.name = "Ellbogen"; c.gain = 1.0f; c.rCuff = 0.15f; c.tauMax = 58.f; c.tauMin = -58.f;
      c.qMin = 0.1f; c.qMax = 2.3f; c.gravCoef = 0.9f; break;
  }
  return c;
}

class App {
 public:
  Joint joint[NJ];
  LoopWinch loop[NL];
  Button btnPower, btnLoop[NL];
  Supervisor sup;
  TreeCounter trees;
  bool light = false;
  float gainLevel[3] = {0.5f, 1.0f, 2.0f};   // Stufen sanft / normal / kräftig
  int level = 1;

  App() { for (int i = 0; i < NJ; i++) joint[i] = Joint(defaultJoint(JointId(i))); setLevel(1); }

  void setLevel(int l) { level = l < 0 ? 0 : (l > 2 ? 2 : l); for (auto& j : joint) j.cfg.gain = gainLevel[level]; }

  Outputs step(const Inputs& in, float dt) {
    Outputs o;
    // Bedienung
    const Press pw = btnPower.step(in.btnPower, dt);
    if (pw == Press::Double) light = !light; else sup.powerButton(pw);
    // beide rechten Taster gleichzeitig = Stufenwechsel: dann keine Schlaufen-Befehle (sonst würde „lang“ eine Schlaufe öffnen)
    if (in.btnLoop[LoopR1] && in.btnLoop[LoopR2]) comboActive_ = true;
    if (!in.btnLoop[LoopR1] && !in.btnLoop[LoopR2] && comboActive_) comboRelease_ = true;
    Press lp[NL];
    for (int i = 0; i < NL; i++) {
      lp[i] = btnLoop[i].step(in.btnLoop[i], dt);
      if (comboActive_ && i != LoopL1) continue;
      loop[i].command(lp[i]); if (lp[i] != Press::None) vibe_ = 0.15f;
    }
    if (comboRelease_) { comboActive_ = comboRelease_ = false; }
    // beide rechten Daumentaster 2 s: Stufe wechseln
    if (in.btnLoop[LoopR1] && in.btnLoop[LoopR2]) { both_ += dt; if (both_ > 2.f && !levelSent_) { setLevel((level + 1) % 3); levelSent_ = true; vibe_ = 0.4f; } }
    else { both_ = 0.f; levelSent_ = false; }
    // Sicherheit
    SafetyInput s = in.safety;
    float maxAssist = 0.f;
    for (auto& j : joint) maxAssist = std::fmax(maxAssist, std::fabs(j.lastForce));
    s.anyAssist = maxAssist > 8.f;
    sup.step(s, in.standingStill, dt);
    // Gelenke
    for (int i = 0; i < NJ; i++) {
      if (sup.scale <= 0.f) { joint[i].off(); o.tau[i] = 0.f; continue; }
      o.tau[i] = joint[i].step(in.joint[i], dt, sup.scale);
    }
    // Schlaufen: unabhängig vom Gelenkmodus (auch aus bleibt alles fest)
    bool anyClosed = false; float load = 0.f;
    for (int i = 0; i < NL; i++) { o.winch[i] = loop[i].step(in.loopCurrentA[i], in.loopOpenSwitch[i], dt); anyClosed |= loop[i].state == LoopState::Closed; }
    for (int i : {ElL, ElR}) load = std::fmax(load, std::fabs(o.tau[i]) / 0.25f);
    trees.step(anyClosed, load);
    // Anzeige
    o.led = sup.led(s);
    o.ledCount = int(std::round(12.f * exo::clampf((in.safety.batteryV - 30.f) / (41.f - 30.f), 0.f, 1.f)));
    o.workLight = light;
    vibe_ = std::fmax(0.f, vibe_ - dt); o.vibrate = vibe_ > 0.f;
    o.trees = trees.count;
    return o;
  }
 private:
  float both_ = 0.f, vibe_ = 0.f;
  bool levelSent_ = false, comboActive_ = false, comboRelease_ = false;
};

}  // namespace exo
