// Simulation und Abnahmetests für den Steuerungskern.
//
// Ein Gelenk (Ellbogen bzw. Schulter) mit Mensch, Manschette und Exo:
//   Mensch-Unterarm (Trägheit, Muskel folgt einer Wunschbewegung, Kraft begrenzt)
//   ── Weichteile + Polster (Feder/Dämpfer) ── Manschette auf Wägezelle ── Exo-Schiene (Trägheit, Reibung, Last)
//   Wägezelle: ADS131M04 1000 Messungen/s (früher HX711 80/s), Rauschen. Regler: 500 Hz. Bus: 1 ms Verzug.
//   Motor: Stromregler 0,4 ms; rechnet t_ff + Kd·(v_soll − v) selbst (RobStride Typ 1), also mit frischer Geschwindigkeit.
// Bauen und starten:  g++ -O2 -std=c++17 -I../src sim_test.cpp -o sim_test && ./sim_test
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <random>
#include <deque>
#include "exo_app.h"
#include "drivers.h"
#include "settings.h"
#include "../esp32/status.h"

using namespace exo;

struct Plant {
  // Mensch
  double Jh = 0.07, qh = 0.6, qhd = 0.0, KpH = 60.0, KdH = 4.0, tauHmax = 40.0;
  // Weichteile zwischen Arm und Manschette (rotatorisch)
  double kc = 45.0, cc = 0.8;
  // Exo-Segment
  double J = 0.05, q = 0.6, qd = 0.0, visc = 0.2, coul = 0.3, gravExo = 0.9;   // Exo-Schiene: 0,9 Nm Schwerkraft
  double tauMotor = 0.0, tauCmd = 0.0, vDes = 0.0, kd = 0.0, motorLag = 0.0004;
  double tauLoad = 0.0;   // Baumlast über die Schlaufen (wirkt nur auf das Exo)
  double tauHuman = 0.0, cuffTorque = 0.0;
  void step(double qDes, double dt) {
    tauHuman = std::fmax(-tauHmax, std::fmin(tauHmax, KpH * (qDes - qh) - KdH * qhd));
    cuffTorque = kc * (qh - q) + cc * (qhd - qd);               // Mensch → Exo
    tauMotor += (tauCmd + kd * (vDes - qd) - tauMotor) * dt / motorLag;
    const double fric = visc * qd + coul * std::tanh(qd / 0.02);
    const double qddExo = (cuffTorque + tauMotor - fric - gravExo * std::sin(q) - tauLoad) / J;
    const double qddH = (tauHuman - cuffTorque) / Jh;
    qd += qddExo * dt; q += qd * dt; qhd += qddH * dt; qh += qhd * dt;
  }
};

struct Result { double maxCuffN = 0, rmsCuffN = 0, humanShare = 0, overshoot = 0, oscTail = 0, maxJump = 0; bool ok = true; };

// Läuft ein Szenario; qDes(t) ist die Wunschbewegung des Menschen
struct Cmd { double t, tau, v, kd; };
template <class F>
Result run(JointConfig cfg, double T, F qDes, double tauLoad, double sensorHz, bool noise, double loadOn = 0.0, Plant p = Plant(), double busLag = 0.001) {
  p.tauLoad = 0.0;
  Joint j(cfg);
  const double r = cfg.rCuff, dtSim = 0.0001, dtCtl = 0.002;
  std::deque<Cmd> bus;
  std::mt19937 rng(7); std::normal_distribution<double> n(0, 0.6);   // ±0,6 N Rauschen
  double held = 0, tSens = 0, tCtl = 0, sum2 = 0; int cnt = 0; Result R; double lastTau = 0; double maxQ = -1e9;
  double shareH = 0, shareN = 0;
  for (double t = 0; t < T; t += dtSim) {
    if (t >= loadOn) p.tauLoad = tauLoad;
    while (!bus.empty() && t >= bus.front().t) { p.tauCmd = bus.front().tau; p.vDes = bus.front().v; p.kd = bus.front().kd; bus.pop_front(); }
    p.step(qDes(t), dtSim);
    tSens += dtSim;
    if (tSens >= 1.0 / sensorHz) { tSens = 0; held = p.cuffTorque / r + (noise ? n(rng) : 0); }
    tCtl += dtSim;
    if (tCtl >= dtCtl) {
      tCtl = 0;
      JointInput in; in.cuffN = (float)held; in.q = (float)p.q; in.qd = (float)p.qd;
      const double tau = j.step(in, (float)dtCtl, 1.0f);
      R.maxJump = std::fmax(R.maxJump, std::fabs(tau - lastTau)); lastTau = tau;
      bus.push_back({t + busLag, tau, j.lastVDes, j.lastKd});
    }
    const double fN = std::fabs(p.cuffTorque / r);
    if (t > 1.0) { R.maxCuffN = std::fmax(R.maxCuffN, fN); sum2 += fN * fN; cnt++; maxQ = std::fmax(maxQ, p.q); }
    if (t > T - 2.0) R.oscTail = std::fmax(R.oscTail, std::fabs(p.qd));
    if (!std::isfinite(p.q) || std::fabs(p.q) > 10) { R.ok = false; R.rmsCuffN = R.maxCuffN = 999; return R; }
    if (tauLoad != 0 && t > T - 1.0) { shareH += p.cuffTorque; shareN += 1; }
  }
  R.rmsCuffN = std::sqrt(sum2 / std::fmax(1, cnt));
  if (tauLoad != 0) R.humanShare = (shareH / shareN) / tauLoad;
  R.overshoot = maxQ;
  return R;
}

static int fails = 0;
static void check(bool ok, const char* what) { std::printf("  [%s] %s\n", ok ? "OK " : "FEHLER", what); if (!ok) fails++; }

