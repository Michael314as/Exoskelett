// Gesamtsteuerung (Entwurf 4): 8 Gelenke, 3 Schlaufenwinden, Taster, Sicherheit, LED-Ring.
// Eingänge und Ausgänge sind reine Daten; die Hardware liest/schreibt main_teensy.cpp.
//
// Moment je Gelenk = Schwerkraft der Exo-Schiene + Vorsteuerung (Baumlast, Körpergewicht im Stand)
//                    + K · (Manschettenkräfte, über die Hebel auf alle Gelenke umgerechnet) + Reibungsausgleich − Dämpfung
// Vorzeichen: positiv = in +q-Richtung. Hüfte und Knie: +q = beugen, also Strecken = negatives Moment.
#pragma once
#include "exo_core.h"
#include <cstring>

namespace exo {

enum JointId : uint8_t { HipL, HipR, KneeL, KneeR, ShL, ShR, ElL, ElR, Lumb, Twist, NJ };   // Lumb: Rumpf beugen, Twist: Rumpf drehen
enum LoopId : uint8_t { LoopR1, LoopR2, LoopL1, NL };   // R1 rechts unten, R2 rechts außen, L1 links unten
enum Side : uint8_t { L, R };

// Körper- und Exo-Maße für die Vorsteuerung (aus docs/kraftverteilung.md; am Prüfstand abgleichen)
struct BodyModel {
  float upperArm = 0.33f, foreArm = 0.26f;    // Schulter → Ellbogen, Ellbogen → Schlaufenkopf [m]
  float trunkCom = 0.30f;                     // Hüftachse → Schwerpunkt Oberkörper + Rahmen [m]
  float thigh = 0.43f;                        // Hüfte → Knie [m]
  float exoUpperKg = 29.5f;                   // Exo-Oberteil (Entwurf 5, Manschetten M2): Rahmen, Elektronikbox, Akku, Arme mit Schulter- und Ellbogenkapseln, Gurte
  float exoComUp = 0.34f, exoComBack = 0.04f; // Schwerpunkt Exo-Oberteil über / hinter der Hüftachse [m]; ohne Motorbox fast senkrecht über der Hüfte
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
  float tau[NJ] = {};                  // Moment je Gelenk [Nm] → Motor über CAN (Vorsteuer-Anteil t_ff)
  float vDes[NJ] = {};                 // Mitgehen: Sollgeschwindigkeit je Gelenk [rad/s] → Motor (Typ 1, v_soll)
  float kd[NJ] = {};                   // Mitgehen: Dämpfung im Motor, aufs Gelenk bezogen [Nm·s/rad] (0 = reines Moment)
  float guard[NJ] = {};                // Schwingungswächter je Gelenk (1 = volles Mitgehen)
  float tauTotal[NJ] = {};             // Gesamtmoment am Gelenk, wie es der Motor rechnet: tau + Kd·(vSoll − v) (Tests, Log)
  float winch[NL] = {};                // −1…+1
  Led led = Led::Off;
  int ledCount = 0;                    // leuchtende LEDs (Akku)
  bool workLight = false;
  bool vibrate = false;                // kurzer Impuls bei Schlaufe zu/auf
  float ff[NJ] = {};                   // Anteil Vorsteuerung (für Log und Tests)
  float stance[2] = {};                // 0 = Schwungbein, 1 = Standbein (geglättet)
  bool lumbarLock = false;             // Lendengelenk sperren (Hubmagnet): Baum in den Schlaufen
  float lumbarDuty = 0.f;              // PWM für den Hubmagneten: 0,2 s voll anziehen, dann 35 % halten (0,54 → 0,19 A am 24-V-Wandler)
  bool warnBackward = false;           // Rücklage: Rumpf zu weit nach hinten
  float heat[NJ] = {};                 // Wärmeschätzung je Motor (1 = Dauerbetrieb mit Nennmoment)
  bool heaterLegs = false, heaterArms = false;   // Heizfolien der Gelenkkapseln (Beine: Hüfte + Knie, Arme: Schulter + Ellbogen)
};

inline JointConfig defaultJoint(JointId id) {
  JointConfig c;
  switch (id) {
    // Entwurf 5: Gelenkkapseln. Hüfte und Schulter: RobStride RS04 direkt auf der Gelenkachse. Knie: RS04 oben am Oberschenkel,
    // Schubstange 1:1 (Parallelogramm). Ellbogen: RS03 oben am Oberarm, Schubstange 1:1. Keine Seile, keine Bowdenzüge.
    case HipL: case HipR:   // stark strecken, schwach beugen; RS04 120 Nm Spitze · 0,97
      c.name = "Hüfte"; c.rCuff = 0.15f; c.tauMin = -110.f; c.tauMax = 35.f; c.qMin = -0.35f; c.qMax = 2.0f;
      c.gravCoef = 2.0f; c.slew = 600.f; c.frictionComp = 0.15f; c.motorPerJoint = 1.f / 0.97f; c.motorRatedNm = 28.f; break;
    case KneeL: case KneeR: // Beugen zusätzlich mechanisch begrenzt: Federpaket in der Schubstange gibt ab ≈ 11 Nm nach
      c.name = "Knie"; c.rCuff = 0.2f; c.tauMin = -110.f; c.tauMax = 10.f; c.qMin = 0.05f; c.qMax = 2.0f;
      c.gravCoef = 0.f; c.frictionComp = 0.2f; c.slew = 600.f; c.motorPerJoint = 1.f / 0.95f; c.motorRatedNm = 28.f; break;
    case ShL: case ShR:     // RS04 direkt
      c.name = "Schulter"; c.rCuff = 0.17f; c.tauMax = 100.f; c.tauMin = -100.f; c.frictionComp = 0.15f;
      c.qMin = -0.4f; c.qMax = 2.6f; c.gravCoef = 2.4f; c.motorPerJoint = 1.f / 0.97f; c.motorRatedNm = 28.f; break;   // Exo-Arm ~2,3 kg mit Ellbogenmotor
    case Lumb:              // Rumpf beugen: RS04 am Becken, Schubstange 1:1 zum Hebel am Lendengelenk; Gasfedern tragen das Exo-Oberteil
      c.name = "Rumpf beugen"; c.rCuff = 0.3f; c.tauMin = -80.f; c.tauMax = 20.f; c.qMin = -0.15f; c.qMax = 0.45f; c.wallWidth = 0.08f;
      c.gravCoef = 0.f; c.slew = 400.f; c.frictionComp = 0.15f; c.motorPerJoint = 1.f / 0.95f; c.motorRatedNm = 28.f; break;
    case Twist:             // Rumpf drehen: RS03 am Becken, Kurbel 40 mm → Stange → Wagen der Bogenführung (≈ 1 : 1,8); Federn zentrieren
      c.name = "Rumpf drehen"; c.rCuff = 0.15f; c.tauMin = -30.f; c.tauMax = 30.f; c.qMin = -0.26f; c.qMax = 0.26f; c.wallWidth = 0.06f;
      c.gravCoef = 0.f; c.slew = 300.f; c.frictionComp = 0.1f; c.motorPerJoint = 1.f / (1.8f * 0.93f); c.motorRatedNm = 16.f; break;
    case ElL: case ElR: default:   // RS03 über Schubstange, 60 Nm Spitze · 0,95
      c.name = "Ellbogen"; c.rCuff = 0.15f; c.tauMax = 56.f; c.tauMin = -56.f; c.frictionComp = 0.15f;
      c.qMin = 0.1f; c.qMax = 2.3f; c.gravCoef = 0.9f; c.motorPerJoint = 1.f / 0.95f; c.motorRatedNm = 16.f; break;
  }
  // Mitgehen (Rev. M2, Studie tools/transparenz.cpp): schnelle Manschetten (ADS131M04, 1 kHz), 40-Hz-Filter, kleines Totband.
  // vSoll = admit · Kraft; Kd im Motor ≈ 0,75 · Segmentträgheit / 0,065 (Trägheit Exo-Segment geschätzt, am Prüfstand P39 einstellen).
  // Glieder: 0,9 rad/s je N, bis 8 rad/s (zügiges Gehen und Armschwung erreichen 5–7 rad/s; RS04/RS03 schaffen am 36-V-Akku ≈ 15 rad/s).
  c.filterHz = 40.f; c.deadbandN = 0.3f; c.admit = 0.9f; c.velMax = 8.f;
  switch (id) {
    case HipL: case HipR:   c.kdJoint = 4.0f; c.viscComp = 0.4f; break;    // Bein mit Kniekapsel ≈ 0,35 kg·m²
    case KneeL: case KneeR: c.kdJoint = 2.9f; c.viscComp = 0.5f; break;    // Unterschenkel-Schiene ≈ 0,25 kg·m²
    case ShL: case ShR:     c.kdJoint = 2.3f; c.viscComp = 0.4f; break;    // Exo-Arm mit Ellbogenkapsel ≈ 0,20 kg·m²
    case Lumb:              c.kdJoint = 10.f; c.viscComp = 0.5f; c.admit = 0.25f; c.velMax = 2.f; break;   // Oberteil träge, Gasfedern tragen: langsam mitgehen
    case Twist:             c.kdJoint = 7.f;  c.viscComp = 0.3f; c.admit = 0.35f; c.velMax = 2.f; break;   // Rucksack drehen ≈ 0,6 kg·m²
    case ElL: case ElR: default: c.kdJoint = 0.75f; c.viscComp = 0.25f; break;   // Unterarm-Schiene ≈ 0,065 kg·m²
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
  uint16_t jointMask = 0x3FF;                // nicht aktive Gelenke bleiben stromlos (Prüfstand)

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

    // Wärmeschutz je Motor, zweifach: Schätzung (Motormoment / Dauermoment in der Kapsel)² mit 3 min Zeitkonstante, und die gemessene
    // Wicklungstemperatur aus der Motor-Rückmeldung (ab 100 °C zurücknehmen, der Motor selbst schaltet bei 135–145 °C ab).
    for (int i = 0; i < NJ; i++) {
      const float r = joint[i].lastTotal * joint[i].cfg.motorPerJoint / joint[i].cfg.motorRatedNm;
      heat_[i] += (r * r - heat_[i]) * clampf(dt / 180.f, 0.f, 1.f); o.heat[i] = heat_[i];
      const float byModel = clampf(1.f - 0.6f * (heat_[i] - 1.f), 0.4f, 1.f), byTemp = clampf(1.f - (in.joint[i].motorTempC - 100.f) / 40.f, 0.4f, 1.f);
      ji[i].limitScale = std::fmin(byModel, byTemp);
    }
    // Schutz bei Sturz: Beine nicht schlaff, sondern 1,5 s weich gedämpft (bremst das Hinsetzen), dann aus
    const bool falling = sup.mode == Mode::SoftOff && sup.reason && !std::strcmp(sup.reason, "Sturz erkannt");
    if (falling && fallT_ < 0.f) fallT_ = 0.f;
    if (!falling) fallT_ = -1.f;
    if (lockOn_ > 0.3f) ji[Lumb].limitScale = 0.f;   // Lendensperre trägt: Beugemotor entlasten (kühlt ab)
    for (int i = 0; i < NJ; i++) {
      if (falling && fallT_ < 1.5f && i <= KneeR) { o.tau[i] = o.tauTotal[i] = clampf(-4.f * in.joint[i].qd, -60.f, 60.f) * (1.f - fallT_ / 1.5f); continue; }
      if (sup.scale <= 0.f || !((jointMask >> i) & 1u)) { joint[i].off(); o.tau[i] = 0.f; continue; }
      o.tau[i] = joint[i].step(ji[i], dt, sup.scale);
      o.vDes[i] = joint[i].lastVDes; o.kd[i] = joint[i].lastKd; o.guard[i] = joint[i].guard; o.tauTotal[i] = joint[i].lastTotal;
      o.ff[i] = ji[i].ffNm;
    }
    // Schlaufen: unabhängig vom Gelenkmodus (auch aus bleibt alles fest)
    // Immer nur eine Winde zur Zeit: der 24-V-Wandler (Mean Well DDR-60L-24) liefert 2,5 A, ein Bandantrieb zieht bis ≈ 1,5 A.
    // Wartende Winden behalten ihren Befehl und starten, sobald die laufende fertig ist (ihre Zeitüberwachung läuft erst dann).
    if (activeWinch_ >= 0 && !loop[activeWinch_].moving()) activeWinch_ = -1;
    for (int i = 0; i < NL && activeWinch_ < 0; i++) if (loop[i].moving()) activeWinch_ = i;
    for (int i = 0; i < NL; i++) o.winch[i] = (i == activeWinch_) ? loop[i].step(in.loopCurrentA[i], in.loopOpenSwitch[i], dt) : 0.f;
    if (activeWinch_ >= 0 && !loop[activeWinch_].moving()) activeWinch_ = -1;
    // Anzeige
    if (fallT_ >= 0.f) fallT_ += dt;
    // Lendengelenk sperren, solange ein Baum (> 5 kg) in den Schlaufen hängt; Rücklage-Warnung
    const float loadN = in.loopN[LoopR1] + in.loopN[LoopR2] + in.loopN[LoopL1];
    lockT_ = loadN > 49.f ? lockT_ + dt : 0.f; o.lumbarLock = lockT_ > 0.3f && sup.mode == Mode::Active;
    // Frostschutz der Gelenkkapseln: Heizfolie an, wenn eine Kapsel der Gruppe unter +3 °C fällt und die Gruppe kaum arbeitet
    // (unter Last heizt der Motor selbst); aus über +8 °C. Kälteste Wicklungstemperatur aus der Motor-Rückmeldung.
    for (int gidx = 0; gidx < 2; gidx++) {
      float tMin = 1e9f, load = 0.f;
      for (int i = 0; i < NJ; i++) {   // Gruppe Beine + Rumpf (Kapseln am Becken) bzw. Arme
        const bool arm = i >= ShL && i <= ElR; if (arm != (gidx == 1)) continue;
        tMin = std::fmin(tMin, in.joint[i].motorTempC); load = std::fmax(load, heat_[i]);
      }
      bool& h = gidx ? heatArms_ : heatLegs_;
      if (tMin < 3.f && load < 0.3f) h = true;
      if (tMin > 8.f || load > 0.5f) h = false;
    }
    o.heaterLegs = heatLegs_; o.heaterArms = heatArms_;
    lockOn_ = o.lumbarLock ? lockOn_ + dt : 0.f; o.lumbarDuty = !o.lumbarLock ? 0.f : lockOn_ < 0.2f ? 1.f : 0.35f;
    o.warnBackward = sup.mode == Mode::Active && in.trunkPitch < -0.15f;
    if (o.warnBackward) { warnT_ += dt; if (std::fmod(warnT_, 0.6f) < 0.15f) vibe_ = std::fmax(vibe_, 0.05f); } else warnT_ = 0.f;
    o.led = sup.led(s);
    o.ledCount = int(std::round(12.f * exo::clampf((in.safety.batteryV - 30.f) / (41.f - 30.f), 0.f, 1.f)));
    o.workLight = light;
    vibe_ = std::fmax(0.f, vibe_ - dt); o.vibrate = vibe_ > 0.f;
    return o;
  }

 private:
  float both_ = 0.f, vibe_ = 0.f, stance_[2] = {1.f, 1.f};
  int activeWinch_ = -1;
  float heat_[NJ] = {}, fallT_ = -1.f, lockT_ = 0.f, lockOn_ = 0.f, warnT_ = 0.f;
  bool heatLegs_ = false, heatArms_ = false;
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
    const float personKg = bodyShare[level] * body.bodyUpperKg + loadShare[level] * std::fmax(0.f, loadKg);
    const float carryKg = body.exoUpperKg + personKg;
    const float stanceSum = std::fmax(0.5f, stance_[0] + stance_[1]);
    for (int k = 0; k < 2; k++) {
      const int hip = k ? HipR : HipL, knee = k ? KneeR : KneeL;
      const float st = stance_[k], share = st / stanceSum;                  // Last auf die belasteten Beine verteilen
      const float w = carryKg * g * share, wP = personKg * g * share, wE = body.exoUpperKg * g * share;
      // Hüfte: Oberkörper neigt sich vor → Exo streckt (negativ). Der Rucksack-Schwerpunkt liegt hinter der Hüftachse: aufrecht
      // kippt er nach hinten, das Exo hält ihn (positiv), sonst zögen die Schultergurte dich zurück.
      const float th = in.trunkPitch;
      ji[hip].ffNm = -wP * body.trunkCom * std::sin(std::fmax(0.f, th)) - wE * (body.exoComUp * std::sin(th) - body.exoComBack * std::cos(th));
      ji[knee].ffNm = -w * 0.5f * body.thigh * std::sin(clampf(in.joint[knee].q, 0.f, 1.6f));
      // Druck am Oberschenkel: Hüfte direkt, Knie mit (Aufstehen und Hinsetzen sind Hüfte und Knie zusammen)
      ji[knee].crossNm = kneeSynergy * cuffForce(hip) * joint[hip].cfg.rCuff;
      // Schwerkraft der Exo-Beinschiene zählt nur im Schwung (im Stand trägt der Boden)
      ji[hip].gravScale = 1.f - st;
      // Standbein: nie aktiv beugen (Knie könnte einknicken); Hüfte nur wenig
      ji[knee].tauMaxNow = (1.f - st) * joint[knee].cfg.tauMax;
      ji[hip].tauMaxNow = joint[hip].cfg.tauMax * (1.f - 0.85f * st);
    }
    // Rumpf beugen: trägt einen Anteil deines Oberkörpers und der Baumlast (das Exo-Oberteil tragen die Gasfedern).
    // Absicht aus den Rumpf-Wägezellen (main_teensy.cpp): Gurte ziehen = vorbeugen (+), ins Polster drücken = aufrichten (−).
    ji[Lumb].ffNm = -personKg * g * 0.25f * std::sin(std::fmax(0.f, in.trunkPitch));
    // Rumpf drehen: keine Vorsteuerung, nur Absicht (Unterschied links/rechts) und die Zentrierfedern
  }
};

}  // namespace exo
