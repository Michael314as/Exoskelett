# Bestellliste Prototyp (Entwurf 4)

Erzeugt aus der Bauteilliste der technischen Ansicht ([exo-technik.html](../visualisierung/exo-technik.html), Reiter „Stückliste“). Stückzahlen für ein komplettes Exo mit einem Akku. Teile mit mehreren Herstellwegen (z. B. „Laser + Kauf“) stehen beim ersten Weg.

**Vor der Bestellung:** Lochabstand der Flanschlager und Lochkreis der Motoren mit den Datenblättern abgleichen und in `tools/fertigung.py` eintragen, dann `python3 tools/fertigung.py` neu laufen lassen (Prüfpunkt F3).

## Empfohlene Reihenfolge (Kosten verteilen, früh messen)

| Stufe | Bestellen | Damit prüfst du |
|---|---|---|
| 1 Prüfstand | 1 Motor AK70-10, Teensy 4.1 + CAN-Transceiver, 1 Wägezelle + HX711, 1 Manschette (Druck), Seilscheibe Ellbogen (Laser), Dyneema, 1 Bowdenzug-Paar, Not-Aus, Netzteil 36 V oder Akku-Adapter | Regelgefühl, Reaktionszeit, Rauschen, Wärme (P10–P17, P33 am Holzarm) |
| 2 Schlaufe | 1 Bandantrieb 5840-31ZY + Treiber, Lochband, Schlaufenkopf, Wägezelle 50 kg | Haltekraft mit 20-kg-Sandsack (P20) — klärt die geschätzten 100 N |
| 3 Rahmen passiv | Halbzeug, Laserteile Rahmen/Hüfte/Beine/Fuß, Lager, Gurte | Passform, Achsen, Lendengelenk (P1–P4, P31, P34) |
| 4 Beine aktiv | 4 Motoren (Hüfte, Knie), Fußkontakt-Wägezellen, Bowdenzüge | Aufstehen, Gehen (P20b, P21, P23) |
| 5 Arme aktiv | 4 Motoren (Schulter, Ellbogen), restliche Manschetten, Schlaufen | Bäume (P22, P27, P28) |

## Kaufteile

