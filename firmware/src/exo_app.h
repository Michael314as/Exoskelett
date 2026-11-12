// Gesamtsteuerung (Entwurf 5): 10 Gelenke, 3 Schlaufenwinden, Taster, Sicherheit, LED-Ring.
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
  const char* notice = "";             // Hinweis im Klartext für die Anzeige, solange kein Fehler ansteht („Akku schwach“, „Motor warm“ …)
  float powerLimit = 1.f;              // Abregelung bei Spannungseinbruch (1 = volle Kraft)
  float tare[NJ] = {};                 // beim Einschalten genullter Anteil der Manschettenkraft [N]
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
    case HipL: case HipR:   c.kdJoint = 4.0f; c.viscComp = 0.4f; c.fallDamp = 15.f; break;    // Bein mit Kniekapsel ≈ 0,35 kg·m²
    case KneeL: case KneeR: c.kdJoint = 2.1f; c.viscComp = 0.5f; c.fallDamp = 20.f; break;    // Unterschenkel + Fuß 0,18 kg·m² um die Knieachse (aus der Bauteilliste)
    case ShL: case ShR:     c.kdJoint = 2.3f; c.viscComp = 0.4f; break;    // Exo-Arm mit Ellbogenkapsel ≈ 0,20 kg·m²
    case Lumb:              c.kdJoint = 10.f; c.viscComp = 0.5f; c.admit = 0.25f; c.velMax = 2.f; c.tareMaxN = 120.f; break;   // Oberteil träge, Gasfedern tragen: langsam mitgehen
    case Twist:             c.kdJoint = 7.f;  c.viscComp = 0.3f; c.admit = 0.35f; c.velMax = 2.f; c.tareMaxN = 60.f; break;   // Rucksack drehen ≈ 0,6 kg·m²
    case ElL: case ElR: default: c.kdJoint = 0.75f; c.viscComp = 0.25f; break;   // Unterarm-Schiene ≈ 0,065 kg·m²
  }
  // Gedämpftes Absinken (Sturz, weicher Fehler): Hüfte 15, Knie 20 Nm·s/rad, höchstens 80 Nm. Bei 1,5 rad/s Einsinken hält ein Knie
  // damit ≈ 30 Nm, etwa den Anteil des Exo-Oberteils in halber Hocke. Die früheren 4 Nm·s/rad hätten das Absinken kaum gebremst.
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
  float loadedLoopN = 49.f;                  // ab hier gilt eine Schlaufe als belastet (≈ 5 kg): Lendensperre, Öffnen nur mit Warnung

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
      if (comboActive_ && i != LoopL1) { openWarn_[i] = 0.f; continue; }
      Press p = lp[i];
      // Öffnen unter Last (Baum hängt in der Schlaufe): „lang“ löst erst nur die Vibrationswarnung aus; geöffnet wird, wenn der
      // Taster danach noch 1 s gehalten bleibt. Loslassen bricht ab. Ohne Last öffnet „lang“ wie bisher sofort.
      if (p == Press::Long && loop[i].state != LoopState::Open && in.loopN[i] > loadedLoopN) { openWarn_[i] = 1.f; vibe_ = 1.f; p = Press::None; }
      else if (openWarn_[i] > 0.f) {
        if (!in.btnLoop[i]) openWarn_[i] = 0.f;
        else if ((openWarn_[i] -= dt) <= 0.f) { openWarn_[i] = 0.f; p = Press::Long; }
      }
      loop[i].command(p); if (p != Press::None) vibe_ = std::fmax(vibe_, 0.15f);
    }
    if (comboRelease_) { comboActive_ = comboRelease_ = false; }
    if (in.btnLoop[LoopR1] && in.btnLoop[LoopR2]) { both_ += dt; if (both_ > 2.f && !levelSent_) { setLevel((level + 1) % 3); levelSent_ = true; vibe_ = 0.4f; } }
    else { both_ = 0.f; levelSent_ = false; }
    // Sicherheit
    SafetyInput s = in.safety;
    float maxAssist = 0.f;
    for (auto& j : joint) maxAssist = std::fmax(maxAssist, std::fabs(j.lastForce));
    s.anyAssist = maxAssist > 8.f;
    // Plausibilität der Eingänge: keine Zahl darf „keine Zahl“ sein, und jedes aktive Gelenk muss in seinem Bereich stehen.
    // Ein Winkel weit außerhalb heißt: Nullpunkt falsch (Motor getauscht, nie eingemessen) oder Encoder gestört. Dann stimmen
    // weder die weichen Wände noch Schwerkraft und Vorsteuerung.
    bool tareOk = true;
    for (int i = 0; i < NJ; i++) {
      if (!((jointMask >> i) & 1u)) continue;
      const JointInput& x = in.joint[i]; const JointConfig& c = joint[i].cfg;
      if (!std::isfinite(x.q) || !std::isfinite(x.qd) || !std::isfinite(x.cuffN)) { s.sensorsOk = false; continue; }
      if (x.q < c.qMin - c.angleMargin || x.q > c.qMax + c.angleMargin) s.anglesOk = false;
      // Nullen beim Einschalten: Manschettenkraft im ruhigen Stand mitteln (0,3 s), beim Übergang auf AKTIV als Nullpunkt übernehmen
      if (sup.mode == Mode::Zeroing) {
        tareLp_[i] = wasZeroing_ ? tareLp_[i] + (x.cuffN - tareLp_[i]) * clampf(dt / 0.3f, 0.f, 1.f) : x.cuffN;
        if (std::fabs(tareLp_[i]) > c.tareMaxN) tareOk = false;
      }
    }
    if (!std::isfinite(in.trunkPitch) || !std::isfinite(s.trunkTiltRate) || !std::isfinite(s.batteryV)) s.imuOk = false;
    wasZeroing_ = sup.mode == Mode::Zeroing;
    sup.step(s, in.standingStill, dt, tareOk);
    if (wasZeroing_ && sup.mode == Mode::Active) for (int i = 0; i < NJ; i++) tare_[i] = ((jointMask >> i) & 1u) ? tareLp_[i] : 0.f;
    zeroBlocked_ = sup.mode == Mode::Zeroing && !tareOk;

    // Standbein erkennen (geglättet, damit beim Aufsetzen nichts springt)
    for (int k = 0; k < 2; k++) { stance_[k] += ((in.footContact[k] ? 1.f : 0.f) - stance_[k]) * clampf(dt / 0.08f, 0.f, 1.f); o.stance[k] = stance_[k]; }

    JointInput ji[NJ];
    for (int i = 0; i < NJ; i++) { ji[i] = in.joint[i]; ji[i].cuffN -= tare_[i]; o.tare[i] = tare_[i]; }
    computeCoupling(in, ji);

    // Wärmeschutz je Motor, zweifach: Schätzung (Motormoment / Dauermoment in der Kapsel)² mit 3 min Zeitkonstante, und die gemessene
    // Wicklungstemperatur aus der Motor-Rückmeldung (ab 100 °C zurücknehmen, der Motor selbst schaltet bei 135–145 °C ab).
    o.powerLimit = sup.powerLimit();
    bool motorWarm = false;
    for (int i = 0; i < NJ; i++) {
      const float r = joint[i].lastTotal * joint[i].cfg.motorPerJoint / joint[i].cfg.motorRatedNm;
      heat_[i] += (r * r - heat_[i]) * clampf(dt / 180.f, 0.f, 1.f); o.heat[i] = heat_[i];
      const float byModel = clampf(1.f - 0.6f * (heat_[i] - 1.f), 0.4f, 1.f), byTemp = clampf(1.f - (in.joint[i].motorTempC - 100.f) / 40.f, 0.4f, 1.f);
      motorWarm |= ji[i].limitScale < 0.999f && ((jointMask >> i) & 1u);
      // Spannungseinbruch: alle Momentgrenzen gemeinsam zurücknehmen, bevor der Akku unter die Abschaltgrenze fällt
      ji[i].limitScale = std::fmin(byModel, byTemp) * o.powerLimit;
    }
    // Schutz bei Sturz und bei weichen Fehlern (Sensor, Lagesensor, Gelenkwinkel): Beine nicht schlaff, sondern 1,5 s gedämpft
    // (bremst das Einsinken), dann aus. Die Dämpfung braucht nur die Gelenkgeschwindigkeit aus dem Motor, keine Manschette.
    const float damp = sup.legDamp();
    if (lockOn_ > 0.3f) ji[Lumb].limitScale = 0.f;   // Lendensperre trägt: Beugemotor entlasten (kühlt ab)
    for (int i = 0; i < NJ; i++) {
      if (damp > 0.f && sup.scale <= 0.f && joint[i].cfg.fallDamp > 0.f && ((jointMask >> i) & 1u)) {
        joint[i].off(); o.tau[i] = o.tauTotal[i] = clampf(-joint[i].cfg.fallDamp * in.joint[i].qd, -80.f, 80.f) * damp; continue;
      }
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
    // Not-Aus gedrückt: auch die Winden stehen (ihre Zeitüberwachung hält an); die Schlaufen halten über das Schneckengetriebe
    for (int i = 0; i < NL; i++) o.winch[i] = (i == activeWinch_ && in.safety.estopOk) ? loop[i].step(in.loopCurrentA[i], in.loopOpenSwitch[i], dt) : 0.f;
    if (activeWinch_ >= 0 && !loop[activeWinch_].moving()) activeWinch_ = -1;
    // Lendengelenk sperren, solange ein Baum (> 5 kg) in den Schlaufen hängt; Rücklage-Warnung
    const float loadN = in.loopN[LoopR1] + in.loopN[LoopR2] + in.loopN[LoopL1];
    lockT_ = loadN > loadedLoopN ? lockT_ + dt : 0.f; o.lumbarLock = lockT_ > 0.3f && sup.mode == Mode::Active;
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
    // Akku schwach: alle 20 s ein langer Vibrationspuls, solange das Exo arbeitet
    if (sup.lowBattery && sup.mode == Mode::Active) { lowT_ += dt; if (lowT_ > 20.f) lowT_ = 0.f; if (lowT_ < 0.4f) vibe_ = std::fmax(vibe_, 0.05f); } else lowT_ = 0.f;
    // Hinweis für die Anzeige (ein Fehlergrund aus der Zustandsmaschine hat Vorrang, siehe main_teensy.cpp)
    o.notice = zeroBlocked_ ? "Nullen: Manschetten entlasten" : sup.mode != Mode::Active ? "" : sup.lowBattery ? "Akku schwach"
             : o.powerLimit < 0.999f ? "Akku unter Last: weniger Kraft" : motorWarm ? "Motor warm" : in.safety.parkStandDown ? "Parkstütze unten"
             : o.warnBackward ? "Rücklage" : "";
    o.led = sup.led(s);
    o.ledCount = int(std::round(12.f * exo::clampf((in.safety.batteryV - 30.f) / (41.f - 30.f), 0.f, 1.f)));
    o.workLight = light;
    vibe_ = std::fmax(0.f, vibe_ - dt); o.vibrate = vibe_ > 0.f;
    return o;
  }

 private:
  float both_ = 0.f, vibe_ = 0.f, stance_[2] = {1.f, 1.f};
  int activeWinch_ = -1;
  float heat_[NJ] = {}, lockT_ = 0.f, lockOn_ = 0.f, warnT_ = 0.f, lowT_ = 0.f;
  float tare_[NJ] = {}, tareLp_[NJ] = {}, openWarn_[NL] = {};
  bool wasZeroing_ = false, zeroBlocked_ = false;
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
