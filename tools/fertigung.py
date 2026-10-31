#!/usr/bin/env python3
"""Fertigungsdateien für das Christbaum-Exo (Entwurf 3.2).

Erzeugt aus Parametern:
  fertigung/laser/*.dxf   Laserteile (Alu/Edelstahl), nur Schnittkonturen in mm, eine Datei je Teil
  fertigung/druck/*.stl   3D-Druckteile für den Bambu Lab A1 mini (PETG/TPU)
  fertigung/teileliste.md Liste mit Material, Dicke, Stückzahl, Größe
  fertigung/laser/uebersicht.svg  alle Laserteile auf einen Blick
und prüft dabei: geschlossene Konturen, Löcher mit Mindeststegbreite, Druckteile wasserdicht und ≤ 180 mm.

Aufruf:  pip install ezdxf manifold3d numpy  &&  python3 tools/fertigung.py
Maße, die vom gekauften Teil abhängen (Lochbild der Flanschlager, Motorflansch), stehen oben als Parameter und
müssen vor der Bestellung mit dem Datenblatt verglichen werden.
"""
import math
import struct
import sys
from pathlib import Path

import ezdxf
import numpy as np
from manifold3d import CrossSection as CS, Manifold

ROOT = Path(__file__).resolve().parent.parent
OUT_L, OUT_D = ROOT / 'fertigung' / 'laser', ROOT / 'fertigung' / 'druck'
SEG = 96

# ------------------------------------------------------------------ Parameter (vom Kaufteil abhängig) ----
FLANSCH_LOCHABSTAND = 67.0   # Flanschlager 2-Loch, Bohrung 12 mm (z. B. KFL001): Lochabstand laut Datenblatt prüfen
FLANSCH_LOCH = 9.0           # Durchgang für M8
MOTOR_LOCHKREIS = 70.0       # Befestigungslochkreis des Motors (AK70-10: laut Datenblatt prüfen)
MOTOR_LOCH = 3.4             # Durchgang M3
MOTOR_ZENTRIER = 40.0        # Zentrierbund Ø
M10, M6, M5_GEWINDE, M4, M3 = 10.5, 6.6, 4.2, 4.5, 3.4   # Durchgänge bzw. Kernloch M5 (Spikes in Edelstahl schneiden)
STEG_MIN = 4.0               # kleinster Abstand Loch–Rand bzw. Loch–Loch (Laser-Faustregel ~ 1 × Blechdicke)

# ------------------------------------------------------------------ Hilfen 2D ----
def circ(r, x=0.0, y=0.0):
    return CS.circle(r, SEG).translate((x, y))

def rrect(w, h, r, x=0.0, y=0.0):
    """Rechteck mit gerundeten Ecken, Mitte (x, y)."""
    r = min(r, w / 2 - 0.01, h / 2 - 0.01)
    core = CS.square((w - 2 * r, h - 2 * r), True)
    return _round(core, r).translate((x, y))

def _round(cs, r):
    from manifold3d import JoinType
    return cs.offset(r, JoinType.Round, 2.0, SEG)

def slot(l, w, x=0.0, y=0.0, ang=0.0):
    """Langloch (Länge über alles l, Breite w)."""
    s = _round(CS.square((max(l - w, 0.01), 0.01), True), w / 2)
    return s.rotate(ang).translate((x, y))

def polar(fn, a0, a1, n=180):
    pts = []
    for i in range(n + 1):
        a = math.radians(a0 + (a1 - a0) * i / n)
        r = fn(a)
        pts.append((r * math.cos(a), r * math.sin(a)))
    return pts

# ------------------------------------------------------------------ Laserteile ----
LASER = []   # (name, cs, material, dicke, stück, zweck, umriss, löcher)

class P:
    """Teil = Umriss minus Löcher. Merkt sich die Löcher einzeln, damit die Prüfung jedes Loch kontrollieren kann."""
    def __init__(self, outline):
        self.outline, self.holes = outline, []
    def __sub__(self, hole):
        self.holes.append(hole); return self
    def cs(self):
        cs = self.outline
        for h in self.holes: cs = cs - h
        return cs

