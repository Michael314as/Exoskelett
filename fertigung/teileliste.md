# Teileliste Fertigung (erzeugt mit tools/fertigung.py)

Alle Maße in mm. DXF: nur Schnittkonturen (Ebene SCHNITT), Maßstab 1:1. STL: Druckteile, Einheit mm.

**Vor der Bestellung prüfen:** Lochabstände der Flanschlager (Tabelle `KFL`: KFL08 37 mm, KFL001 48 mm, KFL002 53 mm) und Lochbilder der Schultermotoren (RobStride RS04: 10 × M4 auf Ø 106, Abtrieb 9 × M5 auf Ø 36) mit den Datenblättern der gekauften Teile. Gasfedern (Entwurf 6): Gabelköpfe M10 (Schulter, SUSPA HY4) bzw. M8 (Ellbogen, Knie, SUSPA HY1) und Einbaulänge mit dem Angebot von SUSPA/Bansbach abgleichen, bevor die Federhebel und Federböcke gelasert werden.

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
| `laser/motortraeger_wange.dxf` | Alu 6082 T6 | 8 mm | 2 | 40 × 120 | 5 | 13.4 mm | Wange zwischen Schultergabel und Motorträger (1 je Schulterkapsel); Langloch zum Ausrichten |
| `laser/abtriebsflansch_rs04.dxf` | Alu 6082 T6 | 8 mm | 2 | 90 × 90 | 14 | 6.2 mm | Flansch Abtrieb RS04 → Oberarmschiene (Schulter); 9 × M5 auf Ø 36 |
| `laser/federhebel_schulter.dxf` | Alu 6082 T6 | 10 mm | 2 | 90 × 118 | 6 | 6.9 mm | LK-06 Federhebel Schulter: auf dem Abtriebsflansch (4 × M6, gleiche Löcher), Auge bei 60 mm: Passschraube Ø 10 (12.9) durch den Gabelkopf M10 der Gasfeder, Loch auf 10 H7 reiben |
| `laser/federhebel_ellbogen.dxf` | Alu 6082 T6 | 10 mm | 2 | 68 × 48 | 5 | 6.7 mm | Federhebel Ellbogen: auf dem Ellbogenbolzen, 3 × M5 in die Unterarmplatte (80 Nm: 1,8 kN je Schraube), Auge bei 32 mm: Passschraube Ø 8 (12.9) durch den Gabelkopf M8, Loch auf 8 H7 reiben |
| `laser/federhebel_knie.dxf` | Alu 6082 T6 | 10 mm | 2 | 76 × 48 | 5 | 5.7 mm | Federhebel Knie: auf dem Kniebolzen, 3 × M5 in die Unterschenkelplatte (100 Nm: 2,1 kN je Schraube), Auge bei 40 mm: Passschraube Ø 8 (12.9), Loch auf 8 H7 reiben |
| `laser/federbock_m10.dxf` | Alu 6082 T6 | 8 mm | 4 | 50 × 60 | 3 | 7.5 mm | LK-04 Federbock Schulter: 2 Wangen je Seite an der Schultergabel (Fußende der Gasfeder HY4), Passschraube Ø 10; 1,5 kN je Wange |
| `laser/federbock_m8.dxf` | Alu 6082 T6 | 6 mm | 8 | 40 × 50 | 3 | 7.5 mm | LK-04 Federbock Ellbogen (Oberarmschiene) und Knie (Oberschenkelschiene): 2 Wangen je Federende, Passschraube Ø 8; 1,25 kN je Wange |
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
| `laser/rumpfdrehung_wagen.dxf` | Alu 6082 T6 | 8 mm | 1 | 80 × 48 | 6 | 9.4 mm | Wagen der Rumpfdrehung RA-12 am oberen Rahmen: 4 Profilrollen auf der Bogenwelle (R 95 mm um die Wirbelsäule) |
| `laser/rumpfdrehung_halter.dxf` | Alu 6082 T6 | 8 mm | 2 | 36 × 44 | 3 | 7.5 mm | Halter der Bogenwelle Ø 10 am unteren Rahmen (2 Stück, an den Enden), Gummianschlag ±15° |
| `laser/parkstuetze_lasche.dxf` | Alu 5754 | 4 mm | 4 | 70 × 56 | 4 | 11.6 mm | Klappgelenk Parkstütze (Rastbolzen Ø6 im oberen Loch) |
| `laser/manschette_schlitten.dxf` | Alu 6082 T6 | 4 mm | 4 | 40 × 34 | 4 | 9.7 mm | Längsschlitten der Armmanschetten M2: Gleitbuchsen einpressen (Bohrung auf 6 H7 reiben), M3 zum Boss der Schale |
| `laser/manschette_stangenhalter.dxf` | Alu 6082 T6 | 5 mm | 8 | 36 × 14 | 3 | 5.6 mm | Stangenhalter des Längsschlittens, 2 je Armmanschette: Stangen Ø 5 eingeklebt, M4 am Wägezellen-Halter; Federn zwischen Halter und Schlitten |

