"""Festigkeit des Gerüsts und der Gelenkteile (Entwurf 5: Gelenkkapseln, 44 kg).

Teil 1, Rahmenrohre: Biegespannung je Bauteil bei Spitzenlast und Dauerlast.
  Werkstoff EN AW-6082 T6, Streckgrenze 250 MPa. Ziel: Spitze mit Sicherheit >= 2 (<= 125 MPa), Dauerlast <= 50 MPa
  (Ermüdung, viele Lastwechsel). Rohre quadratisch, Biegung um die schwache Achse.
Teil 2, Gelenkteile des Antriebs (neu): Schubstange, Zapfen der Kurbeln und Hebel, Gelenkköpfe, Flanschschrauben,
  Sperrbolzen der Lendensperre, Wägezellen im Steigbügel. Das war in Entwurf 4 als „noch zu tun“ offen.
Teil 3, Teleskop: Passt das Innenrohr mit Spiel ins Außenrohr?

Lasten: Momentgrenzen der Firmware (firmware/src/exo_app.h), Exo-Oberteil 29,5 kg, Baum 3 m (41 kg), docs/kraftverteilung.md.
Aufruf: python3 tools/festigkeit.py  → Tabellen für docs/festigkeit.md. Rückgabewert 1, wenn etwas nicht reicht.
"""
import math
import sys

G = 9.81


def W_rect(b, h, t):
    """Widerstandsmoment eines Rechteckrohrs b (Breite) x h (Höhe in Biegerichtung) x t, in mm^3."""
    I = (b * h**3 - (b - 2 * t) * (h - 2 * t)**3) / 12
    return I / (h / 2)


# ---------------------------------------------------------------- Lasten Entwurf 5 ----
EXO_OBEN = 29.5          # kg, Exo-Oberteil (Bauteilliste)
KOERPER_ANTEIL = 13.5    # kg, 30 % des Oberkörpers (Stufe kräftig)
BAUM = 41.0              # kg, Baum 3 m, im schlimmsten Fall ganz auf einem Bein
F_BEIN_SPITZE = (EXO_OBEN + KOERPER_ANTEIL + BAUM) * G        # N, Stand auf einem Bein mit Baum
F_BEIN_DAUER = (EXO_OBEN + KOERPER_ANTEIL) * G                # N, Gehen ohne Baum
M_HUEFTE, M_KNIE, M_SCHULTER, M_ELLBOGEN, M_RUMPF = 110.0, 110.0, 100.0, 56.0, 80.0   # Nm, Grenzen der Firmware

# Bauteil, Belastung (Text), Biegemoment Spitze / Dauer [Nm], Profil
TEILE = [
    ("Rückenrahmen senkrecht (je)", "Baum 400 N × 0,4 m vor dem Lendengelenk + Oberteil, auf 2 Rohre; Schultern 2 × 100 Nm",
     130, 45, (30, 30, 2)),
    ("Hüftschwinge", f"Hüftmoment {M_HUEFTE:.0f} Nm + Last {F_BEIN_SPITZE:.0f} N × 0,2 m (Stand auf einem Bein mit Baum)",
     round(M_HUEFTE + F_BEIN_SPITZE * 0.2), round(15 + F_BEIN_DAUER * 0.2), (35, 35, 3)),
    ("Oberschenkelschiene außen", "Hüft- bzw. Kniemoment 110 Nm + Querkraft der Schubstange", 140, 47, (30, 30, 2)),
    ("Oberschenkelschiene innen (Teleskop)", "wie außen, Überlappung >= 8 cm", 140, 47, (25, 25, 2.5)),
    ("Unterschenkelschiene außen", "Kniemoment", 115, 31, (30, 30, 2)),
    ("Unterschenkelschiene innen", "Kniemoment, unten kleiner", 90, 25, (25, 25, 2)),
    ("Schulterbügel", "Schultermoment 100 Nm + Armlast 400 N × 0,1 m", 140, 45, (30, 30, 3)),
    ("Oberarmschiene", "Ellbogenmoment 56 Nm + Baum am Handgelenk", 110, 35, (25, 25, 3)),
    ("Unterarmschiene", "Schlaufenlast 400 N × 0,26 m", 105, 35, (25, 25, 3)),
    ("Querrohre Rücken", "Torsion/Biegung aus einseitigem Armzug", 50, 15, (20, 20, 2)),
]

# Teleskope: (Name, Außenrohr, Innenrohr). Spiel = Innenmaß außen − Außenmaß innen; 0,5–1,5 mm sind brauchbar.
TELESKOPE = [
    ("Oberschenkel", (30, 30, 2), (25, 25, 2.5)),
    ("Unterschenkel", (30, 30, 2), (25, 25, 2)),
]


