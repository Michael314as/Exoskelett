#!/usr/bin/env python3
"""Kraftverteilung Entwurf 3: statische Abschätzung in der Seitenansicht.

Rechnet für typische Arbeitsschritte, welche Kraft der Baum an den Schlaufen
erzeugt und welche Zusatzmomente daraus an Ellbogen, Schulter, Hüfte und Knie
entstehen, wie viel davon das Exo übernimmt und ob die Motoren reichen.

Aufruf: python3 tools/kraftverteilung.py  (gibt Markdown-Tabellen aus)

Vereinfachungen: alles statisch (kein Schwung), 2D-Seitenansicht, Last auf
beide Körperseiten zusammengefasst, Reibung in Bowdenzügen pauschal.
"""
from math import atan2, cos, sin, sqrt, radians, hypot

G = 9.81
H = 1.75                      # Körpergröße
BODY = 75.0                   # Körpergewicht
SEG = dict(ankle=0.039 * H, shank=0.246 * H, thigh=0.245 * H, torso=0.288 * H, upper=0.186 * H)
LOOP = 0.16                   # Mitte der beiden Schlaufen, gemessen vom Ellbogen am Unterarm
EXO_MASS = 15.0               # Ziel-Gesamtgewicht Exo

# ---------------- Baum ----------------
def tree(L, mass=None):
    """Typisches Gewicht frisch geschnittener Nordmanntanne und Schwerpunktlage ab Schnitt."""
    m = mass if mass is not None else round(15 * (L / 2) ** 2.5)
    bare = min(0.32, max(0.16, 0.12 * L + 0.04))
    xcg = 0.55 * (bare + (L - bare) / 4) + 0.45 * (L / 4)
    return dict(L=L, m=m, W=m * G, xcg=xcg)


# ---------------- Antriebe ----------------
# Motor der Klasse "Robotergelenk mittel" (z. B. 48 V, eingebautes Planetengetriebe)
MOTOR_CONT, MOTOR_PEAK = 9.0, 25.0     # Nm am Abtrieb
DRUM_R = 0.015                         # Seiltrommel-Radius
DRIVES = {
    # Gelenk: (Hebel bzw. Seilscheibenradius am Gelenk, Wirkungsgrad der Seilführung)
    'Schulter': (0.070, 0.70),         # Bowdenzug-Paar auf Seilscheibe Ø 14 cm
    'Ellbogen': (0.050, 0.70),         # Seilscheibe Ø 10 cm
    'Hüfte':    (0.095, 0.90),         # offenes Seil über Rollen, Rolle 9,5 cm hinter der Achse
    'Knie':     (0.060, 0.90),         # gleiches Seil, Rolle vor dem Knie
}


def drive_capacity(joint):
    r, eta = DRIVES[joint]
    f = r / DRUM_R * eta
    return MOTOR_CONT * f, MOTOR_PEAK * f


# ---------------- Geometrie ----------------
def arm_ik(dx, dy, upper=SEG['upper'], fore=LOOP):
    """Ellbogen-Position (relativ zur Schulter) für einen Greifpunkt am Unterarm, Ellbogen nach hinten/unten."""
    d = min(hypot(dx, dy), upper + fore - 1e-6)
    a = atan2(dy, dx)
    b = __import__('math').acos((upper ** 2 + d ** 2 - fore ** 2) / (2 * upper * d))
    ang = a - b                       # Ellbogen unterhalb der Verbindungslinie
    return upper * cos(ang), upper * sin(ang)


def moment(px, py, jx, jy, fx, fy):
    """Moment der Kraft (fx, fy) im Punkt p um das Gelenk j; positiv = gegen den Uhrzeigersinn (x vorne, y oben)."""
    return (px - jx) * fy - (py - jy) * fx


def body_pose(knee_deg, trunk_lean_deg):
    """Knöchel bei (0,0). Knie gebeugt um knee_deg, Rumpf um trunk_lean_deg nach vorne (negativ = zurückgelehnt)."""
    q = radians(knee_deg)
    sh = q * 0.42                     # Schienbein neigt sich nach vorne
    th = q - sh                       # Oberschenkel nach hinten
    ank = (0.0, SEG['ankle'])
    knee = (ank[0] + SEG['shank'] * sin(sh), ank[1] + SEG['shank'] * cos(sh))
    hip = (knee[0] - SEG['thigh'] * sin(th), knee[1] + SEG['thigh'] * cos(th))
    lean = radians(trunk_lean_deg)
    shoulder = (hip[0] + SEG['torso'] * sin(lean), hip[1] + SEG['torso'] * cos(lean))
    return dict(ank=ank, knee=knee, hip=hip, sh=shoulder)


