# Teileliste Fertigung (erzeugt mit tools/fertigung.py)

Alle Maße in mm. DXF: nur Schnittkonturen (Ebene SCHNITT), Maßstab 1:1. STL: Druckteile, Einheit mm.

**Vor der Bestellung prüfen:** Lochabstände der Flanschlager (Tabelle `KFL`: KFL08 37 mm, KFL001 48 mm, KFL002 53 mm) und Lochbilder der Motoren (RobStride RS04: 10 × M4 auf Ø 106, Abtrieb 9 × M5 auf Ø 36; RS03: 8 × M4 auf Ø 98, Abtrieb 6 × M4 auf Ø 30,36) mit den Datenblättern der gekauften Teile.

## Laserteile

| Datei | Material | Dicke | Stück | Größe | Löcher | kleinster Steg | Zweck |
|---|---|---|---|---|---|---|---|
| `laser/eckplatte.dxf` | Alu 5754 | 4 mm | 24 | 60 × 60 | 4 | 14.2 mm | Doppelplatten an allen Rahmenecken (2 je Ecke) |
| `laser/lagerplatte_kfl001.dxf` | Alu 6082 T6 | 6 mm | 8 | 80 × 64 | 6 | 6.9 mm | Gelenkplatten Knie und Schulter für KFL001 (Lochabstand 48 mm), 2 je Gelenk |
| `laser/lagerplatte_kfl08.dxf` | Alu 6082 T6 | 6 mm | 8 | 66 × 56 | 6 | 6.9 mm | Gelenkplatten Ellbogen und Knöchel für KFL08 (Lochabstand 37 mm), 2 je Gelenk |
| `laser/lagerplatte_kfl002.dxf` | Alu 6082 T6 | 8 mm | 2 | 86 × 66 | 6 | 6.9 mm | Hüftlager KFL002 an der Hüftschwinge (Lochabstand 53 mm) |
| `laser/abspreizgabel_wange.dxf` | Alu 6082 T6 | 8 mm | 4 | 60 × 100 | 4 | 14.7 mm | Abspreizgabel HU-01: 2 Wangen je Seite; Ø 16,2 für Passschraube/Kugelsperrbolzen, M6 zum Rahmen, M5 Federhalter |
| `laser/abspreizgabel_steg.dxf` | Alu 6082 T6 | 6 mm | 2 | 82 × 60 | 5 | 9.2 mm | Steg zwischen den Gabelwangen (oben), Langloch für den Zentrierfeder-Halter |
| `laser/zentrierfeder_halter.dxf` | Edelstahl 1.4301 | 3 mm | 4 | 28 × 30 | 2 | 7.6 mm | Halter Zentrierfeder HU-03 (2 je Seite), Ø 6 Zapfen für die Feder |
| `laser/motortraeger_rs04.dxf` | Alu 6082 T6 | 6 mm | 6 | 128 × 128 | 19 | 5.7 mm | Motorträger AN-04 für RS04 (Hüfte, Knie, Schulter): 10 × M4 auf Ø 106 |
| `laser/motortraeger_rs03.dxf` | Alu 6082 T6 | 5 mm | 2 | 116 × 116 | 17 | 5.7 mm | Motorträger AN-04 für RS03 (Ellbogen): 8 × M4 auf Ø 98 |
| `laser/motortraeger_wange.dxf` | Alu 6082 T6 | 8 mm | 12 | 40 × 120 | 5 | 13.4 mm | Wangen zwischen Schiene bzw. Schwinge und Motorträger (2 je Kapsel an Knie und Ellbogen, 1 je Kapsel an Hüfte und Schulter); Langloch zum Ausrichten |
| `laser/abtriebsflansch_rs04.dxf` | Alu 6082 T6 | 8 mm | 4 | 90 × 90 | 14 | 6.2 mm | Flansch Abtrieb RS04 → Oberschenkelplatte (Hüfte) bzw. Oberarmschiene (Schulter); 9 × M5 auf Ø 36 |
| `laser/kurbel_knie.dxf` | Alu 6082 T6 | 10 mm | 2 | 99 × 54 | 11 | 6.2 mm | Kurbel BA-01: auf dem Abtrieb des Kniemotors (9 × M5), Zapfen M8 bei 60 mm |
| `laser/kurbel_ellbogen.dxf` | Alu 6082 T6 | 8 mm | 2 | 83 × 46 | 8 | 5.6 mm | Kurbel AR-04: auf dem Abtrieb des Ellbogenmotors (6 × M4), Zapfen M6 bei 50 mm |
| `laser/kniehebel.dxf` | Alu 6082 T6 | 10 mm | 2 | 96 × 48 | 5 | 5.7 mm | Kniehebel BA-04: auf dem Kniebolzen, 3 × M5 in die Unterschenkelplatte, Zapfen M8 bei 60 mm |
| `laser/ellbogenhebel.dxf` | Alu 6082 T6 | 8 mm | 2 | 80 × 40 | 5 | 4.7 mm | Ellbogenhebel AR-08: auf dem Ellbogenbolzen, 3 × M4, Zapfen M6 bei 50 mm |
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
| `druck/haube_links.stl` | PETG | 1 | 155 × 150 × 50 | 113 | ja | 6 | Haube Elektronikbox, linke Hälfte |
| `druck/haube_rechts.stl` | PETG | 1 | 155 × 150 × 50 | 112 | ja | 9 | Haube Elektronikbox, rechte Hälfte |
| `druck/kurbelschutz_knie.stl` | PETG transparent | 2 | 101 × 198 × 18 | 27 | diagonal | 2 | Fingerschutz RA-11 über Scheibe Ø 156: nichts kann zwischen Seil und Scheibe geraten |
| `druck/kurbelschutz_ellbogen.stl` | PETG transparent | 2 | 87 × 174 × 16 | 21 | ja | 2 | Fingerschutz RA-11 über Scheibe Ø 132: nichts kann zwischen Seil und Scheibe geraten |
| `druck/kapselhaube_rs04.stl` | PETG-CF | 6 | 136 × 136 × 64 | 81 | ja | 2 | Kapselhaube AN-02 für Motor Ø 120: O-Ring-Nut, M16 Kabel, M12 Druckausgleich; innen 5 mm Dämmung und Heizfolie einkleben |
| `druck/kapselhaube_rs03.stl` | PETG-CF | 2 | 114 × 114 × 66 | 66 | ja | 2 | Kapselhaube AN-02 für Motor Ø 98: O-Ring-Nut, M16 Kabel, M12 Druckausgleich; innen 5 mm Dämmung und Heizfolie einkleben |
| `druck/akkuschacht_wand.stl` | PETG-CF | 4 | 116 × 176 × 10 | 87 | ja | 7 | Akkuschacht EN-02: Innen- und Außenwand je Seite (zweiter Schacht nur bei Bedarf bestücken) |
| `druck/akkuschacht_stirn.stl` | PETG-CF | 2 | 76 × 176 × 6 | 74 | ja | 7 | Stirnwand Akkuschacht mit Fenster für den Kontaktträger |
| `druck/fersenschale.stl` | PETG | 2 | 50 × 118 × 50 | 31 | ja | 4 | Fersenschale BE-13 am Steigbügel, Schlitze für den Ratschenriemen |

Druckeinstellungen: PETG 0,2 mm Schicht, 4 Wände, 30 % Gyroid, Manschetten liegend auf der Schnittkante; TPU 95A langsam (25 mm/s).
