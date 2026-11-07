// Erzeugt ein Prüfstand-Log wie die Firmware (`log 1`), aus der Simulation mit bekannten Werten für Trägheit, Reibung und
// Schwerkraft des Holzarms. Damit wird tools/auswertung.py geprüft: Es muss diese Werte wiederfinden und die richtige
// Einstellung für das Mitgehen vorschlagen.
//
//   log_demo            Werkseinstellung (Mitgehen, Rev. M2), sauberer Bus
//   log_demo stoerung   Dämpfung im Motor 3-mal zu hoch und 2 ms Busverzug: der Schwingungswächter greift ein,
//                       die Auswertung muss „Kd senken“ vorschlagen
//   log_demo traege     Mitgehen viel zu schwach eingestellt (admit 0,15): die Auswertung muss „admit höher“ vorschlagen
//
// Spalten wie main_teensy.cpp: t_us,q,qd,kraft_N,moment_Nm,v_soll,kd,gesamt_Nm,motor_Nm,waechter,schwing_N
#include <cstdio>
#include <cmath>
#include <cstring>
#include <deque>
#include <random>
#include "exo_app.h"
using namespace exo;

int main(int argc, char** argv) {
  const bool stoerung = argc > 1 && !std::strcmp(argv[1], "stoerung"), traege = argc > 1 && !std::strcmp(argv[1], "traege");
  const double J = 0.05, visc = 0.2, coul = 0.3, grav = 0.9;          // „wahre“ Werte des Prüfstands
  const double kc = 6000.0 * 0.15 * 0.15, cc = 0.02 * kc, Jh = 0.07, r = 0.15, Kh = 15.0, Bh = 1.0;
  double q = 1.1, qd = 0, qh = 1.1, qhd = 0, tauM = 0, held = 0;
  JointConfig cfg = defaultJoint(ElR); cfg.frictionComp = 0.f; cfg.viscComp = 0.f; cfg.gravCoef = 0.f;   // vor dem Abgleich
  if (stoerung) cfg.kdJoint *= 3.f;
  if (traege) cfg.admit = 0.15f;
  Joint j(cfg);
  struct Cmd { double t, tau, v, kd; };
  std::deque<Cmd> bus; Cmd cmd{0, 0, 0, 0};
  const double lag = stoerung ? 0.002 : 0.001;
  std::mt19937 rng(3); std::normal_distribution<double> noise(0, 0.15), encN(0, 0.0005), tqN(0, 0.05);
  const double dt = 0.0001; double tS = 0, tC = 0;
  std::printf("t_us,q,qd,kraft_N,moment_Nm,v_soll,kd,gesamt_Nm,motor_Nm,waechter,schwing_N\n");
  for (double t = 0; t < 30; t += dt) {
    // Mensch führt den Holzarm: Mischung aus langsamen und schnellen Bewegungen (Anregung für die Auswertung)
    const double w1 = 2 * M_PI * 0.4, w2 = 2 * M_PI * 1.7, w3 = 2 * M_PI * 3.1;
    const double qDes = 1.1 + 0.45 * std::sin(w1 * t) + 0.15 * std::sin(w2 * t) + 0.05 * std::sin(w3 * t);
    const double vDes = 0.45 * w1 * std::cos(w1 * t) + 0.15 * w2 * std::cos(w2 * t) + 0.05 * w3 * std::cos(w3 * t);
    const double aDes = -0.45 * w1 * w1 * std::sin(w1 * t) - 0.15 * w2 * w2 * std::sin(w2 * t) - 0.05 * w3 * w3 * std::sin(w3 * t);
    const double tauH = std::fmax(-40.0, std::fmin(40.0, Jh * aDes + Kh * (qDes - qh) + Bh * (vDes - qhd)));
    const double cuff = kc * (qh - q) + cc * (qhd - qd);
    while (!bus.empty() && t >= bus.front().t) { cmd = bus.front(); bus.pop_front(); }
    tauM += (cmd.tau + cmd.kd * (cmd.v - qd) - tauM) * dt / 0.0004;   // Motor: t_ff + Kd·(v_soll − v), Stromregler 0,4 ms
    const double qdd = (cuff + tauM - visc * qd - coul * std::tanh(qd / 0.02) - grav * std::sin(q)) / J;
    qd += qdd * dt; q += qd * dt; qhd += (tauH - cuff) / Jh * dt; qh += qhd * dt;
    if ((tS += dt) >= 0.001) { tS = 0; held = cuff / r + noise(rng); }
    if ((tC += dt) >= 0.002) {
      tC = 0;
      JointInput in; in.cuffN = float(held); in.q = float(q + encN(rng)); in.qd = float(qd);
      const float tau = j.step(in, 0.002f, 1.f);
      bus.push_back({t + lag, tau, j.lastVDes, j.lastKd});
      std::printf("%lu,%.5f,%.4f,%.2f,%.3f,%.3f,%.3f,%.3f,%.3f,%.2f,%.2f\n", (unsigned long)(t * 1e6), in.q, in.qd, in.cuffN, tau,
                  double(j.lastVDes), double(j.lastKd), double(j.lastTotal), tauM + tqN(rng), double(j.guard), double(j.oscN));
    }
  }
}
