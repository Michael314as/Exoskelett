#!/usr/bin/env python3
"""Prüfstand-Log auswerten: Trägheit, Reibung und Schwerkraft des Exo-Gelenks bestimmen und das Mitgehen bewerten (P39).

Log aufnehmen: Exo aus, Konsole `pruefstand [gelenk]`, `log 1 [gelenk]` (ohne Angabe 7 = Ellbogen rechts), Ausgabe in eine
Datei mitschreiben (z. B. mit `teensy_serial` oder dem seriellen Monitor), Exo einschalten und das Gelenk 30 s mit der Hand
an der Manschette führen: langsam und schnell, über den ganzen Bereich.

Auswerten:  python3 tools/auswertung.py log.csv [--hebel 0.15] [--kd 0.75] [--admit 0.9]
            [--erwartet J,visk,coul,grav] [--erwartet-vorschlag ok|kd_senken|admit_hoeher]

Modell (Gelenk ohne Mensch, der wirkt über die gemessene Manschettenkraft):
    Motormoment + Manschettenkraft · Hebel = J · q'' + visk · q' + coul · sign(q') + grav · sin(q)
Motormoment: die Rückmeldung des Motors (Spalte motor_Nm), sonst t_ff + Kd·(v_soll − v), sonst t_ff (alte Logs mit
5 Spalten, reiner Momentbetrieb). Lineare Ausgleichsrechnung über alle Messpunkte.

Mitgehen: Effektivwert der Manschettenkraft in Bewegung (Ziel < 4 N), Spitzen, wie oft der Schwingungswächter
eingegriffen hat. Daraus ein Vorschlag für kdJoint, admit, frictionComp, viscComp und gravCoef (firmware/src/exo_app.h).
"""
import argparse
import csv
import sys

import numpy as np

COLS = ('t', 'q', 'qd', 'f', 'tau', 'vdes', 'kd', 'total', 'motor', 'guard', 'osc')


def load(path):
    rows = []
    with open(path, newline='') as fh:
        for row in csv.reader(fh):
            try:
                vals = [float(x) for x in row]
            except ValueError:
                continue                                   # Kopfzeile, Konsolentext
            if len(vals) >= 5:
                rows.append(vals[:11])
    width = min(len(r) for r in rows)
    a = np.array([r[:width] for r in rows])
    d = {k: a[:, i] for i, k in enumerate(COLS[:width])}
    d['t'] = d['t'] * 1e-6
    return d


def smooth(x, n=7):
    k = np.ones(n) / n
    return np.convolve(x, k, mode='same')


def motor_torque(d, motor_lag=0.002):
    """Bestes verfügbares Motormoment und seine Herkunft."""
    if 'motor' in d:
        return d['motor'], 'Motor-Rückmeldung'
    if 'total' in d:
        return d['total'], 't_ff + Kd·(v_soll − v)'
    dt = np.median(np.diff(d['t']))
    tau = d['tau']; tm = tau.copy()                       # reiner Momentbetrieb: Motor folgt dem Befehl leicht verzögert
    for i in range(1, len(tm)):
        tm[i] = tm[i - 1] + (tau[i - 1] - tm[i - 1]) * min(1.0, dt / motor_lag)
    return tm, 'Befehl t_ff (altes Log)'


def identify(d, lever):
    dt = np.median(np.diff(d['t']))
    v = smooth(d['qd'])
    acc = np.gradient(v, dt)
    tm, src = motor_torque(d)
    y = smooth(tm, 3) + smooth(d['f'], 3) * lever
    A = np.column_stack([acc, v, np.tanh(v / 0.02), np.sin(d['q'])])
    keep = slice(20, len(y) - 20)
    coef, *_ = np.linalg.lstsq(A[keep], y[keep], rcond=None)
    resid = y[keep] - A[keep] @ coef
    r2 = 1 - resid.var() / y[keep].var()
    return dict(J=coef[0], visk=coef[1], coul=coef[2], grav=coef[3], r2=r2, n=len(y), dt=dt, src=src)