def laser(name, part, mat, t, n, zweck):
    if not isinstance(part, P): part = P(part)
    LASER.append((name, part.cs(), mat, t, n, zweck, part.outline, part.holes))

def check_holes(outline, holes):
    """Jedes Loch ganz im Umriss, mit Mindeststeg zum Rand und zu jedem anderen Loch."""
    errs = []
    inner = outline.offset(-STEG_MIN, __import__('manifold3d').JoinType.Round, 2.0, SEG)
    for i, h in enumerate(holes):
        if abs((h - inner).area()) > 1e-3:
            errs.append(f'Loch {i + 1} näher als {STEG_MIN} mm am Rand')
        grown = h.offset(STEG_MIN, __import__('manifold3d').JoinType.Round, 2.0, SEG)
        for j in range(i + 1, len(holes)):
            if (grown ^ holes[j]).area() > 1e-3:
                errs.append(f'Loch {i + 1} und {j + 1} näher als {STEG_MIN} mm')
    return errs

# Eckplatten Rückenrahmen (Rohr 25 × 25 an Querrohr 20 × 20)
laser('eckplatte', P(rrect(60, 60, 6)) - circ(M6 / 2, -15, 15) - circ(M6 / 2, 15, 15) - circ(M6 / 2, -15, -15) - circ(M6 / 2, 15, -15),
      'Alu 5754', 4, 24, 'Doppelplatten an allen Rahmenecken (2 je Ecke)')

# Lagerplatte für Hüft-, Knie-, Schulter- und Ellbogenlager (Flanschlager 2-Loch + Passschraube)
def lagerplatte(w, h):
    a = FLANSCH_LOCHABSTAND / 2
    return (P(rrect(w, h, 8)) - circ(M10 / 2) - circ(FLANSCH_LOCH / 2, -a, 0) - circ(FLANSCH_LOCH / 2, a, 0)
            - circ(M6 / 2, -a + 8, -h / 2 + 10) - circ(M6 / 2, a - 8, -h / 2 + 10) - circ(M6 / 2, 0, h / 2 - 10))
laser('lagerplatte', lagerplatte(95, 70), 'Alu 5754', 6, 16, 'Gelenkplatten Hüfte, Knie, Schulter, Ellbogen (je 2 Platten pro Gelenk)')

# Abspreizgelenk Hüfte: Gabellasche mit Scharnierbohrung, Anschlagbohrungen für Gummipuffer
laser('abspreiz_lasche', P(rrect(90, 45, 10)) - circ(M10 / 2, 25, 0) - circ(M6 / 2, -25, 10) - circ(M6 / 2, -25, -10) - slot(14, M4, 0, 14) - slot(14, M4, 0, -14),
      'Alu 5754', 6, 8, 'Scharnier Abspreizgelenk (4 je Seite: 2 Gabel, 2 Zunge)')

# Seilscheiben aus Schichten: Bord (voller Ø), Rille (Ø − 2 · Rillentiefe), Trennscheibe. Löcher fluchten durch alle Lagen.
def scheibe(R, rille=False, trenn=False, schrauben=6, pcd=None, erleichtern=True):
    r = R - 4.0 if rille else R
    cs = P(circ(r)) - circ(M10 / 2)
    pcd = pcd or (R * 0.62)
    for i in range(schrauben):
        a = 2 * math.pi * i / schrauben
        cs = cs - circ(M4 / 2, pcd * math.cos(a), pcd * math.sin(a))
    if erleichtern and R >= 60:                                                 # kleine Scheiben bleiben voll (Steg)
        for i in range(4):
            a = 2 * math.pi * i / 4 + math.pi / 4
            cs = cs - circ(R * 0.17, R * 0.38 * math.cos(a), R * 0.38 * math.sin(a))
    # Seilklemme: Bohrung durch die Rillenlage, in der das Seil endet
    if rille:
        cs = cs - circ(1.6, (r - 9.0) * math.cos(-1.2), (r - 9.0) * math.sin(-1.2))   # Seil durch die Lage, Klemmschraube daneben
    return cs
