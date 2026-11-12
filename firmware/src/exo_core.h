// Steuerungskern des Christbaum-Exos (Entwurf 5, Rev. S1: Sicherheits-Durchsicht).
//
// Hardwareunabhängig: keine Arduino-Abhängigkeit, kein dynamischer Speicher. Läuft auf dem Teensy 4.1
// (siehe main_teensy.cpp) und auf dem PC in der Simulation (siehe ../test/sim_test.cpp).
//
// Prinzip: Die blauen Manschetten sitzen auf Wägezellen. Gemessen wird die Kraft zwischen Mensch und Exo.
// Jedes Gelenk gibt ein Motormoment aus:
//   Moment = Schwerkraftausgleich + Vorsteuerung + K · (Manschettenkraft · Hebelarm + Kopplung) − Dämpfung · Gelenkgeschwindigkeit
// mit Reibungsausgleich, Totband, Tiefpass, weicher Ein-/Ausblendung, Grenzen und Sicherheitsüberwachung.
#pragma once
#include <cmath>
#include <cstdint>

namespace exo {

inline float clampf(float x, float a, float b) { return x < a ? a : (x > b ? b : x); }
inline float signf(float x) { return x > 0.f ? 1.f : (x < 0.f ? -1.f : 0.f); }

// Tiefpass 1. Ordnung
struct LowPass {
  float y = 0.f;
  bool init = false;
  float step(float x, float dt, float fc) {
    if (!init) { y = x; init = true; return y; }
    const float a = dt / (dt + 1.f / (2.f * 3.14159265f * fc));
    y += a * (x - y);
    return y;
  }
  void reset() { init = false; y = 0.f; }
};

// Totband ohne Sprung: Kräfte unter `db` zählen nicht, darüber geht es stetig weiter
inline float deadband(float x, float db) { return std::fabs(x) <= db ? 0.f : x - signf(x) * db; }

/* ------------------------------------------------------------------ Gelenk ---- */
struct JointConfig {
  const char* name = "";
  float gain = 1.0f;           // Verstärkung K (1 = doppelte Kraft)
  float rCuff = 0.15f;         // Hebelarm der Manschette [m]
  float deadbandN = 2.0f;      // Totband der Manschettenkraft [N] (Rauschen der Wägezelle ~0,6 N)
  float filterHz = 8.0f;       // Tiefpass Manschettenkraft [Hz]
  float damping = 0.3f;        // Dämpfung [Nm·s/rad] – klein halten, sonst fühlt sich das Exo zäh an
  float frictionComp = 0.25f;  // Ausgleich der Reibung in Lagern und Bowdenzügen [Nm]
  float tauMax = 40.f;         // Grenzen am Gelenk [Nm]
  float tauMin = -40.f;
  bool pullOnly = false;       // Beinseil: zieht nur (Moment ≥ Vorspannung)
  float pretension = 1.0f;     // Seil-Vorspannung bei pullOnly [Nm am Gelenk]
  float qMin = -0.2f, qMax = 2.0f;   // Arbeitsbereich [rad]
  float wallWidth = 0.15f;     // weiche Wand vor den Endanschlägen [rad]
  float velMax = 4.0f;         // Geschwindigkeitsgrenze [rad/s]
  float gravCoef = 0.0f;       // Schwerkraftausgleich der Exo-Schiene: gravCoef · sin(q + gravPhase) [Nm]
  float gravPhase = 0.0f;
  float slew = 400.f;          // max. Momentänderung [Nm/s] – nichts ruckt
  float motorPerJoint = 1.f;   // Motormoment je Nm am Gelenk = 1 / (Übersetzung · Wirkungsgrad), für die Wärmeschätzung
  float motorRatedNm = 28.f;   // Dauermoment des Motors in der Kühlkörper-Kapsel (Stiftkühlkörper Ø 140, Luft −10…+5 °C); am Prüfstand messen (P33)
  // Mitgehen (Admittanz, ab Rev. M2): das Gelenk läuft mit vSoll = admit · Manschettenkraft mit, der Motor regelt intern
  // (RobStride Typ 1, Kd-Anteil, ≈ 10 kHz im Motor) auf diese Geschwindigkeit. 0 = alter reiner Momentenbetrieb.
  float admit = 0.f;           // [rad/s je N] Manschettenkraft (Studie: 0,6 bei 500 Hz Regeltakt, ≤ 1 ms CAN-Verzug)
  float kdJoint = 0.f;         // Geschwindigkeits-Dämpfung im Motor, auf das Gelenk bezogen [Nm·s/rad] (≈ 0,75 · Segmentträgheit / 0,065)
  float viscComp = 0.f;        // Ausgleich der geschwindigkeitsabhängigen Reibung [Nm·s/rad]
  float guardOnN = 2.0f;       // Schwingungswächter: Anteil > 6 Hz in der Manschettenkraft (Effektivwert) ab dem zurückgeregelt wird [N]
  float guardOffN = 1.0f;      // … und unter dem es sich langsam wieder erholt [N]
  // Sicherheit (Rev. S1)
  float tareMaxN = 30.f;       // größter Nullpunkt, den das Nullen beim Einschalten übernimmt [N]; darüber: Manschette ist belastet
  float angleMargin = 0.35f;   // Gelenkwinkel weiter als das außerhalb von qMin…qMax = Nullpunkt falsch oder Encoder gestört [rad]
  float fallDamp = 0.f;        // Dämpfung beim gedämpften Absinken (Sturz, Sensorfehler) [Nm·s/rad]; 0 = stromlos
};

struct JointInput {
  float cuffN = 0.f;           // Manschettenkraft, positiv = Mensch drückt in +q-Richtung
  bool cuffValid = true;
  float q = 0.f, qd = 0.f;     // Gelenkwinkel und -geschwindigkeit (Motor-Encoder)
  // von der Gesamtsteuerung (exo_app.h) gesetzt:
  float crossNm = 0.f;         // Manschettendruck anderer Gelenke, über die Hebel umgerechnet [Nm], wird verstärkt wie der eigene
  float ffNm = 0.f;            // Vorsteuerung: Baumlast, Körpergewicht im Stand [Nm], nicht verstärkt
  float gravScale = 1.f;       // Anteil Schwerkraftausgleich der Schiene (Bein im Stand: 0)
  float tauMaxNow = 1e9f;      // zusätzliche obere Grenze in diesem Takt (Standbein: nicht beugen)
  float limitScale = 1.f;      // Abregelung bei warmem Motor (1 = volle Grenzen)
  float motorTempC = 25.f;     // Wicklungstemperatur aus der Motor-Rückmeldung (RS06 meldet sie in jedem Antwortrahmen)
};

class Joint {
 public:
  JointConfig cfg;
  explicit Joint(const JointConfig& c = JointConfig()) : cfg(c) {}

