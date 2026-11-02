# Teileliste Fertigung (erzeugt mit tools/fertigung.py)

Alle Maße in mm. DXF: nur Schnittkonturen (Ebene SCHNITT), Maßstab 1:1. STL: Druckteile, Einheit mm.

**Vor der Bestellung prüfen:** Lochabstand der Flanschlager (Parameter `FLANSCH_LOCHABSTAND` = 67 mm) und Lochkreis der Motoren (`MOTOR_LOCHKREIS` = 70 mm) mit den Datenblättern der gekauften Teile.

## Laserteile

| Datei | Material | Dicke | Stück | Größe | Löcher | kleinster Steg | Zweck |
|---|---|---|---|---|---|---|---|
| `laser/eckplatte.dxf` | Alu 5754 | 4 mm | 24 | 60 × 60 | 4 | 14.2 mm | Doppelplatten an allen Rahmenecken (2 je Ecke) |
| `laser/lagerplatte.dxf` | Alu 6082 T6 | 6 mm | 16 | 95 × 70 | 6 | 13.9 mm | Gelenkplatten Knie, Knöchel, Schulter, Ellbogen (je 2 Platten pro Gelenk) |
| `laser/abspreizgabel_wange.dxf` | Alu 6082 T6 | 8 mm | 4 | 60 × 100 | 4 | 14.7 mm | Abspreizgabel HU-01: 2 Wangen je Seite; Ø 16,2 für Passschraube/Kugelsperrbolzen, M6 zum Rahmen, M5 Federhalter |
| `laser/abspreizgabel_steg.dxf` | Alu 6082 T6 | 6 mm | 2 | 82 × 60 | 5 | 9.2 mm | Steg zwischen den Gabelwangen (oben), Langloch für den Zentrierfeder-Halter |
| `laser/zentrierfeder_halter.dxf` | Edelstahl 1.4301 | 3 mm | 4 | 28 × 30 | 2 | 7.6 mm | Halter Zentrierfeder HU-03 (2 je Seite), Ø 6 Zapfen für die Feder |
| `laser/hueftmotor_platte.dxf` | Alu 6082 T6 | 8 mm | 2 | 130 × 110 | 9 | 9.2 mm | Motorplatte an der Hüftschwinge HU-05; Langlöcher ±5 mm spannen das Seil Trommel → Scheibe Ø 120 |
| `laser/seilscheibe_schulter_bord.dxf` | Alu 5754 | 4 mm | 4 | 140 × 140 | 11 | 4.9 mm | Seilscheibe schulter Ø140: Bordscheiben außen |
| `laser/seilscheibe_schulter_rille.dxf` | Alu 5754 | 3 mm | 4 | 132 × 132 | 12 | 4.9 mm | Seilscheibe schulter: Rillenlagen, 3 mm für Seil Ø 3 |
| `laser/seilscheibe_schulter_trenn.dxf` | Alu 5754 | 2 mm | 2 | 140 × 140 | 11 | 4.9 mm | Seilscheibe schulter: Trennlage zwischen den beiden Rillen |
| `laser/seilscheibe_ellbogen_bord.dxf` | Alu 5754 | 4 mm | 4 | 100 × 100 | 7 | 16.8 mm | Seilscheibe ellbogen Ø100: Bordscheiben außen |
| `laser/seilscheibe_ellbogen_rille.dxf` | Alu 5754 | 3 mm | 4 | 92 × 92 | 8 | 7.4 mm | Seilscheibe ellbogen: Rillenlagen, 3 mm für Seil Ø 3 |
| `laser/seilscheibe_ellbogen_trenn.dxf` | Alu 5754 | 2 mm | 2 | 100 × 100 | 7 | 16.8 mm | Seilscheibe ellbogen: Trennlage zwischen den beiden Rillen |
| `laser/seilscheibe_huefte_bord.dxf` | Alu 5754 | 4 mm | 4 | 120 × 120 | 11 | 7.3 mm | Seilscheibe huefte Ø120: Bordscheiben außen |
| `laser/seilscheibe_huefte_rille.dxf` | Alu 5754 | 3 mm | 4 | 112 × 112 | 12 | 7.3 mm | Seilscheibe huefte: Rillenlagen, 3 mm für Seil Ø 3 |
| `laser/seilscheibe_huefte_trenn.dxf` | Alu 5754 | 2 mm | 2 | 120 × 120 | 11 | 7.3 mm | Seilscheibe huefte: Trennlage zwischen den beiden Rillen |
| `laser/knie_beugescheibe_bord.dxf` | Alu 5754 | 4 mm | 4 | 90 × 90 | 7 | 14.8 mm | Beugescheibe Knie Ø 90 (Beugezug mit Federpatrone): Bordscheiben |
| `laser/knie_beugescheibe_rille.dxf` | Alu 5754 | 3 mm | 2 | 82 × 82 | 8 | 7.4 mm | Beugescheibe Knie: Rillenlage |
| `laser/knie_kurvenscheibe_bord.dxf` | Alu 5754 | 4 mm | 4 | 130 × 116 | 5 | 7.8 mm | Knie-Kurvenscheibe Streckzug: Bordscheiben (2 je Knie) |
| `laser/knie_kurvenscheibe_rille.dxf` | Alu 5754 | 3 mm | 2 | 122 × 109 | 6 | 6.4 mm | Knie-Kurvenscheibe: Rillenlage, Seilende bei 0° |
| `laser/steigbuegel.dxf` | Edelstahl 1.4301 | 4 mm | 2 | 60 × 205 | 11 | 14.7 mm | Steigbügel BE-11: Abwicklung 60 × 205, Laschen an den Linien ±67,5 mm 90° hochkanten (Kantdienstleister) |
| `laser/drehteller_scheibe.dxf` | Edelstahl 1.4301 | 2 mm | 4 | 64 × 64 | 5 | 7.8 mm | Drehteller BE-14: Laufscheiben über und unter dem Axial-Nadellager AXK (2 je Fuß), M4 für die Wägezellen-Platte |
| `laser/fersenbuegel_lasche.dxf` | Alu 6082 T6 | 8 mm | 2 | 40 × 50 | 3 | 7.9 mm | Fersenbügel BE-09 am Knöchellager (Rundstab Ø 16 wird eingeschweißt oder geklemmt) |
| `laser/kippgelenk_gabel.dxf` | Alu 6082 T6 | 6 mm | 4 | 30 × 40 | 2 | 11.5 mm | Kippgelenk BE-10 hinter der Ferse: 2 Wangen je Fuß, Bolzen Ø 8 mit Gummibuchse |
| `laser/lendengelenk_lasche.dxf` | Alu 6082 T6 | 8 mm | 8 | 70 × 50 | 3 | 11.5 mm | Scharnier Lendengelenk: 2 Laschen oben + 2 unten je Rahmenrohr; Bolzen Ø 10, Elastomerfeder dazwischen |
| `laser/lendensperre_rastsegment.dxf` | Edelstahl 1.4301 | 4 mm | 1 | 103 × 98 | 8 | 6.7 mm | Sperre RA-09: Hubmagnet-Bolzen rastet in 5 Stellungen (−25 … +25°), sperrt bei Last |
| `laser/gasfeder_halter.dxf` | Alu 6082 T6 | 5 mm | 4 | 44 × 34 | 3 | 7.9 mm | Halter Gasfeder RA-10 (oben und unten), M6 für den Kugelzapfen |
| `laser/schultergabel_wange.dxf` | Alu 6082 T6 | 10 mm | 4 | 55 × 80 | 3 | 11.5 mm | Schultergabel SC-02: 2 Wangen je Seite, Hohlachse Ø 22 (Seile laufen innen durch) |
| `laser/schulterschlitten.dxf` | Alu 6082 T6 | 8 mm | 2 | 70 × 55 | 4 | 14.6 mm | Schlitten SC-12: 2 Linearlager LM8UU (Ø 15) auf Wellen Ø 8, ±30 mm Hub |
| `laser/schlaufentraeger_rechts.dxf` | Alu 6082 T6 | 6 mm | 1 | 200 × 30 | 9 | 9.2 mm | Schlaufenträger rechts (2 Köpfe: unten + außen), kalt biegen nach Schablone |
| `laser/schlaufentraeger_links.dxf` | Alu 6082 T6 | 6 mm | 1 | 200 × 30 | 7 | 9.2 mm | Schlaufenträger links (1 Kopf unten) |
| `laser/motorplatte.dxf` | Alu 5754 | 4 mm | 1 | 290 × 350 | 40 | 7.9 mm | Montageplatte Antriebsbox (Lochkreis Motor vor Bestellung prüfen) |
| `laser/parkstuetze_lasche.dxf` | Alu 5754 | 4 mm | 4 | 70 × 56 | 4 | 11.6 mm | Klappgelenk Parkstütze (Rastbolzen Ø6 im oberen Loch) |