| ID | Teil | Ausführung | Stück | Masse | Quelle |
|---|---|---|---|---|---|
| RA-04 | Schutzbügel | CFK-Rundrohr Ø 22 × 1,5 (gewickelt), Alu-Eckverbinder geklebt | 2 | 0.70 kg | Händler nach Ausführung |
| RA-05 | Gummipuffer | Gummipuffer Typ A Ø 40 × 30, M8 | 4 | 0.12 kg | Händler nach Ausführung |
| RA-07 | Parkstütze | Vierkant 20 × 20 × 2, Kugelsperrbolzen GN 113 Ø 6 | 2 | 0.70 kg | Händler nach Ausführung |
| RA-09 | Lendengelenk mit Sperre | Scharnier Alu 8 mm, Elastomerfeder Ø 24 (Shore 70), Bolzen Ø 10, Hubmagnet 24 V mit Sperrbolzen | 2 | 0.90 kg | Händler nach Ausführung |
| RA-10 | Gasfeder Lendengelenk | Gasfeder 150 N, Hub 50 mm, Ø 15/6 mm, Kugelpfannen M6 (Katalogteil) | 2 | 0.24 kg | Händler nach Ausführung |
| GU-01 | Hüftgurt | Hüftflossen eines Trekkingrucksacks (Ersatzteil), 100 mm | 1 | 0.45 kg | Händler nach Ausführung |
| GU-02 | Schultergurte und Polster | S-Schultergurte, Abstandsgewirke 10 mm | 3 | 1.50 kg | Händler nach Ausführung |
| GU-03 | Riemen und Schnallen | Gurtband 20/25 mm, Leiterschnallen, Fidlock-Brustgurt | 3 | 0.36 kg | Händler nach Ausführung |
| EN-01 | Akku | Stihl AP 500 S (36 V, 337 Wh, IPX4, 70 × 116 × 164 mm) | 1 (+1 optional, meist schon vorhanden) | 2.00 kg | [STIHL AP 500 S (Gewicht, Wh)](https://stihl.com.fj/en/p/batteries-chargers-ap-500-s-battery-124181) |
| EL-01 | Hauptplatine | PJRC Teensy 4.1 (2,4 × 0,7 Zoll = 61 × 18 mm, ≈ 9 g), CAN-Transceiver SN65HVD230, CR2032 | 1 | 0.08 kg | [PJRC Teensy 4.1 (Maße)](https://eckstein-shop.de/Maintain_1::PJRC-Marken-EN?qf=4) |
| EL-02 | Spannungswandler | Mean Well DDR-60L-24 (Eingang 18–75 V, 24 V / 2,5 A, 60 W, 92 %) + Pololu D36V28F5 (Eingang 5,3–50 V, 5 V / 3,2 A) + Pololu D36V28F12 für das Licht | 1 | 0.25 kg | [Mean Well DDR-60L-24 / Pololu D36V28F5](https://www.pololu.com/product/3782) |
| EL-03 | Windentreiber | 3 × Pololu G2 18v17 | 3 | 0.09 kg | Händler nach Ausführung |
| EL-04 | Schütz, Hauptsicherung | Lastrelais 24 V / 80 A, Midi-Sicherung 40 A | 1 | 0.20 kg | Händler nach Ausführung |
| EL-06 | Ideal-Dioden-Modul | 2 × LTC4359-Modul | 1 | 0.03 kg | Händler nach Ausführung |
| EL-07 | Lagesensor | BNO085 (Adafruit 4754) | 1 | 0.01 kg | Händler nach Ausführung |
| EL-08 | Stromschiene mit Sicherungen | Cu 6 × 6, 8 × Flachsicherung 15 A | 1 | 0.12 kg | Händler nach Ausführung |
| EL-09 | Leistungsleitungen | Silikonlitze 4 mm² / 1,5 mm² | 1 | 0.20 kg | Händler nach Ausführung |
| EL-10 | CAN-Bus | geschirmte Twisted-Pair-Leitung | 1 | 0.05 kg | Händler nach Ausführung |
| EL-11 | Rundstecker | M16 IP67 (Binder Serie 423) | 7 | 0.21 kg | Händler nach Ausführung |
| AN-E | Ellbogenmotor | CubeMars AK70-10 KV100 (Planetengetriebe 10:1, CAN, Encoder) | 2 | 1.04 kg | [CubeMars AK70-10](https://www.cubemars.com/goods-1031-AK70-10.html) |
| AN-K | Kniemotor | CubeMars AK70-10 KV100 (Planetengetriebe 10:1, CAN, Encoder) | 2 | 1.04 kg | [CubeMars AK70-10](https://www.cubemars.com/goods-1031-AK70-10.html) |
| AN-S | Schultermotor | CubeMars AK70-10 KV100 (Planetengetriebe 10:1, CAN, Encoder) | 2 | 1.04 kg | [CubeMars AK70-10](https://www.cubemars.com/goods-1031-AK70-10.html) |
| HU-02 | Achsbolzen | Passschraube Ø 16 + Kugelsperrbolzen GN 113.3-16 | 2 | 0.20 kg | Händler nach Ausführung |
| HU-03 | Zentrierfeder | Druckfeder 1,1 × 13 × 25 (z. B. Gutekunst D-218) | 2 | 0.02 kg | Händler nach Ausführung |
| HU-06 | Hüftlager | Flanschlager KFL002 (Bohrung 15 mm, 2RS) | 2 | 0.40 kg | Händler nach Ausführung |
| BA-01 | Hüftmotor | CubeMars AK70-10 KV100 (Planetengetriebe 10:1, CAN, Encoder) | 2 | 1.04 kg | [CubeMars AK70-10](https://www.cubemars.com/goods-1031-AK70-10.html) |
| BA-04 | Seilschleife Hüfte | Liros D-Pro 3 mm (Dyneema SK78) | 4 | 0.04 kg | [LIROS D-Pro 3 mm, 950 daN](https://awn.de/en/products/liros-d-pro-tauwerk-meterware-1-m) |
| BA-07 | Federpatrone | Druckfeder 2 × 16 × 40 mit Hülse | 2 | 0.10 kg | Händler nach Ausführung |
| BA-11 | Bowdenhülle Knie | Jagwire LEX-SL 5 mm (kompressionsarm, PTFE-Liner) | 4 | 0.24 kg | Händler nach Ausführung |
| BA-12 | Innenzug Knie | Edelstahl 7 × 7 Ø 2 | 4 | 0.08 kg | Händler nach Ausführung |
| BE-02 | Steckbolzen Lochraster | Kugelsperrbolzen Ø 6 | 8 | 0.16 kg | Händler nach Ausführung |
| BE-04 | Kniegelenklager | Flanschlager KFL001 (12 mm) + Passschraube M12 | 2 | 0.24 kg | Händler nach Ausführung |
| BE-08 | Knöchellager | Flanschlager KFL08 (8 mm) | 2 | 0.14 kg | Händler nach Ausführung |
| BE-10 | Kippgelenk | Bolzen Ø 8 mit Gummibuchse | 2 | 0.06 kg | Händler nach Ausführung |
| BE-12 | Spikes | Schraubspikes M5, Hartmetallspitze | 12 | 0.06 kg | Händler nach Ausführung |
| BE-13 | Fersenschale, Ristriemen | PETG, Steigeisen-Ratschenriemen | 2 | 0.20 kg | Händler nach Ausführung |
| BE-14 | Drehteller Steigbügel | Axial-Nadellager AXK 3047 + Zentrierfeder, Edelstahlscheiben | 2 | 0.16 kg | Händler nach Ausführung |
| SE-02 | Wägezelle Manschette | Biegebalken YZC-133, 20 kg, 80 × 12,7 × 12,7 mm | 6 | 0.18 kg | Händler nach Ausführung |
| SE-03 | Messverstärker | HX711-Modul 24 × 16 mm, vergossen (Ausbau: ADS1220) | 6 | 0.03 kg | Händler nach Ausführung |
| SE-06 | Fußkontakt | 4 × Halbbrücken-Wägezelle 50 kg (Personenwaagen-Bauform 34 × 34 × 8 mm, 1,0 mV/V, Grenzlast 150 %) je Steigbügel, als Vollbrücke an einem HX711 | 2 | 0.30 kg | [50-kg-Halbbrücken-Wägezelle](https://www.sparkfun.com/products/10245) |
| SE-07 | Wägezelle Schlaufenkopf | Biegebalken YZC-1B, 50 kg | 3 | 0.12 kg | Händler nach Ausführung |
| SE-08 | Drehring Unterarm-Manschette | Gleitring PA6 / iglidur, Ø 100 mm | 2 | 0.06 kg | Händler nach Ausführung |
| SC-03 | Hohlachse Schulter | Stahlrohr Ø 22 × 3, Kugelsperrbolzen | 2 | 0.16 kg | Händler nach Ausführung |
| SC-04 | Umlenkrolle | Seilrolle Ø 24 mit Kugellager 625-2RS im Rollenbock | 12 | 0.24 kg | Händler nach Ausführung |
| SC-07 | Armfeder | Schenkelfeder ≈ 0,05 Nm/° (z. B. Gutekunst T-40) | 2 | 0.06 kg | Händler nach Ausführung |
| SC-09 | Schulterlager | Flanschlager KFL001 (12 mm) | 2 | 0.24 kg | Händler nach Ausführung |
| SC-11 | Seilpaar Schulter | Liros D-Pro 3 mm (Bruchlast 950 daN) | 2 | 0.02 kg | [LIROS D-Pro 3 mm, 950 daN](https://awn.de/en/products/liros-d-pro-tauwerk-meterware-1-m) |
| SC-12 | Gleitführung Schulterblatt | 2 Führungswellen Ø 8 h6 + Linearlager LM8UU, Druckfeder | 2 | 0.20 kg | Händler nach Ausführung |
| AR-03 | Ellbogenlager, Anschlag | Flanschlager KFL08, TPU | 2 | 0.16 kg | Händler nach Ausführung |
| AR-06 | Bowdenhülle Ellbogen | Jagwire LEX-SL 5 mm | 4 | 0.24 kg | Händler nach Ausführung |
| AR-07 | Innenzug Ellbogen | Edelstahl 7 × 7 Ø 2 | 4 | 0.08 kg | Händler nach Ausführung |
| SG-03 | Schnappschäkel | Wichard 2673 (Edelstahl) | 3 | 0.09 kg | Händler nach Ausführung |
| SG-06 | Bandantrieb | Schneckengetriebemotor 5840-31ZY 24 V, 27 U/min (gleiche Baugröße 58 × 40 mm wie die 10-U/min-Variante) + Stachelrad r 16 mm | 3 | 0.75 kg | [5840-31ZY 24 V 10 U/min](https://probots.co.in/pb-5840-31zy-24v-10-rpm-dc-worm-gear-reduction-motor.html) |
| SG-07 | Bedienteil Hand | Wippschalter IP67, LED, Handschlaufe | 2 | 0.10 kg | Händler nach Ausführung |
| BD-01 | Not-Aus mit LED-Ring | Eaton M22-PVT/K11 + WS2812B-Ring 12 LEDs | 1 | 0.09 kg | Händler nach Ausführung |
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
| BA-03 | Trommel Ø 20 | Alu Ø 20, Bordscheiben Ø 32 | 2 | 0.06 kg | Händler nach Ausführung |
| BE-01 | Oberschenkelschiene | 6082 Vierkant 30 × 30 × 2,5 außen / 25 × 25 × 2,5 innen | 2 | 0.90 kg | Händler nach Ausführung |
| BE-06 | Unterschenkelschiene | 6082 Vierkant 30 × 30 × 2 / 25 × 25 × 2 | 2 | 0.80 kg | Händler nach Ausführung |
| BE-09 | Fersenbügel | Alu-Rundstab Ø 16 | 2 | 0.30 kg | Händler nach Ausführung |
| SC-01 | Schulterbügel | 6082 Vierkant 30 × 30 × 3 | 2 | 0.60 kg | Händler nach Ausführung |
| SC-06 | Schulternabe | Alu-Block, Gleitlager | 2 | 0.24 kg | Händler nach Ausführung |
| SC-08 | Schulterschwinge | 6082 Vierkant 30 × 30 × 3 | 2 | 0.70 kg | Händler nach Ausführung |
| AR-01 | Oberarmschiene | 6082 Vierkant 25 × 25 × 3 | 2 | 0.40 kg | Händler nach Ausführung |
| AR-02 | Unterarmschiene | 6082 Vierkant 25 × 25 × 3 | 2 | 0.36 kg | Händler nach Ausführung |
| SG-02 | Schlaufenkopf | Alu-Block mit Bandschlitz, Zahnleiste TPU | 3 | 0.24 kg | Händler nach Ausführung |
| SG-05 | Bandkanal und Speicher | Alu-U 12 × 12 mit PTFE-Profil | 3 | 0.15 kg | Händler nach Ausführung |

## Laserteile (DXF in fertigung/laser, Laserdienst)

| ID | Teil | Ausführung | Stück | Masse | Quelle |
|---|---|---|---|---|---|
| RA-03 | Eck-Doppelplatte, Lochleiste | AlMg3 4 mm | 12 | 0.48 kg | Händler nach Ausführung |
| AN-01 | Boxgehäuse | Alu 1,5 mm gekantet (Motorplatte trägt), PETG-Deckel | 1 | 0.65 kg | Händler nach Ausführung |
| AN-T | Doppeltrommel | Alu-Rohr Ø 30 × 3, gelaserte Bordscheiben | 6 | 0.36 kg | Händler nach Ausführung |
| HU-01 | Abspreizgabel | EN AW-6082 8 mm, verschraubt | 2 | 0.50 kg | Händler nach Ausführung |
| HU-07 | Oberschenkelplatte, Anschlag | Alu 8 mm, TPU 95A | 2 | 0.14 kg | Händler nach Ausführung |
| BA-02 | Motorplatte | Alu 4 mm, Distanzbolzen M5 | 2 | 0.30 kg | Händler nach Ausführung |
| BA-05 | Seilscheibe Ø 120 | 3 Lagen Alu 4 mm + 2 Zwischenlagen | 2 | 0.36 kg | Händler nach Ausführung |
| BA-06 | Hüllenanschlag Knie | Alu 4 mm | 2 | 0.06 kg | Händler nach Ausführung |
| BA-09 | Kurvenscheibe Knie | Alu 10 mm, Profil r 50 → 80 mm | 2 | 0.16 kg | Händler nach Ausführung |
| BA-10 | Beugescheibe Knie Ø 90 | Alu 10 mm | 2 | 0.10 kg | Händler nach Ausführung |
| BE-03 | Knieplatten | Alu 6 mm | 2 | 0.12 kg | Händler nach Ausführung |
| BE-11 | Steigbügel | Edelstahl 1.4301, 4 mm | 2 | 0.50 kg | Händler nach Ausführung |
| SE-04 | Überlastpuffer, Halter | TPU, Alu | 6 | 0.12 kg | Händler nach Ausführung |
| SC-02 | Schultergabel | Alu 10 mm | 2 | 0.24 kg | Händler nach Ausführung |
| SC-05 | Rollenträger | Alu 4 mm, Streben | 2 | 0.12 kg | Händler nach Ausführung |
| SC-10 | Seilscheibe Schulter Ø 140 | 3 Lagen Alu | 2 | 0.30 kg | Händler nach Ausführung |
| AR-04 | Seilscheibe Ellbogen Ø 100 | 3 Lagen Alu | 2 | 0.16 kg | Händler nach Ausführung |
| AR-08 | Hüllenanschlag Ellbogen | Alu 4 mm | 2 | 0.06 kg | Händler nach Ausführung |
| SG-01 | Schlaufenträger | Alu 6 mm | 2 | 0.24 kg | Händler nach Ausführung |

## Druckteile (STL in fertigung/druck, Bambu A1 mini)

| ID | Teil | Ausführung | Stück | Masse | Quelle |
|---|---|---|---|---|---|
| RA-08 | Hauben | PETG matt, Wand 1,2 mm mit Rippen, Dichtung EPDM | 3 | 0.75 kg | Händler nach Ausführung |
| RA-11 | Fingerschutz Seilscheiben | Polycarbonat 1,5 mm, durchsichtig, geschraubt | 2 | 0.10 kg | Händler nach Ausführung |
| EN-02 | Akkuschacht | PETG-CF, Führungsschienen Alu 4 × 4, Kontaktträger | 2 | 0.50 kg | Händler nach Ausführung |
| BA-08 | Hüllenhalter | PETG | 2 | 0.02 kg | Händler nach Ausführung |
| BE-05 | Knieanschlag | TPU 95A | 2 | 0.02 kg | Händler nach Ausführung |
| BE-07 | Schienbeinschale | PETG, Polster 6 mm, Klett | 2 | 0.24 kg | Händler nach Ausführung |
| SE-01 | Manschette | PETG-Schale 5 mm, Polster 6 mm, Klett | 6 | 0.60 kg | Händler nach Ausführung |

## Selbst fertigen

| ID | Teil | Ausführung | Stück | Masse | Quelle |
|---|---|---|---|---|---|
| KB-01 | Kabelbaum Hüftmotor | 2 × 1,5 mm² + CAN in Gewebeschlauch 10 mm | 2 | 0.30 kg | Händler nach Ausführung |
| KB-02 | Kabelbaum Bein | 6-adrig geschirmt | 2 | 0.20 kg | Händler nach Ausführung |
| KB-03 | Kabelbaum Arm | 12-adrig, Gewebeschlauch | 2 | 0.30 kg | Händler nach Ausführung |
| KB-04 | Kabel Licht, Anzeige, Not-Aus | LiYCY 4 × 0,25 mm² | 1 | 0.05 kg | Händler nach Ausführung |
| SG-04 | Spannband | Edelstahl-Lochband 20 × 0,3 mm, Gummi 2 mm innen | 3 | 0.18 kg | Händler nach Ausführung |

Hinweis: Viele Quellen sind Händlerseiten, keine Herstellerdatenblätter. Maße und Lochbilder vor der Bestellung am Datenblatt prüfen.