def rahmen():
    print("| Bauteil | Belastung | Moment Spitze / Dauer | Profil | σ Spitze / Dauer | ok |")
    print("|---|---|---|---|---|---|")
    bad = 0
    for name, last, mp, md, prof in TEILE:
        w = W_rect(*prof)
        sp, sd = mp * 1e3 / w, md * 1e3 / w
        ok = sp <= 125 and sd <= 50
        bad += not ok
        print(f"| {name} | {last} | {mp} / {md} Nm | **{prof[0]:g} × {prof[1]:g} × {prof[2]:g}** | {sp:.0f} / {sd:.0f} MPa | {'ja' if ok else 'NEIN'} |")
    return bad


def teleskope():
    print("| Teleskop | Außenrohr | Innenrohr | Spiel | ok |")
    print("|---|---|---|---|---|")
    bad = 0
    for name, a, i in TELESKOPE:
        spiel = a[0] - 2 * a[2] - i[0]
        ok = 0.5 <= spiel <= 1.5
        bad += not ok
        print(f"| {name} | {a[0]:g} × {a[1]:g} × {a[2]:g} (innen {a[0] - 2 * a[2]:g}) | {i[0]:g} × {i[1]:g} × {i[2]:g} | {spiel:g} mm | {'ja' if ok else 'NEIN'} |")
    return bad


# ---------------------------------------------------------------- Gelenkteile ----
def stangenkraft(moment, hebel_m, winkel_grad):
    """Kraft in der Schubstange des Parallelogramms: F = M / (r · cos φ), φ = Auslenkung des Hebels aus der Querlage."""
    return moment / (hebel_m * math.cos(math.radians(winkel_grad)))


def zapfen(F, d_schaft, abstand_mm, streck):
    """Einschnittiger Zapfen (Gelenkkopf sitzt neben der Kurbel): Biegung am Einspannquerschnitt.
    Rückgabe (Spannung MPa, Sicherheit gegen Fließen)."""
    w = math.pi * d_schaft**3 / 32
    s = F * abstand_mm / w
    return s, streck / s


