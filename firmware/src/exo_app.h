// Gesamtsteuerung (Entwurf 4): 8 Gelenke, 3 Schlaufenwinden, Taster, Sicherheit, LED-Ring.
// Eingänge und Ausgänge sind reine Daten; die Hardware liest/schreibt main_teensy.cpp.
//
// Moment je Gelenk = Schwerkraft der Exo-Schiene + Vorsteuerung (Baumlast, Körpergewicht im Stand)
//                    + K · (Manschettenkräfte, über die Hebel auf alle Gelenke umgerechnet) + Reibungsausgleich − Dämpfung
// Vorzeichen: positiv = in +q-Richtung. Hüfte und Knie: +q = beugen, also Strecken = negatives Moment.
#pragma once
#include "exo_core.h"

namespace exo {

enum JointId : uint8_t { HipL, HipR, KneeL, KneeR, ShL, ShR, ElL, ElR, NJ };
enum LoopId : uint8_t { LoopR1, LoopR2, LoopL1, NL };   // R1 rechts unten, R2 rechts außen, L1 links unten
enum Side : uint8_t { L, R };

// Körper- und Exo-Maße für die Vorsteuerung (aus docs/kraftverteilung.md; am Prüfstand abgleichen)
struct BodyModel {
  float upperArm = 0.33f, foreArm = 0.26f;    // Schulter → Ellbogen, Ellbogen → Schlaufenkopf [m]
  float trunkCom = 0.30f;                     // Hüftachse → Schwerpunkt Oberkörper + Rahmen [m]
  float thigh = 0.43f;                        // Hüfte → Knie [m]
  float exoUpperKg = 11.f;                    // Rahmen, Box, Akku, Arme
  float bodyUpperKg = 45.f;                   // Oberkörper, Kopf, Arme des Menschen
};

struct Inputs {
  JointInput joint[NJ];
  bool btnPower = false;               // Ein/Aus-Taster am Schultergurt
  bool btnLoop[NL] = {};               // Daumentaster (rechts 2, links 1)
  float loopCurrentA[NL] = {};
  bool loopOpenSwitch[NL] = {};
  float loopN[NL] = {};                // Wägezelle im Schlaufenkopf: Zug des Stamms [N]
  bool footContact[2] = {true, true};  // Drucksensor im Steigbügel: Bein belastet
  float trunkPitch = 0.f;              // Rumpf vorgeneigt [rad] (Lagesensor)
  SafetyInput safety;
  bool standingStill = true;           // Lagesensor: ruhig (zum Nullen)
};

struct Outputs {
  float tau[NJ] = {};                  // Moment je Gelenk [Nm] → Motor über CAN
  float winch[NL] = {};                // −1…+1
  Led led = Led::Off;
  int ledCount = 0;                    // leuchtende LEDs (Akku)
  bool workLight = false;
  bool vibrate = false;                // kurzer Impuls bei Schlaufe zu/auf
  float ff[NJ] = {};                   // Anteil Vorsteuerung (für Log und Tests)
  float stance[2] = {};                // 0 = Schwungbein, 1 = Standbein (geglättet)
};

inline JointConfig defaultJoint(JointId id) {
  JointConfig c;
  switch (id) {
    case HipL: case HipR:   // Hüftmotor mit Seilgetriebe 1:6: stark strecken, schwach beugen
      c.name = "Hüfte"; c.rCuff = 0.15f; c.tauMin = -140.f; c.tauMax = 35.f; c.qMin = -0.35f; c.qMax = 2.0f;
      c.gravCoef = 2.0f; c.slew = 600.f; break;            // Exo-Bein ~2,5 kg, wirkt nur im Schwung
    case KneeL: case KneeR: // Bowdenzug-Paar; Beugen mechanisch durch die Federpatrone begrenzt (~11 Nm)
      c.name = "Knie"; c.rCuff = 0.f; c.tauMin = -110.f; c.tauMax = 10.f; c.qMin = 0.05f; c.qMax = 2.0f;
      c.gravCoef = 0.f; c.frictionComp = 0.4f; c.slew = 600.f; break;
    case ShL: case ShR:
      c.name = "Schulter"; c.rCuff = 0.17f; c.tauMax = 100.f; c.tauMin = -100.f;
      c.qMin = -0.4f; c.qMax = 2.6f; c.gravCoef = 2.4f; break;              // Exo-Arm ~1,4 kg
    default:
      c.name = "Ellbogen"; c.rCuff = 0.15f; c.tauMax = 58.f; c.tauMin = -58.f;
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
  BodyModel body;
  bool light = false;
  float gainLevel[3] = {0.5f, 1.0f, 2.0f};   // Verstärkung der Manschetten: sanft / normal / kräftig
  float loadShare[3] = {0.5f, 0.67f, 0.8f};  // so viel der Baumlast trägt das Exo von selbst
  float bodyShare[3] = {0.1f, 0.2f, 0.3f};   // so viel vom Oberkörpergewicht tragen die Exo-Beine im Stand
  float kneeSynergy = 0.6f;                  // Oberschenkel-Druck hilft auch dem Knie (Aufstehen = Hüfte und Knie)
  int level = 1;
  uint8_t jointMask = 0xFF;                  // nicht aktive Gelenke bleiben stromlos (Prüfstand)

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
    if (in.btnLoop[LoopR1] && in.btnLoop[LoopR2]) { both_ += dt; if (both_ > 2.f && !levelSent_) { setLevel((level + 1) % 3); levelSent_ = true; vibe_ = 0.4f; } }
    else { both_ = 0.f; levelSent_ = false; }
    // Sicherheit
    SafetyInput s = in.safety;
    float maxAssist = 0.f;
    for (auto& j : joint) maxAssist = std::fmax(maxAssist, std::fabs(j.lastForce));
    s.anyAssist = maxAssist > 8.f;
    sup.step(s, in.standingStill, dt);

    // Standbein erkennen (geglättet, damit beim Aufsetzen nichts springt)
    for (int k = 0; k < 2; k++) { stance_[k] += ((in.footContact[k] ? 1.f : 0.f) - stance_[k]) * clampf(dt / 0.08f, 0.f, 1.f); o.stance[k] = stance_[k]; }

    JointInput ji[NJ];
    for (int i = 0; i < NJ; i++) ji[i] = in.joint[i];
    computeCoupling(in, ji);

    for (int i = 0; i < NJ; i++) {
      if (sup.scale <= 0.f || !((jointMask >> i) & 1u)) { joint[i].off(); o.tau[i] = 0.f; continue; }
      o.tau[i] = joint[i].step(ji[i], dt, sup.scale);
      o.ff[i] = ji[i].ffNm;
    }
    // Schlaufen: unabhängig vom Gelenkmodus (auch aus bleibt alles fest)
    for (int i = 0; i < NL; i++) o.winch[i] = loop[i].step(in.loopCurrentA[i], in.loopOpenSwitch[i], dt);
    // Anzeige
    o.led = sup.led(s);
    o.ledCount = int(std::round(12.f * exo::clampf((in.safety.batteryV - 30.f) / (41.f - 30.f), 0.f, 1.f)));
    o.workLight = light;
    vibe_ = std::fmax(0.f, vibe_ - dt); o.vibrate = vibe_ > 0.f;
    return o;
  }

 private:
  float both_ = 0.f, vibe_ = 0.f, stance_[2] = {1.f, 1.f};
  bool levelSent_ = false, comboActive_ = false, comboRelease_ = false;

  // Gefilterte Manschettenkraft eines Gelenks (aus dem letzten Takt) ohne Totband-Anteil
  float cuffForce(int i) const { return deadband(joint[i].lastForce, joint[i].cfg.deadbandN); }

  // Kopplung der Gelenke: Manschettenkräfte und Baumlast über die Hebel auf alle beteiligten Gelenke
  void computeCoupling(const Inputs& in, JointInput* ji) {
    const float g = 9.81f;
    // Arme: Druck am Unterarm wirkt auch an der Schulter (Hebel = Abstand Schulter → Manschette quer zum Unterarm)
    for (int k = 0; k < 2; k++) {
      const int sh = k ? ShR : ShL, el = k ? ElR : ElL;
      const float qe = in.joint[el].q, qs = in.joint[sh].q + in.trunkPitch;
      const float rFo = joint[el].cfg.rCuff + body.upperArm * std::cos(qe);
      ji[sh].crossNm = cuffForce(el) * rFo;
      // Baumlast am Handgelenk (senkrecht): Momente an Schulter und Ellbogen, Anteil je Stufe
      const float fLoop = k ? (in.loopN[LoopR1] + in.loopN[LoopR2]) : in.loopN[LoopL1];
      const float a = loadShare[level] * std::fmax(0.f, fLoop);
      ji[el].ffNm = a * body.foreArm * std::sin(qs + qe);
      ji[sh].ffNm = a * (body.upperArm * std::sin(qs) + body.foreArm * std::sin(qs + qe));
    }
    // Beine: im Stand tragen sie einen Teil von Oberkörper, Exo und Baum; im Schwung laufen sie frei mit
    const float loadKg = (in.loopN[LoopR1] + in.loopN[LoopR2] + in.loopN[LoopL1]) / g;
    const float carryKg = body.exoUpperKg + bodyShare[level] * body.bodyUpperKg + loadShare[level] * std::fmax(0.f, loadKg);
    const float stanceSum = std::fmax(0.5f, stance_[0] + stance_[1]);
    for (int k = 0; k < 2; k++) {
      const int hip = k ? HipR : HipL, knee = k ? KneeR : KneeL;
      const float st = stance_[k], share = st / stanceSum;                  // Last auf die belasteten Beine verteilen
      const float w = carryKg * g * share;
      // Hüfte: Oberkörper neigt sich vor → Exo streckt (negativ). Knie: Hebel des Gewichts wächst mit der Beugung.
      ji[hip].ffNm = -w * body.trunkCom * std::sin(std::fmax(0.f, in.trunkPitch));
      ji[knee].ffNm = -w * 0.5f * body.thigh * std::sin(clampf(in.joint[knee].q, 0.f, 1.6f));
      // Druck am Oberschenkel: Hüfte direkt, Knie mit (Aufstehen und Hinsetzen sind Hüfte und Knie zusammen)
      ji[knee].crossNm = kneeSynergy * cuffForce(hip) * joint[hip].cfg.rCuff;
      // Schwerkraft der Exo-Beinschiene zählt nur im Schwung (im Stand trägt der Boden)
      ji[hip].gravScale = 1.f - st;
      // Standbein: nie aktiv beugen (Knie könnte einknicken); Hüfte nur wenig
      ji[knee].tauMaxNow = (1.f - st) * joint[knee].cfg.tauMax;
      ji[hip].tauMaxNow = joint[hip].cfg.tauMax * (1.f - 0.85f * st);
    }
  }
};

}  // namespace exo