for nm, R, pair in (('schulter', 70, True), ('ellbogen', 50, True), ('huefte', 95, False)):
    laser(f'seilscheibe_{nm}_bord', scheibe(R), 'Alu 5754', 4, 2 * 2, f'Seilscheibe {nm} Ø{2 * R}: Bordscheiben außen')
    laser(f'seilscheibe_{nm}_rille', scheibe(R, rille=True), 'Alu 5754', 3, (2 if pair else 1) * 2, f'Seilscheibe {nm}: Rillenlage(n), 3 mm für Seil/Zug bis 2,5 mm')
    if pair:
        laser(f'seilscheibe_{nm}_trenn', scheibe(R, trenn=True), 'Alu 5754', 2, 2, f'Seilscheibe {nm}: Trennlage zwischen den beiden Rillen')

# Knie-Kurvenscheibe: Radius 50 mm gestreckt bis 80 mm in der Hocke (wie camR im 3D-Modell)
def camR(a):
    return 50 + 30 * min(1.0, max(0.0, (a + math.radians(11)) / math.radians(111)))
def kurve(rille):
    d = 4.0 if rille else 0.0
    pts = polar(lambda a: camR(a) - d, -110, 190, 240) + [(0.0, 0.0)]
    cs = P(CS([pts]) + circ(22)) - circ(M10 / 2) - circ(M4 / 2, 0, 16) - circ(M4 / 2, 0, -16) - circ(M4 / 2, 16, 0) - circ(M4 / 2, -16, 0)
    if rille:
        a = math.radians(-70)
        cs = cs - circ(1.6, (camR(a) - 12) * math.cos(a), (camR(a) - 12) * math.sin(a))
    return cs
laser('knie_kurvenscheibe_bord', kurve(False), 'Alu 5754', 4, 4, 'Knie-Kurvenscheibe: Bordscheiben (2 je Knie)')
laser('knie_kurvenscheibe_rille', kurve(True), 'Alu 5754', 3, 2, 'Knie-Kurvenscheibe: Rillenlage')

# Steigbügel: offener Sohlenrahmen, Spike-Gewinde M5, Schlitze für Riemen, Bohrungen für den Seitenbügel
def sohle():
    L, W = 290.0, 110.0
    p = P(_round(CS.square((L - W, 0.01), True), W / 2))                  # Stadionform wie eine Schuhsohle
    p - _round(CS.square((40, 0.01), True), 33).translate((-71, 0))        # zwei Aussparungen: 22 mm Rand, 16 mm Steg
    p - _round(CS.square((40, 0.01), True), 33).translate((71, 0))
    for x in (-105, -55, 0, 55, 105):                                      # 10 Spikes M5 im Rand
        for sgn in (-1, 1):
            p - circ(M5_GEWINDE / 2, x, sgn * 45)
    for x in (-25, 25):                                                    # Seitenbügel
        for sgn in (-1, 1):
            p - circ(M6 / 2, x, sgn * 44)
    p - slot(24, 6, -136, 0, 90)                                           # Riemenschlitze Ferse/Spitze
    p - slot(24, 6, 136, 0, 90)
    return p
laser('steigbuegel_sohle', sohle(), 'Edelstahl 1.4301', 4, 2, 'Sohlenrahmen, Spikes M5 einschrauben (Gewinde schneiden)')
laser('steigbuegel_seitenlasche', P(rrect(50, 130, 8)) - circ(M10 / 2, 0, 45) - circ(M6 / 2, -12, -50) - circ(M6 / 2, 12, -50) - slot(20, M6, 0, -5, 90),
      'Edelstahl 1.4301', 4, 2, 'Seitenbügel Sprunggelenk → Sohle (Langloch zur Höhenanpassung)')