# ---------------- Lastfälle ----------------
def case_rows():
    rows = []

    def add(name, tr, pose, grips, note=''):
        """grips: Liste (Name, Punkt relativ zur Schulter (dx, dy), Kraft auf den Arm (fx, fy) in N)."""
        S, Hp, K = pose['sh'], pose['hip'], pose['knee']
        res = dict(name=name, tree=tr, grips=[], note=note)
        hip_m = knee_m = 0.0
        fx_tot = fy_tot = 0.0
        for gname, (dx, dy), (fx, fy) in grips:
            ex, ey = arm_ik(dx, dy)
            px, py = S[0] + dx, S[1] + dy
            m_el = moment(px, py, S[0] + ex, S[1] + ey, fx, fy)
            m_sh = moment(px, py, S[0], S[1], fx, fy)
            hip_m += moment(px, py, Hp[0], Hp[1], fx, fy)
            knee_m += moment(px, py, K[0], K[1], fx, fy)
            fx_tot += fx; fy_tot += fy
            res['grips'].append(dict(name=gname, F=hypot(fx, fy), loop=hypot(fx, fy) / 2, el=m_el, sh=m_sh))
        res['hip'] = hip_m
        res['knee'] = knee_m
        res['Fv'] = -fy_tot
        res['Fh'] = fx_tot
        # Kippen: gemeinsamer Schwerpunkt von Körper, Exo und Last über der Standfläche?
        body_x = 0.55 * (Hp[0] + S[0]) / 2 + 0.45 * (K[0] + Hp[0]) / 2
        exo_x = Hp[0] - 0.12
        mom = BODY * G * body_x + EXO_MASS * G * exo_x
        for _, (dx, dy), (fx, fy) in grips:
            mom += -(S[0] + dx) * fy + (S[1] + dy) * fx
        x_cop = mom / ((BODY + EXO_MASS) * G - fy_tot)
        res['cop'] = x_cop
        rows.append(res)

    # 1 Aufrichten: Schnittende steht am Boden (Drehpunkt), rechter Arm hält es, linker Unterarm hebt weiter oben.
    for L in (2, 3, 4):
        tr = tree(L)
        xl = min(0.5 * L, 0.95)                # so weit Richtung Spitze reicht der linke Unterarm in der Hocke
        th = radians(15)
        Fl = tr['W'] * tr['xcg'] * cos(th) / xl
        add(f'Aufrichten {L} m', tr, body_pose(70, 35),
            [('links', (0.30, -0.30), (0, -Fl)), ('rechts', (0.20, -0.42), (0.15 * Fl, 0))],
            'Schnittende steht am Boden, linker Unterarm hebt bei 15°')

    # 2 Tragen fast senkrecht: rechte Schlaufen tragen das Gewicht, linker Arm stützt nur seitlich
    for L in (2, 3, 4):
        tr = tree(L)
        add(f'Tragen {L} m', tr, body_pose(15, -6),
            [('rechts', (0.33, -0.30), (0, -tr['W'])), ('links', (0.38, -0.05), (-0.08 * tr['W'], 0))],
            'Baum fast senkrecht vor dem Körper')

    # 3 Hochheben: Schnittende auf etwa 1,2 bis 1,4 m
    for L in (2, 3, 4):
        tr = tree(L)
        add(f'Hochheben {L} m', tr, body_pose(10, -8),
            [('rechts', (0.38, -0.05), (0, -tr['W'])), ('links', (0.36, 0.15), (-0.08 * tr['W'], 0))],
            'Schnittende knapp über Ladefläche')

    # 4 Schleifen: Stümpfe in den Schlaufen, Arme hängen, Spitzen schleifen. mu = Reibung Äste auf nassem Schnee/Schlamm.
    for n, L, mu in ((2, 2, 0.7), (4, 2, 0.7), (2, 3, 0.7)):
        tr = tree(L)
        Vt = tr['W'] * (L - tr['xcg']) / L      # Anteil Gewicht an den Händen, Rest liegt auf der Spitze
        Ft = mu * (tr['W'] - Vt)                # Schleppkraft je Baum
        per = n / 2
        add(f'Schleifen {n} × {L} m', tr, body_pose(10, 12),
            [('rechts', (-0.08, -0.47), (-Ft * per, -Vt * per)), ('links', (-0.08, -0.47), (-Ft * per, -Vt * per))],
            f'{n} Bäume, Reibung {mu}')
    return rows


def fmt(x):
    return f'{abs(x):.0f}'


def main():
    rows = case_rows()
    print('| Schritt | Baum | Kraft je Arm (N) | je Schlaufe (N) | Ellbogen je Arm (Nm) | Schulter je Arm (Nm) | Hüfte beide (Nm) | Knie beide (Nm) | Schwerpunkt vor Knöchel (cm) |')
    print('| --- | --- | --- | --- | --- | --- | --- | --- | --- |')
    for r in rows:
        tr = r['tree']
        g = max(r['grips'], key=lambda q: q['F'])
        el = max(abs(q['el']) for q in r['grips'])
        sh = max(abs(q['sh']) for q in r['grips'])
        print(f"| {r['name']} | {tr['m']} kg | {g['F']:.0f} | {g['loop']:.0f} | {el:.0f} | {sh:.0f} | {fmt(r['hip'])} | {fmt(r['knee'])} | {r['cop'] * 100:.0f} |")
    print()
    print('Antriebe je Körperseite (nach Reibung):')
    print()
    print('| Gelenk | Dauer (Nm) | Spitze (Nm) |')
    print('| --- | --- | --- |')
    for j in DRIVES:
        c, p = drive_capacity(j)
        print(f'| {j} | {c:.0f} | {p:.0f} |')
    # Seilkräfte
    print()
    print('Seilkraft bei Motorspitze:', f'{MOTOR_PEAK / DRUM_R:.0f} N je Seil')


if __name__ == '__main__':
    main()
