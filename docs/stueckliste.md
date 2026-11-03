# Stückliste und Herstellweg (Entwurf 5: Gelenkkapseln)

Jedes Teil des Exos mit Menge, Material, **wie es entsteht** und welches Werkzeug dafür nötig ist.
Damit ist geprüft, dass nichts eine Drehbank, Fräse oder Schweißanlage braucht.

Herstellweg:
- **Kauf:** fertiges Teil bestellen.
- **Laser:** DXF-Zeichnung bei einem Online-Laserdienst hochladen, Teil kommt fertig zugeschnitten (Alu oder Edelstahl, 3–10 mm). Bohrungen für Lager werden mitgelasert.
- **Druck:** 3D-Druck auf dem Bambu Lab A1 mini (Bauraum 18 × 18 × 18 cm).
- **Säge + Bohren:** Profil ablängen, nach gedruckter Bohrschablone bohren.
- **Dienst:** Oberfläche beim Betrieb um die Ecke.

Preise sind Richtwerte (Stand November 2026). Motoren nach recherchierten Händlerpreisen, siehe [bestellung-prototyp.md](bestellung-prototyp.md).

**Gesamt Material, ein Exo:** etwa **6.000–7.100 €**, davon Motoren (7 × RobStride RS04, 3 × RS03) 3.200–3.600 €, alles andere ≈ 2.400–3.000 € (Rev. M2: Manschetten mit BOA, Längsschlitten, schneller Messung und Schienbein-Sensor ≈ +300 €).
Dazu einmalig Werkzeug 280–500 €. Stihl-Akku vorhanden. Erster Schritt Prüfstand (eine Kapsel) + Schlaufe ≈ 600 €.

---

## 1. Rahmen, Hüfte, Beine

| Teil | Menge | Material, Maß | Herstellweg | etwa |
|---|---|---|---|---|
| Rückenrahmen senkrecht | 2 | Alu-Vierkantrohr 25 × 25 × 2, je 60 cm | Säge + Bohren | 15 € |
| Rückenrahmen quer | 3 | Alu-Vierkantrohr 20 × 20 × 2, je 22 cm | Säge + Bohren | 6 € |
| Eck-Doppelplatten | 12 | Alu 4 mm, 60 × 60 | Laser | 25 € |
| Gurtsystem: Hüftgurt, S-Schultergurte, Lageverstellriemen, Brustgurt, Lenden- und Schulterblattpolster | 1 Satz | Trekkingrucksack-Ersatzteile oder genäht (Gurtband 50 mm, Abstandsgewirke), Magnet- und Leiterschnallen | Kauf / Nähen | 80–130 € |
| Abspreizgelenk: Gabel | 2 | Alu 8 mm, 2 Platten + Steg | Laser | 25 € |
| Abspreizgelenk: Nabe, Achsbolzen, Kugelsperrbolzen Ø 16 | 2 Satz | Alu-Block, 2 Gleitlager, Bolzen | Kauf + Bohren | 40 € |
| Zentrierfedern + Gummipuffer | 4 + 4 | Druckfeder Ø 13, TPU | Kauf / Druck | 10 € |
| Hüftschwinge | 2 | Alu-Vierkantrohr 35 × 35 × 3 + Eckplatten | Säge + Bohren, Laser | 25 € |
| Hüftlager | 2 | Flanschlager UCFL/KFL, 2RS | Kauf | 20 € |
| Ober-/Unterschenkelschienen außen | 4 | Alu 25 × 25 × 2 | Säge + Bohren (Lochraster per Schablone) | 15 € |
| Ober-/Unterschenkelschienen innen | 4 | Alu 20 × 20 × 2 | Säge + Bohren | 10 € |
| Lagerplatten für die Flanschlager (KFL001 Lochabstand 48 mm: Knie, Schulter; KFL08 37 mm: Ellbogen, Knöchel; KFL002 53 mm: Hüfte) | 8 + 8 + 2 | Alu 6082, 6 / 8 mm | Laser | 35 € |
| Kniegelenk-Lager | 2 | Flanschlager klein, 2RS | Kauf | 14 € |
| Passschrauben, Nylonmuttern | 1 Satz | M10/M12, Edelstahl | Kauf | 25 € |
| Steckbolzen mit Federring | 8 | Ø 6 mm | Kauf | 15 € |
| Steigbügel unter der Sohlenmitte | 2 | Edelstahl 4 mm (Bügel + Seitenwangen) | Laser + Biegen | 30 € |
| Spikes | 12 | Schraubspikes | Kauf | 10 € |
| Fersenschale, Ratschen-Ristriemen | 2 Satz | PETG / Steigeisen-Ersatzteil | Druck + Kauf | 30 € |
| Knöchellager (Flanschlager klein) | 2 | 2RS | Kauf | 14 € |
| Fersenbügel mit Kippgelenk hinter der Ferse | 2 Satz | Alu-Rohr 16 mm gebogen oder Laser-Laschen, Bolzen mit Gummibuchsen | Laser + Kauf | 25 € |