# Schlaufenkopf-Halter (Unterarmschiene → Schlaufenkopf)
laser('schlaufenkopf_lasche', P(rrect(110, 40, 8)) - circ(M6 / 2, -42, 0) - circ(M6 / 2, -26, 0) - circ(M5_GEWINDE / 2 + 0.3, 20, 10) - circ(M5_GEWINDE / 2 + 0.3, 20, -10)
      - circ(3.0, 40, 0), 'Alu 5754', 4, 3, 'Halter Schlaufenkopf (rechts 2, links 1); 6-mm-Loch für Seildurchführung')

# Motorplatte der Antriebsbox: 6 Motoren in 3 Reihen × 2
def motorplatte():
    cs = P(rrect(290, 360, 10))
    for row, y in enumerate((-110, 0, 110)):
        for s in (-1, 1):
            x = s * 70
            cs = cs - circ(MOTOR_ZENTRIER / 2 + 0.2, x, y)
            for i in range(4):
                a = math.pi / 4 + i * math.pi / 2
                cs = cs - circ(MOTOR_LOCH / 2, x + MOTOR_LOCHKREIS / 2 * math.cos(a), y + MOTOR_LOCHKREIS / 2 * math.sin(a))
    for x in (-125, 125):
        for y in (-160, -40, 40, 160):
            cs = cs - circ(M6 / 2, x, y)
    cs = cs - slot(60, 10, 0, -170, 0) - slot(60, 10, 0, 170, 0)
    return cs
laser('motorplatte', motorplatte(), 'Alu 5754', 4, 1, 'Montageplatte Antriebsbox (Lochkreis Motor vor Bestellung prüfen)')

# Parkstütze: Klappgelenk-Lasche mit Rastloch
laser('parkstuetze_lasche', P(rrect(70, 56, 8)) - circ(M10 / 2, -20, -8) - circ(M6 / 2, 18, 10) - circ(M6 / 2, 18, -10) - circ(6.2 / 2, -20, 12),
      'Alu 5754', 4, 4, 'Klappgelenk Parkstütze (Rastbolzen Ø6 im oberen Loch)')

# ------------------------------------------------------------------ Druckteile (3D) ----
DRUCK = []   # (name, manifold, material, stück, zweck)

def druck(name, m, mat, n, zweck):
    DRUCK.append((name, m, mat, n, zweck))

def ring_sector(r_in, r_out, a0, a1):
    pts = polar(lambda a: r_out, a0, a1, 120) + polar(lambda a: r_in, a1, a0, 120)
    return CS([pts])

def manschette(r, h, name, n):
    """Halbschale (200°) mit Boss für die Wägezelle an der Seite und Schlitzen für den Klettgurt."""
    t = 5.0
    shell = Manifold.extrude(ring_sector(r, r + t, -100, 100), h)
    boss = Manifold.cube((14, 22, 26), True).translate((0, r + t + 7, h / 2))
    m = shell + boss
    for z in (h / 2 - 6, h / 2 + 6):                                             # 2 × M3 in den Boss (Wägezelle)
        m = m - Manifold.cylinder(30, 1.6, 1.6, 24).rotate((90, 0, 0)).translate((0, r + t + 20, z))
    for a in (-92, 92):                                                          # Gurtschlitze an beiden Enden
        x, y = (r + t / 2) * math.cos(math.radians(a)), (r + t / 2) * math.sin(math.radians(a))
        m = m - Manifold.cube((t * 3, 4, h * 0.6), True).rotate((0, 0, a)).translate((x, y, h / 2))
    druck(name, m, 'PETG', n, f'Manschette r = {r:.0f} mm, Höhe {h:.0f} mm; Polster (Abstandsgewirke) innen einkleben')

manschette(72, 70, 'manschette_oberschenkel', 2)
manschette(55, 60, 'manschette_unterschenkel', 2)
manschette(50, 60, 'manschette_oberarm', 2)
manschette(42, 50, 'manschette_unterarm', 2)