  // scale: 0…1 aus der Zustandsmaschine (weiches Ein-/Ausblenden)
  float step(const JointInput& in, float dt, float scale) {
    const float raw = in.cuffValid ? in.cuffN : 0.f;
    const float f = lp_.step(raw, dt, cfg.filterHz);
    lastForce = f;
    const float fUse = deadband(f, cfg.deadbandN);
    const bool adm = cfg.admit > 0.f && !cfg.pullOnly;
    if (adm) {
      // Schwingungswächter: schwingt das Gelenk mit der Manschette (zu straffe Manschette, Verzug am Bus, Kälte),
      // steckt Energie über 6 Hz in der Kraft. Dann Mitgehen schnell zurücknehmen (bis 25 %), langsam wieder freigeben.
      const float hp1 = raw - hpLp_.step(raw, dt, 6.f), hp = hp1 - hpLp2_.step(hp1, dt, 6.f);   // Hochpass 2. Ordnung: Gehen (1–2 Hz) bleibt draußen
      oscN = std::sqrt(env_.step(hp * hp, dt, 3.f));
      if (oscN > cfg.guardOnN) guard = std::fmax(0.25f, guard - 4.f * dt);
      else if (oscN < cfg.guardOffN) guard = std::fmin(1.f, guard + 0.3f * dt);
    }
    const float g = adm ? guard : 1.f;
    float assist = g * cfg.gain * (fUse * cfg.rCuff + in.crossNm);
    float vDes = adm ? g * cfg.admit * (fUse * cfg.rCuff + in.crossNm) / rRef() : 0.f;   // Knie ohne eigene Manschette: über die Kopplung
    // weiche Wand vor den Endanschlägen: Unterstützung in Richtung Anschlag läuft aus, Rückstellmoment baut sich auf
    const float dLo = in.q - cfg.qMin, dHi = cfg.qMax - in.q;
    // Auslauf quadratisch: weit vor dem Anschlag kaum spürbar, kurz davor fast null (Mitgehen und Kraftverstärkung gleich)
    auto fade = [&](float d) { const float x = clampf(d / cfg.wallWidth, 0.f, 1.f); return x * x; };
    vDes = clampf(vDes, -cfg.velMax, cfg.velMax);
    if (dHi < cfg.wallWidth && assist > 0.f) assist *= fade(dHi);
    if (dLo < cfg.wallWidth && assist < 0.f) assist *= fade(dLo);
    if (dHi < cfg.wallWidth && vDes > 0.f) vDes *= fade(dHi);
    if (dLo < cfg.wallWidth && vDes < 0.f) vDes *= fade(dLo);
    // zu schnell: Unterstützung in Bewegungsrichtung zurücknehmen
    if (std::fabs(in.qd) > cfg.velMax && signf(assist) == signf(in.qd)) assist *= clampf(2.f - std::fabs(in.qd) / cfg.velMax, 0.f, 1.f);
    const float grav = in.gravScale * cfg.gravCoef * std::sin(in.q + cfg.gravPhase);
    // Reibung: im Mitgeh-Betrieb nach der Sollgeschwindigkeit (die kennt der Regler sicher), sonst nach der gemessenen
    const float vf = adm ? vDes : in.qd;
    const float fric = cfg.frictionComp * std::tanh(vf / 0.05f) + cfg.viscComp * vf;   // weich um den Stillstand, damit nichts zittert
    const float kd = adm ? scale * cfg.kdJoint : 0.f;
    float tau = scale * (grav + in.ffNm + assist + fric - (adm ? 0.f : cfg.damping * in.qd));
    if (cfg.pullOnly) tau = std::fmax(tau, cfg.pretension * scale);
    // Grenzen gelten für das Gesamtmoment, das der Motor rechnet: tau + Kd·(vSoll − v)
    const float pd = kd * (vDes - in.qd);
    const float hi = std::fmin(cfg.tauMax * in.limitScale, in.tauMaxNow), lo = cfg.pullOnly ? 0.f : cfg.tauMin * in.limitScale;
    tau = clampf(tau + pd, lo, hi) - pd;
    // Änderungsrate begrenzen
    const float maxStep = cfg.slew * dt;
    tau = clampf(tau, last_ - maxStep, last_ + maxStep);
    last_ = tau; lastVDes = scale * vDes; lastKd = kd; lastTotal = tau + pd;
    return tau;
  }
  void off() { last_ = 0.f; lastTotal = 0.f; lastVDes = 0.f; lastKd = 0.f; guard = 1.f; lp_.reset(); hpLp_.reset(); hpLp2_.reset(); env_.reset(); }
  float lastTau() const { return last_; }
  // statische Verstärkung beim Halten: Motor = (K + Kd·admit/r) · Manschettenmoment → Mensch trägt 1 / (1 + Keff)
  float effectiveGain() const { return cfg.gain + (cfg.admit > 0.f && !cfg.pullOnly ? cfg.kdJoint * cfg.admit / rRef() : 0.f); }
  float rRef() const { return cfg.rCuff > 0.f ? cfg.rCuff : 0.2f; }
  float lastForce = 0.f;
  float lastTotal = 0.f;             // Gesamtmoment am Gelenk (t_ff + Kd-Anteil), für Wärmeschätzung
  float lastVDes = 0.f, lastKd = 0.f;  // an den Motor: Sollgeschwindigkeit [rad/s, Gelenk] und Dämpfung [Nm·s/rad, Gelenk]
  float guard = 1.f, oscN = 0.f;       // Schwingungswächter: Faktor 0,25…1 und gemessener Schwinganteil [N]

