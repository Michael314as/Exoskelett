// Transparenz-Studie Manschetten M2 („Mitgehen“): Mensch – Manschette – Exo-Segment – Motor, für Ellbogen, Schulter, Knie.
// Mensch: kennt seinen Arm (Vorsteuerung Jh·a) + entspannte Muskel-Eigenimpedanz Kh/Bh. Manschette: Feder k = Steifigkeit · r².
// Motor: RobStride Typ 1, rechnet t_ff + Kd·(v_soll − v) intern (Stromregler 0,4 ms), Befehle kommen mit Busverzug.
// Regler „F“: v_soll = G/10 · Kraft [rad/s je N], t_ff = Schwerkraft + Reibung + Kraft·r, Schwingungswächter (> 6 Hz).
// Ergebnis (Rev. M2): G = 6, Kd = 0,75 · J/0,065, 500 Hz Regeltakt, ≤ 1 ms Verzug → 2,5–4 N rms bei 2–15 kN/m Manschette.
// Bauen:  g++ -O2 -std=c++17 -I../firmware/src transparenz.cpp -o transparenz && ./transparenz
#include <cstdio>
#include <cmath>
#include <random>
#include <functional>
#include <deque>
#include "exo_core.h"
using namespace exo;
struct Seg { const char* name; double Jh, Jx, grav, coul, visc, r, Kh, Bh; };
struct Res { double rms, peak, effort; bool unstable; };
// Teensy-Regler liefert {tff, pDes, vDes, Kp, Kd}; Motor rechnet PD selbst mit 10 kHz (RobStride MIT-Modus)
struct Cmd { double tff = 0, p = 0, v = 0, kp = 0, kd = 0; };
using Ctl = std::function<Cmd(double F, double q, double qd, double dt)>;
Res sim(const Seg& s, double kt, double fHz, double amp, double sensHz, double noiseN, double ctlHz, double canLag, Ctl ctl, double T = 10) {
  const double kc = kt * s.r * s.r, cc = 0.02 * kc;
  double qh = 1, qhd = 0, q = 1, qd = 0, tauM = 0, held = 0, tS = 0, tC = 0, s2 = 0, pk = 0, e2 = 0; int n = 0;
  Cmd cmd; std::deque<std::pair<double, Cmd>> qq;
  std::mt19937 rng(3); std::normal_distribution<double> nd(0, noiseN);
  const double dt = 0.00005;
  for (double t = 0; t < T; t += dt) {
    const double w = 2 * M_PI * fHz, qdes = 1 + amp * std::sin(w * t), vdes = amp * w * std::cos(w * t), ades = -amp * w * w * std::sin(w * t);
    // Mensch: kennt seinen eigenen Arm (Vorsteuerung Jh·a), Muskel-Eigenimpedanz Kh/Bh, Moment begrenzt
    const double tauH = std::fmax(-60, std::fmin(60, s.Jh * ades + s.Kh * (qdes - qh) + s.Bh * (vdes - qhd)));
    const double cuff = kc * (qh - q) + cc * (qhd - qd);
    while (!qq.empty() && t >= qq.front().first) { cmd = qq.front().second; qq.pop_front(); }
    const double tauReq = std::fmax(-120, std::fmin(120, cmd.tff + cmd.kp * (cmd.p - q) + cmd.kd * (cmd.v - qd)));
    tauM += (tauReq - tauM) * dt / 0.0004;            // Stromregler ~400 µs
    const double fric = s.visc * qd + s.coul * std::tanh(qd / 0.02);
    const double qddx = (cuff + tauM - fric - s.grav * std::sin(q)) / s.Jx;
    const double qddh = (tauH - cuff) / s.Jh;
    qd += qddx * dt; q += qd * dt; qhd += qddh * dt; qh += qhd * dt;
    tS += dt; if (tS >= 1.0 / sensHz) { tS = 0; held = cuff / s.r + nd(rng); }
    tC += dt; if (tC >= 1.0 / ctlHz) { tC = 0; qq.push_back({t + canLag, ctl(held, q, qd, 1.0 / ctlHz)}); }
    const double F = std::fabs(cuff / s.r);
    if (t > 3) { s2 += F * F; n++; pk = std::fmax(pk, F); e2 += tauH * tauH; }
    if (!std::isfinite(q) || std::fabs(q - 1) > 3) return {999, 999, 999, true};
  }
  return {std::sqrt(s2 / n), pk, std::sqrt(e2 / n), pk > 150};
}
static void pr(const char* lab, Res r) { if (r.unstable) std::printf("   %-38s INSTABIL\n", lab); else std::printf("   %-38s rms %5.1f N  max %5.1f N  Muskel %4.1f Nm\n", lab, r.rms, r.peak, r.effort); }
int main() {
  Seg segs[] = {{"Ellbogen", 0.07, 0.065, 1.8, 0.45, 0.25, 0.15, 15, 1.0},
                {"Schulter", 0.35, 0.20, 5.5, 0.7, 0.4, 0.17, 30, 2.0},
                {"Knie", 0.45, 0.25, 6.0, 0.9, 0.5, 0.20, 40, 2.5}};
  struct Case { const char* n; double sensHz, noise, lag, gErr, fErr, kdS, ctl = 1000; };
  Case cases[] = {{"500/500 lag0.5", 500, 0.15, 0.0005, 1, 1, 1, 500}, {"500/500 lag1", 500, 0.15, 0.001, 1, 1, 1, 500},
   {"500/500 lag1.5", 500, 0.15, 0.0015, 1, 1, 1, 500}, {"500/500 lag2", 500, 0.15, 0.002, 1, 1, 1, 500},
   {"500/500 lag1 Rausch0.6", 500, 0.6, 0.001, 1, 1, 1, 500}, {"500/500 lag1 Reib x2", 500, 0.15, 0.001, 1, 0.5, 1, 500},
   {"500/500 lag1 Kd x2", 500, 0.15, 0.001, 1, 1, 2, 500}, {"500/500 lag1 Kd x0.5", 500, 0.15, 0.001, 1, 1, 0.5, 500}};
  for (auto& s : segs) { std::printf("\n== %s\n", s.name);
   for (double FF : {1.0}) for (double G : {6.0, 8.0}) for (auto& c : cases) { double worst = 0, wpk = 0; bool uns = false;
    for (double kt : {2000.0, 6000.0, 15000.0}) for (auto [fHz, amp] : {std::pair{0.5, 0.5}, std::pair{1.2, 0.35}, std::pair{2.0, 0.2}}) {
      Seg sp = s; sp.coul *= 1.0 / c.fErr; sp.visc *= 1.0 / c.fErr;   // reale Reibung größer als Modell
      LowPass lp, hpLp, env; double gS = 1.0;
      auto r = sim(sp, kt, fHz, amp, c.sensHz, c.noise, c.ctl, c.lag, [&](double F, double q, double qd, double dt) {
          // Stabilitätswächter: Anteil > 6 Hz in der Manschettenkraft -> Verstärkung zurücknehmen
          const double hp = F - hpLp.step(F, dt, 6.0); const double e = std::sqrt(env.step(hp * hp, dt, 3.0));
          if (e > 2.0) gS = std::fmax(0.25, gS - 4.0 * dt); else if (e < 1.0) gS = std::fmin(1.0, gS + 0.3 * dt);
          double f = deadband(lp.step(F, dt, 40), 0.2) * s.r; double v = std::fmax(-6, std::fmin(6, gS * G * f / (s.r * 10)));
          const double ff = c.gErr * s.grav * std::sin(q) + s.coul * std::tanh(qd / 0.05) + s.visc * qd + gS * FF * f;
          return Cmd{ff, 0, v, 0, c.kdS * 0.75 * s.Jx / 0.065}; });
      uns |= r.unstable; worst = std::fmax(worst, r.rms); wpk = std::fmax(wpk, r.peak); }
    std::printf("  FF=%.1f G=%.0f %-22s %s schlechtester rms %5.1f N  max %5.1f N\n", FF, G, c.n, uns ? "INSTABIL" : "        ", worst, wpk); } }
}