## Druckteile (Bambu Lab A1 mini, 180 × 180 × 180 mm)

| Datei | Material | Stück | Größe | Volumen cm³ | passt | Genus | Zweck |
|---|---|---|---|---|---|---|---|
| `druck/manschette_oberschenkel.stl` | PETG | 2 | 90 × 172 × 70 | 95 | ja | 5 | Manschette r = 72 mm, Höhe 70 mm; Polster (Abstandsgewirke) innen einkleben |
| `druck/manschette_unterschenkel.stl` | PETG | 2 | 70 × 138 × 60 | 65 | ja | 5 | Manschette r = 55 mm, Höhe 60 mm; Polster (Abstandsgewirke) innen einkleben |
| `druck/manschette_oberarm.stl` | PETG | 2 | 65 × 128 × 60 | 59 | ja | 5 | Manschette r = 50 mm, Höhe 60 mm; Polster (Abstandsgewirke) innen einkleben |
| `druck/manschette_unterarm.stl` | PETG | 2 | 55 × 112 × 50 | 43 | ja | 5 | Manschette r = 42 mm, Höhe 50 mm; Polster (Abstandsgewirke) innen einkleben |
| `druck/bohrschablone_rohr25.stl` | PETG | 1 | 150 × 34 × 22 | 40 | ja | 7 | Lochraster der Außenrohre 25 × 25 (Steckbolzen Ø6) |
| `druck/bohrschablone_rohr20.stl` | PETG | 1 | 150 × 29 × 18 | 33 | ja | 7 | Lochraster der Innenrohre 20 × 20 |
| `druck/waegezellen_halter.stl` | PETG | 8 | 30 × 34 × 16 | 10 | ja | 2 | Halter je Wägezelle (an der Schiene) |
| `druck/ueberlastpuffer.stl` | TPU 95A | 16 | 10 × 10 × 6 | 0 | ja | 1 | Überlastpuffer (2 je Wägezelle), 1 mm Spalt einstellen |
| `druck/tastergriff_rechts.stl` | PETG | 1 | 60 × 38 × 34 | 46 | ja | 2 | Daumentaster rechts (2 Schlaufen) mit Heizfolie innen |
| `druck/tastergriff_links.stl` | PETG | 1 | 60 × 38 × 34 | 47 | ja | 1 | Daumentaster links (1 Schlaufe) |
| `druck/haube_links.stl` | PETG | 1 | 155 × 150 × 50 | 113 | ja | 6 | Haube Antriebsbox, linke Hälfte |
| `druck/haube_rechts.stl` | PETG | 1 | 155 × 150 × 50 | 112 | ja | 9 | Haube Antriebsbox, rechte Hälfte |
| `druck/fingerschutz_huefte.stl` | PETG transparent | 2 | 80 × 162 × 24 | 22 | ja | 2 | Fingerschutz RA-11 über Scheibe Ø 120: nichts kann zwischen Seil und Scheibe geraten |
| `druck/fingerschutz_knie.stl` | PETG transparent | 2 | 76 × 190 × 22 | 23 | diagonal | 2 | Fingerschutz RA-11 über Scheibe Ø 160: nichts kann zwischen Seil und Scheibe geraten |
| `druck/fingerschutz_schulter.stl` | PETG transparent | 2 | 91 × 182 × 24 | 26 | diagonal | 2 | Fingerschutz RA-11 über Scheibe Ø 140: nichts kann zwischen Seil und Scheibe geraten |
| `druck/fingerschutz_ellbogen.stl` | PETG transparent | 2 | 69 × 142 × 22 | 17 | ja | 2 | Fingerschutz RA-11 über Scheibe Ø 100: nichts kann zwischen Seil und Scheibe geraten |
| `druck/akkuschacht_wand.stl` | PETG-CF | 4 | 116 × 176 × 10 | 87 | ja | 7 | Akkuschacht EN-02: Innen- und Außenwand je Seite (zweiter Schacht nur bei Bedarf bestücken) |
| `druck/akkuschacht_stirn.stl` | PETG-CF | 2 | 76 × 176 × 6 | 74 | ja | 7 | Stirnwand Akkuschacht mit Fenster für den Kontaktträger |
| `druck/fersenschale.stl` | PETG | 2 | 50 × 118 × 50 | 31 | ja | 4 | Fersenschale BE-13 am Steigbügel, Schlitze für den Ratschenriemen |

Druckeinstellungen: PETG 0,2 mm Schicht, 4 Wände, 30 % Gyroid, Manschetten liegend auf der Schnittkante; TPU 95A langsam (25 mm/s).
