// Simulation und Abnahmetests für den Steuerungskern.
//
// Ein Gelenk (Ellbogen bzw. Schulter) mit Mensch, Manschette und Exo:
//   Mensch-Unterarm (Trägheit, Muskel folgt einer Wunschbewegung, Kraft begrenzt)
//   ── Weichteile + Polster (Feder/Dämpfer) ── Manschette auf Wägezelle ── Exo-Schiene (Trägheit, Reibung, Last)
//   Wägezelle: 80 oder 320 Messungen/s, Rauschen. Regler: 200 Hz. Motor: 2 ms Verzögerung.
// Bauen und starten:  g++ -O2 -std=c++17 -I../src sim_test.cpp -o sim_test && ./sim_test
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <random>
#include "exo_app.h"

using namespace exo;

struct Plant {
  // Mensch
  double Jh = 0.07, qh = 0.6, qhd = 0.0, KpH = 60.0, KdH = 4.0, tauHmax = 40.0;
  // Weichteile zwischen Arm und Manschette (rotatorisch)
  double kc = 45.0, cc = 0.8;
  // Exo-Segment
  double J = 0.05, q = 0.6, qd = 0.0, visc = 0.2, coul = 0.3, gravExo = 0.9;   // Exo-Schiene: 0,9 Nm Schwerkraft
  double tauMotor = 0.0, tauCmd = 0.0, motorLag = 0.002;
  double tauLoad = 0.0;   // Baumlast über die Schlaufen (wirkt nur auf das Exo)
  double tauHuman = 0.0, cuffTorque = 0.0;
  void step(double qDes, double dt) {
    tauHuman = std::fmax(-tauHmax, std::fmin(tauHmax, KpH * (qDes - qh) - KdH * qhd));
    cuffTorque = kc * (qh - q) + cc * (qhd - qd);               // Mensch → Exo
    tauMotor += (tauCmd - tauMotor) * dt / motorLag;
    const double fric = visc * qd + coul * std::tanh(qd / 0.02);
    const double qddExo = (cuffTorque + tauMotor - fric - gravExo * std::sin(q) - tauLoad) / J;
    const double qddH = (tauHuman - cuffTorque) / Jh;
    qd += qddExo * dt; q += qd * dt; qhd += qddH * dt; qh += qhd * dt;
  }
};

struct Result { double maxCuffN = 0, rmsCuffN = 0, humanShare = 0, overshoot = 0, oscTail = 0, maxJump = 0; bool ok = true; };