## 2. Antrieb: Gelenkkapseln und Schubstangen

| Teil | Menge | Material, Maß | Herstellweg | etwa |
|---|---|---|---|---|
| Robotergelenk-Motor RobStride RS04 (Hüfte, Knie, Schulter, Rumpf beugen) | 7 | 40 Nm Nenn / 120 Nm Spitze, 1,42 kg, Ø 120 × 52; ≈ 295–320 $ (China ¥ 1.199) | Kauf | 1.950–2.200 € |
| Robotergelenk-Motor RobStride RS03 (Ellbogen, Rumpf drehen) | 3 | 20 Nm Nenn / 60 Nm Spitze, 0,88 kg; ≈ 225–245 $ (China ¥ 999) | Kauf | 500–560 € |
| (beides inkl. Versand, Einfuhrumsatzsteuer 19 %, Zoll ≈ 2,7 %) | | | | |
| Stiftkühlkörper Fischer ICK S R 140 (RS04) / Ø 120 (RS03), Stifte auf 30 mm kürzen, nach Bohrschablone bohren | 6 + 2 | Alu schwarz eloxiert | Kauf + Bohren | 200–260 € |
| Wärmeleitpads 1 mm, 6 W/mK | 8 | | Kauf | 25 € |
| Wangen (Motorträger) | 12 | Alu 6082, 8 mm | Laser | 40 € |
| Abtriebsflansch RS04 (Oberschenkel, Oberarm) | 4 | Alu 8 mm | Laser | 20 € |
| Kapselmäntel | 8 | PETG-CF, Wand 2,4 mm, zwei O-Ring-Nuten | Druck | 15 € |
| V-Ringe, O-Ringe, Kabelverschraubungen M16 | 8 Satz | NBR, Messing vernickelt | Kauf | 40 € |
| Druckausgleichselemente M12 | 8 | Membran (z. B. GORE) | Kauf | 50 € |
| Heizbänder Silikon 12 V 4 W (Abtriebsseite) | 8 | | Kauf | 50 € |
| Kurbeln Knie (r 60) / Ellbogen (r 50), Hebel Knie / Ellbogen | 2 + 2 + 2 + 2 | Alu 6082, 8–10 mm | Laser | 30 € |
| Schubstangen | 2 + 2 | Stahlrohr Ø 12 × 2 (Knie), Ø 10 × 1,5 (Ellbogen), Gewinde M8 / M6 | Säge + Kleben/Löten | 15 € |
| Gelenkköpfe Edelstahl, DIN ISO 12240-4 | 4 × M8 + 4 × M6 | wartungsfrei (z. B. SKF SIKAC 8 M / 6 M) | Kauf | 70 € |
| Federpaket Knie (Druckfeder mit Hülse in der Stange) | 2 | Federstahl | Kauf + Druck | 15 € |
| Rumpfantrieb: Kurbeln, Hebel am Lendengelenk, Träger (Laser), Stangen Ø 12 / Ø 10, 4 Gelenkköpfe | 1 Satz | Laser + Kauf | 60 € |
| Rumpf-Wägezellen 20 kg (Gurtanker, Schulterblattpolster), am ADS131M04 | 4 | Kauf | 20 € |
| Bremswiderstand mit Schaltung (ab 46 V, 10 Ω / 200 W) | 1 | Aluminium-Gehäusewiderstand, MOSFET, Komparator | Kauf + Löten | 30 € |

*Warum keine Seile mehr:* siehe [konzept.md](konzept.md), Abschnitt 5.

## 3. Arme, Manschetten, Greifer

