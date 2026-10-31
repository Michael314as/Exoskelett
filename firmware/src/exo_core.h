// Steuerungskern des Christbaum-Exos (Entwurf 3.2).
//
// Hardwareunabhängig: keine Arduino-Abhängigkeit, kein dynamischer Speicher. Läuft auf dem Teensy 4.1
// (siehe main_teensy.cpp) und auf dem PC in der Simulation (siehe ../test/sim_test.cpp).
//
// Prinzip: Die blauen Manschetten sitzen auf Wägezellen. Gemessen wird die Kraft zwischen Mensch und Exo.
// Jedes Gelenk gibt ein Motormoment aus:
//   Moment = Schwerkraftausgleich + K · Manschettenkraft · Hebelarm − Dämpfung · Gelenkgeschwindigkeit
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
};

struct JointInput {
  float cuffN = 0.f;           // Manschettenkraft, positiv = Mensch drückt in +q-Richtung
  bool cuffValid = true;
  float q = 0.f, qd = 0.f;     // Gelenkwinkel und -geschwindigkeit (Motor-Encoder)
};

class Joint {
 public:
  JointConfig cfg;
  explicit Joint(const JointConfig& c = JointConfig()) : cfg(c) {}

  // scale: 0…1 aus der Zustandsmaschine (weiches Ein-/Ausblenden)
  float step(const JointInput& in, float dt, float scale) {
    const float f = lp_.step(in.cuffValid ? in.cuffN : 0.f, dt, cfg.filterHz);
    lastForce = f;
    const float fUse = deadband(f, cfg.deadbandN);
    float assist = cfg.gain * fUse * cfg.rCuff;
    // weiche Wand vor den Endanschlägen: Unterstützung in Richtung Anschlag läuft aus, Rückstellmoment baut sich auf
    const float dLo = in.q - cfg.qMin, dHi = cfg.qMax - in.q;
    if (dHi < cfg.wallWidth && assist > 0.f) assist *= clampf(dHi / cfg.wallWidth, 0.f, 1.f);
    if (dLo < cfg.wallWidth && assist < 0.f) assist *= clampf(dLo / cfg.wallWidth, 0.f, 1.f);
    // zu schnell: Unterstützung in Bewegungsrichtung zurücknehmen
    if (std::fabs(in.qd) > cfg.velMax && signf(assist) == signf(in.qd)) assist *= clampf(2.f - std::fabs(in.qd) / cfg.velMax, 0.f, 1.f);
    const float grav = cfg.gravCoef * std::sin(in.q + cfg.gravPhase);
    const float fric = cfg.frictionComp * std::tanh(in.qd / 0.05f);   // weich um den Stillstand, damit nichts zittert
    float tau = scale * (grav + assist + fric - cfg.damping * in.qd);
    if (cfg.pullOnly) tau = std::fmax(tau, cfg.pretension * scale);
    tau = clampf(tau, cfg.pullOnly ? 0.f : cfg.tauMin, cfg.tauMax);
    // Änderungsrate begrenzen
    const float maxStep = cfg.slew * dt;
    tau = clampf(tau, last_ - maxStep, last_ + maxStep);
    last_ = tau;
    return tau;
  }
  void off() { last_ = 0.f; lp_.reset(); }
  float lastTau() const { return last_; }
  float lastForce = 0.f;

 private:
  LowPass lp_;
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
  float timeout = 8.f;
  LoopState state = LoopState::Open;
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
  bool canOk = true;           // alle Motoren melden sich
  bool sensorsOk = true;       // keine Wägezelle ausgefallen oder am Anschlag
  float batteryV = 36.f;
  float trunkTiltRate = 0.f;   // [rad/s] aus der IMU (Sturzerkennung)
  bool parkStandDown = false;  // Parkstütze ausgeklappt
  bool anyAssist = false;      // irgendein Gelenk unterstützt gerade
};

class Supervisor {
 public:
  Mode mode = Mode::Off;
  float scale = 0.f;           // 0…1 an alle Gelenke
  float rampUp = 3.0f;         // sanftes Aufwachen
  float rampDown = 0.3f;       // weich aus
  float fallRate = 2.5f;       // ab dieser Rumpf-Kippgeschwindigkeit: Sturz
  float minBatteryV = 30.f;
  const char* reason = "";

  void powerButton(Press p) {
    if (p == Press::Short && (mode == Mode::Off || mode == Mode::Fault)) { mode = Mode::Zeroing; t_ = 0.f; reason = ""; }
    if (p == Press::Long && mode != Mode::Off) mode = Mode::SoftOff;
  }
  // Nullen fertig, wenn 2 s ruhig gestanden
  void step(const SafetyInput& s, bool stillForZeroing, float dt) {
    t_ += dt;
    // harte Fehler: sofort stromlos, keine Rampe
    if (mode != Mode::Off) {
      if (!s.estopOk) { fault("Not-Aus"); }
      else if (!s.canOk) { fault("Motor meldet sich nicht"); }
      else if (!s.sensorsOk) { fault("Sensor-Manschette gestört"); }
      else if (s.batteryV < minBatteryV) { fault("Akku leer"); }
    }
    switch (mode) {
      case Mode::Zeroing:
        scale = 0.f;
        if (!stillForZeroing) t_ = 0.f;
        if (t_ >= 2.f) { mode = Mode::Active; t_ = 0.f; }
        break;
      case Mode::Active:
        if (std::fabs(s.trunkTiltRate) > fallRate) { mode = Mode::SoftOff; reason = "Sturz erkannt"; rampOverride_ = 0.1f; break; }
        if (s.parkStandDown) { scale = std::fmax(0.f, scale - dt / rampDown); break; }   // auf der Stütze: keine Unterstützung
        scale = std::fmin(1.f, scale + dt / rampUp);
        break;
      case Mode::SoftOff:
        scale = std::fmax(0.f, scale - dt / (rampOverride_ > 0.f ? rampOverride_ : rampDown));
        if (scale <= 0.f) { mode = Mode::Off; rampOverride_ = 0.f; }
        break;
      case Mode::Fault: case Mode::Off:
        scale = 0.f; break;
    }
  }
  Led led(const SafetyInput& s) const {
    switch (mode) {
      case Mode::Off: return Led::Off;
      case Mode::Zeroing: return Led::WhitePulse;
      case Mode::Fault: return Led::Red;
      case Mode::SoftOff: return Led::Yellow;
      case Mode::Active: return s.parkStandDown ? Led::Yellow : (s.anyAssist ? Led::Blue : Led::Green);
    }
    return Led::Off;
  }
 private:
  void fault(const char* why) { mode = Mode::Fault; scale = 0.f; reason = why; }
  float t_ = 0.f, rampOverride_ = 0.f;
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
