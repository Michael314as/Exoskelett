# Bestellliste Prototyp (Entwurf 5: Gelenkkapseln)

Erzeugt aus der Bauteilliste der technischen Ansicht ([exo-technik.html](../visualisierung/exo-technik.html), Reiter „Stückliste“). Stückzahlen für ein komplettes Exo mit einem Akku (zweiter Akku und Parkstütze nicht mitgezählt). Teile mit mehreren Herstellwegen stehen beim ersten Weg.

**Vor der Bestellung:** Lochabstände der Flanschlager und Lochbilder der Motoren (RS04: 10 × M4 auf Ø 106, Abtrieb 9 × M5 auf Ø 36; RS03: 8 × M4 auf Ø 98, Abtrieb 6 × M4 auf Ø 30,36) am gelieferten Teil prüfen und `python3 tools/fertigung.py` laufen lassen (Prüfpunkt F3).

## Empfohlene Reihenfolge (Kosten verteilen, früh messen)

| Stufe | Bestellen | Damit prüfst du |
|---|---|---|
| 1 Prüfstand | 1 × RobStride RS04 + CAN-USB-Adapter, Kapselteile (Stiftkühlkörper, Wärmeleitpad, Mantel, V-Ring, O-Ringe, Druckausgleich, Heizband), Kurbel, Schubstange mit Gelenkköpfen, Kniehebel, Teensy 4.1 + CAN-Transceiver, Manschetten-Messplatine (ADS131M04) + 1 Wägezelle, 1 Manschette M2 (Schale, BOA, Schlitten), Not-Aus, Bremswiderstand | Kapsel dicht und eisfrei (P35, P36), Mitgehen einstellen (P39), Wärme (P10–P17, P33) |
| 2 Schlaufe | 1 Bandantrieb 5840-31ZY + Treiber, Lochband, Schlaufenkopf, Wägezelle 50 kg | Haltekraft mit 20-kg-Sandsack (P20) |
| 3 Rahmen passiv | Halbzeug, Laserteile Rahmen/Hüfte/Beine/Fuß, Lager, Gurte | Passform, Achsen, Lendengelenk (P1–P4, P31, P34) |
| 4 Beine aktiv | 3 weitere RS04 (Hüften, zweites Knie), Fußkontakt-Wägezellen | Aufstehen, Gehen (P20b, P21, P23) |
| 5 Arme aktiv | 2 RS04 (Schultern) + 2 RS03 (Ellbogen), restliche Manschetten, Schlaufen | Bäume (P22, P27, P28) |

## Stufe 1 und 2 konkret (Preisstand November 2026)

Recherchierte Preise mit Quelle, sonst Erfahrungswerte (≈). Verbindlich ist der Shop beim Kauf.