// Läuft ein Szenario; qDes(t) ist die Wunschbewegung des Menschen
template <class F>
Result run(JointConfig cfg, double T, F qDes, double tauLoad, double sensorHz, bool noise, double loadOn = 0.0) {
  Plant p; p.tauLoad = 0.0;
  Joint j(cfg);
  const double r = cfg.rCuff, dtSim = 0.0002, dtCtl = 0.005;
  std::mt19937 rng(7); std::normal_distribution<double> n(0, 0.6);   // ±0,6 N Rauschen
  double held = 0, tSens = 0, tCtl = 0, sum2 = 0; int cnt = 0; Result R; double lastTau = 0; double maxQ = -1e9;
  double shareH = 0, shareN = 0;
  for (double t = 0; t < T; t += dtSim) {
    if (t >= loadOn) p.tauLoad = tauLoad;
    p.step(qDes(t), dtSim);
    tSens += dtSim;
    if (tSens >= 1.0 / sensorHz) { tSens = 0; held = p.cuffTorque / r + (noise ? n(rng) : 0); }
    tCtl += dtSim;
    if (tCtl >= dtCtl) {
      tCtl = 0;
      JointInput in; in.cuffN = (float)held; in.q = (float)p.q; in.qd = (float)p.qd;
      const double tau = j.step(in, (float)dtCtl, 1.0f);
      R.maxJump = std::fmax(R.maxJump, std::fabs(tau - lastTau)); lastTau = tau; p.tauCmd = tau;
    }
    const double fN = std::fabs(p.cuffTorque / r);
    if (t > 1.0) { R.maxCuffN = std::fmax(R.maxCuffN, fN); sum2 += fN * fN; cnt++; maxQ = std::fmax(maxQ, p.q); }
    if (t > T - 2.0) R.oscTail = std::fmax(R.oscTail, std::fabs(p.qd));
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
  JointConfig elbow; elbow.name = "Ellbogen"; elbow.gain = 1.0f; elbow.rCuff = 0.15f;
  elbow.gravCoef = 0.9f; elbow.qMin = 0.1f; elbow.qMax = 2.3f; elbow.tauMax = 58.f; elbow.tauMin = -58.f;

  std::printf("1) Transparenz: Arm frei bewegen (0,5 Hz, ±0,5 rad), ohne Last\n");
  {
    auto qd = [](double t) { return 1.0 + 0.5 * std::sin(2 * M_PI * 0.5 * t); };
    Result off = run([&] { JointConfig c = elbow; c.gain = 0; c.gravCoef = 0; c.damping = 0; c.frictionComp = 0; return c; }(), 8, qd, 0, 320, true);
    Result on = run(elbow, 8, qd, 0, 320, true);
    std::printf("     Manschettenkraft ohne Regelung: max %.1f N, rms %.1f N\n", off.maxCuffN, off.rmsCuffN);
    std::printf("     Manschettenkraft mit Regelung:  max %.1f N, rms %.1f N\n", on.maxCuffN, on.rmsCuffN);
    check(on.rmsCuffN < 0.6 * off.rmsCuffN, "Exo fühlt sich mit Regelung deutlich leichter an (rms < 60 % von ungeregelt)");
    check(on.maxCuffN < 25.0, "Spitzenkraft an der Manschette beim freien Bewegen unter 25 N");
  }

  std::printf("2) Verstärkung: Baumlast 20 Nm am Unterarm halten\n");
  for (float K : {0.5f, 1.0f, 2.0f, 3.0f}) {
    JointConfig c = elbow; c.gain = K;
    Result R = run(c, 6, [](double) { return 1.2; }, 20.0, 320, false);
    const double expect = 1.0 / (1.0 + K);
    std::printf("     K = %.1f: du trägst %.0f %% (erwartet ~%.0f %%), Zittern %.3f rad/s\n", K, R.humanShare * 100, expect * 100, R.oscTail);
    check(std::fabs(R.humanShare - expect) < 0.12, "Anteil des Menschen passt zu 1/(1+K)");
    check(R.oscTail < 0.05, "kein Schwingen");
  }

  std::printf("3) Stabilität: Schwingungsgrenze suchen (80 Hz HX711 und 320 Hz NAU7802)\n");
  for (double hz : {80.0, 320.0}) {
    float lastStable = 0;
    for (float K = 0.5f; K <= 8.0f; K += 0.5f) {
      JointConfig c = elbow; c.gain = K;
      Result R = run(c, 6, [](double t) { return t < 2 ? 0.8 : 1.4; }, 0, hz, true);
      if (R.oscTail < 0.05) lastStable = K; else break;
    }
    std::printf("     Wägezelle %3.0f Hz: stabil bis K = %.1f\n", hz, lastStable);
    check(lastStable >= 3.0f, "Reserve: stabil mindestens bis K = 3 (Obergrenze in der Software)");
  }

  std::printf("4) Weichheit: Sprung in der Wunschbewegung\n");
  {
    Result R = run(elbow, 6, [](double t) { return t < 2 ? 0.8 : 1.4; }, 0, 320, true);
    std::printf("     größter Momentsprung je Regeltakt: %.2f Nm, Überschwingen: %.3f rad über Ziel\n", R.maxJump, R.overshoot - 1.4);
    check(R.maxJump <= elbow.slew * 0.005 + 1e-3, "Moment ändert sich höchstens um die eingestellte Rate (nichts ruckt)");
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
    check(o.tau[ShR] > 5.f && o.led == Led::Blue, "Druck an der Oberarm-Manschette: Schultermotor hilft, Ring blau");
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
    in.joint[LegR].cuffN = -50.f; in.joint[LegR].q = 1.0f; run(1.f);
    check(o.tau[LegR] >= 0.f, "Bein bei Beugedruck: Seil nur leicht gespannt, nie drückend");
    in.safety.estopOk = false; run(0.005f);
    bool allZero = true; for (int i = 0; i < NJ; i++) allZero &= o.tau[i] == 0.f;
    check(allZero && o.led == Led::Red, "Not-Aus: alle Momente sofort 0, Ring rot");
  }

  std::printf("\n%s (%d Fehler)\n", fails ? "NICHT BESTANDEN" : "ALLE TESTS BESTANDEN", fails);
  return fails ? 1 : 0;
}
