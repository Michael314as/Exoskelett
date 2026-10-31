# Teileliste Fertigung (erzeugt mit tools/fertigung.py)

Alle Maße in mm. DXF: nur Schnittkonturen (Ebene SCHNITT), Maßstab 1:1. STL: Druckteile, Einheit mm.

**Vor der Bestellung prüfen:** Lochabstand der Flanschlager (Parameter `FLANSCH_LOCHABSTAND` = 67 mm) und Lochkreis der Motoren (`MOTOR_LOCHKREIS` = 70 mm) mit den Datenblättern der gekauften Teile.

## Laserteile

| Datei | Material | Dicke | Stück | Größe | Löcher | kleinster Steg | Zweck |
|---|---|---|---|---|---|---|---|
| `laser/eckplatte.dxf` | Alu 5754 | 4 mm | 24 | 60 × 60 | 4 | 14.2 mm | Doppelplatten an allen Rahmenecken (2 je Ecke) |
| `laser/lagerplatte.dxf` | Alu 5754 | 6 mm | 16 | 95 × 70 | 6 | 13.9 mm | Gelenkplatten Hüfte, Knie, Schulter, Ellbogen (je 2 Platten pro Gelenk) |
| `laser/abspreiz_lasche.dxf` | Alu 5754 | 6 mm | 8 | 90 × 45 | 5 | 12.7 mm | Scharnier Abspreizgelenk (4 je Seite: 2 Gabel, 2 Zunge) |
| `laser/seilscheibe_schulter_bord.dxf` | Alu 5754 | 4 mm | 4 | 140 × 140 | 11 | 4.9 mm | Seilscheibe schulter Ø140: Bordscheiben außen |
| `laser/seilscheibe_schulter_rille.dxf` | Alu 5754 | 3 mm | 4 | 132 × 132 | 12 | 4.9 mm | Seilscheibe schulter: Rillenlage(n), 3 mm für Seil/Zug bis 2,5 mm |
| `laser/seilscheibe_schulter_trenn.dxf` | Alu 5754 | 2 mm | 2 | 140 × 140 | 11 | 4.9 mm | Seilscheibe schulter: Trennlage zwischen den beiden Rillen |
| `laser/seilscheibe_ellbogen_bord.dxf` | Alu 5754 | 4 mm | 4 | 100 × 100 | 7 | 16.8 mm | Seilscheibe ellbogen Ø100: Bordscheiben außen |
| `laser/seilscheibe_ellbogen_rille.dxf` | Alu 5754 | 3 mm | 4 | 92 × 92 | 8 | 4.1 mm | Seilscheibe ellbogen: Rillenlage(n), 3 mm für Seil/Zug bis 2,5 mm |
| `laser/seilscheibe_ellbogen_trenn.dxf` | Alu 5754 | 2 mm | 2 | 100 × 100 | 7 | 16.8 mm | Seilscheibe ellbogen: Trennlage zwischen den beiden Rillen |
| `laser/seilscheibe_huefte_bord.dxf` | Alu 5754 | 4 mm | 4 | 190 × 190 | 11 | 7.4 mm | Seilscheibe huefte Ø190: Bordscheiben außen |
| `laser/seilscheibe_huefte_rille.dxf` | Alu 5754 | 3 mm | 2 | 182 × 182 | 12 | 7.4 mm | Seilscheibe huefte: Rillenlage(n), 3 mm für Seil/Zug bis 2,5 mm |
| `laser/knie_kurvenscheibe_bord.dxf` | Alu 5754 | 4 mm | 4 | 135 × 129 | 5 | 4.6 mm | Knie-Kurvenscheibe: Bordscheiben (2 je Knie) |
| `laser/knie_kurvenscheibe_rille.dxf` | Alu 5754 | 3 mm | 2 | 127 × 121 | 6 | 4.6 mm | Knie-Kurvenscheibe: Rillenlage |
| `laser/steigbuegel_sohle.dxf` | Edelstahl 1.4301 | 4 mm | 2 | 290 × 110 | 18 | 5.1 mm | Sohlenrahmen, Spikes M5 einschrauben (Gewinde schneiden) |
| `laser/steigbuegel_seitenlasche.dxf` | Edelstahl 1.4301 | 4 mm | 2 | 50 × 130 | 4 | 11.5 mm | Seitenbügel Sprunggelenk → Sohle (Langloch zur Höhenanpassung) |
| `laser/schlaufenkopf_lasche.dxf` | Alu 5754 | 4 mm | 3 | 110 × 40 | 5 | 9.4 mm | Halter Schlaufenkopf (rechts 2, links 1); 6-mm-Loch für Seildurchführung |
| `laser/motorplatte.dxf` | Alu 5754 | 4 mm | 1 | 290 × 360 | 40 | 13.1 mm | Montageplatte Antriebsbox (Lochkreis Motor vor Bestellung prüfen) |
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

Druckeinstellungen: PETG 0,2 mm Schicht, 4 Wände, 30 % Gyroid, Manschetten liegend auf der Schnittkante; TPU 95A langsam (25 mm/s).
