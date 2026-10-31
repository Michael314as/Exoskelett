# Verdrahtung (Entwurf 3.2)

Muss mit den `PIN_*`-Konstanten in [`firmware/src/main_teensy.cpp`](../firmware/src/main_teensy.cpp) übereinstimmen.

## Leistung

```
Stihl AP 500 S (36 V) ── Adapter ── Hauptsicherung 40 A (ATO/Midi) ── Not-Aus-Pilz (Öffner) ──┬── Schütz (Spule über Teensy Pin 31 + Treiber)
                                                                                              │      └── 6 Motoren (Sternverteilung, je Abgang 15-A-Sicherung)
                                                                                              ├── DC/DC 36→24 V 5 A ── 3 Windentreiber, LED-Strahler (über 12-V-Regler)
                                                                                              └── DC/DC 36→5 V 3 A ── Teensy, ESP32-C3, HX711, IMU, LED-Ring
Not-Aus-Rückmeldung: zweiter Kontakt des Pilzes → Pin 30 (offen = Not-Aus gedrückt)
```

| Leitung | Querschnitt | Hinweis |
|---|---|---|
| Akku → Hauptsicherung → Not-Aus → Schütz | 4 mm² | so kurz wie möglich, Kabelschuhe gecrimpt |
| Schütz → Motoren (je Motor) | 1,5 mm² | XT30-Stecker oder Motor-Originalstecker |
| CAN-Bus | verdrillt, geschirmt | 120-Ω-Abschluss am Teensy und am letzten Motor |
| Wägezellen | 4-adrig geschirmt | Schirm nur am Teensy-Ende auf Masse |
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
| 10–13 | frei (SPI, Erweiterung) | 31 | Schütz |
| 14 / 15 / 16 | Windenstrom R1 / R2 / L1 (0,5 V/A) | 32 | Parkstütze unten (Endschalter) |
| 17 | Akkuspannung (Teiler 1:15) | 33 | Arbeitslicht (MOSFET) |
| 18 / 19 | I2C SDA / SCL → BNO085 | 34 | HX711 Takt (alle 8) |
| 20 | LED-Ring Daten (WS2812, 330 Ω) | 35–41 | HX711 Daten Zellen 1–7 |

Zellen: 0/1 Oberschenkel L/R, 2/3 Unterschenkel L/R, 4/5 Oberarm L/R, 6/7 Unterarm L/R.

## Motoren (CAN-IDs)

| ID | Motor | Übersetzung Gelenk : Motor |
|---|---|---|
| 1 / 2 | Bein links / rechts | 6,3 (Hüftscheibe Ø 19 / Trommel Ø 3) |
| 3 / 4 | Schulter links / rechts | 4,67 |
| 5 / 6 | Ellbogen links / rechts | 3,33 |

IDs vor dem Einbau einzeln mit dem Hersteller-Tool setzen. Danach im Stand prüfen: Jeder Motor antwortet.