| Teil | Stück | Preis | Quelle |
|---|---|---|---|
| RobStride RS04 (120 Nm Spitze, CAN) | 1 | ≈ 300–400 € inkl. Versand und Einfuhr | [RS04-Handbuch](https://github.com/RobStride/Product_Information), [Händler](https://www.roboticscenter.ai/store/product/robstride-04) |
| CAN-USB-Adapter (zum Setzen der Motor-ID) | 1 | ≈ 15 € | Elektronik-Händler |
| PJRC Teensy 4.1 + CAN-Transceiver SN65HVD230 | 1 | ≈ 47 € | [Geizhals](https://geizhals.at/teensy-4-1-a2738866.html) |
| Kapselteile: Stiftkühlkörper Fischer ICK S R 140 × 50, Wärmeleitpad, Wangen (Laser), Mantel (Druck), V-Ring, O-Ringe, Druckausgleich M12, Heizband 4 W, Kabelverschraubung | 1 Satz | ≈ 75 € | [Kühlkörper (DigiKey)](https://www.digikey.de/en/products/detail/fischer-elektronik/ICK-S-R-140-x-50/26281855), Laserdienst, Handel |
| Kurbel, Kniehebel (Laser), Stahlrohr Ø 12 × 2, 2 Gelenkköpfe M8, Federpaket | 1 Satz | ≈ 45 € | Laserdienst, Handel |
| Wägezelle 20 kg, Manschetten-Messplatine (3 × ADS131M04, Platine bestückt), Not-Aus, Lastrelais, Sicherung, Bremswiderstand | 1 Satz | ≈ 125 € | Elektronik-Händler, Platinenfertiger |
| Manschette M2 zum Testen: BOA-Verschluss, Schlitten (2 Stangen Ø 5, 2 iglidur-Buchsen), Viskopolster, Druck PETG-CF | 1 Satz | ≈ 40 € | [BOA](https://www.boafit.com/), [igus](https://www.igus.de/product/iglidur_J) |
| **Stufe 1 gesamt** | | **≈ 650–750 €** | |
| Schneckengetriebemotor 5840-31ZY 24 V, **27 U/min** | 1 | ≈ 18,50 $ | [RobotShop 27 U/min](https://www.robotshop.com/es/products/dyna-engine-high-torque-24v-27rpm-double-axis-worm-gear-motor-5840-31zy) |
| Motortreiber Pololu G2 18v17 | 1 | ≈ 40 € | [Pololu #2991](https://www.pololu.com/product/2991) |
| Edelstahl-Lochband 20 × 0,3, Gummi, Schnappschäkel, Wägezelle 50 kg | 1 Satz | ≈ 45 € | Handel |
| **Stufe 2 gesamt** | | **≈ 105 €** | |

Mit Stufe 1 und 2 sind die größten offenen Fragen messbar: Bleibt die Kapsel dicht und eisfrei? Fühlt sich die Regelung gut an? Hält die Schlaufe? Erst danach lohnt die Bestellung der übrigen 5 RS04 und 2 RS03 (≈ 2.200–2.500 €).

## Kaufteile

| ID | Teil | Ausführung | Stück | Masse | Quelle |
|---|---|---|---|---|---|
| RA-04 | Schutzbügel | CFK-Rundrohr Ø 22 × 1,5 (gewickelt), Alu-Eckverbinder geklebt | 2 | 0.70 kg | Händler nach Ausführung |
| RA-05 | Gummipuffer | Gummipuffer Typ A Ø 40 × 30, M8 | 4 | 0.12 kg | Händler nach Ausführung |
| RA-09 | Lendengelenk mit Sperre | Scharnier Alu 8 mm, Elastomerfeder Ø 24 (Shore 70), Bolzen Ø 10, Hubmagnet Intertec ITS-LZ-3263-D-24VDC mit Sperrbolzen Ø 8 | 2 | 0.90 kg | Händler nach Ausführung |
| RA-10 | Gasfeder Lendengelenk | Gasfeder 150 N, Ø 15/6 mm, Hub 80 mm, Einbaulänge 236 mm (Stabilus-Bauform), Kugelpfannen M6 | 2 | 0.24 kg | Händler nach Ausführung |
| GU-01 | Hüftgurt | Hüftflossen eines Trekkingrucksacks (Ersatzteil), 100 mm | 1 | 0.45 kg | Händler nach Ausführung |
| GU-02 | Schultergurte und Polster | S-Schultergurte, Abstandsgewirke 10 mm | 3 | 0.51 kg | Händler nach Ausführung |
| GU-03 | Riemen und Schnallen | Gurtband 20/25 mm, Leiterschnallen, Fidlock-Brustgurt | 3 | 0.36 kg | Händler nach Ausführung |
| EN-01 | Akku | Stihl AP 500 S (36 V, 337 Wh, IPX4, 70 × 116 × 164 mm) | 1 | 2.00 kg | [STIHL AP 500 S (Gewicht, Wh)](https://stihl.com.fj/en/p/batteries-chargers-ap-500-s-battery-124181) |
| EL-01 | Hauptplatine | PJRC Teensy 4.1 (2,4 × 0,7 Zoll = 61 × 18 mm, ≈ 9 g), 2 × CAN-Transceiver SN65HVD230 (CAN1 Beine + Rumpf beugen, CAN3 Arme + Rumpf drehen), CR2032 | 1 | 0.08 kg | [PJRC Teensy 4.1 (Maße)](https://eckstein-shop.de/Maintain_1::PJRC-Marken-EN?qf=4) |
| EL-02 | Spannungswandler | Mean Well DDR-60L-24 (Eingang 18–75 V, 24 V / 2,5 A, 60 W, 92 %) + Pololu D36V28F5 (Eingang 5,3–50 V, 5 V / 3,2 A) + Pololu D36V28F12 für das Licht | 1 | 0.25 kg | [Mean Well DDR-60L-24 / Pololu D36V28F5](https://www.pololu.com/product/3782) |
| EL-03 | Windentreiber | 3 × Pololu G2 18v17 | 3 | 0.09 kg | Händler nach Ausführung |
| EL-04 | Schütz, Hauptsicherung | Gleichstromschütz ≥ 60 V DC / 100 A mit Spule 24 V (EV-Relais, Schließer), Midi-Sicherung 60 A träge | 1 | 0.20 kg | Händler nach Ausführung |
| EL-06 | Ideal-Dioden-Modul | 2 × LTC4359-Modul | 1 | 0.03 kg | Händler nach Ausführung |
| EL-07 | Lagesensor | BNO085 (Adafruit 4754) | 1 | 0.01 kg | Händler nach Ausführung |
| EL-08 | Stromschiene mit Sicherungen | Cu 6 × 6, 6 × Sicherung 20 A (RS04, Rumpf), 4 × 15 A (RS03, Reserve) | 1 | 0.12 kg | Händler nach Ausführung |
| EL-09 | Leistungsleitungen | Silikonlitze 4 mm² / 1,5 mm² | 1 | 0.20 kg | Händler nach Ausführung |
| EL-10 | CAN-Bus | geschirmte Twisted-Pair-Leitung, 2 Stränge | 1 | 0.05 kg | Händler nach Ausführung |
| EL-11 | Rundstecker | M16 IP67 (Binder Serie 423) | 7 | 0.21 kg | Händler nach Ausführung |
| AN-03 | Dichtung und Heizung | V-Ring am Abtrieb, Druckausgleichselement M12, Heizfolie Silikon 12 V 4 W mit NTC, Kabelverschraubung M16 | 8 | 0.24 kg | Händler nach Ausführung |
| AN-E | Ellbogenkapsel-Motor | RobStride RS03 (Planetengetriebe 9:1, CAN 2.0, zwei 14-Bit-Encoder) | 2 | 1.76 kg | [RobStride-Handbuch](https://github.com/RobStride/Product_Information) |
| AN-H | Hüftkapsel-Motor | RobStride RS04 (Planetengetriebe 9:1, CAN 2.0, zwei 14-Bit-Encoder) | 2 | 2.84 kg | [RobStride-Handbuch](https://github.com/RobStride/Product_Information) |
| AN-K | Kniekapsel-Motor | RobStride RS04 (Planetengetriebe 9:1, CAN 2.0, zwei 14-Bit-Encoder) | 2 | 2.84 kg | [RobStride-Handbuch](https://github.com/RobStride/Product_Information) |
| AN-S | Schulterkapsel-Motor | RobStride RS04 (Planetengetriebe 9:1, CAN 2.0, zwei 14-Bit-Encoder) | 2 | 2.84 kg | [RobStride-Handbuch](https://github.com/RobStride/Product_Information) |
| HU-02 | Achsbolzen | Passschraube Ø 16 + Kugelsperrbolzen GN 113.3-16 | 2 | 0.20 kg | Händler nach Ausführung |
| HU-03 | Zentrierfeder | Druckfeder DIN EN 13906, Draht 1,1 mm, außen 13 mm, Länge 25 mm, Edelstahl (Katalogteil) | 2 | 0.02 kg | Händler nach Ausführung |
| HU-06 | Hüftlager | Flanschlager KFL002 (Bohrung 15 mm, Lochabstand 53 mm, 2RS) | 2 | 0.40 kg | Händler nach Ausführung |
| BA-03 | Gelenkköpfe Knie | Gelenkkopf M8 Edelstahl, DIN ISO 12240-4 (z. B. SKF SIKAC 8 M), wartungsfrei | 4 | 0.12 kg | Händler nach Ausführung |
| BA-05 | Federpaket | Druckfeder in Hülse in der Schubstange, vorgespannt | 2 | 0.08 kg | Händler nach Ausführung |
| BE-02 | Steckbolzen Lochraster | Kugelsperrbolzen Ø 6 | 8 | 0.16 kg | Händler nach Ausführung |
| BE-04 | Kniegelenklager | Flanschlager KFL001 (12 mm, Lochabstand 48 mm) + Passschraube M12 | 2 | 0.24 kg | Händler nach Ausführung |
| BE-08 | Knöchellager | Flanschlager KFL08 (8 mm, Lochabstand 37 mm) | 2 | 0.14 kg | Händler nach Ausführung |
| BE-10 | Kippgelenk | Bolzen Ø 8 mit Gummibuchse | 2 | 0.06 kg | Händler nach Ausführung |
| BE-12 | Spikes | Schraubspikes M5, Hartmetallspitze | 12 | 0.06 kg | Händler nach Ausführung |
| BE-13 | Fersenschale, Ristriemen | PETG, Steigeisen-Ratschenriemen | 2 | 0.20 kg | Händler nach Ausführung |
| BE-14 | Drehteller Steigbügel | Axial-Nadellager AXK 3047 + Zentrierfeder, Edelstahlscheiben | 2 | 0.16 kg | Händler nach Ausführung |
| SE-02 | Wägezelle Manschette | Biegebalken YZC-133, 20 kg, 80 × 12,7 × 12,7 mm | 8 | 0.24 kg | Händler nach Ausführung |
| SE-03 | Messwandler Manschetten | TI ADS131M04 (4 Kanäle gleichzeitig, 24 Bit, SPI), 3 Stück auf einer Platine in der Elektronikbox, gemeinsamer 8,192-MHz-Takt | 3 | 0.02 kg | [TI ADS131M04](https://www.ti.com/product/ADS131M04) |
| SE-06 | Fußkontakt | 4 × Halbbrücken-Wägezelle 50 kg (Personenwaagen-Bauform 34 × 34 × 8 mm, 1,0 mV/V, Grenzlast 150 %) je Steigbügel, als Vollbrücke an einem HX711 | 2 | 0.30 kg | [50-kg-Halbbrücken-Wägezelle](https://www.sparkfun.com/products/10245) |
| SE-07 | Wägezelle Schlaufenkopf | Biegebalken YZC-1B, 50 kg | 3 | 0.12 kg | Händler nach Ausführung |
| SE-08 | Drehring Unterarm-Manschette | Gleitring PA6 / iglidur, Ø 100 mm | 2 | 0.06 kg | Händler nach Ausführung |
| SE-10 | BOA-Drehverschluss | BOA Fit System, Orthesen-Drehverschluss mit Stahlseil-Schnürung (z. B. Typ L6 / H+), 2 Bänder je Manschette | 8 | 0.24 kg | [BOA Fit System](https://www.boafit.com/) |
| SE-11 | Längsschlitten Manschette | 2 × Edelstahlstange Ø 5 mm, 2 Gleitbuchsen iglidur JSM-0506 (Trockenlauf, kältefest), Alu-Schlitten, 2 Zentrierfedern ≈ 0,5 N/mm, ±12 mm Weg | 8 | 0.40 kg | [igus iglidur J](https://www.igus.de/product/iglidur_J) |
| SC-03 | Achse Schulter | Stahlrohr Ø 22 × 3, Kugelsperrbolzen | 2 | 0.16 kg | Händler nach Ausführung |
| SC-07 | Armfeder | Schenkelfeder ≈ 0,05 Nm/°, Edelstahl (Katalogteil nach Federrechner auslegen) | 2 | 0.06 kg | Händler nach Ausführung |
| SC-09 | Schulterlager, Abtriebsflansch | Flanschlager KFL001 (12 mm, Lochabstand 48 mm), Flansch 9 × M5 auf Ø 36 | 2 | 0.28 kg | Händler nach Ausführung |
| SC-12 | Gleitführung Schulterblatt | 2 Führungswellen Ø 8 h6 + Linearlager LM8UU, Druckfeder | 2 | 0.20 kg | Händler nach Ausführung |
| AR-03 | Ellbogenlager, Anschlag | Flanschlager KFL08 (8 mm, Lochabstand 37 mm), TPU | 2 | 0.16 kg | Händler nach Ausführung |
| AR-07 | Gelenkköpfe Ellbogen | Gelenkkopf M6 Edelstahl, DIN ISO 12240-4 (z. B. SKF SIKAC 6 M) | 4 | 0.08 kg | Händler nach Ausführung |
| SG-03 | Schnappschäkel | Wichard Schnappschäkel 50 mm, festes Auge, Edelstahl (Bruchlast 2 000 kg) | 3 | 0.09 kg | Händler nach Ausführung |
| SG-06 | Bandantrieb | Schneckengetriebemotor 5840-31ZY 24 V, 27 U/min (gleiche Baugröße 58 × 40 mm wie die 10-U/min-Variante) + Stachelrad r 16 mm | 3 | 0.75 kg | [5840-31ZY 24 V 10 U/min](https://probots.co.in/pb-5840-31zy-24v-10-rpm-dc-worm-gear-reduction-motor.html) |
| SG-07 | Bedienteil Hand | Wippschalter IP67, LED, Handschlaufe | 2 | 0.10 kg | Händler nach Ausführung |
| BD-01 | Not-Aus mit LED-Ring | Eaton M22-PVT + 2 × M22-K01 (zwei Öffner: Schützspule und Rückmeldung) + WS2812B-Ring 12 LEDs | 1 | 0.09 kg | Händler nach Ausführung |
| BD-02 | Ein/Aus-Taster | Drucktaster IP67 | 1 | 0.02 kg | Händler nach Ausführung |
| BD-03 | Anzeige Oberarm | Waveshare 1,54" E-Paper + ESP32-C3 SuperMini | 1 | 0.06 kg | Händler nach Ausführung |
| BD-04 | Arbeitslicht | LED-Strahler 10 W, 12 V | 2 | 0.24 kg | Händler nach Ausführung |

## Halbzeug (Metallhandel, Zuschnitt)

| ID | Teil | Ausführung | Stück | Masse | Quelle |
|---|---|---|---|---|---|
| RA-01 | Rahmenrohr senkrecht | EN AW-6082 T6 Vierkantrohr 30 × 30 × 2 | 2 | 0.72 kg | Händler nach Ausführung |
| RA-02 | Querrohr | EN AW-6082 T6 Vierkantrohr 20 × 20 × 2 | 1 | 0.09 kg | Händler nach Ausführung |
| HU-04 | Nabe | Alu-Block 56 × 40 × 40, 2 Gleitlager iglidur G Ø 16 | 2 | 0.40 kg | Händler nach Ausführung |
| HU-05 | Hüftschwinge | EN AW-6082 Vierkantrohr 35 × 35 × 3 | 2 | 1.10 kg | Händler nach Ausführung |
| BA-02 | Schubstange Knie | Stahlrohr E235 Ø 12 × 2, L ≈ 310 mm, Gewindeeinsätze M8 eingeschweißt; Kurbelzapfen Passschraube ISO 7379 Ø 8, 12.9 | 2 | 0.30 kg | Händler nach Ausführung |
| BE-01 | Oberschenkelschiene | 6082 Vierkant 30 × 30 × 2 außen / 25 × 25 × 2,5 innen (1 mm Spiel) | 2 | 0.90 kg | Händler nach Ausführung |
| BE-06 | Unterschenkelschiene | 6082 Vierkant 30 × 30 × 2 / 25 × 25 × 2 | 2 | 0.80 kg | Händler nach Ausführung |
| BE-09 | Fersenbügel | Alu-Rundstab Ø 16 | 2 | 0.30 kg | Händler nach Ausführung |
| SC-01 | Schulterbügel | 6082 Vierkant 30 × 30 × 3 | 2 | 0.60 kg | Händler nach Ausführung |
| SC-06 | Schulternabe | Alu-Block, Gleitlager | 2 | 0.24 kg | Händler nach Ausführung |
| SC-08 | Schulterschwinge | 6082 Vierkant 30 × 30 × 3 | 2 | 0.70 kg | Händler nach Ausführung |
| AR-01 | Oberarmschiene | 6082 Vierkant 25 × 25 × 3 | 2 | 0.40 kg | Händler nach Ausführung |
| AR-02 | Unterarmschiene | 6082 Vierkant 25 × 25 × 3 | 2 | 0.36 kg | Händler nach Ausführung |
| AR-06 | Schubstange Ellbogen | Stahlrohr Ø 10 × 1,5, L ≈ 280 mm, Gewinde M6 | 2 | 0.16 kg | Händler nach Ausführung |
| SG-02 | Schlaufenkopf | Alu-Block mit Bandschlitz, Zahnleiste TPU | 3 | 0.24 kg | Händler nach Ausführung |
| SG-05 | Bandkanal und Speicher | Alu-U 12 × 12 mit PTFE-Profil | 3 | 0.15 kg | Händler nach Ausführung |

## Laserteile (DXF in fertigung/laser, Laserdienst)

| ID | Teil | Ausführung | Stück | Masse | Quelle |
|---|---|---|---|---|---|
| RA-03 | Eck-Doppelplatte, Lochleiste | AlMg3 4 mm | 12 | 0.48 kg | Händler nach Ausführung |
| AN-01 | Elektronikbox | Alu 1,5 mm gekantet, PETG-Deckel, 85 × 270 × 240 mm | 1 | 0.45 kg | Händler nach Ausführung |
| AN-04 | Motorträger | EN AW-6082 6 mm: Deckelplatte mit Lochbild des Motors, Wangen um die Kurbel | 8 | 0.96 kg | Händler nach Ausführung |
| HU-01 | Abspreizgabel | EN AW-6082 8 mm, verschraubt | 2 | 0.50 kg | Händler nach Ausführung |
| HU-07 | Oberschenkelplatte, Abtriebsflansch | Alu 8 mm, Flansch 9 × M5 auf Ø 36 (Abtrieb RS04), TPU 95A | 2 | 0.20 kg | Händler nach Ausführung |
| BA-01 | Kurbel Knie | EN AW-6082 10 mm, r 60 mm, Klemmnabe auf dem Abtrieb (9 × M5) | 2 | 0.12 kg | Händler nach Ausführung |
| BA-04 | Kniehebel | EN AW-6082 10 mm, r 60 mm, Nabe auf dem Kniebolzen | 2 | 0.12 kg | Händler nach Ausführung |
| BE-03 | Knieplatten | Alu 6 mm | 2 | 0.12 kg | Händler nach Ausführung |
| BE-11 | Steigbügel | Edelstahl 1.4301, 4 mm | 2 | 0.50 kg | Händler nach Ausführung |
| SE-04 | Überlastpuffer, Halter | TPU, Alu | 8 | 0.16 kg | Händler nach Ausführung |
| SC-02 | Schultergabel | Alu 10 mm | 2 | 0.24 kg | Händler nach Ausführung |
| AR-04 | Kurbel Ellbogen | EN AW-6082 8 mm, r 50 mm, auf dem Abtrieb (6 × M4) | 2 | 0.08 kg | Händler nach Ausführung |
| AR-08 | Ellbogenhebel | EN AW-6082 8 mm, r 50 mm, Nabe auf dem Ellbogenbolzen | 2 | 0.08 kg | Händler nach Ausführung |
| SG-01 | Schlaufenträger | Alu 6 mm | 2 | 0.24 kg | Händler nach Ausführung |

## Druckteile (STL in fertigung/druck, Bambu A1 mini)

| ID | Teil | Ausführung | Stück | Masse | Quelle |
|---|---|---|---|---|---|
| RA-08 | Hauben | PETG matt, Wand 1,2 mm mit Rippen, Dichtung EPDM | 3 | 0.30 kg | Händler nach Ausführung |
| RA-11 | Kurbelschutz | PETG transparent 1,5 mm, geschraubt | 4 | 0.12 kg | Händler nach Ausführung |
| EN-02 | Akkuschacht | PETG-CF, Führungsschienen Alu 4 × 4, Kontaktträger | 2 | 0.50 kg | Händler nach Ausführung |
| AN-02 | Kapselhaube | PETG-CF, Wand 2 mm, O-Ring, 5 mm Dämmung innen | 8 | 0.72 kg | Händler nach Ausführung |
| BE-05 | Knieanschlag | TPU 95A | 2 | 0.02 kg | Händler nach Ausführung |
| SE-01 | Manschette M2 | PETG-CF-Schale 4 mm, 45 % breiter (Oberarm 87, Unterarm 73, Oberschenkel 102, Schienbein 87 mm), Viskopolster 4 mm perforiert mit Wechselbezug, 3 Größen | 8 | 1.04 kg | Händler nach Ausführung |

## Selbst fertigen

| ID | Teil | Ausführung | Stück | Masse | Quelle |
|---|---|---|---|---|---|
| KB-01 | Kabelbaum Beinkapseln | 2 × 2,5 mm² + CAN + Heizung in Gewebeschlauch 10 mm | 2 | 0.40 kg | Händler nach Ausführung |
| KB-02 | Kabelbaum Bein | 6-adrig geschirmt | 2 | 0.20 kg | Händler nach Ausführung |
| KB-03 | Kabelbaum Arm | 12-adrig, Gewebeschlauch | 2 | 0.30 kg | Händler nach Ausführung |
| KB-04 | Kabel Licht, Anzeige, Not-Aus | LiYCY 4 × 0,25 mm² | 1 | 0.05 kg | Händler nach Ausführung |
| SG-04 | Spannband | Edelstahl-Lochband 20 × 0,3 mm, Gummi 2 mm innen | 3 | 0.18 kg | Händler nach Ausführung |

Hinweis: Viele Quellen sind Händlerseiten, keine Herstellerdatenblätter. Maße und Lochbilder vor der Bestellung am Datenblatt prüfen.
