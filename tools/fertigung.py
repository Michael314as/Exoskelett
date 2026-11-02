#!/usr/bin/env python3
"""Fertigungsdateien für das Christbaum-Exo (Entwurf 5: Gelenkkapseln).

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
# Flanschlager 2-Loch (Händlerdaten, vor der Bestellung am gelieferten Teil prüfen): Bohrung, Lochabstand, Schraubenloch
KFL = {'KFL08': (8, 37.0, 4.8), 'KFL001': (12, 48.0, 6.6), 'KFL002': (15, 53.0, 7.0)}
# Motoren RobStride (Maßzeichnungen in den Handbüchern, github.com/RobStride/Product_Information):
#   RS04: Ø 120 × 52, Gehäuse 10 × M4 auf Ø 106 (vorne und hinten), Abtrieb 9 × M5 auf Ø 36 + 3 Stifte Ø 6, Bund Ø 94
#   RS03: Flansch Ø 98 (8 × M4 auf Ø 98), Körper Ø 80 × 54, Abtrieb 6 × M4 auf Ø 30,36 + 3 Stifte Ø 4
RS04 = dict(lk=106.0, n=10, loch=4.5, ab_lk=36.0, ab_n=9, ab_loch=5.5)
RS03 = dict(lk=98.0, n=8, loch=4.5, ab_lk=30.36, ab_n=6, ab_loch=4.5)
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
def lagerplatte(typ, w, h):
    welle, abstand, loch = KFL[typ]
    a = abstand / 2
    return (P(rrect(w, h, 8)) - circ((welle + 1.0) / 2) - circ(loch / 2, -a, 0) - circ(loch / 2, a, 0)
            - circ(M6 / 2, -w / 2 + 10, -h / 2 + 10) - circ(M6 / 2, w / 2 - 10, -h / 2 + 10) - circ(M6 / 2, 0, h / 2 - 10))
laser('lagerplatte_kfl001', lagerplatte('KFL001', 80, 64), 'Alu 6082 T6', 6, 8, 'Gelenkplatten Knie und Schulter für KFL001 (Lochabstand 48 mm), 2 je Gelenk')
laser('lagerplatte_kfl08', lagerplatte('KFL08', 66, 56), 'Alu 6082 T6', 6, 8, 'Gelenkplatten Ellbogen und Knöchel für KFL08 (Lochabstand 37 mm), 2 je Gelenk')
laser('lagerplatte_kfl002', lagerplatte('KFL002', 86, 66), 'Alu 6082 T6', 8, 2, 'Hüftlager KFL002 an der Hüftschwinge (Lochabstand 53 mm)')

def lochkreis(cs, lk, n, d, x=0.0, y=0.0, dreh=0.0):
    for i in range(n):
        a = math.radians(dreh + 360.0 / n * i)
        cs = cs - circ(d / 2, x + lk / 2 * math.cos(a), y + lk / 2 * math.sin(a))
    return cs

# Abspreizgabel Hüfte (HU-01): zwei Wangen je Seite, Achse Ø 16 nach vorne durch das Hüftgelenk, oben an den Rahmen geschraubt
laser('abspreizgabel_wange', P(rrect(60, 100, 8)) - circ(16.2 / 2, 0, -25) - circ(M6 / 2, -15, 32) - circ(M6 / 2, 15, 32) - circ(M5_GEWINDE / 2 + 0.3, 0, 12),
      'Alu 6082 T6', 8, 4, 'Abspreizgabel HU-01: 2 Wangen je Seite; Ø 16,2 für Passschraube/Kugelsperrbolzen, M6 zum Rahmen, M5 Federhalter')
laser('abspreizgabel_steg', P(rrect(82, 60, 6)) - circ(M6 / 2, -30, -18) - circ(M6 / 2, 30, -18) - circ(M6 / 2, -30, 18) - circ(M6 / 2, 30, 18) - slot(20, M4, 0, 0),
      'Alu 6082 T6', 6, 2, 'Steg zwischen den Gabelwangen (oben), Langloch für den Zentrierfeder-Halter')
laser('zentrierfeder_halter', P(rrect(28, 30, 4)) - circ(M4 / 2, 0, 7) - circ(6.2 / 2, 0, -6),
      'Edelstahl 1.4301', 3, 4, 'Halter Zentrierfeder HU-03 (2 je Seite), Ø 6 Zapfen für die Feder')

# ---------- Gelenkkapseln (Entwurf 5) ----------
# Motorträger: Der gekaufte Stiftkühlkörper ist zugleich der Deckel (Lochbild nach Bohrschablone); die Wangen verbinden ihn mit Schiene bzw. Schwinge
laser('motortraeger_wange', P(rrect(40, 120, 6)) - circ(M6 / 2, 0, 50) - circ(M6 / 2, 0, -50) - slot(24, M6, 0, 0, 90) - circ(M6 / 2, -10, -30) - circ(M6 / 2, 10, -30),
      'Alu 6082 T6', 8, 12, 'Wangen zwischen Schiene bzw. Schwinge und Motorträger (2 je Kapsel an Knie und Ellbogen, 1 je Kapsel an Hüfte und Schulter); Langloch zum Ausrichten')
# Abtriebsflansche: Oberschenkel (Hüfte) und Oberarm (Schulter) am Abtrieb des RS04; Stiftlöcher Ø 6 nach dem Motor bohren (Steg)
def abtriebsflansch(m, w, h):
    p = lochkreis(P(rrect(w, h, 8)) - circ(9), m['ab_lk'], m['ab_n'], m['ab_loch'])
    for x in (-w / 2 + 10, w / 2 - 10):
        for y in (-h / 2 + 10, h / 2 - 10):
            p = p - circ(M6 / 2, x, y)
    return p
laser('abtriebsflansch_rs04', abtriebsflansch(RS04, 90, 90), 'Alu 6082 T6', 8, 4, 'Flansch Abtrieb RS04 → Oberschenkelplatte (Hüfte) bzw. Oberarmschiene (Schulter); 9 × M5 auf Ø 36')
# Kurbeln und Hebel (Parallelogramm 1:1): Kurbel am Motorabtrieb, Hebel am Gelenk, Zapfen für die Gelenkköpfe
def kurbel(m, r, zapfen, nabe):
    p = P(circ(nabe) + rrect(r, 2 * zapfen + 8, zapfen + 4, r / 2, 0) + circ(zapfen + 4, r, 0)) - circ(6)
    p = lochkreis(p, m['ab_lk'], m['ab_n'], m['ab_loch'], dreh=20)
    return p - circ(zapfen / 2, r, 0)
laser('kurbel_knie', kurbel(RS04, 60.0, 8.2, 27.0), 'Alu 6082 T6', 10, 2, 'Kurbel BA-01: auf dem Abtrieb des Kniemotors (9 × M5), Zapfen M8 bei 60 mm')
laser('kurbel_ellbogen', kurbel(RS03, 50.0, 6.2, 23.0), 'Alu 6082 T6', 8, 2, 'Kurbel AR-04: auf dem Abtrieb des Ellbogenmotors (6 × M4), Zapfen M6 bei 50 mm')
def hebel(r, zapfen, bohrung, lk, n, d, nabe):
    p = P(circ(nabe) + rrect(r, 2 * zapfen + 8, zapfen + 4, r / 2, 0) + circ(zapfen + 4, r, 0)) - circ(bohrung / 2)
    p = lochkreis(p, lk, n, d, dreh=30)
    return p - circ(zapfen / 2, r, 0)
laser('kniehebel', hebel(60.0, 8.2, 12.5, 32.0, 3, M5_GEWINDE + 0.3, 24.0), 'Alu 6082 T6', 10, 2, 'Kniehebel BA-04: auf dem Kniebolzen, 3 × M5 in die Unterschenkelplatte, Zapfen M8 bei 60 mm')
laser('ellbogenhebel', hebel(50.0, 6.2, 8.5, 26.0, 3, M4, 20.0), 'Alu 6082 T6', 8, 2, 'Ellbogenhebel AR-08: auf dem Ellbogenbolzen, 3 × M4, Zapfen M6 bei 50 mm')

# Steigbügel (BE-11) unter der Sohlenmitte: Abwicklung, die beiden Laschen werden 90° hochgekantet (Biegelinie ±67,5 mm)
def steigbuegel():
    L, B, lasche = 60.0, 135.0, 35.0
    p = P(rrect(L, B + 2 * lasche, 6))
    for x in (-21, 21):                                                    # 6 Spikes M5 (Gewinde schneiden)
        for y in (-50, 0, 50):
            p - circ(M5_GEWINDE / 2, x, y)
    p - circ(M10 / 2, 0, 25)                                              # Zapfen Drehteller BE-14 (vorne versetzt, zwischen den Spikes)
    for s in (-1, 1):                                                      # Laschen: Fersenschale + Ristriemen
        p - circ(M6 / 2, -14, s * (B / 2 + 20)) - circ(M6 / 2, 14, s * (B / 2 + 20))
    return p
laser('steigbuegel', steigbuegel(), 'Edelstahl 1.4301', 4, 2, 'Steigbügel BE-11: Abwicklung 60 × 205, Laschen an den Linien ±67,5 mm 90° hochkanten (Kantdienstleister)')
laser('drehteller_scheibe', P(circ(32)) - circ(M10 / 2) - circ(M4 / 2, 22, 0) - circ(M4 / 2, -22, 0) - circ(M4 / 2, 0, 22) - circ(M4 / 2, 0, -22),
      'Edelstahl 1.4301', 2, 4, 'Drehteller BE-14: Laufscheiben über und unter dem Axial-Nadellager AXK (2 je Fuß), M4 für die Wägezellen-Platte')
laser('fersenbuegel_lasche', P(rrect(40, 50, 6)) - circ(M10 / 2, 0, 10) - circ(M6 / 2, -10, -14) - circ(M6 / 2, 10, -14),
      'Alu 6082 T6', 8, 2, 'Fersenbügel BE-09 am Knöchellager (Rundstab Ø 16 wird eingeschweißt oder geklemmt)')
laser('kippgelenk_gabel', P(rrect(30, 40, 5)) - circ(8.2 / 2, 0, 8) - circ(M5_GEWINDE / 2 + 0.3, 0, -10),
      'Alu 6082 T6', 6, 4, 'Kippgelenk BE-10 hinter der Ferse: 2 Wangen je Fuß, Bolzen Ø 8 mit Gummibuchse')

# Lendengelenk (RA-09): Scharnierlaschen, Rastsegment für den Hubmagneten, Gasfeder-Halter
laser('lendengelenk_lasche', P(rrect(70, 50, 8)) - circ(M10 / 2, 20, 0) - circ(M6 / 2, -20, 12) - circ(M6 / 2, -20, -12),
      'Alu 6082 T6', 8, 8, 'Scharnier Lendengelenk: 2 Laschen oben + 2 unten je Rahmenrohr; Bolzen Ø 10, Elastomerfeder dazwischen')
def rastsegment():
    pts = polar(lambda a: 85.0, -35, 35, 60) + [(0.0, 0.0)]
    p = P(CS([pts]) + circ(18)) - circ(M10 / 2)
    for a in (-25, -12.5, 0, 12.5, 25):                                    # Rastlöcher für den Sperrbolzen Ø 8
        p - circ(8.5 / 2, 70 * math.cos(math.radians(a)), 70 * math.sin(math.radians(a)))
    p - circ(M6 / 2, 32, 8) - circ(M6 / 2, 32, -8)
    return p
laser('lendensperre_rastsegment', rastsegment(), 'Edelstahl 1.4301', 4, 1, 'Sperre RA-09: Hubmagnet-Bolzen rastet in 5 Stellungen (−25 … +25°), sperrt bei Last')
laser('gasfeder_halter', P(rrect(44, 34, 5)) - circ(M6 / 2, -12, -7) - circ(M6 / 2, 12, -7) - circ(M6 / 2, 0, 8),
      'Alu 6082 T6', 5, 4, 'Halter Gasfeder RA-10 (oben und unten), M6 für den Kugelzapfen')

# Schulter: Gabel (SC-02) mit Hohlachse Ø 22 und Schlitten der Schulterblatt-Führung (SC-12)
laser('schultergabel_wange', P(rrect(55, 80, 8)) - circ(22.2 / 2, 0, -18) - circ(M6 / 2, -14, 26) - circ(M6 / 2, 14, 26),
      'Alu 6082 T6', 10, 4, 'Schultergabel SC-02: 2 Wangen je Seite, Hohlachse Ø 22 (Seile laufen innen durch)')
laser('schulterschlitten', P(rrect(70, 55, 6)) - circ(15.2 / 2, -18, 0) - circ(15.2 / 2, 18, 0) - circ(M6 / 2, 0, 18) - circ(M6 / 2, 0, -18),
      'Alu 6082 T6', 8, 2, 'Schlitten SC-12: 2 Linearlager LM8UU (Ø 15) auf Wellen Ø 8, ±30 mm Hub')

# Schlaufenträger (SG-01): gerades Band, nach Biegeschablone um das Handgelenk gebogen
def schlaufentraeger(koepfe):
    p = P(rrect(200, 30, 6)) - circ(M6 / 2, -88, 0) - circ(M6 / 2, -70, 0)
    for x in (-30, 10, 50):                                                # Clips des Bandkanals
        p - circ(M4 / 2, x, 7)
    for i in range(koepfe):                                                # je Schlaufenkopf 2 × M5
        x = 82 - i * 50
        p - circ(M5_GEWINDE / 2 + 0.3, x, -6) - circ(M5_GEWINDE / 2 + 0.3, x - 14, -6)
    return p
laser('schlaufentraeger_rechts', schlaufentraeger(2), 'Alu 6082 T6', 6, 1, 'Schlaufenträger rechts (2 Köpfe: unten + außen), kalt biegen nach Schablone')
laser('schlaufentraeger_links', schlaufentraeger(1), 'Alu 6082 T6', 6, 1, 'Schlaufenträger links (1 Kopf unten)')

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
druck('haube_links', haube('links'), 'PETG', 1, 'Haube Elektronikbox, linke Hälfte')
druck('haube_rechts', haube('rechts'), 'PETG', 1, 'Haube Elektronikbox, rechte Hälfte')

def fingerschutz(R, breite, name, n, bogen=200):
    """Halbschale über einer Seilscheibe (RA-11): Mantel mit Abstand 6 mm, eine Seitenwand, Schraublaschen. Druck in transparentem PETG."""
    t, ra = 1.8, R + 6.0
    mantel = Manifold.extrude(ring_sector(ra, ra + t, -bogen / 2, bogen / 2), breite)
    wand = Manifold.extrude(ring_sector(R * 0.55, ra + t, -bogen / 2, bogen / 2), t)
    m = mantel + wand
    for a in (-bogen / 2 + 6, bogen / 2 - 6):                             # Laschen mit M4 an den Enden
        x, y = (ra + 8) * math.cos(math.radians(a)), (ra + 8) * math.sin(math.radians(a))
        lug = Manifold.cube((14, 14, t * 2), True).translate((x, y, t))
        m = m + lug - Manifold.cylinder(10, M4 / 2, M4 / 2, 24).translate((x, y, -1))
    druck(name, m, 'PETG transparent', n, f'Fingerschutz RA-11 über Scheibe Ø {2 * R:.0f}: nichts kann zwischen Seil und Scheibe geraten')
fingerschutz(78, 18, 'kurbelschutz_knie', 2, bogen=200)
fingerschutz(66, 16, 'kurbelschutz_ellbogen', 2, bogen=200)

def kapselmantel(r_motor, laenge, name, n):
    """Mantel der Gelenkkapsel (AN-02): offener Ring aus PETG-CF, Wand 2 mm, an beiden Enden eine O-Ring-Nut (gegen Kühlkörper
    bzw. Motorträger), seitlich M16 für die Kabelverschraubung und M12 für das Druckausgleichselement. Keine Dämmung: Die Wärme
    soll über den Kühlkörper hinaus."""
    ri = r_motor + 3.0; t = 2.4; h = laenge + 4.0
    m = Manifold.cylinder(h, ri + t, ri + t, 96) - Manifold.cylinder(h + 2, ri, ri, 96).translate((0, 0, -1))
    for z in (0.0, h - 2.0):                                                                          # O-Ring-Nuten an den Stirnseiten
        m = m - (Manifold.cylinder(2.0, ri + 1.6, ri + 1.6, 96) - Manifold.cylinder(2.0, ri + 0.4, ri + 0.4, 96)).translate((0, 0, z))
    m = m - Manifold.cylinder(t * 4, 8.1, 8.1, 32).rotate((90, 0, 0)).translate((0, -(ri - t), h / 2))   # M16 Kabel (unten)
    m = m - Manifold.cylinder(t * 4, 6.1, 6.1, 32).rotate((90, 0, 0)).rotate((0, 0, 40)).translate(((ri - t) * math.sin(math.radians(40)), -(ri - t) * math.cos(math.radians(40)), h / 2))   # M12 Druckausgleich
    druck(name, m, 'PETG-CF', n, f'Kapselmantel AN-02 für Motor Ø {2 * r_motor:.0f}: O-Ring-Nuten beidseitig, M16 Kabel, M12 Druckausgleich nach unten')
kapselmantel(60.0, 52.0, 'kapselmantel_rs04', 6)
kapselmantel(49.0, 54.0, 'kapselmantel_rs03', 2)

def bohrschablone_kuehlkoerper(m, r_kk, name):
    """Aufsteckschablone für den gekauften Stiftkühlkörper: Lochbild des Motorgehäuses + 2 Löcher für die Wangen, mit Bohrbuchsen."""
    t = 6.0
    s0 = Manifold.cylinder(t, r_kk + 3, r_kk + 3, 96) - Manifold.cylinder(t, 12, 12, 48)
    rand = Manifold.cylinder(8.0, r_kk + 3, r_kk + 3, 96) - Manifold.cylinder(8.0, r_kk + 0.3, r_kk + 0.3, 96)   # Kragen umgreift den Kühlkörper
    s0 = s0 + rand.translate((0, 0, t))
    for k in range(m['n']):
        a = math.radians(18 + 360.0 / m['n'] * k)
        s0 = s0 - Manifold.cylinder(t + 2, 1.65, 1.65, 24).translate((m['lk'] / 2 * math.cos(a), m['lk'] / 2 * math.sin(a), -1))   # Ø 3,3 vorbohren, dann M4 schneiden
    for x in (-(r_kk - 9), r_kk - 9):
        s0 = s0 - Manifold.cylinder(t + 2, 2.5, 2.5, 24).translate((x, 0, -1))                                                     # Ø 5 für M6-Kernloch der Wangen
    druck(name, s0, 'PETG', 1, f'Bohrschablone für den Stiftkühlkörper (Lochkreis Ø {m["lk"]:.0f}, Motorgehäuse); Pad zwischen Motor und Kühlkörper')
bohrschablone_kuehlkoerper(RS04, 70.0, 'bohrschablone_kuehlkoerper_rs04')
bohrschablone_kuehlkoerper(RS03, 60.0, 'bohrschablone_kuehlkoerper_rs03')

def akkuschacht_wand():
    """Seitenwand Akkuschacht EN-02 (Stihl AP 500 S 70 × 116 × 164): Einschub von hinten, Nut für Alu-Führungsschiene 4 × 4."""
    L, H, t = 116.0, 176.0, 4.0
    m = Manifold.cube((L, H, t)).translate((-L / 2, -H / 2, 0))
    for y in (-60, 60):
        m = m + Manifold.cube((L, 9, 6)).translate((-L / 2, y - 4.5, t)) - Manifold.cube((L + 2, 4.4, 4.4)).translate((-L / 2 - 1, y - 2.2, t + 1.8))
    for x in (-40, 40):
        for y in (-80, 80):
            m = m - Manifold.cylinder(20, M4 / 2, M4 / 2, 24).translate((x, y, -1))
    for y in (-30, 0, 30):                                                 # Wasserablauf / Lüftung
        m = m - Manifold.cube((60, 4, 20), True).translate((0, y, 0))
    return m
druck('akkuschacht_wand', akkuschacht_wand(), 'PETG-CF', 4, 'Akkuschacht EN-02: Innen- und Außenwand je Seite (zweiter Schacht nur bei Bedarf bestücken)')

def akkuschacht_stirn():
    """Stirnwand mit Kontaktträger: Fenster für die Stihl-Kontaktleiste, Dichtlippe aus TPU separat."""
    W, H, t = 76.0, 176.0, 6.0
    m = Manifold.cube((W, H, t)).translate((-W / 2, -H / 2, 0))
    m = m - Manifold.cube((22, 46, t + 2), True).translate((0, 0, t / 2))
    for y in (-35, 35):
        m = m - Manifold.cylinder(20, 1.4, 1.4, 16).translate((0, y, -1))   # Kontaktträger M3 (Kernloch)
    for x in (-28, 28):
        for y in (-80, 80):
            m = m - Manifold.cylinder(20, M4 / 2, M4 / 2, 24).translate((x, y, -1))
    return m
druck('akkuschacht_stirn', akkuschacht_stirn(), 'PETG-CF', 2, 'Stirnwand Akkuschacht mit Fenster für den Kontaktträger')

def fersenschale():
    t, r, h = 4.0, 56.0, 50.0
    m = Manifold.extrude(ring_sector(r, r + t, 100, 260), h)
    for a in (110, 250):
        x, y = (r + t / 2) * math.cos(math.radians(a)), (r + t / 2) * math.sin(math.radians(a))
        m = m - Manifold.cylinder(30, M6 / 2, M6 / 2, 24).rotate((0, 90, 0)).rotate((0, 0, a)).translate((x, y, 12)).translate((-math.cos(math.radians(a)) * 10, -math.sin(math.radians(a)) * 10, 0))
        m = m - Manifold.cube((t * 3, 4, 26), True).rotate((0, 0, a)).translate((x, y, 34))
    return m
druck('fersenschale', fersenschale(), 'PETG', 2, 'Fersenschale BE-13 am Steigbügel, Schlitze für den Ratschenriemen')

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
          '**Vor der Bestellung prüfen:** Lochabstände der Flanschlager (Tabelle `KFL`: KFL08 37 mm, KFL001 48 mm, KFL002 53 mm) und Lochbilder der Motoren (RobStride RS04: 10 × M4 auf Ø 106, Abtrieb 9 × M5 auf Ø 36; RS03: 8 × M4 auf Ø 98, Abtrieb 6 × M4 auf Ø 30,36) mit den Datenblättern der gekauften Teile.', '',
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