int main() {
  // „alt“: Momentbetrieb wie bis Rev. T2 (HX711 80 Hz, Filter 8 Hz, Totband 2 N). „neu“: Mitgehen (Admittanz) wie in exo_app.h
  JointConfig elbow; elbow.name = "Ellbogen"; elbow.gain = 1.0f; elbow.rCuff = 0.15f;
  elbow.gravCoef = 0.9f; elbow.qMin = 0.1f; elbow.qMax = 2.3f; elbow.tauMax = 58.f; elbow.tauMin = -58.f;
  JointConfig neu = defaultJoint(ElR); neu.gain = 1.0f;

  std::printf("1) Transparenz: Arm frei bewegen, ohne Last (Ellbogen)\n");
  for (auto [fHz, amp] : {std::pair{0.5, 0.5}, std::pair{1.2, 0.35}}) {
    auto qd = [fHz = fHz, amp = amp](double t) { return 1.0 + amp * std::sin(2 * M_PI * fHz * t); };
    Result off = run([&] { JointConfig c = elbow; c.gain = 0; c.gravCoef = 0; c.damping = 0; c.frictionComp = 0; return c; }(), 8, qd, 0, 1000, true);
    Result alt = run(elbow, 8, qd, 0, 80, true);
    Result nw = run(neu, 8, qd, 0, 1000, true);
    std::printf("     %.1f Hz ±%.2f rad: ohne Regelung rms %.1f N (max %.1f) | alt rms %.1f N (max %.1f) | Mitgehen rms %.1f N (max %.1f)\n",
                fHz, amp, off.rmsCuffN, off.maxCuffN, alt.rmsCuffN, alt.maxCuffN, nw.rmsCuffN, nw.maxCuffN);
    check(nw.rmsCuffN < 0.7 * alt.rmsCuffN && nw.rmsCuffN < 0.45 * off.rmsCuffN, "Mitgehen: deutlich weniger Manschettenkraft als bisher (< 70 %) und als ungeregelt (< 45 %)");
    check(nw.rmsCuffN < 3.5 && nw.maxCuffN < 8.0, "Mitgehen: Arm frei bewegen mit unter 3,5 N rms (≈ 350 g), Spitze unter 8 N");
  }

  std::printf("2) Verstärkung: Baumlast 20 Nm am Unterarm halten\n");
  for (float K : {0.5f, 1.0f, 2.0f}) {
    JointConfig c = neu; c.gain = K;
    Result R = run(c, 6, [](double) { return 1.2; }, 20.0, 1000, false);
    // Halten: Motor = K·T + Kd·min(admit·T/r, vmax), Mensch T, zusammen die Last. Kleine Lasten: 1/(1+Keff); große: Mitgeh-Anteil gedeckelt.
    double lo = 0, hi = 20;
    for (int it = 0; it < 60; it++) { const double T = 0.5 * (lo + hi); const double m = K * T + c.kdJoint * std::fmin(c.admit * T / c.rCuff, c.velMax); (T + m > 20.0 ? hi : lo) = T; }
    const double expect = lo / 20.0;
    std::printf("     Stufe K = %.1f (bei kleinen Lasten wirksam %.1f): du trägst %.0f %% (erwartet ~%.0f %%), Zittern %.3f rad/s\n", K, Joint(c).effectiveGain(), R.humanShare * 100, expect * 100, R.oscTail);
    check(std::fabs(R.humanShare - expect) < 0.08, "Anteil des Menschen passt zu K und gedeckeltem Mitgeh-Anteil");
    check(R.oscTail < 0.05, "kein Schwingen");
  }
  {
    float lastStable = 0;
    for (float K = 0.5f; K <= 8.0f; K += 0.5f) {
      JointConfig c = elbow; c.gain = K;
      Result R = run(c, 6, [](double t) { return t < 2 ? 0.8 : 1.4; }, 0, 80, true);
      if (R.oscTail < 0.05) lastStable = K; else break;
    }
    std::printf("     Rückfallebene Momentbetrieb (admit = 0, HX711 80 Hz): stabil bis K = %.1f\n", lastStable);
    check(lastStable >= 3.0f, "Rückfallebene Momentbetrieb: stabil mindestens bis K = 3");
  }

  std::printf("3) Stabilität Mitgehen: straffe und weiche Manschette, alle Stufen, Sprung in der Bewegung\n");
  for (double kt : {2000.0, 6000.0, 15000.0}) for (float K : {0.5f, 1.0f, 2.0f}) {
    Plant p; p.kc = kt * 0.15 * 0.15; p.cc = 0.02 * p.kc;
    JointConfig c = neu; c.gain = K;
    Result R = run(c, 6, [](double t) { return t < 2 ? 0.8 : 1.4; }, 0, 1000, true, 0.0, p);
    if (R.oscTail >= 0.05 || !R.ok) std::printf("     Manschette %5.0f N/m, K = %.1f: Zittern %.3f rad/s\n", kt, K, R.oscTail);
    check(R.ok && R.oscTail < 0.05, "kein Schwingen");
  }

  std::printf("4) Weichheit: Sprung in der Wunschbewegung\n");
  {
    Result R = run(neu, 6, [](double t) { return t < 2 ? 0.8 : 1.4; }, 0, 1000, true);
    std::printf("     größter Momentsprung je Regeltakt: %.2f Nm, Überschwingen: %.3f rad über Ziel\n", R.maxJump, R.overshoot - 1.4);
    check(R.maxJump <= neu.slew * 0.002 + 1e-3, "Moment ändert sich höchstens um die eingestellte Rate (nichts ruckt)");
    check(R.overshoot - 1.4 < 0.1, "Überschwingen unter 0,1 rad");
  }

  std::printf("5) Beinseil zieht nur\n");
  {
    JointConfig leg = elbow; leg.pullOnly = true; leg.tauMin = 0; leg.gravCoef = 0;
    Joint j(leg); float minTau = 1e9;
    for (int i = 0; i < 2000; i++) { JointInput in; in.cuffN = 60.f * std::sin(i * 0.01f); in.q = 1.0f; in.qd = 0.5f * std::cos(i * 0.01f); minTau = std::fmin(minTau, j.step(in, 0.005f, 1.f)); }
    std::printf("     kleinstes Moment: %.2f Nm\n", minTau);
    check(minTau >= 0.f, "Moment nie negativ (Seil kann nicht drücken)");
  }

  std::printf("6) Sicherheit und sanftes Aufwachen\n");
  {
    Supervisor s; SafetyInput in;
    s.powerButton(Press::Short);
    float t = 0; for (; t < 2.5f; t += 0.005f) s.step(in, true, 0.005f);
    check(s.mode == Mode::Active, "nach 2 s ruhig stehen ist das Exo aktiv");
    float t1 = 0; while (s.scale < 0.999f && t1 < 10) { s.step(in, true, 0.005f); t1 += 0.005f; }
    std::printf("     volle Unterstützung nach %.1f s\n", t1 + 0.5f);
    check(t1 + 0.5f > 2.5f && t1 + 0.5f < 3.6f, "Unterstützung steigt über etwa 3 s an");
    in.sensorsOk = false; s.step(in, true, 0.005f);
    check(s.mode == Mode::Fault && s.scale == 0.f, "Sensorfehler: im selben Regeltakt stromlos");
    std::printf("     Grund: %s\n", s.reason);
    Supervisor s2; SafetyInput in2; s2.powerButton(Press::Short); for (int i = 0; i < 1200; i++) s2.step(in2, true, 0.005f);
    in2.trunkTiltRate = 3.5f; float tf = 0; while (s2.mode != Mode::Off && tf < 2) { s2.step(in2, true, 0.005f); tf += 0.005f; }
    std::printf("     Sturz: weich aus nach %.2f s (%s)\n", tf, s2.reason);
    check(tf <= 0.12f, "Sturz: Motoren in 0,1 s weich");
    Supervisor s3; SafetyInput in3; s3.powerButton(Press::Short); for (int i = 0; i < 1200; i++) s3.step(in3, true, 0.005f);
    in3.parkStandDown = true; for (int i = 0; i < 100; i++) s3.step(in3, true, 0.005f);
    check(s3.scale == 0.f && s3.led(in3) == Led::Yellow, "Parkstütze unten: keine Unterstützung, Ring gelb");
  }

  std::printf("7) Taster, Schlaufenwinde, Baumzähler\n");
  {
    auto press = [](Button& b, float dur) { Press got = Press::None; for (float t = 0; t < dur; t += 0.005f) { Press p = b.step(true, 0.005f); if (p != Press::None) got = p; }
                                            for (float t = 0; t < 0.6f; t += 0.005f) { Press p = b.step(false, 0.005f); if (p != Press::None) got = p; } return got; };
    Button b;
    check(press(b, 0.15f) == Press::Short, "kurz erkannt");
    check(press(b, 1.3f) == Press::Long, "lang erkannt");
    Press d = Press::None; for (int k = 0; k < 2; k++) { for (float t = 0; t < 0.1f; t += 0.005f) b.step(true, 0.005f); for (float t = 0; t < 0.1f; t += 0.005f) { Press p = b.step(false, 0.005f); if (p != Press::None) d = p; } }
    for (float t = 0; t < 0.5f; t += 0.005f) { Press p = b.step(false, 0.005f); if (p != Press::None) d = p; }
    check(d == Press::Double, "doppelt erkannt (Licht)");
    LoopWinch w; w.command(Press::Short); float t = 0, u = 0, cur = 0.3f;
    while (w.state == LoopState::Closing && t < 10) { u = w.step(cur, false, 0.005f); t += 0.005f; if (t > 3.0f) cur = 2.5f; }
    std::printf("     Schlaufe zu nach %.1f s, Motor danach %.0f (Schneckengetriebe hält)\n", t, u);
    check(w.state == LoopState::Closed && u == 0.f, "Winde schaltet bei Zugkraft ab und hält stromlos");
    w.command(Press::Long); t = 0; while (w.state == LoopState::Opening && t < 10) { w.step(0, t > 2.f, 0.005f); t += 0.005f; }
    check(w.state == LoopState::Open, "lang drücken öffnet bis zum Endschalter");
    TreeCounter c; for (int k = 0; k < 3; k++) { for (int i = 0; i < 50; i++) c.step(true, 300); for (int i = 0; i < 50; i++) c.step(false, 0); }
    check(c.count == 3, "Baumzähler zählt jeden Baum genau einmal");
  }

  std::printf("8) Gesamtsteuerung: Einschalten, Licht, Schlaufe, Stufe, Not-Aus\n");
  {
    App app; Inputs in; Outputs o; const float dt = 0.005f;
    auto run = [&](float sec) { for (float t = 0; t < sec; t += dt) o = app.step(in, dt); };
    auto tap = [&](bool& b, float dur) { b = true; run(dur); b = false; run(0.05f); };
    tap(in.btnPower, 0.15f); run(0.5f);
    check(app.sup.mode == Mode::Zeroing && o.led == Led::WhitePulse, "kurz Ein: Nullen, Ring pulsiert weiß");
    run(2.5f);
    check(app.sup.mode == Mode::Active && o.led == Led::Green, "nach dem Nullen bereit, Ring grün");
    in.joint[ShR].cuffN = 40.f; in.joint[ShR].q = 0.8f; run(3.5f);
    check(o.tauTotal[ShR] > 5.f && o.led == Led::Blue, "Druck an der Oberarm-Manschette: Schultermotor hilft, Ring blau");
    in.joint[ShR].cuffN = 0.f; run(1.f);
    tap(in.btnPower, 0.1f); tap(in.btnPower, 0.1f); run(0.5f);
    check(o.workLight, "Doppeltipp: Arbeitslicht an");
    tap(in.btnLoop[LoopR1], 0.15f); run(0.5f);
    check(app.loop[LoopR1].state == LoopState::Closing && o.winch[LoopR1] > 0.f, "Daumentaster: Schlaufe 1 zieht zu");
    in.loopCurrentA[LoopR1] = 2.5f; run(0.3f); in.loopCurrentA[LoopR1] = 0.f;
    check(app.loop[LoopR1].state == LoopState::Closed, "Schlaufe 1 zu (Stamm gefasst)");
    in.btnLoop[LoopR1] = in.btnLoop[LoopR2] = true; run(2.2f); in.btnLoop[LoopR1] = in.btnLoop[LoopR2] = false; run(0.6f);
    check(app.level == 2, "beide rechten Taster 2 s: Stufe kräftig");
    check(app.loop[LoopR1].state == LoopState::Closed, "Stufenwechsel öffnet keine Schlaufe (Baum bleibt fest)");
    in.joint[HipR].cuffN = 50.f; in.joint[HipR].q = 1.0f; in.joint[KneeR].q = 1.0f; run(1.f);
    check(o.tauTotal[KneeR] <= 0.001f, "Standbein, Beugedruck am Oberschenkel: Knie beugt nie aktiv");
    in.joint[HipR].cuffN = 0.f;
    in.safety.estopOk = false; run(0.005f);
    bool allZero = true; for (int i = 0; i < NJ; i++) allZero &= o.tauTotal[i] == 0.f;
    check(allZero && o.led == Led::Red, "Not-Aus: alle Momente sofort 0, Ring rot");
  }

  std::printf("9) Treiber: Motor-Protokoll (RobStride RS06) und Wägezellen-Verstärker\n");
  {
    MotorLimits L; uint8_t b[8];
    uint32_t id = packTorqueCommand(5, 10.f, L, b);
    const float back = uintToFloat((id >> 8) & 0xFFFF, -L.tMax, L.tMax, 16);
    std::printf("     10 Nm verpackt und zurück: %.4f Nm, ID 0x%08X\n", back, unsigned(id));
    check((id >> 24) == kTypeControl && (id & 0xFF) == 5 && std::fabs(back - 10.f) < 0.002f, "Momentbefehl Typ 1: Moment in der 29-Bit-ID, Auflösung besser als 0,002 Nm");
    check(b[4] == 0 && b[5] == 0 && b[6] == 0 && b[7] == 0, "Kp = Kd = 0: reiner Momentbetrieb");
    id = packTorqueCommand(5, 99.f, L, b);
    check(((id >> 8) & 0xFFFF) == 0xFFFF, "zu großes Moment wird auf die Motorgrenze 36 Nm begrenzt");
    id = packSpecial(kTypeEnable, 7, b);
    check((id >> 24) == kTypeEnable && ((id >> 8) & 0xFF) == kHostId && (id & 0xFF) == 7, "Einschalten Typ 3 an Motor 7");
    // Rückmeldung bauen: Motor 3, Position 1,0 rad, Geschwindigkeit -2 rad/s, Moment 3 Nm, 41,5 °C
    const uint32_t p = floatToUint(1.f, -L.pMax, L.pMax, 16), v = floatToUint(-2.f, -L.vMax, L.vMax, 16), t = floatToUint(3.f, -L.tMax, L.tMax, 16);
    const uint8_t r[8] = {uint8_t(p >> 8), uint8_t(p), uint8_t(v >> 8), uint8_t(v), uint8_t(t >> 8), uint8_t(t), uint8_t(415 >> 8), uint8_t(415 & 0xFF)};
    const uint32_t rid = (uint32_t(kTypeFeedback) << 24) | (2u << 22) | (3u << 8) | kHostId;
    const MotorState ms = unpackReply(rid, r, L);
    check(ms.valid && ms.id == 3 && ms.fault == 0 && std::fabs(ms.pos - 1.f) < 0.001f && std::fabs(ms.vel + 2.f) < 0.002f && std::fabs(ms.torque - 3.f) < 0.002f && std::fabs(ms.tempC - 41.5f) < 0.01f,
          "Motorantwort Typ 2 richtig entpackt (inkl. Temperatur)");
    const MotorState bad = unpackReply(rid | (1u << 18), r, L);
    check(bad.fault != 0, "Fehlerbit (Übertemperatur) aus der ID erkannt");
    Transmission tr; tr.ratio = 1.f; tr.eta = 0.97f;
    std::printf("     Gelenkkapsel: 100 Nm an der Schulter brauchen %.1f Nm am RS04 (Grenze 120 Nm)\n", tr.motorTorqueFor(100.f));
    check(tr.motorTorqueFor(100.f) <= kRS04.tMax && tr.motorTorqueFor(110.f) <= kRS04.tMax, "Schulter 100 Nm und Hüfte 110 Nm liegen im Bereich des RS04 (direkt)");
    Transmission te; te.ratio = 1.f; te.eta = 0.95f;
    check(te.motorTorqueFor(56.f) <= kRS03.tMax, "Ellbogen 56 Nm über Schubstange liegt im Bereich des RS03");
    const uint32_t id4 = packTorqueCommand(2, 100.f, kRS04, b);
    check(std::fabs(uintToFloat((id4 >> 8) & 0xFFFF, -kRS04.tMax, kRS04.tMax, 16) - 100.f) < 0.01f, "RS04: Moment mit dem eigenen Wertebereich (±120 Nm) verpackt");
    int32_t want = -12345; uint32_t bits = uint32_t(want) & 0xFFFFFF; int k = 23;
    const int32_t got = hx711ReadRaw([&] { return k >= 0 ? ((bits >> k--) & 1u) != 0 : false; });
    check(got == want, "HX711: negativer 24-Bit-Wert richtig gelesen");
    CellCal cal; cal.offset = 1000; cal.nPerCount = 0.01f;
    check(std::fabs(cal.toNewton(3000) - 20.f) < 1e-3f && !cal.plausible(0x7FFFFF), "Kalibrierung und Erkennung „Sensor am Anschlag“");
  }

  std::printf("10) Einstellungen und Werkstatt-Konsole\n");
  {
    Settings s; seal(s);
    check(valid(s), "Prüfsumme gültig nach dem Versiegeln");
    s.level = 2; check(!valid(s), "Änderung ohne Versiegeln wird erkannt (kaputtes EEPROM → Werkseinstellungen)");
    static int32_t fakeRaw = 1000; static Settings saved; static char lastOut[96];
    ConsoleIO io; io.rawCell = [](int) { return fakeRaw; }; io.print = [](const char* t) { std::snprintf(lastOut, sizeof lastOut, "%s", t); };
    io.save = [](const Settings& x) { saved = x; return valid(x); };
    Settings c;
    runCommand("null 4", c, io);
    fakeRaw = 1000 + 4200 * 10;                                       // 10 kg-Gewicht: 42 000 Schritte
    runCommand("kal 4 10000", c, io);
    check(std::fabs(c.cell[4].toNewton(1000 + 4200 * 10) - 98.1f) < 0.1f, "Kalibrierung mit 10 kg ergibt 98,1 N");
    fakeRaw = 1000; runCommand("null 5", c, io); fakeRaw = 1000 + 20; check(!runCommand("kal 5 10000", c, io), "kein Gewicht erkannt → Kalibrierung abgelehnt");
    check(runCommand("stufe 0", c, io) && c.level == 0, "Stufe per Konsole");
    check(runCommand("speichern", c, io) && valid(saved) && saved.level == 0, "Speichern versiegelt die Einstellungen");
    check(!runCommand("quatsch", c, io), "unbekannter Befehl abgewiesen");
    static int clk = -1; io.setClock = [](int h, int m) { clk = h * 60 + m; };
    check(runCommand("uhr 14 32", c, io) && clk == 14 * 60 + 32 && !runCommand("uhr 25 0", c, io), "Konsole: Uhr stellen, unsinnige Zeit abgewiesen");
    check(maintenanceDue(40.5f) != nullptr && maintenanceDue(12.f) == nullptr, "Wartungshinweis nach 40 Betriebsstunden");
  }

  std::printf("11) Statuszeile Teensy → ESP32-C3 (Display, Handy)\n");
  {
    ExoStatus st = parseStatus("S,AKTIV,82,2,135,14:32,41.5,\n");
    check(st.valid && !std::strcmp(st.mode, "AKTIV") && st.batteryPct == 82 && st.level == 2 && st.restMin == 135 && !std::strcmp(st.clock, "14:32") && std::fabs(st.hours - 41.5f) < 0.01f, "gültige Zeile richtig gelesen (Uhrzeit, Restzeit)");
    ExoStatus f = parseStatus("S,FEHLER,40,1,60,09:05,2.0,Sensor-Manschette gestört\n");
    check(f.valid && !std::strcmp(f.reason, "Sensor-Manschette gestört"), "Fehlergrund wird übertragen");
    check(!parseStatus("S,AKTIV,82").valid && !parseStatus("Unsinn").valid && !parseStatus("S,X,150,9,0,0,0,").valid, "kaputte Zeilen werden verworfen");
  }

  std::printf("12) Prüfstand-Modus: nur Ellbogen rechts aktiv\n");
  {
    Settings c; static char out[96]; ConsoleIO io; io.print = [](const char* t) { std::snprintf(out, sizeof out, "%s", t); };
    static bool logState = false; io.setLog = [](bool on) { logState = on; };
    check(runCommand("pruefstand", c, io) && c.jointMask == (1u << ElR), "Konsole: pruefstand setzt nur Ellbogen rechts");
    check(runCommand("log 1", c, io) && logState, "Konsole: Messdaten-Log an");
    App app; app.jointMask = c.jointMask; Inputs in; Outputs o; const float dt = 0.005f;
    auto run = [&](float sec) { for (float t = 0; t < sec; t += dt) o = app.step(in, dt); };
    in.btnPower = true; run(0.15f); in.btnPower = false; run(6.f);
    in.joint[ShR].cuffN = 40.f; in.joint[ShR].q = 0.8f; in.joint[ElR].cuffN = 40.f; in.joint[ElR].q = 1.0f; run(1.f);
    check(o.tauTotal[ShR] == 0.f && std::fabs(o.tauTotal[ElR]) > 3.f, "nur das Prüfstand-Gelenk bekommt Moment, die anderen bleiben stromlos");
    check(runCommand("voll", c, io) && c.jointMask == 0x3FF, "Konsole: voll schaltet alle 10 Gelenke frei");
  }

  std::printf("13) Entwurf 4: Kopplung der Manschetten, Baumlast, Standbein/Schwungbein\n");
  {
    const float dt = 0.005f;
    auto ready = [&](App& app, Inputs& in, Outputs& o) { in.btnPower = true; for (float t = 0; t < 0.15f; t += dt) o = app.step(in, dt); in.btnPower = false; for (float t = 0; t < 6.f; t += dt) o = app.step(in, dt); };
    auto run = [&](App& app, Inputs& in, Outputs& o, float sec) { for (float t = 0; t < sec; t += dt) o = app.step(in, dt); };
    {
      App app; Inputs in; Outputs o; ready(app, in, o);
      in.joint[ShR].q = 0.6f; in.joint[ElR].q = 0.8f; run(app, in, o, 1.f); const float base = o.tauTotal[ShR];
      in.joint[ElR].cuffN = 40.f; run(app, in, o, 1.f);
      std::printf("     Druck 40 N nur am Unterarm: Schulter %+.1f Nm (ohne Druck %+.1f Nm)\n", o.tauTotal[ShR], base);
      check(o.tauTotal[ShR] - base > 4.f, "Druck am Unterarm hilft auch der Schulter (Hebel über den Oberarm)");
    }
    {
      App app; Inputs in; Outputs o; ready(app, in, o);
      in.joint[ShR].q = 0.8f; in.joint[ElR].q = 1.2f; run(app, in, o, 1.f); const float sh0 = o.tauTotal[ShR], el0 = o.tauTotal[ElR];
      in.loopN[LoopR1] = 100.f; in.loopN[LoopR2] = 100.f; run(app, in, o, 1.f);
      const float expEl = 0.67f * 200.f * 0.26f * std::sin(2.0f);
      std::printf("     Baum 200 N in den Schlaufen, ohne Manschettendruck: Ellbogen %+.1f Nm (erwartet %+.1f), Schulter %+.1f Nm\n", o.tauTotal[ElR] - el0, expEl, o.tauTotal[ShR] - sh0);
      check(std::fabs((o.tauTotal[ElR] - el0) - expEl) < 2.f && o.tauTotal[ShR] - sh0 > 30.f, "Baumlast: das Exo trägt seinen Anteil von selbst (Stufe normal: zwei Drittel)");
      in.loopN[LoopR1] = in.loopN[LoopR2] = 400.f; run(app, in, o, 1.f);
      check(o.tauTotal[ElR] <= 58.f + 1e-3f, "sehr schwerer Baum: Ellbogen bleibt in seiner Momentgrenze");
    }
    {
      App app; Inputs in; Outputs o; ready(app, in, o);
      in.trunkPitch = 0.5f; in.joint[HipR].q = 1.0f; in.joint[KneeR].q = 1.2f; in.joint[HipL].q = 1.0f; in.joint[KneeL].q = 1.2f; run(app, in, o, 1.f);
      const float hip0 = o.tauTotal[HipR], knee0 = o.tauTotal[KneeR];
      in.loopN[LoopR1] = 300.f; run(app, in, o, 1.f);
      std::printf("     Hocke im Stand: Hüfte %+.1f Nm, Knie %+.1f Nm; mit Baum 300 N: %+.1f / %+.1f Nm\n", hip0, knee0, o.tauTotal[HipR], o.tauTotal[KneeR]);
      check(hip0 < -8.f && knee0 < -10.f, "Hocke im Stand: Hüfte und Knie strecken von selbst (tragen Exo und einen Teil des Oberkörpers)");
      check(o.tauTotal[KneeR] < knee0 - 5.f, "mit Baum tragen die Beine mehr");
      in.loopN[LoopR1] = 0.f; in.footContact[1] = false; run(app, in, o, 0.5f);
      check(std::fabs(o.ff[KneeR]) < 0.5f && o.stance[1] < 0.05f, "Schwungbein: keine Gewichtsvorsteuerung, Bein läuft frei");
      check(o.tauTotal[KneeL] < knee0 - 5.f, "das andere Bein übernimmt die ganze Last");
      in.joint[HipR].cuffN = 60.f; run(app, in, o, 1.f);
      std::printf("     Schwungbein, Bein anheben (60 N am Oberschenkel): Hüfte %+.1f Nm, Knie %+.1f Nm\n", o.tauTotal[HipR], o.tauTotal[KneeR]);
      check(o.tauTotal[HipR] > 8.f && o.tauTotal[KneeR] > 0.5f && o.tauTotal[KneeR] <= 10.f, "Schwungbein: Hüfte und Knie helfen beim Anheben, Knie-Beugen begrenzt auf 10 Nm");
      in.footContact[1] = true; run(app, in, o, 0.5f);
      std::printf("     Fuß wieder belastet: Knie %+.2f Nm, Hüfte %+.2f Nm\n", o.tauTotal[KneeR], o.tauTotal[HipR]);
      check(o.tauTotal[KneeR] <= 0.001f && o.tauTotal[HipR] <= 35.f * 0.15f + 0.1f, "Fuß wieder belastet: kein aktives Beugen mehr am Knie, Hüfte nur wenig");
      // Schienbein-Manschette (M2): im Schwung folgt das Knie dem Unterschenkel in beide Richtungen, im Stand nie aktiv beugen
      in.joint[HipR].cuffN = 0.f; in.footContact[1] = false; in.joint[KneeR].cuffN = -15.f; run(app, in, o, 0.5f);
      const float kExt = o.tauTotal[KneeR], vExt = o.vDes[KneeR];
      in.joint[KneeR].cuffN = 15.f; run(app, in, o, 0.5f);
      std::printf("     Schwungbein, Schienbein-Manschette ±15 N: Knie strecken %+.1f Nm (v %+.1f rad/s), beugen %+.1f Nm (v %+.1f rad/s)\n", kExt, vExt, o.tauTotal[KneeR], o.vDes[KneeR]);
      check(kExt < -2.f && vExt < -1.f && o.tauTotal[KneeR] > 2.f && o.vDes[KneeR] > 1.f, "Schwungbein: Knie geht mit dem Unterschenkel mit (strecken und beugen)");
      in.footContact[1] = true; run(app, in, o, 0.5f);
      check(o.tauTotal[KneeR] <= 0.001f, "Standbein: Schienbein-Druck beugt das Knie nicht");
      in.joint[KneeR].cuffN = 0.f;
    }
    {
      App app; Inputs in; Outputs o; ready(app, in, o);
      in.trunkPitch = 0.f; run(app, in, o, 1.f);
      const float exp = 0.5f * 29.5f * 9.81f * 0.04f;
      std::printf("     aufrecht im Stand: Hüfte %+.1f Nm je Seite (erwartet %+.1f: Rucksack hinter der Hüftachse)\n", o.ff[HipR], exp);
      check(std::fabs(o.ff[HipR] - exp) < 1.f && std::fabs(o.ff[HipL] - exp) < 1.f, "aufrecht: Hüftmotoren halten den Rucksack, der nach hinten kippen will");
      const float up0 = o.ff[HipR];
      in.trunkPitch = 0.6f; run(app, in, o, 1.f);
      std::printf("     35° vorgebeugt: Hüfte %+.1f Nm je Seite\n", o.ff[HipR]);
      check(o.ff[HipR] < -10.f && up0 - o.ff[HipR] > 20.f, "vorgebeugt: Hüftmotoren tragen Oberkörper und Exo nach oben (Rucksack kippt jetzt nach vorne)");
    }
  }

  std::printf("14) Schlaufenwinden laufen nacheinander (24-V-Wandler 2,5 A)\n");
  {
    App app; Inputs in; Outputs o; const float dt = 0.005f;
    auto run = [&](float sec) { for (float t = 0; t < sec; t += dt) o = app.step(in, dt); };
    in.btnLoop[LoopR1] = true; in.btnLoop[LoopL1] = true; run(0.12f); in.btnLoop[LoopR1] = false; in.btnLoop[LoopL1] = false; run(0.5f);
    int active = 0; for (int i = 0; i < NL; i++) active += o.winch[i] != 0.f;
    check(active == 1 && o.winch[LoopR1] > 0.f && app.loop[LoopL1].state == LoopState::Closing, "zwei Schlaufen gleichzeitig befohlen: nur eine Winde läuft, die andere wartet");
    in.loopCurrentA[LoopR1] = 2.5f; run(0.3f); in.loopCurrentA[LoopR1] = 0.f; run(0.5f);
    check(app.loop[LoopR1].state == LoopState::Closed && o.winch[LoopL1] > 0.f, "erste Schlaufe zu: jetzt läuft die zweite");
    run(9.f);
    check(app.loop[LoopL1].state == LoopState::Closing, "Wartezeit zählt nicht zur Zeitüberwachung (12 s ab Start)");
  }

  std::printf("15) Sicherheit und Komfort: Sturzdämpfung, Lendensperre, Rücklage, Wärme\n");
  {
    const float dt = 0.005f;
    App app; Inputs in; Outputs o;
    auto run = [&](float sec) { for (float t = 0; t < sec; t += dt) o = app.step(in, dt); };
    in.btnPower = true; run(0.15f); in.btnPower = false; run(6.f);
    in.loopN[LoopR1] = 150.f; run(0.5f);
    check(o.lumbarLock, "Baum 15 kg in der Schlaufe: Lendengelenk gesperrt");
    check(o.lumbarDuty > 0.3f && o.lumbarDuty < 0.4f, "Hubmagnet hält mit 35 % (0,19 A): 24-V-Wandler reicht auch, wenn gleichzeitig eine Winde zieht");
    in.loopN[LoopR1] = 0.f; run(0.5f);
    check(!o.lumbarLock, "Baum abgelegt: Lendengelenk wieder frei");
    in.trunkPitch = -0.25f; bool vib = false; for (float t = 0; t < 1.f; t += dt) { o = app.step(in, dt); vib |= o.vibrate; }
    check(o.warnBackward && vib, "Rücklage: Warnung und Vibration");
    in.trunkPitch = 0.f;
    in.joint[KneeR].qd = 3.f; in.safety.trunkTiltRate = 3.f; run(0.05f);
    check(app.sup.mode == Mode::SoftOff && o.tauTotal[KneeR] < -5.f, "Sturz: Knie bremst die Bewegung (Dämpfung), statt schlaff zu werden");
    run(2.f);
    check(o.tauTotal[KneeR] == 0.f, "nach 1,5 s Sturzdämpfung: stromlos");
    in.safety.trunkTiltRate = 0.f;
    App hot; Inputs hi; Outputs ho; hi.btnPower = true; for (float t = 0; t < 0.15f; t += dt) ho = hot.step(hi, dt); hi.btnPower = false; for (float t = 0; t < 6.f; t += dt) ho = hot.step(hi, dt);
    hi.joint[ShR].cuffN = 700.f; hi.joint[ShR].q = 0.8f; for (float t = 0; t < 2.f; t += dt) ho = hot.step(hi, dt); const float t0 = ho.tauTotal[ShR];
    float tFull = 0.f; for (float t = 0; t < 600.f; t += dt) { ho = hot.step(hi, dt); if (ho.tauTotal[ShR] >= 0.95f * t0) tFull = t; }
    std::printf("     Schulter hält %.0f Nm mit voller Kraft für %.0f s (Kühlkörper-Kapsel)\n", t0, tFull);
    check(tFull >= 12.f, "Kühlkörper-Kapsel: Schulter hält 100 Nm mindestens 12 s mit voller Kraft");
    std::printf("     Schulter Dauerlast: anfangs %.0f Nm, nach 10 min %.0f Nm (Wärme %.2f)\n", t0, ho.tauTotal[ShR], ho.heat[ShR]);
    check(ho.heat[ShR] > 1.f && ho.tauTotal[ShR] < t0 * 0.95f && ho.tauTotal[ShR] >= 0.4f * 100.f - 1.f, "warmer Motor: Moment wird zurückgenommen, nicht abgeschaltet");
    App hot2; Inputs h2; Outputs o2; h2.btnPower = true; for (float t = 0; t < 0.15f; t += dt) o2 = hot2.step(h2, dt); h2.btnPower = false; for (float t = 0; t < 6.f; t += dt) o2 = hot2.step(h2, dt);
    h2.joint[ShR].cuffN = 700.f; h2.joint[ShR].q = 0.8f; h2.joint[ShR].motorTempC = 124.f; for (float t = 0; t < 2.f; t += dt) o2 = hot2.step(h2, dt);
    std::printf("     Wicklung 124 °C gemessen: Schulter %.0f Nm statt %.0f Nm\n", o2.tau[ShR], t0);
    check(o2.tau[ShR] < 0.45f * t0 && o2.tau[ShR] > 0.f, "gemessene Motortemperatur 124 °C: Moment auf 40 % zurück, bevor der Motor bei 135 °C selbst abschaltet");
    App cold; Inputs ci; Outputs co; ci.btnPower = true; for (float t = 0; t < 0.15f; t += dt) co = cold.step(ci, dt); ci.btnPower = false; for (float t = 0; t < 6.f; t += dt) co = cold.step(ci, dt);
    for (int i = 0; i < NJ; i++) ci.joint[i].motorTempC = -8.f;
    for (float t = 0; t < 0.5f; t += dt) co = cold.step(ci, dt);
    check(co.heaterLegs && co.heaterArms, "Frost (−8 °C) in Ruhe: Heizfolien der Gelenkkapseln an");
    for (int i = 0; i < NJ; i++) ci.joint[i].motorTempC = 10.f;
    for (float t = 0; t < 0.5f; t += dt) co = cold.step(ci, dt);
    check(!co.heaterLegs && !co.heaterArms, "Kapseln warm (+10 °C): Heizung aus");
  }

  std::printf("16) Rumpf: beugen (RS04) und drehen (RS03) nach den Rumpf-Wägezellen\n");
  {
    const float dt = 0.005f;
    auto ready = [&](App& app, Inputs& in, Outputs& o) { in.btnPower = true; for (float t = 0; t < 0.15f; t += dt) o = app.step(in, dt); in.btnPower = false; for (float t = 0; t < 6.f; t += dt) o = app.step(in, dt); };
    auto run = [&](App& app, Inputs& in, Outputs& o, float sec) { for (float t = 0; t < sec; t += dt) o = app.step(in, dt); };
    App app; Inputs in; Outputs o; ready(app, in, o);
    in.joint[Lumb].q = 0.1f; run(app, in, o, 0.5f); const float l0 = o.tauTotal[Lumb];
    in.joint[Lumb].cuffN = 60.f; run(app, in, o, 1.f); const float fwd = o.tauTotal[Lumb];
    in.joint[Lumb].cuffN = -60.f; run(app, in, o, 1.f); const float back = o.tauTotal[Lumb];
    std::printf("     Rumpf: Gurte ziehen 60 N → %+.1f Nm, ins Polster drücken → %+.1f Nm (ohne Druck %+.1f)\n", fwd, back, l0);
    check(fwd > l0 + 5.f && back < l0 - 5.f, "Rumpf beugen folgt den Gurten (vor) und dem Rückenpolster (auf)");
    in.joint[Lumb].cuffN = 0.f; in.trunkPitch = 0.6f; run(app, in, o, 1.f);
    std::printf("     35° vorgebeugt: Rumpf trägt %+.1f Nm (Anteil deines Oberkörpers)\n", o.ff[Lumb]);
    check(o.ff[Lumb] < -5.f && o.tauTotal[Lumb] >= -80.f - 1e-3f, "vorgebeugt: Rumpfmotor trägt einen Anteil, bleibt in der Grenze 80 Nm");
    in.loopN[LoopR1] = 150.f; run(app, in, o, 1.5f);
    check(o.lumbarLock && std::fabs(o.tauTotal[Lumb]) < 0.01f, "Baum in der Schlaufe: Lendensperre trägt, Beugemotor stromlos (kühlt ab)");
    in.loopN[LoopR1] = 0.f; in.trunkPitch = 0.f; run(app, in, o, 0.5f);
    in.joint[Twist].cuffN = 40.f; run(app, in, o, 1.f); const float tw = o.tauTotal[Twist];
    in.joint[Twist].cuffN = 400.f; run(app, in, o, 1.f);
    std::printf("     Drehen: 40 N Unterschied → %+.1f Nm, sehr stark → %+.1f Nm\n", tw, o.tauTotal[Twist]);
    check(tw > 2.f && o.tauTotal[Twist] <= 30.f + 1e-3f, "Rumpf drehen folgt dem Unterschied links/rechts, höchstens 30 Nm");
    in.joint[Twist].cuffN = 400.f; in.joint[Twist].q = 0.25f; run(app, in, o, 0.5f);
    check(o.tauTotal[Twist] < 10.f, "Drehen nahe am Anschlag (±15°): Hilfe läuft aus");
  }

  std::printf("17) Mitgehen an Schulter und Knie (Schwungbein): leicht, robust, Schwingungswächter\n");
  {
    struct Seg { const char* n; JointId id; double Jh, KpH, KdH, J, visc, coul, grav, r; };
    for (Seg g : {Seg{"Schulter", ShR, 0.35, 150, 10, 0.20, 0.4, 0.7, 2.4, 0.17}, Seg{"Knie", KneeR, 0.45, 200, 12, 0.25, 0.5, 0.9, 0.0, 0.20}}) {
      JointConfig c = defaultJoint(g.id); c.gain = 1.0f; if (c.rCuff <= 0.f) c.rCuff = 0.2f;   // Knie: im Test eigene Manschette am Unterschenkel
      c.qMin = -1.f; c.qMax = 3.f;
      JointConfig a = c; a.admit = 0.f; a.filterHz = 8.f; a.deadbandN = 2.f; a.damping = 0.3f; a.viscComp = 0.f;   // alt
      auto mk = [&](double kt) { Plant p; p.Jh = g.Jh; p.KpH = g.KpH; p.KdH = g.KdH; p.J = g.J; p.visc = g.visc; p.coul = g.coul; p.gravExo = g.grav;
                                 p.kc = kt * g.r * g.r; p.cc = 0.02 * p.kc; p.tauHmax = 80; p.qh = p.q = 1.0; return p; };
      auto qd = [](double t) { return 1.0 + 0.35 * std::sin(2 * M_PI * 1.2 * t); };
      double worst = 0, worstAlt = 0;
      for (double kt : {2000.0, 6000.0, 15000.0}) {
        worst = std::fmax(worst, run(c, 8, qd, 0, 1000, true, 0.0, mk(kt)).rmsCuffN);
        worstAlt = std::fmax(worstAlt, run(a, 8, qd, 0, 80, true, 0.0, mk(kt)).rmsCuffN);
      }
      std::printf("     %s 1,2 Hz ±0,35 rad, Manschette 2–15 kN/m: alt bis %.1f N rms, Mitgehen bis %.1f N rms\n", g.n, worstAlt, worst);
      check(worst < 5.0 && worst < 0.6 * worstAlt, "Mitgehen: unter 5 N rms bei jeder Manschettensteifigkeit, deutlich leichter als bisher");
      // Schätzfehler: echte Trägheit und Reibung des Exo-Glieds halb bzw. doppelt so groß wie angenommen
      double worstJ = 0;
      for (double fJ : {0.5, 2.0}) for (double kt : {2000.0, 15000.0}) {
        Plant pj = mk(kt); pj.J *= fJ; pj.coul *= fJ; pj.visc *= fJ;
        worstJ = std::fmax(worstJ, run(c, 8, qd, 0, 1000, true, 0.0, pj).rmsCuffN);
      }
      std::printf("     %s, Trägheit und Reibung falsch geschätzt (×0,5 … ×2): bis %.1f N rms\n", g.n, worstJ);
      check(worstJ < 6.0, "Mitgehen bleibt leicht, auch wenn Trägheit und Reibung des Exo-Glieds um Faktor 2 falsch geschätzt sind");
      // Schritt: Knie/Schulter schnell 1,4 Hz ±0,6 rad (≈ Schwungphase beim zügigen Gehen)
      auto qg = [](double t) { return 1.0 + 0.6 * std::sin(2 * M_PI * 1.4 * t); };
      const Result Rg = run(c, 8, qg, 0, 1000, true, 0.0, mk(6000));
      const Result Rga = run(a, 8, qg, 0, 80, true, 0.0, mk(6000));
      std::printf("     %s, schnelle Schwungbewegung 1,4 Hz ±34°: alt %.1f N rms (max %.1f), Mitgehen %.1f N rms (max %.1f)\n", g.n, Rga.rmsCuffN, Rga.maxCuffN, Rg.rmsCuffN, Rg.maxCuffN);
      if (g.id == KneeR) {   // Knie-Beugen ist aus Sicherheitsgründen auf 10 Nm begrenzt (Federpaket 11 Nm); die Trägheit des Exo-Unterschenkels braucht hier ≈ 12 Nm
        check(Rg.ok && Rg.rmsCuffN < 0.5 * Rga.rmsCuffN, "schnelle Schwungbewegung Knie: weniger als halb so viel wie bisher (Rest = 10-Nm-Beugegrenze)");
        JointConfig hi = c; hi.tauMax = 30.f;
        const Result Rh = run(hi, 8, qg, 0, 1000, true, 0.0, mk(6000));
        std::printf("     zum Vergleich ohne 10-Nm-Beugegrenze: %.1f N rms (max %.1f)\n", Rh.rmsCuffN, Rh.maxCuffN);
        check(Rh.rmsCuffN < 8.0, "ohne Beugegrenze wäre auch das Knie unter 8 N: die Grenze, nicht die Regelung, setzt den Rest");
      } else check(Rg.ok && Rg.rmsCuffN < 8.0 && Rg.rmsCuffN < 0.5 * Rga.rmsCuffN, "schnelle Schwungbewegung: unter 8 N rms, weniger als halb so viel wie bisher");
      // Störfall: Dämpfung im Motor doppelt so groß und 2 ms Busverzug (z. B. Bus überlastet): darf nicht aufschaukeln
      JointConfig bad = c; bad.kdJoint *= 2.f;
      Result R = run(bad, 10, qd, 0, 1000, true, 0.0, mk(15000), 0.002);
      Joint probe(bad); (void)probe;
      std::printf("     Störfall Kd ×2, 2 ms Verzug, straffe Manschette: rms %.1f N, max %.1f N\n", R.rmsCuffN, R.maxCuffN);
      check(R.ok && R.rmsCuffN < 15.0, "Schwingungswächter fängt Störfall ab (begrenzt, kein Aufschaukeln)");
    }
    Joint j(defaultJoint(ElR)); JointInput in; in.q = 1.f;
    for (int i = 0; i < 1500; i++) { in.cuffN = 8.f * std::sin(i * 0.002f * 2 * M_PI * 15.f); j.step(in, 0.002f, 1.f); }   // 15-Hz-Zittern an der Manschette
    std::printf("     15-Hz-Schwingung 8 N an der Manschette: Wächter %.2f\n", j.guard);
    check(j.guard < 0.5f, "Schwingungswächter nimmt das Mitgehen zurück");
    for (int i = 0; i < 2000; i++) { in.cuffN = 0.f; j.step(in, 0.002f, 1.f); }
    check(j.guard > 0.9f, "… und gibt es nach Ruhe langsam wieder frei");
    for (int i = 0; i < 2000; i++) { in.cuffN = 15.f * std::sin(i * 0.002f * 2 * M_PI * 1.0f); j.step(in, 0.002f, 1.f); }
    check(j.guard > 0.95f, "normale Bewegung (1 Hz, 15 N) löst den Wächter nicht aus");
  }
  {
    std::printf("     Motorbefehl Typ 1 mit Mitgehen: ");
    uint8_t b[8]; const uint32_t id = packMitCommand(7, 10.f, 2.f, 1.5f, kRS03, b);
    const uint32_t v = (b[2] << 8) | b[3], d = (b[6] << 8) | b[7];
    std::printf("v %.2f rad/s, Kd %.2f\n", uintToFloat(v, -20.f, 20.f, 16), uintToFloat(d, 0.f, 100.f, 16));
    check((id & 0xFF) == 7 && ((id >> 24) & 0x1F) == 1 && std::fabs(uintToFloat(v, -20.f, 20.f, 16) - 2.f) < 0.01f && std::fabs(uintToFloat(d, 0.f, 100.f, 16) - 1.5f) < 0.01f && b[4] == 0 && b[5] == 0,
          "Typ 1: Moment in der ID, v_soll und Kd in den Daten, Kp = 0");
    Transmission tw{1.8f, 0.93f};
    check(std::fabs(tw.motorKdFor(7.f) * 1.8f * 1.8f * 0.93f - 7.f) < 1e-4f && std::fabs(tw.motorVelFor(1.f) - 1.8f) < 1e-6f, "Rumpf drehen: Kd und Geschwindigkeit richtig auf den Motor umgerechnet");
    uint8_t f[18]; adsWregFrame(kAdsRegClock, kAdsClock1k, f);
    check(f[0] == 0x61 && f[1] == 0x80 && f[3] == 0x0F && f[4] == 0x16, "ADS131M04: WREG CLOCK = 0x0F16 (1 kS/s, OSR 4096, Hochauflösung)");
    uint8_t r[18] = {0x05, 0x0F, 0, 0x7F, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0xFF, 0xFF, 0xFE, 0x00, 0x10, 0x00};
    int32_t ch[4]; const uint16_t st = adsParseFrame(r, ch);
    check(adsStatusDataReady(st) && ch[0] == 8388607 && ch[1] == -8388608 && ch[2] == -2 && ch[3] == 4096, "ADS131M04: Rahmen mit Vorzeichen richtig gelesen");
  }

  std::printf("\n%s (%d Fehler)\n", fails ? "NICHT BESTANDEN" : "ALLE TESTS BESTANDEN", fails);
  return fails ? 1 : 0;
}
