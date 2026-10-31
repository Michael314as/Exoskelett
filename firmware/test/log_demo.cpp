// Erzeugt ein Prüfstand-Log wie die Firmware (log 1): t_us,q,qd,kraft_N,moment_Nm – aus der Simulation mit bekannten
// Werten für Trägheit, Reibung und Schwerkraft. Damit wird tools/auswertung.py geprüft: Es muss diese Werte wiederfinden.
#include <cstdio>
#include <cmath>
#include <random>
#include "exo_app.h"
using namespace exo;

int main() {
  const double J = 0.05, visc = 0.2, coul = 0.3, grav = 0.9;          // „wahre“ Werte des Prüfstands
  const double kc = 45.0, cc = 0.8, Jh = 0.07, r = 0.15;
  double q = 0.6, qd = 0, qh = 0.6, qhd = 0, tauM = 0, tauCmd = 0, held = 0;
  JointConfig cfg = defaultJoint(ElR); cfg.frictionComp = 0.f; cfg.gravCoef = 0.f; cfg.gain = 0.6f;   // Werkseinstellung vor dem Abgleich
  Joint j(cfg);
  std::mt19937 rng(3); std::normal_distribution<double> noise(0, 0.6), encN(0, 0.0005);
  const double dt = 0.0002; double tS = 0, tC = 0;
  std::printf("t_us,q,qd,kraft_N,moment_Nm\n");
  for (double t = 0; t < 30; t += dt) {
    // Mensch führt den Holzarm: Mischung aus langsamen und schnellen Bewegungen (Anregung für die Auswertung)
    const double qDes = 1.1 + 0.45 * std::sin(2 * M_PI * 0.4 * t) + 0.15 * std::sin(2 * M_PI * 1.7 * t) + 0.05 * std::sin(2 * M_PI * 3.1 * t);
    const double tauH = std::fmax(-40.0, std::fmin(40.0, 60.0 * (qDes - qh) - 4.0 * qhd));
    const double cuff = kc * (qh - q) + cc * (qhd - qd);
    tauM += (tauCmd - tauM) * dt / 0.002;
    const double qdd = (cuff + tauM - visc * qd - coul * std::tanh(qd / 0.02) - grav * std::sin(q)) / J;
    qd += qdd * dt; q += qd * dt; qhd += (tauH - cuff) / Jh * dt; qh += qhd * dt;
    if ((tS += dt) >= 1.0 / 320) { tS = 0; held = cuff / r + noise(rng); }
    if ((tC += dt) >= 0.005) {
      tC = 0;
      JointInput in; in.cuffN = float(held); in.q = float(q + encN(rng)); in.qd = float(qd);
      tauCmd = j.step(in, 0.005f, 1.f);
      std::printf("%lu,%.5f,%.4f,%.2f,%.3f\n", (unsigned long)(t * 1e6), in.q, in.qd, in.cuffN, tauCmd);
    }
  }
}