 private:
  LowPass lp_, hpLp_, hpLp2_, env_;
  float last_ = 0.f;
};

/* ------------------------------------------------------------- Taster -------- */
// Erkennt kurz, lang und doppelt. Ein Taster: true = gedrückt.
enum class Press : uint8_t { None, Short, Long, Double };
class Button {
 public:
  Press step(bool down, float dt) {
    Press out = Press::None;
    if (down) {
      held_ += dt;
      if (!longSent_ && held_ >= kLong) { longSent_ = true; pendingShort_ = false; out = Press::Long; }
    } else {
      if (held_ > 0.02f && !longSent_) {            // losgelassen nach kurzem Druck
        if (pendingShort_ && sinceShort_ < kDouble) { pendingShort_ = false; out = Press::Double; }
        else { pendingShort_ = true; sinceShort_ = 0.f; }
      }
      held_ = 0.f; longSent_ = false;
      if (pendingShort_) {
        sinceShort_ += dt;
        if (sinceShort_ >= kDouble) { pendingShort_ = false; out = Press::Short; }
      }
    }
    return out;
  }
 private:
  static constexpr float kLong = 1.0f, kDouble = 0.35f;
  float held_ = 0.f, sinceShort_ = 0.f;
  bool longSent_ = false, pendingShort_ = false;
};

/* ------------------------------------------------------- Schlaufenwinde ------ */
// Schneckengetriebe, selbsthemmend: Nach dem Zuziehen wird der Motor stromlos, die Schlaufe hält trotzdem.
enum class LoopState : uint8_t { Open, Closing, Closed, Opening, Fault };
class LoopWinch {
 public:
  float closeCurrentA = 2.2f;  // Zugkraft-Grenze über den Motorstrom
  float timeout = 12.f;           // Bandantrieb 5840-31ZY: Schließen ≈ 7,5 s
  LoopState state = LoopState::Open;
  bool moving() const { return state == LoopState::Closing || state == LoopState::Opening; }
  void command(Press p) {
    if (p == Press::Short && (state == LoopState::Open || state == LoopState::Fault)) { state = LoopState::Closing; t_ = 0.f; }
    if (p == Press::Long && state != LoopState::Open) { state = LoopState::Opening; t_ = 0.f; }
  }
  // Ausgabe: Motorspannung −1…+1 (zu = +)
  float step(float currentA, bool openEndSwitch, float dt) {
    t_ += dt;
    switch (state) {
      case LoopState::Closing:
        if (currentA >= closeCurrentA && t_ > 0.15f) { state = LoopState::Closed; return 0.f; }
        if (t_ > timeout) { state = LoopState::Fault; return 0.f; }
        return t_ < 0.3f ? t_ / 0.3f : 1.f;           // sanfter Anlauf
      case LoopState::Opening:
        if (openEndSwitch) { state = LoopState::Open; return 0.f; }
        if (t_ > timeout) { state = LoopState::Fault; return 0.f; }
        return -1.f;
      default: return 0.f;
    }
  }
 private:
  float t_ = 0.f;
};

/* ---------------------------------------------------- Zustandsmaschine -------- */
enum class Mode : uint8_t { Off, Zeroing, Active, SoftOff, Fault };
enum class Led : uint8_t { Off, WhitePulse, Green, Blue, Yellow, Red };

struct SafetyInput {
  bool estopOk = true;         // Not-Aus-Kreis geschlossen
  bool canOk = true;           // alle Motoren melden sich, sind eingeschaltet und ohne Fehler
  bool sensorsOk = true;       // keine Wägezelle ausgefallen, eingefroren oder am Anschlag
  bool imuOk = true;           // Lagesensor liefert Werte
  bool anglesOk = true;        // Gelenkwinkel plausibel (setzt die Gesamtsteuerung)
  bool timingOk = true;        // Regeltakt wird eingehalten
  float batteryV = 36.f;
  float trunkTiltRate = 0.f;   // [rad/s] aus der IMU (Sturzerkennung)
  bool parkStandDown = false;  // Parkstütze ausgeklappt
  bool anyAssist = false;      // irgendein Gelenk unterstützt gerade
};

// Fehlerreaktionen (Rev. S1), nach Schwere:
//   hart   Not-Aus, Motor/Bus, Spannungseinbruch: im selben Takt stromlos, Schütz fällt ab.
//   weich  Sensor, Lagesensor, Gelenkwinkel, Regeltakt: Unterstützung im selben Takt aus, aber die Beine sinken 1,5 s gedämpft
//          ab (Motoren und Bus sind ja in Ordnung), erst dann fällt das Schütz ab. Mit 44 kg Exo ist „sofort schlaff“ gefährlicher.
//   Sturz  wie weich, Unterstützung läuft in 0,1 s aus; danach AUS statt FEHLER.
//   Akku   Warnung unter 32 V, darunter nimmt die Kraft stufenlos ab (powerLimit), unter 30 V (1 s gemittelt) weich aus in 2 s.
class Supervisor {
 public:
  Mode mode = Mode::Off;
  float scale = 0.f;           // 0…1 an alle Gelenke
  float rampUp = 3.0f;         // sanftes Aufwachen
  float rampDown = 0.3f;       // weich aus
  float fallRate = 2.5f;       // ab dieser Rumpf-Kippgeschwindigkeit: Sturz …
  float fallConfirm = 0.02f;   // … wenn sie so lange anhält [s] (ein einzelner Stoß am Rucksack ist kein Sturz)
  float dampTime = 1.5f;       // gedämpftes Absinken der Beine nach Sturz oder weichem Fehler [s]
  float minBatteryV = 30.f;    // darunter (1 s gemittelt): weich ausschalten
  float warnBatteryV = 32.f;   // darunter: Warnung „Akku schwach“
  float brownoutV = 26.f;      // darunter (schnell gemessen): hart aus, bevor Wandler und Motoren von selbst aussteigen
  float deratePerV = 0.25f;    // Abregelung bei Spannungseinbruch: je Volt unter 33 V ein Viertel weniger Moment (bis 30 %)
  const char* reason = "";
  bool lowBattery = false;