def bohrschablone(profil, n_loecher=7, pitch=20.0, loch=6.6):
    """U-Schablone zum Aufschieben auf das Vierkantrohr, Bohrbuchsen-Löcher im 20-mm-Raster."""
    w, L, t = profil + 0.6, n_loecher * pitch + 10, 4.0
    body = Manifold.cube((L, w + 2 * t, profil * 0.7 + t)).translate((-L / 2, -(w + 2 * t) / 2, 0))
    slotm = Manifold.cube((L + 2, w, profil * 0.7 + 1)).translate((-L / 2 - 1, -w / 2, t))
    m = body - slotm
    for i in range(n_loecher):
        x = -L / 2 + 5 + pitch / 2 + i * pitch
        m = m - Manifold.cylinder(t * 3, loch / 2, loch / 2, 32).translate((x, 0, -1))
    return m
druck('bohrschablone_rohr25', bohrschablone(25), 'PETG', 1, 'Lochraster der Außenrohre 25 × 25 (Steckbolzen Ø6)')
druck('bohrschablone_rohr20', bohrschablone(20), 'PETG', 1, 'Lochraster der Innenrohre 20 × 20')

def zellenhalter():
    """Halter Wägezelle (Biegebalken 12,7 × 12,7 × 80) an der Schiene, mit Aufnahme für die Überlastpuffer."""
    base = Manifold.cube((30, 34, 16), True)
    m = base - Manifold.cube((13.2, 40, 13.2), True).translate((0, 0, 4))      # Tasche für den Balken
    for y in (-8, 8):
        m = m - Manifold.cylinder(30, 1.7, 1.7, 24).translate((0, y, -15))      # M3 in die Zelle
    for x in (-10, 10):
        m = m - Manifold.cylinder(30, 3.3, 3.3, 24).rotate((90, 0, 0)).translate((x, 0, -3))   # M6 zur Schiene
    return m
druck('waegezellen_halter', zellenhalter(), 'PETG', 8, 'Halter je Wägezelle (an der Schiene)')

def puffer():
    return Manifold.cylinder(6, 5, 4.5, 32) - Manifold.cylinder(8, 1.7, 1.7, 16).translate((0, 0, -1))
druck('ueberlastpuffer', puffer(), 'TPU 95A', 16, 'Überlastpuffer (2 je Wägezelle), 1 mm Spalt einstellen')

def tastergriff(n_taster):
    body = Manifold.cube((60, 38, 34), True)
    body = body - Manifold.cylinder(40, 9, 9, 48).rotate((0, 90, 0)).translate((-30, 0, -6))   # Rohr Ø18 der Handschlaufe
    for i in range(n_taster):
        y = 0 if n_taster == 1 else (-9 + 18 * i)
        body = body - Manifold.cylinder(20, 8.1, 8.1, 48).translate((14, y, 8))                # IP67-Taster Ø16
    body = body - Manifold.cube((50, 26, 18), True).translate((0, 0, 4))                         # Kabelraum
    return body
druck('tastergriff_rechts', tastergriff(2), 'PETG', 1, 'Daumentaster rechts (2 Schlaufen) mit Heizfolie innen')
druck('tastergriff_links', tastergriff(1), 'PETG', 1, 'Daumentaster links (1 Schlaufe)')

def haube(seite):
    """Haube über der Antriebsbox, in zwei Hälften (je ≤ 180 mm), Lüftungsschlitze, Verschraubung in der Mitte."""
    L, W, H, t = 155.0, 150.0, 50.0, 2.4
    outer = Manifold.cube((L, W, H)).translate((0, -W / 2, 0))
    inner = Manifold.cube((L - t + 0.01 if seite == 'links' else L - t, W - 2 * t, H - t)).translate((t if seite == 'rechts' else 0, -W / 2 + t, -0.01))
    m = outer - inner
    for i in range(6):
        m = m - Manifold.cube((90, 4, 10), True).translate((L / 2, -50 + i * 20, H - 1))
    fl = Manifold.cube((8, W - 2 * t, 12)).translate(((L - 8) if seite == 'links' else 0, -W / 2 + t, H - t - 12))
    m = m + fl
    for y in (-50, 0, 50):
        m = m - Manifold.cylinder(30, 1.7, 1.7, 24).rotate((0, 90, 0)).translate((-5, y, H - t - 6))
    return m if seite == 'links' else m.mirror((1, 0, 0))
