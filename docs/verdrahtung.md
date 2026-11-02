# Verdrahtung (Entwurf 4)

Muss mit den `PIN_*`-Konstanten in [`firmware/src/main_teensy.cpp`](../firmware/src/main_teensy.cpp) übereinstimmen.

## Leistung

```
Stihl AP 500 S (36 V) ── Adapter ── Hauptsicherung 40 A (ATO/Midi) ── Not-Aus-Pilz (Öffner) ──┬── Schütz (Spule über Teensy Pin 31 + Treiber)
                                                                                              │      └── 8 Motoren (Sternverteilung, je Abgang 15-A-Sicherung; 2 davon an den Hüften)
                                                                                              ├── DC/DC 36→24 V 5 A ── 3 Windentreiber, LED-Strahler (über 12-V-Regler)
                                                                                              └── DC/DC 36→5 V 3 A ── Teensy, ESP32-C3 + E-Paper (rechter Oberarm), HX711, IMU, LED-Ring
Not-Aus-Rückmeldung: zweiter Kontakt des Pilzes → Pin 30 (offen = Not-Aus gedrückt)
```

| Leitung | Querschnitt | Hinweis |
|---|---|---|
| Akku → Hauptsicherung → Not-Aus → Schütz | 4 mm² | so kurz wie möglich, Kabelschuhe gecrimpt |
| Schütz → Motoren (je Motor) | 1,5 mm² | XT30-Stecker oder Motor-Originalstecker |
| CAN-Bus | verdrillt, geschirmt | 120-Ω-Abschluss am Teensy und am letzten Motor |
| Wägezellen | 4-adrig geschirmt | Schirm nur am Teensy-Ende auf Masse |
| Hüftmotoren | 1,5 mm² + CAN in einem Kabel | über die Hüftschwinge, mit Schlaufe am Abspreizgelenk (±20°) |
| Fußkontakt (4 Wägezellen + HX711 je Fuß) | 4-adrig geschirmt, HX711 vergossen im Steigbügel | am Unterschenkel entlang, Stecker am Knie |
| Display | 4-adrig (5 V, Masse, TX, frei) | im Gewebeschlauch über die Schulter zum rechten Oberarm |
| Steuerung 5 V | 0,5 mm² | eigene Sicherung 2 A |

**Spannungsmessung Akku:** Teiler 140 kΩ / 10 kΩ an Pin 17 (Faktor 15), 100 nF gegen Masse.
**Schütz-Ansteuerung:** Pin 31 über Logic-Level-MOSFET mit Freilaufdiode. Beim Reset ist der Pin hochohmig, ein
Pulldown 10 kΩ hält das Schütz dann sicher offen.

## Teensy 4.1

| Pin | Funktion | Pin | Funktion |
|---|---|---|---|
| 0 / 1 | Serial1 RX / TX zum ESP32-C3 (Display, Handy) | 21 | Vibrationsmotor (MOSFET) |
| 2 | Ein/Aus-Taster (gegen Masse) | 22 / 23 | CAN1 TX / RX → Transceiver |
| 3 / 4 / 5 | Daumentaster Schlaufe R1 / R2 / L1 | 24 / 25 / 26 | Winde R1 / R2 / L1 Richtung |
| 6 / 7 / 8 | Winde R1 / R2 / L1 PWM | 27 / 28 / 29 | Winde R1 / R2 / L1 Endschalter offen |
| 9 | HX711 Daten Zelle 0 (Oberschenkel L) | 30 | Not-Aus-Rückmeldung |
| 10 / 11 / 12 | HX711 Daten Zellen 6 / 7 / 8 (Schlaufen R1 / R2 / L1) | 31 | Schütz |
| 13 | Hubmagnet Lendensperre ITS-LZ-3263-D-24VDC (Logic-MOSFET, Freilaufdiode; PWM: 0,2 s voll, dann 35 % halten; stromlos = frei) | 38 / 39 | HX711 Fußkontakt links / rechts (DOUT, Takt gemeinsam an 34), je 4 Halbbrücken-Zellen 50 kg |
| 14 / 15 / 16 | Windenstrom R1 / R2 / L1 (0,5 V/A) | 32 | Parkstütze unten (Endschalter) |
| 17 | Akkuspannung (Teiler 1:15) | 33 | Arbeitslicht (MOSFET) |
| 18 / 19 | I2C SDA / SCL → BNO085 | 34 | HX711 Takt (alle 9) |
| 20 | LED-Ring Daten (WS2812, 330 Ω) | 35 / 36 / 37 / 40 / 41 | HX711 Daten Zellen 1–5 |
| VBAT | Knopfzelle CR2032 für die Uhr | | |

Zellen: 0/1 Oberschenkel L/R, 2/3 Oberarm L/R, 4/5 Unterarm L/R, 6/7/8 Schlaufenkopf R1 (unten) / R2 (außen) / L1. Uhr stellen: Konsole `uhr 14 32`.

## Motoren (CAN-IDs)

| ID | Motor | Übersetzung Gelenk : Motor |
|---|---|---|
| 1 / 2 | Hüfte links / rechts (Hüftkapsel RS04, direkt) | 1 : 1 |
| 3 / 4 | Knie links / rechts (Kniekapsel RS04, Schubstange) | 1 : 1 |
| 5 / 6 | Schulter links / rechts (Schulterkapsel RS04, direkt) | 1 : 1 |
| 7 / 8 | Ellbogen links / rechts (Ellbogenkapsel RS03, Schubstange) | 1 : 1 |

IDs vor dem Einbau einzeln mit dem Hersteller-Tool setzen (CAN-USB-Adapter, RobStride-Software). Danach im Stand prüfen: Jeder Motor antwortet.

**Zusätzliche Ausgänge (Entwurf 5):** Pin 42 Heizfolien der Beinkapseln, Pin 43 Heizfolien der Armkapseln, je über einen Logic-MOSFET aus dem 12-V-Wandler (Lötpunkte auf der Unterseite des Teensy 4.1; die SD-Karte wird nicht benutzt).
**Bremswiderstand:** zwischen Akku-Plus und Minus hinter dem Schütz, eigene Schaltung (Komparator + MOSFET, ein ab 46 V, aus ab 44 V), 10 Ω / 200 W am Rahmen gekühlt; arbeitet unabhängig von der Software.
