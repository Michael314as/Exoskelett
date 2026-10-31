#!/usr/bin/env python3
"""Prüfstand-Log auswerten: Trägheit, Reibung und Schwerkraft des Exo-Gelenks bestimmen.

Log aufnehmen: Exo aus, Konsole `pruefstand`, `log 1`, Ausgabe in eine Datei mitschreiben (z. B. mit
`teensy_serial` oder dem seriellen Monitor), Exo einschalten und den Holzarm 30 s mit der Hand an der Manschette führen:
langsam und schnell, über den ganzen Bereich.

Auswerten:  python3 tools/auswertung.py log.csv [--hebel 0.15] [--erwartet J,visk,coul,grav]

Modell (Gelenk ohne Mensch, der wirkt über die gemessene Manschettenkraft):
    Motormoment + Manschettenkraft · Hebel = J · q'' + visk · q' + coul · sign(q') + grav · sin(q)
Lineare Ausgleichsrechnung über alle Messpunkte. Ergebnis: Werte für die Simulation (firmware/test/sim_test.cpp,
struct Plant) und für die Firmware (JointConfig: frictionComp = coul, gravCoef = grav).
"""
import argparse
import csv
import sys

import numpy as np


def load(path):
    t, q, qd, f, tau = [], [], [], [], []
    with open(path, newline='') as fh:
        for row in csv.reader(fh):
            try:
                vals = [float(x) for x in row[:5]]
            except ValueError:
                continue                                   # Kopfzeile, Konsolentext
            if len(vals) < 5:
                continue
            t.append(vals[0] * 1e-6); q.append(vals[1]); qd.append(vals[2]); f.append(vals[3]); tau.append(vals[4])
    return map(np.asarray, (t, q, qd, f, tau))


def smooth(x, n=7):
    k = np.ones(n) / n
    return np.convolve(x, k, mode='same')


def identify(t, q, qd, f, tau, lever, motor_lag=0.002):
    dt = np.median(np.diff(t))
    v = smooth(qd)
    acc = np.gradient(v, dt)
    # Motormoment folgt dem Befehl mit kleiner Verzögerung
    tm = tau.copy()
    for i in range(1, len(tm)):
        tm[i] = tm[i - 1] + (tau[i - 1] - tm[i - 1]) * min(1.0, dt / motor_lag)
    y = tm + smooth(f, 3) * lever
    A = np.column_stack([acc, v, np.tanh(v / 0.02), np.sin(q)])
    keep = slice(20, len(y) - 20)
    coef, *_ = np.linalg.lstsq(A[keep], y[keep], rcond=None)
    resid = y[keep] - A[keep] @ coef
    r2 = 1 - resid.var() / y[keep].var()
    return dict(J=coef[0], visk=coef[1], coul=coef[2], grav=coef[3], r2=r2, n=len(y), dt=dt)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('log'); ap.add_argument('--hebel', type=float, default=0.15)
    ap.add_argument('--erwartet', help='J,visk,coul,grav zum Vergleich (Test mit Simulationsdaten)')
    a = ap.parse_args()
    res = identify(*load(a.log), a.hebel)
    print(f"{res['n']} Messpunkte, Takt {res['dt'] * 1000:.1f} ms, Modellgüte R² = {res['r2']:.3f}")
    print(f"Trägheit J     = {res['J']:.4f} kg·m²")
    print(f"Reibung viskos = {res['visk']:.3f} Nm·s/rad")
    print(f"Reibung trocken= {res['coul']:.3f} Nm      → Firmware frictionComp = {max(0, res['coul']) * 0.8:.2f} (80 %, damit nichts von allein läuft)")
    print(f"Schwerkraft    = {res['grav']:.3f} Nm      → Firmware gravCoef = {res['grav']:.2f}")
    if res['r2'] < 0.8:
        print('WARNUNG: Modell passt schlecht (R² < 0,8). Mehr und schnellere Bewegung aufnehmen, Hebel prüfen.')
    if a.erwartet:
        want = dict(zip(('J', 'visk', 'coul', 'grav'), map(float, a.erwartet.split(','))))
        bad = [k for k, w in want.items() if abs(res[k] - w) > 0.2 * abs(w) + 0.02]
        print('Vergleich mit Sollwerten:', 'alle innerhalb 20 %' if not bad else 'ABWEICHUNG bei ' + ', '.join(bad))
        return 1 if bad else 0
    return 0


if __name__ == '__main__':
    sys.exit(main())