def gelenkteile():
    rows, offen = [], []

    def zeile(teil, last, wert, grenze, einheit, hinweis="", gegenbeispiel=False):
        sf = grenze / wert
        ok = sf >= 2.0
        knapp = 1.3 <= sf < 2.0
        if sf < 1.3 and not gegenbeispiel: offen.append(teil)
        rows.append(f"| {teil} | {last} | {wert:.0f} {einheit} | {grenze:.0f} {einheit} | {sf:.1f} | {'ja' if ok else ('knapp' if knapp else 'NEIN')} | {hinweis} |")

    # Knie: Hebel 60 mm, arbeitet bis ±55° aus der Querlage
    Fk = stangenkraft(M_KNIE, 0.060, 55)
    Fk_typ = stangenkraft(50, 0.060, 40)      # typisch: Hocke mit Last
    Fe = stangenkraft(M_ELLBOGEN, 0.050, 55)
    Fr = stangenkraft(M_RUMPF, 0.050, 25)
    # Schubstange Knie: Stahlrohr Ø 12 × 2, L 310 mm, beidseitig gelenkig (Euler)
    I = math.pi * (12**4 - 8**4) / 64
    zeile("Schubstange Knie Ø 12 × 2, L 310", f"Knicken bei {M_KNIE:.0f} Nm, Hebel 55° ausgelenkt", Fk, math.pi**2 * 210000 * I / 310**2, "N")
    I = math.pi * (10**4 - 7**4) / 64
    zeile("Schubstange Ellbogen Ø 10 × 1,5, L 280", f"Knicken bei {M_ELLBOGEN:.0f} Nm", Fe, math.pi**2 * 210000 * I / 280**2, "N")
    # Gelenkköpfe: statische Tragzahl laut Katalog (SKF SIKAC 8 M ≈ 12,9 kN, 6 M ≈ 8,15 kN; beim Kauf am Datenblatt prüfen)
    zeile("Gelenkkopf M8 (Knie, Rumpf)", "statische Tragzahl", max(Fk, Fr), 12900, "N", "Katalogwert prüfen")
    zeile("Gelenkkopf M6 (Ellbogen)", "statische Tragzahl", Fe, 8150, "N", "Katalogwert prüfen")
    # Zapfen an Kurbel und Hebel, einschnittig: Abstand Kurbelfläche → Kugelmitte = halbe Kugelbreite (M8: 6 mm, M6: 4,5 mm) + 1 mm Scheibe
    s, _ = zapfen(Fk, 6.47, 7.0, 640)
    zeile("Zapfen Knie als Schraube M8 8.8 (Gewinde im Biegequerschnitt)", "Biegung im Gewindegrund", s, 640, "MPa", "**so nicht bauen** (Gegenbeispiel): bricht auf Dauer im Gewinde", gegenbeispiel=True)
    s, _ = zapfen(Fk, 8.0, 7.0, 1080)
    zeile("Zapfen Knie als Passschraube Ø 8 (ISO 7379, 12.9)", "Biegung im glatten Schaft, Spitze", s, 1080, "MPa", "Bohrung 8 H7 reiben")
    s, _ = zapfen(Fk_typ, 8.0, 7.0, 1080)
    zeile("… dieselbe Passschraube, typische Last (50 Nm)", "schwellend, zulässig ≈ 400 MPa (Dauerfestigkeit Schaft 12.9)", s, 400, "MPa")
    s, _ = zapfen(Fr, 8.0, 7.0, 1080)
    zeile("Zapfen Rumpf beugen, Passschraube Ø 8 (12.9)", f"{M_RUMPF:.0f} Nm, Hebel 50 mm", s, 1080, "MPa")
    s, _ = zapfen(Fe, 6.0, 5.5, 1080)
    zeile("Zapfen Ellbogen als Passschraube Ø 6 (12.9)", "Biegung im glatten Schaft, Spitze", s, 1080, "MPa", "Bohrung 6 H7 reiben")
    # Lochleibung im Alu der Kurbel (10 mm bzw. 8 mm dick), zulässig ≈ 1,5 × Streckgrenze = 375 MPa
    zeile("Lochleibung Kurbel Knie (Alu 10 mm, Ø 8)", "Flächenpressung am Zapfen", Fk / (8 * 10), 375, "MPa")
    zeile("Lochleibung Kurbel Ellbogen (Alu 8 mm, Ø 6)", "Flächenpressung am Zapfen", Fe / (6 * 8), 375, "MPa")
    # Flansch der Kurbel am Motorabtrieb: 9 × M5 auf Ø 36 (RS04), Reibschluss: Vorspannkraft M5 8.8 ≈ 6,4 kN, µ = 0,15
    zeile("Kurbel am Abtrieb RS04: 9 × M5 auf Ø 36", "Umfangskraft je Schraube gegen Reibschluss (6,4 kN × 0,15)", M_KNIE / 0.018 / 9, 6400 * 0.15, "N",
          "knapp: 2 Stifte Ø 6 setzen (in der Zeichnung vorgesehen), Lochbild am Motor prüfen")
    zeile("Kurbel am Abtrieb RS03: 6 × M4 auf Ø 30,4", "Umfangskraft je Schraube gegen Reibschluss (4,0 kN × 0,15)", M_ELLBOGEN / 0.0152 / 6, 4000 * 0.15, "N",
          "Reibschluss reicht nicht sicher: 2 Stifte Ø 5 setzen, sonst tragen die Schrauben auf Abscheren")
    # Lendensperre: Bolzen Ø 8 auf Radius 70 mm, Rastsegment Edelstahl 4 mm; Baum 400 N × 0,4 m + 20 % Oberkörper vorgebeugt
    M_lende = 400 * 0.4 + 9 * G * 0.25
    F = M_lende / 0.070
    zeile("Sperrbolzen Lendensperre Ø 8", f"Abscheren bei {M_lende:.0f} Nm am Lendengelenk", F / (math.pi * 16), 0.6 * 500, "MPa", "Bolzen braucht eine eigene Führung; der Hubmagnet darf die Querkraft nicht tragen")
    zeile("Rastsegment Edelstahl 4 mm", "Lochleibung", F / (8 * 4), 1.5 * 210, "MPa")
    # Steigbügel: 4 Halbbrücken-Zellen je 50 kg zwischen Bügel und Sohle. Über sie läuft nur, was der MENSCH auf den Bügel stellt
    # (das Exo stützt sich über den Rahmen direkt auf den Bügel): 75 kg + bis ≈ 20 kg selbst getragene Last, beim Auftreten ≈ 1,5-fach
    stat = (75 + 20) * G
    zeile("Wägezellen im Steigbügel (4 × 50 kg), beim Auftreten", "Mensch + selbst getragener Lastanteil auf einem Bein, × 1,5", 1.5 * stat, 4 * 50 * G, "N",
          "reicht; Zellen so einbauen, dass ein Anschlag bei ≈ 250 kg trägt (Sprung, Stolpern)")

    print("| Teil | Nachweis | Beanspruchung | zulässig / Grenze | Sicherheit | ok | Hinweis |")
    print("|---|---|---|---|---|---|---|")
    print("\n".join(rows))
    print(f"\nStangenkraft: Knie {Fk:.0f} N (Spitze), {Fk_typ:.0f} N (typisch); Ellbogen {Fe:.0f} N; Rumpf beugen {Fr:.0f} N.")
    return offen


def main():
    print("## Rahmenrohre\n")
    bad = rahmen()
    print("\n## Teleskope\n")
    bad_t = teleskope()
    print("\n## Gelenkteile des Antriebs\n")
    offen = gelenkteile()
    print(f"\nRahmen: {'alle Teile ausreichend' if not bad else f'{bad} Teile zu schwach'}; "
          f"Teleskope: {'passen' if not bad_t else f'{bad_t} passen nicht'}; "
          f"Gelenkteile: {'alle mit Sicherheit >= 1,3' if not offen else 'offen: ' + '; '.join(offen)}")
    return 1 if (bad or bad_t) else 0


if __name__ == "__main__":
    sys.exit(main())