## Druckteile (Bambu Lab A1 mini, 180 × 180 × 180 mm)

| Datei | Material | Stück | Größe | Volumen cm³ | passt | Genus | Zweck |
|---|---|---|---|---|---|---|---|
| `druck/manschette_oberschenkel.stl` | PETG-CF | 2 | 96 × 170 × 102 | 116 | ja | 4 | Beinmanschette r = 72 mm, Breite 102 mm (Entwurf 6: ohne Messung): Viskopolster 4 mm mit Wechselbezug, Klettgurt 40 mm mit Leiterschnalle (SE-15), Boss direkt an die Schiene |
| `druck/manschette_schienbein.stl` | PETG-CF | 2 | 74 × 136 × 87 | 78 | ja | 5 | Beinmanschette r = 55 mm, Breite 87 mm (Entwurf 6: ohne Messung): Viskopolster 4 mm mit Wechselbezug, Klettgurt 40 mm mit Leiterschnalle (SE-15), Boss direkt an die Schiene |
| `druck/manschette_oberarm.stl` | PETG-CF | 2 | 68 × 126 × 87 | 73 | ja | 6 | Manschette M2 r = 50 mm, Breite 87 mm; Viskopolster 4 mm mit Wechselbezug innen (Klett am Bezug), BOA-Drehrad + 2 Seilbänder, Schlitten an den Boss |
| `druck/manschette_unterarm.stl` | PETG-CF | 2 | 58 × 110 × 72 | 54 | ja | 6 | Manschette M2 r = 42 mm, Breite 72 mm; Viskopolster 4 mm mit Wechselbezug innen (Klett am Bezug), BOA-Drehrad + 2 Seilbänder, Schlitten an den Boss |
| `druck/bohrschablone_rohr30.stl` | PETG | 1 | 150 × 39 × 25 | 47 | ja | 7 | Lochraster der Außenrohre 30 × 30 an Ober- und Unterschenkel (Steckbolzen Ø6) |
| `druck/bohrschablone_rohr25.stl` | PETG | 1 | 150 × 34 × 22 | 40 | ja | 7 | Lochraster der Innenrohre 25 × 25 (Teleskop) und der Armschienen 25 × 25 |
| `druck/waegezellen_halter.stl` | PETG | 4 | 30 × 34 × 16 | 10 | ja | 2 | Halter je Wägezelle der Armmanschetten (an der Schiene) |
| `druck/ueberlastpuffer.stl` | TPU 95A | 8 | 10 × 10 × 6 | 0 | ja | 1 | Überlastpuffer (2 je Wägezelle), 1 mm Spalt einstellen |
| `druck/tastergriff_rechts.stl` | PETG | 1 | 60 × 38 × 34 | 44 | ja | 5 | Griff rechts: 2 Daumentaster (Schlaufen), Sperrhebel des Arms mit Bowdenzug und Mikroschalter, Heizfolie innen |
| `druck/tastergriff_links.stl` | PETG | 1 | 60 × 38 × 34 | 45 | ja | 4 | Griff links: 1 Daumentaster (Schlaufe), Sperrhebel des Arms mit Bowdenzug und Mikroschalter |
| `druck/haube_links.stl` | PETG | 1 | 110 × 140 × 60 | 90 | ja | 6 | Deckel der flachen Elektronikbox (60 × 140 × 220), linke Hälfte |
| `druck/haube_rechts.stl` | PETG | 1 | 110 × 140 × 60 | 90 | ja | 9 | Deckel der flachen Elektronikbox, rechte Hälfte |
| `druck/kapselmantel_rs04.stl` | PETG-CF | 2 | 131 × 131 × 56 | 51 | ja | 3 | Kapselmantel AN-02 für Motor Ø 120: O-Ring-Nuten beidseitig, M16 Kabel, M12 Druckausgleich nach unten |
| `druck/bohrschablone_kuehlkoerper_rs04.stl` | PETG | 1 | 146 × 146 × 14 | 107 | ja | 13 | Bohrschablone für den Stiftkühlkörper (Lochkreis Ø 106, Motorgehäuse); Pad zwischen Motor und Kühlkörper |
| `druck/akkuschacht_wand.stl` | PETG-CF | 2 | 116 × 176 × 10 | 87 | ja | 7 | Akkuschacht EN-02 stehend oben am Rücken: Rück- und Außenwand, Einstecken von oben (Entwurf 6: ein Schacht, der Ersatzakku bleibt im Fahrzeug) |
| `druck/akkuschacht_stirn.stl` | PETG-CF | 1 | 76 × 176 × 6 | 74 | ja | 7 | Boden des Akkuschachts mit Fenster für den Kontaktträger (Kontakte unten, Wasser läuft ab) |
| `druck/fersenschale.stl` | PETG | 2 | 50 × 118 × 50 | 31 | ja | 4 | Fersenschale BE-13 am Steigbügel, Schlitze für den Ratschenriemen |

Druckeinstellungen: PETG 0,2 mm Schicht, 4 Wände, 30 % Gyroid, Manschetten liegend auf der Schnittkante; TPU 95A langsam (25 mm/s).
