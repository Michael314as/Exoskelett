"""Festigkeit der Rahmenteile (Entwurf 4): Biegespannung je Bauteil bei Spitzenlast und Dauerlast.

Werkstoff: AlMgSi1 (EN AW-6082 T6), Streckgrenze 250 MPa. Ziel: Spitzenlast mit Sicherheit >= 2 (<= 125 MPa),
Dauerlast <= 50 MPa (Ermüdung, viele Lastwechsel). Rohre quadratisch bzw. rechteckig, Biegung um die schwache Achse.
Lasten aus docs/kraftverteilung.md (Spitzenmomente der Motoren, Aufrichten 3-m-Baum).
Aufruf: python3 tools/festigkeit.py  → Tabelle für docs/festigkeit.md
"""

def W_rect(b, h, t):
    """Widerstandsmoment eines Rechteckrohrs b (Breite) x h (Höhe in Biegerichtung) x t, in mm^3."""
    I = (b * h**3 - (b - 2 * t) * (h - 2 * t)**3) / 12
    return I / (h / 2)

# Bauteil, Belastung (Text), Biegemoment Spitze / Dauer [Nm], Profil alt, Profil neu
TEILE = [
    ("Rückenrahmen senkrecht (je)", "beide Schultern 2 × 100 Nm + Box 11 kg, auf 2 Rohre", 115, 40, (25, 25, 2), (30, 30, 2)),
    ("Hüftschwinge", "Hüftmoment 140 Nm + Last 510 N × 0,2 m (Stand auf einem Bein)", 240, 80, (25, 25, 2), (35, 35, 3)),
    ("Oberschenkelschiene außen", "Hüft- bzw. Kniemoment, größter Wert", 140, 47, (25, 25, 2), (30, 30, 2.5)),
    ("Oberschenkelschiene innen (Teleskop)", "wie außen, Überlappung >= 8 cm", 140, 47, (20, 20, 2), (25, 25, 2.5)),
    ("Unterschenkelschiene außen", "Kniemoment", 115, 31, (25, 25, 2), (30, 30, 2)),
    ("Unterschenkelschiene innen", "Kniemoment, unten kleiner", 90, 25, (20, 20, 2), (25, 25, 2)),
    ("Schulterbügel", "Schultermoment 100 Nm + Armlast 400 N × 0,1 m", 140, 45, (20, 20, 2), (30, 30, 3)),
    ("Oberarmschiene", "Ellbogenmoment 58 Nm + Baum am Handgelenk", 110, 35, (22, 22, 2), (25, 25, 3)),
    ("Unterarmschiene", "Schlaufenlast 400 N × 0,26 m", 105, 35, (22, 22, 2), (25, 25, 3)),
    ("Querrohre Rücken", "Torsion/Biegung aus einseitigem Armzug", 50, 15, (20, 20, 2), (20, 20, 2)),
]

def main():
    print("| Bauteil | Belastung | Moment Spitze / Dauer | vorher | σ Spitze / Dauer | neu | σ Spitze / Dauer | ok |")
    print("|---|---|---|---|---|---|---|---|")
    bad = 0
    for name, last, mp, md, alt, neu in TEILE:
        wa, wn = W_rect(*alt), W_rect(*neu)
        sa, sn = (mp * 1e3 / wa, md * 1e3 / wa), (mp * 1e3 / wn, md * 1e3 / wn)
        ok = sn[0] <= 125 and sn[1] <= 50
        bad += not ok
        f = lambda p: f"{p[0]:g} × {p[1]:g} × {p[2]:g}"
        print(f"| {name} | {last} | {mp} / {md} Nm | {f(alt)} | {sa[0]:.0f} / {sa[1]:.0f} MPa | **{f(neu)}** | {sn[0]:.0f} / {sn[1]:.0f} MPa | {'ja' if ok else 'NEIN'} |")
    print(f"\n{'alle Teile ausreichend' if not bad else f'{bad} Teile zu schwach'}")

if __name__ == "__main__":
    main()