def assess(d, res, kd_now, admit_now):
    f = d['f'] - np.median(d['f'])                        # Nullpunkt der Zelle herausnehmen
    moving = np.abs(d['qd']) > 0.3
    a = dict(rms=float(np.sqrt(np.mean(f[moving] ** 2))) if moving.any() else 0.0,
             peak=float(np.percentile(np.abs(f), 99)), moving=float(moving.mean()))
    if 'guard' in d:
        a['guard_share'] = float(np.mean(d['guard'] < 0.95)); a['guard_min'] = float(d['guard'].min())
        a['osc'] = float(np.percentile(d['osc'], 95))
    else:
        a['guard_share'] = 0.0; a['guard_min'] = 1.0; a['osc'] = 0.0
    kd_model = max(0.1, 0.75 * res['J'] / 0.065)          # Faustformel aus der Studie tools/transparenz.cpp
    if a['guard_share'] > 0.02:
        a['verdict'] = 'kd_senken'
        a['kd'] = min(kd_model, 0.75 * kd_now) if kd_now else kd_model
        a['admit'] = admit_now
        a['why'] = (f"Schwingungswächter hat in {a['guard_share'] * 100:.0f} % der Zeit eingegriffen: Dämpfung im Motor zu hoch "
                    f"oder Bus zu langsam. Kd senken; bleibt es, Buslast und Verzug prüfen.")
    elif a['rms'] > 4.0:
        a['verdict'] = 'admit_hoeher'
        a['kd'] = kd_model
        a['admit'] = min(1.2, admit_now * 1.2) if admit_now else 0.9
        a['why'] = f"Manschettenkraft in Bewegung {a['rms']:.1f} N (Ziel < 4 N), kein Schwingen: Mitgehen etwas stärker."
    else:
        a['verdict'] = 'ok'
        a['kd'] = kd_model
        a['admit'] = admit_now
        a['why'] = f"Manschettenkraft in Bewegung {a['rms']:.1f} N (Ziel < 4 N), Wächter ruhig: Einstellung passt."
    return a


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('log'); ap.add_argument('--hebel', type=float, default=0.15)
    ap.add_argument('--kd', type=float, help='aktuelles kdJoint (sonst aus dem Log)')
    ap.add_argument('--admit', type=float, default=0.9, help='aktuelles admit [rad/s je N]')
    ap.add_argument('--erwartet', help='J,visk,coul,grav zum Vergleich (Test mit Simulationsdaten)')
    ap.add_argument('--erwartet-vorschlag', choices=('ok', 'kd_senken', 'admit_hoeher'), help='Vorschlag, den der Test erwartet')
    a = ap.parse_args()
    d = load(a.log)
    res = identify(d, a.hebel)
    print(f"{res['n']} Messpunkte, Takt {res['dt'] * 1000:.1f} ms, Motormoment aus: {res['src']}, Modellgüte R² = {res['r2']:.3f}")
    print(f"Trägheit J     = {res['J']:.4f} kg·m²")
    print(f"Reibung viskos = {res['visk']:.3f} Nm·s/rad")
    print(f"Reibung trocken= {res['coul']:.3f} Nm")
    print(f"Schwerkraft    = {res['grav']:.3f} Nm")
    if 'vdes' not in d:
        print('HINWEIS: altes Log (5 Spalten): gilt nur für den reinen Momentbetrieb (admit = 0). Beim Mitgehen fehlt der '
              'Dämpfungsanteil des Motors, dann mit aktueller Firmware neu aufnehmen.')
    if res['r2'] < 0.8:
        print('WARNUNG: Modell passt schlecht (R² < 0,8). Mehr und schnellere Bewegung aufnehmen, Hebel prüfen.')
    fail = False
    kd_now = a.kd if a.kd is not None else (float(np.median(d['kd'][d['kd'] > 0])) if 'kd' in d and (d['kd'] > 0).any() else None)
    if 'vdes' in d or a.kd is not None:
        m = assess(d, res, kd_now, a.admit)
        print()
        print('Mitgehen (P39):')
        print(f"  Manschettenkraft in Bewegung: {m['rms']:.1f} N rms (Ziel < 4 N), Spitze (99 %) {m['peak']:.1f} N, bewegt {m['moving'] * 100:.0f} % der Zeit")
        print(f"  Schwingungswächter: griff in {m['guard_share'] * 100:.1f} % der Zeit ein (kleinster Faktor {m['guard_min']:.2f}), Schwinganteil 95 % < {m['osc']:.2f} N")
        print(f"  Bewertung: {m['why']}")
        print('  Vorschlag für firmware/src/exo_app.h (dieses Gelenk):')
        print(f"    c.kdJoint = {m['kd']:.2f}f; c.admit = {m['admit']:.2f}f; c.frictionComp = {max(0, res['coul']) * 0.8:.2f}f; "
              f"c.viscComp = {max(0, res['visk']) * 0.8:.2f}f; c.gravCoef = {res['grav']:.2f}f;")
        print('    (Reibung zu 80 % ausgleichen, damit nichts von allein läuft; danach erneut messen)')
        if a.erwartet_vorschlag:
            ok = m['verdict'] == a.erwartet_vorschlag
            print('Vergleich Vorschlag:', 'wie erwartet' if ok else f"ABWEICHUNG: {m['verdict']} statt {a.erwartet_vorschlag}")
            fail |= not ok
    else:
        print(f"→ Firmware frictionComp = {max(0, res['coul']) * 0.8:.2f} (80 %, damit nichts von allein läuft), gravCoef = {res['grav']:.2f}")
    if a.erwartet:
        want = dict(zip(('J', 'visk', 'coul', 'grav'), map(float, a.erwartet.split(','))))
        bad = [k for k, w in want.items() if abs(res[k] - w) > 0.2 * abs(w) + 0.02]
        print('Vergleich mit Sollwerten:', 'alle innerhalb 20 %' if not bad else 'ABWEICHUNG bei ' + ', '.join(bad))
        fail |= bool(bad)
    return 1 if fail else 0


if __name__ == '__main__':
    sys.exit(main())