| Teil | Menge | Material, Maß | Herstellweg | etwa |
|---|---|---|---|---|
| Schulterbügel | 2 | Alu 20 × 20 × 2 + Laschen | Säge + Bohren, Laser | 15 € |
| Ober-/Unterarmschienen | 4 | Alu 20 × 20 × 2 / 25 × 25 × 2 | Säge + Bohren | 12 € |
| Schulter-/Ellbogenlager | 4 | Flanschlager klein | Kauf | 28 € |
| Manschetten-Schalen M2 (Oberarm, Unterarm, Oberschenkel, Schienbein; 3 Größen) | 8 | PETG-CF 4 mm, 45 % breiter, nach Handy-Scan | Druck (größte ~17 × 12 × 9 cm) | 30 € |
| Viskopolster mit Wechselbezug | 8 | dichter PU-Viskoschaum 4 mm, perforiert, Funktionsstoff, Klett nur am Bezug | Kauf, Schere | 40 € |
| BOA-Drehverschluss mit 2 Seilbändern | 8 | BOA Fit System (Orthesen-Variante) | Kauf | 120–200 € |
| Längsschlitten | 8 | 2 × Edelstahlstange Ø 5, 2 × iglidur JSM-0506-05, Alu-Schlitten (Laser), 2 Zentrierfedern ≈ 0,5 N/mm, ±12 mm | Kauf + Laser | 50 € |
| Wägezellen 20 kg | 8 | Biegebalken YZC-133 | Kauf | 40 € |
| Wägezellen-Halter, Überlastpuffer | 8 Satz | PETG/TPU | Druck | 5 € |
| Schlaufenkopf | 2 | Alu 15 × 40 Flachprofil + Laserlaschen, TPU-Zahnleiste | Säge + Bohren, Laser, Druck | 20 € |
| Schlaufen | 3 | Gummischlauch 12 mm, Federstahldraht 1,5 mm, Dyneema 4 mm | Kauf, zusammenstecken | 20 € |
| Schnappschäkel mit Zuglasche | 3 | Edelstahl | Kauf | 30 € |
| Schlaufenantrieb | 3 | Schneckengetriebemotor 5840-31ZY 24 V 10 U/min (45 kg·cm) + Stachelrad r 16 mm | Kauf | 75 € |
| Daumentaster-Griffe | 2 | PETG + IP67-Taster, Heizfolie | Druck + Kauf | 40 € |

## 4. Elektronik