druck('haube_links', haube('links'), 'PETG', 1, 'Haube Antriebsbox, linke Hälfte')
druck('haube_rechts', haube('rechts'), 'PETG', 1, 'Haube Antriebsbox, rechte Hälfte')

# ------------------------------------------------------------------ Ausgabe und Prüfung ----
def write_stl(path, m):
    mesh = m.to_mesh()
    v = np.asarray(mesh.vert_properties)[:, :3]
    f = np.asarray(mesh.tri_verts)
    with open(path, 'wb') as fh:
        fh.write(b'Christbaum-Exo'.ljust(80, b' '))
        fh.write(struct.pack('<I', len(f)))
        for tri in f:
            a, b, c = v[tri[0]], v[tri[1]], v[tri[2]]
            nrm = np.cross(b - a, c - a); ln = np.linalg.norm(nrm); nrm = nrm / ln if ln > 0 else nrm
            fh.write(struct.pack('<12fH', *nrm, *a, *b, *c, 0))

def min_web(cs):
    """kleinster Abstand zwischen zwei Konturen (Loch–Rand, Loch–Loch) als Näherung über die Punkte."""
    polys = [np.asarray(p) for p in cs.to_polygons()]
    best = 1e9
    for i in range(len(polys)):
        for j in range(i + 1, len(polys)):
            a, b = polys[i], polys[j]
            d = np.sqrt(((a[:, None, :] - b[None, :, :]) ** 2).sum(-1)).min()
            best = min(best, d)
    return best