  void powerButton(Press p) {
    if (p == Press::Short && (mode == Mode::Off || mode == Mode::Fault)) { mode = Mode::Zeroing; t_ = 0.f; reason = ""; fallT_ = 0.f; hold_ = 0.f; }
    if (p == Press::Long && (mode == Mode::Zeroing || mode == Mode::Active)) softStop("", Mode::Off, rampDown, 0.f);
    if (p == Press::Long && mode == Mode::Fault) mode = Mode::Off;
  }
  // Nullen fertig, wenn 2 s ruhig gestanden (und die Manschetten dabei unbelastet waren: zeroOk)
  void step(const SafetyInput& s, bool stillForZeroing, float dt, bool zeroOk = true) {
    t_ += dt;
    // Akku: schnell gefiltert für Einbruch und Abregelung, langsam für „leer“ (Lastspitzen sollen nicht abschalten)
    if (!vInit_) { vFast_ = vSlow_ = s.batteryV; vInit_ = true; }
    vFast_ += (s.batteryV - vFast_) * clampf(dt / 0.05f, 0.f, 1.f);
    vSlow_ += (s.batteryV - vSlow_) * clampf(dt / 1.0f, 0.f, 1.f);
    if (vSlow_ < warnBatteryV) lowBattery = true; else if (vSlow_ > warnBatteryV + 1.f) lowBattery = false;
    if (mode != Mode::Off && mode != Mode::Fault) {
      // harte Fehler: sofort stromlos, keine Rampe
      if (!s.estopOk) fault("Not-Aus");
      else if (!s.canOk) fault("Motor meldet sich nicht");
      else if (vFast_ < brownoutV) fault("Akku-Spannung eingebrochen");
      else if (mode != Mode::SoftOff) {
        // weiche Fehler: Unterstützung sofort aus, Beine gedämpft, dann FEHLER
        const char* why = !s.sensorsOk ? "Sensor-Manschette gestört" : !s.imuOk ? "Lagesensor gestört" : !s.anglesOk ? "Gelenkwinkel unplausibel"
                        : !s.timingOk ? "Regeltakt überlastet" : nullptr;
        if (why) { if (mode == Mode::Zeroing) fault(why); else softStop(why, Mode::Fault, 0.f, dampTime); }
        else if (vSlow_ < minBatteryV) { if (mode == Mode::Zeroing) fault("Akku leer"); else softStop("Akku leer", Mode::Fault, 2.0f, 0.f); }
      }
    }
    switch (mode) {
      case Mode::Zeroing:
        scale = 0.f;
        if (!stillForZeroing) t_ = 0.f;
        if (t_ >= 2.f && zeroOk) { mode = Mode::Active; t_ = 0.f; }
        break;
      case Mode::Active:
        fallT_ = std::fabs(s.trunkTiltRate) > fallRate ? fallT_ + dt : 0.f;
        if (fallT_ >= fallConfirm) { softStop("Sturz erkannt", Mode::Off, 0.1f, dampTime); break; }
        if (s.parkStandDown) { scale = std::fmax(0.f, scale - dt / rampDown); break; }   // auf der Stütze: keine Unterstützung
        scale = std::fmin(1.f, scale + dt / rampUp);
        break;
      case Mode::SoftOff:
        scale = ramp_ > 0.f ? std::fmax(0.f, scale - dt / ramp_) : 0.f;
        hold_ = std::fmax(0.f, hold_ - dt);
        if (scale <= 0.f && hold_ <= 0.f) mode = end_;
        break;
      case Mode::Fault: case Mode::Off:
        scale = 0.f; break;
    }
  }
  // Gedämpftes Absinken der Beine: 1 → 0 über dampTime (0 = keine Dämpfung aktiv)
  float legDamp() const { return mode == Mode::SoftOff && holdTotal_ > 0.f ? hold_ / holdTotal_ : 0.f; }
  // Abregelung der Momentgrenzen bei Spannungseinbruch (1 = volle Kraft): hält den Akku über der Abschaltgrenze
  float powerLimit() const { return clampf(1.f - deratePerV * (minBatteryV + 3.f - vFast_), 0.3f, 1.f); }
  float batteryFast() const { return vFast_; }
  Led led(const SafetyInput& s) const {
    switch (mode) {
      case Mode::Off: return Led::Off;
      case Mode::Zeroing: return Led::WhitePulse;
      case Mode::Fault: return Led::Red;
      case Mode::SoftOff: return end_ == Mode::Fault ? Led::Red : Led::Yellow;
      case Mode::Active: return s.parkStandDown ? Led::Yellow : (s.anyAssist ? Led::Blue : Led::Green);
    }
    return Led::Off;
  }
 private:
  void fault(const char* why) { mode = Mode::Fault; scale = 0.f; reason = why; hold_ = holdTotal_ = 0.f; }
  void softStop(const char* why, Mode end, float ramp, float hold) {
    mode = Mode::SoftOff; reason = why; end_ = end; ramp_ = ramp; hold_ = holdTotal_ = hold;
    if (ramp <= 0.f) scale = 0.f;
  }
  float t_ = 0.f, ramp_ = 0.3f, hold_ = 0.f, holdTotal_ = 0.f, fallT_ = 0.f;
  float vFast_ = 36.f, vSlow_ = 36.f;
  bool vInit_ = false;
  Mode end_ = Mode::Off;
};

/* ------------------------------------------------------------ Baumzähler ----- */
// Zählt einen Baum, wenn eine Schlaufe zu ist und Last trägt, und erst wieder, nachdem sie geöffnet wurde.
class TreeCounter {
 public:
  int count = 0;
  void step(bool loopClosed, float loadN) {
    if (loopClosed && loadN > 60.f && !armed_) { count++; armed_ = true; }
    if (!loopClosed) armed_ = false;
  }
 private:
  bool armed_ = false;
};

}  // namespace exo