| Teil | Menge | Herstellweg | etwa |
|---|---|---|---|
| Teensy 4.1 + 2 CAN-Transceiver (zwei Busse) | 1 | Kauf | 55 € |
| Manschetten-Messplatine: 3 × TI ADS131M04, 8,192-MHz-Oszillator, 3,3-V-Brückenspeisung, 12 Stecker | 1 | Platine fertigen lassen + bestücken | 60 € |
| ESP32-C3 (Funk zum Handy) | 1 | Kauf | 8 € |
| Lagesensor (IMU, Adafruit BNO085 #4754) | 1 | Kauf | 30 € |
| Wägezellen 50 kg (Schlaufenköpfe) + HX711 | 3 + 3 | Kauf | 20 € |
| Fußkontakt: Halbbrücken-Wägezellen 50 kg (34 × 34 mm) + HX711 | 8 + 2 | Kauf | 20 € |
| Knopfzelle CR2032 + Halter (Uhr) | 1 | Kauf | 3 € |
| DC/DC 36→24 V, 36→5 V | 2 | Kauf | 30 € |
| Motortreiber für Winden (Pololu G2 18v17, 44,95 $) | 3 | Kauf | 125 € |
| Hauptsicherung, Schütz, Pilz-Not-Aus | 1 Satz | Kauf | 50 € |
| Stihl-Akku-Adapter | 1 | Druck oder Kauf | 20–80 € |
| Stecker IP67, Kabel, Gewebeschlauch | 1 Satz | Kauf, crimpen | 60 € |
| Platine | 1 | Lochraster oder bestellt | 10 € |
| LED-Ring, Vibrationsmotor, Summer | 1 Satz | Kauf | 15 € |
| Anzeige rechter Oberarm: E-Paper 1,54" + Gehäuse mit Scheibe | 1 | Kauf + Druck | 20 € |
| LED-Arbeitsstrahler 10 W | 2 | Kauf | 30 € |

## 5. Komfort und Finish

| Teil | Menge | Herstellweg | etwa |
|---|---|---|---|
| Parkstütze, abnehmbar (Alu 20 × 20, Steckaufnahme mit Kugelsperrbolzen, Gummifüße): bleibt am Fahrzeug | 2 | Säge + Bohren, Kauf | 40 € |
| Schutzbügel CFK-Rundrohr Ø 22 × 1,5, Alu-Eckverbinder geklebt, Gummipuffer | 1 Satz | Kauf + Kleben | 45 € |
| Magnet-Schnallen | 3 | Kauf | 40 € |
| Kugelsperrbolzen für Transport | 6 | Kauf | 30 € |
| Haube über der Elektronikbox | 1 Satz | Druck (2 Teile, je < 18 cm) | 10 € |
| Schutzhauben Cordura | 1 Satz | Kauf/Nähen lassen | 30 € |
| Eloxieren oder Pulverbeschichten | 1 Auftrag | Dienst | 60–150 € |
| Lendengelenk: Scharnierlaschen, Rastsegment, Elastomerfeder, Bolzen Ø 10 | 1 Satz | Laser + Kauf | 35 € |
| Rumpfdrehung: Edelstahlwelle Ø 10 auf R 95 mm gebogen (Biegedienst), 4 Profilrollen INA LFR 5201-10-2Z, Wagen und 2 Halter (Laser), 2 Zentrierfedern, TPU-Anschläge | 1 Satz | Kauf + Laser | 70 € |
| Gasfedern 150 N, Ø 15/6, Hub 80 mm, L 236 mm, Kugelpfannen M6, Halter | 2 + 4 | Kauf + Laser | 30 € |
| Hubmagnet Intertec ITS-LZ-3263-D-24VDC + Sperrbolzen Ø 8 (Lendensperre) | 1 | Kauf | 40 € |
| Kurbelschutz Knie und Ellbogen | 4 | Druck PETG transparent | 5 € |
| Drehteller Steigbügel: Axial-Nadellager AXK 3047, 2 Laufscheiben, Zentrierfeder | 2 Satz | Kauf + Laser | 25 € |
| Drehring Unterarm (Dünnring- oder Kunststoff-Kugellager Ø 80–90) | 2 | Kauf | 30 € |
| Schulterschlitten: Wellen Ø 8 h6, Linearlager LM8UU, Druckfeder | 2 Satz | Kauf + Laser | 25 € |

---

## 6. Prüfung: passt jedes Druckteil in 18 × 18 × 18 cm?

| Druckteil | Größte Abmessung | passt? |
|---|---|---|
| Oberschenkel-Manschette | etwa 16 × 9 × 8 cm | ja |
| Unterschenkel-Manschette | etwa 12 × 8 × 7 cm | ja |
| Arm-Manschetten | etwa 11 × 7 × 6 cm | ja |
| Haube Antriebsbox (31 × 15 cm) | in 2 Teile geteilt, je 16 × 15 × 5 cm | ja, verschraubt |
| Box-Wände (36 × 29 cm) | in 4 Teile geteilt oder Alu-Blech vom Laser | ja |
| Akku-Adapter | etwa 19 × 9 × 4 cm | knapp: diagonal legen oder teilen |
| Daumentaster-Griff | etwa 6 × 4 × 4 cm | ja |
| Bohrschablonen | bis 18 cm, längere Schienen in Etappen | ja |

---

## 7. Werkzeug, das du brauchst

| Werkzeug | Wofür | etwa (falls nicht vorhanden) |
|---|---|---|
| Bohrständer + HSS-Bohrer | alle Bohrungen in Rohren | 100–200 € |
| Kappsäge mit Alu-Blatt oder Handbügelsäge + Gehrungslade | Profile ablängen | 30–150 € |
| Feilen, Entgrater | Kanten runden: macht viel vom „hochwertig“ aus | 25 € |
| Gewindeschneider M5–M8 | einzelne Gewinde | 25 € |
| Drehmomentschlüssel klein | Lagerschrauben gleichmäßig anziehen | 40 € |
| Crimpzange, Lötkolben, Multimeter | Elektronik | 60 € |
| 3D-Drucker | vorhanden (A1 mini) | 0 € |

**Nicht nötig:** Drehbank, Fräse, Schweißgerät, CNC.