def main():
    OUT_L.mkdir(parents=True, exist_ok=True); OUT_D.mkdir(parents=True, exist_ok=True)
    errors, rows_l, rows_d, svg_parts = [], [], [], []
    for name, cs, mat, t, n, zweck, outline, holes in LASER:
        polys = cs.to_polygons()
        if not polys or cs.area() <= 0:
            errors.append(f'{name}: leere Kontur'); continue
        doc = ezdxf.new('R2010', setup=False); doc.units = ezdxf.units.MM
        msp = doc.modelspace()
        for p in polys:
            msp.add_lwpolyline([(float(x), float(y)) for x, y in p], close=True, dxfattribs={'layer': 'SCHNITT'})
        doc.saveas(OUT_L / f'{name}.dxf')
        (x0, y0, x1, y1) = cs.bounds()
        web = min_web(cs)
        if web < STEG_MIN:
            errors.append(f'{name}: Steg nur {web:.1f} mm (< {STEG_MIN} mm)')
        errors += [f'{name}: {e}' for e in check_holes(outline, holes)]
        if len(polys) - 1 != len(holes):
            errors.append(f'{name}: {len(holes)} Löcher gezeichnet, aber {len(polys) - 1} Innenkonturen (Löcher verschmolzen oder offen)')
        rows_l.append((name, mat, t, n, f'{x1 - x0:.0f} × {y1 - y0:.0f}', len(polys) - 1, f'{web:.1f}', zweck))
        svg_parts.append((name, polys, (x0, y0, x1, y1)))
    for name, m, mat, n, zweck in DRUCK:
        if m.status().name != 'NoError' or m.volume() <= 0:
            errors.append(f'{name}: Körper ungültig ({m.status().name})'); continue
        bb = m.bounding_box()
        size = [bb[3] - bb[0], bb[4] - bb[1], bb[5] - bb[2]]
        fits = sorted(size)[-1] <= 180.0 or (sorted(size)[-1] <= 254 and sorted(size)[-2] <= 180 and sorted(size)[0] <= 180 and math.hypot(sorted(size)[-1], sorted(size)[-2]) <= 180 * 1.414)
        if max(size) > 180.0 and not fits:
            errors.append(f'{name}: {max(size):.0f} mm passt nicht in den A1 mini')
        write_stl(OUT_D / f'{name}.stl', m)
        rows_d.append((name, mat, n, ' × '.join(f'{s:.0f}' for s in size), f'{m.volume() / 1000:.0f}', 'ja' if max(size) <= 180 else 'diagonal', m.genus(), zweck))
    # Übersicht als SVG
    x, y, rowh, Wsheet, items = 10.0, 10.0, 0.0, 1400.0, []
    for name, polys, (x0, y0, x1, y1) in svg_parts:
        w, h = x1 - x0, y1 - y0
        if x + w > Wsheet: x, y, rowh = 10.0, y + rowh + 40, 0.0
        d = ' '.join('M ' + ' L '.join(f'{px - x0 + x:.1f},{(y1 - py) + y:.1f}' for px, py in p) + ' Z' for p in polys)
        items.append(f'<path d="{d}" fill="#c9d0d4" fill-rule="evenodd" stroke="#1d2a33" stroke-width="0.6"/>'
                     f'<text x="{x:.0f}" y="{y + h + 14:.0f}" font-size="11" font-family="sans-serif">{name}</text>')
        x += w + 30; rowh = max(rowh, h)
    H = y + rowh + 40
    (OUT_L / 'uebersicht.svg').write_text(f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {Wsheet + 20:.0f} {H:.0f}" width="{Wsheet + 20:.0f}" height="{H:.0f}">'
                                          f'<rect width="100%" height="100%" fill="#fff"/>{"".join(items)}</svg>', encoding='utf-8')
    # Teileliste
    md = ['# Teileliste Fertigung (erzeugt mit tools/fertigung.py)', '',
          'Alle Maße in mm. DXF: nur Schnittkonturen (Ebene SCHNITT), Maßstab 1:1. STL: Druckteile, Einheit mm.', '',
          '**Vor der Bestellung prüfen:** Lochabstand der Flanschlager (Parameter `FLANSCH_LOCHABSTAND` = '
          f'{FLANSCH_LOCHABSTAND:.0f} mm) und Lochkreis der Motoren (`MOTOR_LOCHKREIS` = {MOTOR_LOCHKREIS:.0f} mm) mit den Datenblättern der gekauften Teile.', '',
          '## Laserteile', '', '| Datei | Material | Dicke | Stück | Größe | Löcher | kleinster Steg | Zweck |', '|---|---|---|---|---|---|---|---|']
    md += [f'| `laser/{r[0]}.dxf` | {r[1]} | {r[2]} mm | {r[3]} | {r[4]} | {r[5]} | {r[6]} mm | {r[7]} |' for r in rows_l]
    md += ['', '## Druckteile (Bambu Lab A1 mini, 180 × 180 × 180 mm)', '', '| Datei | Material | Stück | Größe | Volumen cm³ | passt | Genus | Zweck |', '|---|---|---|---|---|---|---|---|']
    md += [f'| `druck/{r[0]}.stl` | {r[1]} | {r[2]} | {r[3]} | {r[4]} | {r[5]} | {r[6]} | {r[7]} |' for r in rows_d]
    md += ['', 'Druckeinstellungen: PETG 0,2 mm Schicht, 4 Wände, 30 % Gyroid, Manschetten liegend auf der Schnittkante; TPU 95A langsam (25 mm/s).', '']
    (ROOT / 'fertigung' / 'teileliste.md').write_text('\n'.join(md), encoding='utf-8')
    print(f'{len(rows_l)} Laserteile, {len(rows_d)} Druckteile geschrieben')
    for e in errors: print('FEHLER:', e)
    return 1 if errors else 0

if __name__ == '__main__':
    sys.exit(main())
