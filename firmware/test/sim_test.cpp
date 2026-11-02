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
    in.joint[HipR].cuffN = 50.f; in.joint[HipR].q = 1.0f; in.joint[KneeR].q = 1.0f; run(1.f);
    check(o.tau[KneeR] <= 0.001f, "Standbein, Beugedruck am Oberschenkel: Knie beugt nie aktiv");
    in.joint[HipR].cuffN = 0.f;
    in.safety.estopOk = false; run(0.005f);
    bool allZero = true; for (int i = 0; i < NJ; i++) allZero &= o.tau[i] == 0.f;
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
    check(o.tau[ShR] == 0.f && std::fabs(o.tau[ElR]) > 3.f, "nur das Prüfstand-Gelenk bekommt Moment, die anderen bleiben stromlos");
    check(runCommand("voll", c, io) && c.jointMask == 0xFF, "Konsole: voll schaltet alle 8 Gelenke frei");
  }

  std::printf("13) Entwurf 4: Kopplung der Manschetten, Baumlast, Standbein/Schwungbein\n");
  {
    const float dt = 0.005f;
    auto ready = [&](App& app, Inputs& in, Outputs& o) { in.btnPower = true; for (float t = 0; t < 0.15f; t += dt) o = app.step(in, dt); in.btnPower = false; for (float t = 0; t < 6.f; t += dt) o = app.step(in, dt); };
    auto run = [&](App& app, Inputs& in, Outputs& o, float sec) { for (float t = 0; t < sec; t += dt) o = app.step(in, dt); };
    {
      App app; Inputs in; Outputs o; ready(app, in, o);
      in.joint[ShR].q = 0.6f; in.joint[ElR].q = 0.8f; run(app, in, o, 1.f); const float base = o.tau[ShR];
      in.joint[ElR].cuffN = 40.f; run(app, in, o, 1.f);
      std::printf("     Druck 40 N nur am Unterarm: Schulter %+.1f Nm (ohne Druck %+.1f Nm)\n", o.tau[ShR], base);
      check(o.tau[ShR] - base > 4.f, "Druck am Unterarm hilft auch der Schulter (Hebel über den Oberarm)");
    }
    {
      App app; Inputs in; Outputs o; ready(app, in, o);
      in.joint[ShR].q = 0.8f; in.joint[ElR].q = 1.2f; run(app, in, o, 1.f); const float sh0 = o.tau[ShR], el0 = o.tau[ElR];
      in.loopN[LoopR1] = 100.f; in.loopN[LoopR2] = 100.f; run(app, in, o, 1.f);
      const float expEl = 0.67f * 200.f * 0.26f * std::sin(2.0f);
      std::printf("     Baum 200 N in den Schlaufen, ohne Manschettendruck: Ellbogen %+.1f Nm (erwartet %+.1f), Schulter %+.1f Nm\n", o.tau[ElR] - el0, expEl, o.tau[ShR] - sh0);
      check(std::fabs((o.tau[ElR] - el0) - expEl) < 2.f && o.tau[ShR] - sh0 > 30.f, "Baumlast: das Exo trägt seinen Anteil von selbst (Stufe normal: zwei Drittel)");
      in.loopN[LoopR1] = in.loopN[LoopR2] = 400.f; run(app, in, o, 1.f);
      check(o.tau[ElR] <= 58.f + 1e-3f, "sehr schwerer Baum: Ellbogen bleibt in seiner Momentgrenze");
    }
    {
      App app; Inputs in; Outputs o; ready(app, in, o);
      in.trunkPitch = 0.5f; in.joint[HipR].q = 1.0f; in.joint[KneeR].q = 1.2f; in.joint[HipL].q = 1.0f; in.joint[KneeL].q = 1.2f; run(app, in, o, 1.f);
      const float hip0 = o.tau[HipR], knee0 = o.tau[KneeR];
      in.loopN[LoopR1] = 300.f; run(app, in, o, 1.f);
      std::printf("     Hocke im Stand: Hüfte %+.1f Nm, Knie %+.1f Nm; mit Baum 300 N: %+.1f / %+.1f Nm\n", hip0, knee0, o.tau[HipR], o.tau[KneeR]);
      check(hip0 < -8.f && knee0 < -10.f, "Hocke im Stand: Hüfte und Knie strecken von selbst (tragen Exo und einen Teil des Oberkörpers)");
      check(o.tau[KneeR] < knee0 - 5.f, "mit Baum tragen die Beine mehr");
      in.loopN[LoopR1] = 0.f; in.footContact[1] = false; run(app, in, o, 0.5f);
      check(std::fabs(o.ff[KneeR]) < 0.5f && o.stance[1] < 0.05f, "Schwungbein: keine Gewichtsvorsteuerung, Bein läuft frei");
      check(o.tau[KneeL] < knee0 - 5.f, "das andere Bein übernimmt die ganze Last");
      in.joint[HipR].cuffN = 60.f; run(app, in, o, 1.f);
      std::printf("     Schwungbein, Bein anheben (60 N am Oberschenkel): Hüfte %+.1f Nm, Knie %+.1f Nm\n", o.tau[HipR], o.tau[KneeR]);
      check(o.tau[HipR] > 8.f && o.tau[KneeR] > 0.5f && o.tau[KneeR] <= 10.f, "Schwungbein: Hüfte und Knie helfen beim Anheben, Knie-Beugen begrenzt auf 10 Nm");
      in.footContact[1] = true; run(app, in, o, 0.5f);
      check(o.tau[KneeR] <= 0.001f && o.tau[HipR] <= 35.f * 0.15f + 0.01f, "Fuß wieder belastet: kein aktives Beugen mehr am Knie, Hüfte nur wenig");
    }
    {
      App app; Inputs in; Outputs o; ready(app, in, o);
      in.trunkPitch = 0.f; run(app, in, o, 1.f);
      const float exp = 0.5f * 25.5f * 9.81f * 0.03f;
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
    check(app.sup.mode == Mode::SoftOff && o.tau[KneeR] < -5.f, "Sturz: Knie bremst die Bewegung (Dämpfung), statt schlaff zu werden");
    run(2.f);
    check(o.tau[KneeR] == 0.f, "nach 1,5 s Sturzdämpfung: stromlos");
    in.safety.trunkTiltRate = 0.f;
    App hot; Inputs hi; Outputs ho; hi.btnPower = true; for (float t = 0; t < 0.15f; t += dt) ho = hot.step(hi, dt); hi.btnPower = false; for (float t = 0; t < 6.f; t += dt) ho = hot.step(hi, dt);
    hi.joint[ShR].cuffN = 700.f; hi.joint[ShR].q = 0.8f; for (float t = 0; t < 2.f; t += dt) ho = hot.step(hi, dt); const float t0 = ho.tau[ShR];
    float tFull = 0.f; for (float t = 0; t < 600.f; t += dt) { ho = hot.step(hi, dt); if (ho.tau[ShR] >= 0.95f * t0) tFull = t; }
    std::printf("     Schulter hält %.0f Nm mit voller Kraft für %.0f s (Kühlkörper-Kapsel)\n", t0, tFull);
    check(tFull >= 12.f, "Kühlkörper-Kapsel: Schulter hält 100 Nm mindestens 12 s mit voller Kraft");
    std::printf("     Schulter Dauerlast: anfangs %.0f Nm, nach 10 min %.0f Nm (Wärme %.2f)\n", t0, ho.tau[ShR], ho.heat[ShR]);
    check(ho.heat[ShR] > 1.f && ho.tau[ShR] < t0 * 0.95f && ho.tau[ShR] >= 0.4f * 100.f - 1.f, "warmer Motor: Moment wird zurückgenommen, nicht abgeschaltet");
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

  std::printf("\n%s (%d Fehler)\n", fails ? "NICHT BESTANDEN" : "ALLE TESTS BESTANDEN", fails);
  return fails ? 1 : 0;
}
