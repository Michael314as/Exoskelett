# Steuerung: so merkt das Exo, was du willst (Entwurf 4)

Das wichtigste Gefühl beim Tragen: **Du bewegst dich, das Exo folgt, und schwere Lasten werden leichter.** Kein
Joystick, keine Knöpfe zum Bewegen. Dafür braucht das Exo drei Arten von Information:

| Frage | Sensor | Wofür |
|---|---|---|
| Wohin willst du? | **Sensor-Manschetten** (Wägezellen) an Oberarm, Unterarm, Oberschenkel | Bewegung: Das Exo folgt deinem Druck |
| Was muss es tragen? | **Wägezellen in den Schlaufenköpfen**, Gelenkwinkel, Rumpfneigung | Last: Das Exo trägt seinen Anteil von selbst |
| Welches Bein steht? | **Wägezellen im Steigbügel** | Standbein trägt, Schwungbein läuft frei |

Die Antwort darauf rechnet der Teensy 200-mal pro Sekunde und schickt jedem der 8 Motoren ein Moment.

---

## 1. Die Sensor-Manschette

```
   Arm / Bein (Polster)
        │
   ┌────┴──────┐   blaue PETG-Schale, umschließt das Glied vorne, Klettgurt hinten
   │  Schale   │
   └────┬──────┘
        │  Steg
   ╔════╧════╗    Wägezelle (Biegebalken 20 kg), misst nur quer zur Schiene
   ╚════╤════╝    zwei TPU-Puffer mit 1 mm Spalt: Überlastschutz
        │
   Halter an der Schiene
```

- **Gemessen wird nur die Kraft quer zur Schiene**, in der Bewegungsrichtung des Gelenks. Gurtzug längs und Verdrehen
  sieht die Zelle nicht.
- **Straff, dünn gepolstert:** Jeder Millimeter Weichteil zwischen dir und der Zelle macht die Regelung schwammiger.
  Polster 5–6 mm, Schale eng.
- **Wo die Manschetten sitzen:** Oberarm (Schulter), Unterarm kurz vor dem Handgelenk (Ellbogen), Oberschenkel über dem
  Knie (Hüfte und Knie). Am Unterschenkel sitzt nur eine einfache Schale **ohne** Sensor: Dort misst eine Zelle beim
  Gehen hauptsächlich das Schwingen des Exo-Beins, nicht deine Absicht.
- **Die Baumlast geht nicht durch die Manschetten**, sondern über die Schlaufen direkt in die Unterarmschiene. Sonst
  würde das Exo jeden Baum als „du willst nach unten“ deuten.

## 2. Das Regelgesetz

Für jedes Gelenk:

```
Moment = Schwerkraft der Exo-Schiene                    (das Exo trägt sich selbst)
       + Vorsteuerung: Baumlast, Körpergewicht im Stand  (das Exo trägt seinen Anteil, ohne dass du drückst)
       + K · Druck an den Manschetten · Hebel            (das Exo folgt dir, verstärkt)
       + Reibungsausgleich − kleine Dämpfung             (leichtgängig, aber ruhig)
```

**a) Druck → Moment, für alle Gelenke, die ihn bewegen können.** Eine Kraft an der Unterarm-Manschette wirkt
physikalisch an Ellbogen **und** Schulter. Die Steuerung rechnet sie deshalb über die Hebel auf beide Gelenke um
(Hebel an der Schulter = Abstand Manschette zum Ellbogen + Oberarmlänge × cos Ellbogenwinkel). Ergebnis: Hebst du den
ganzen Arm nur über den Unterarm, hilft die Schulter mit; vorher hätte nur der Ellbogen reagiert. Gleich am Bein:
Druck am Oberschenkel geht an die Hüfte und mit 60 % ans Knie, weil Aufstehen und Hinsetzen immer beide Gelenke sind.

**b) Vorsteuerung aus der Baumlast.** Jeder Schlaufenkopf sitzt auf einer kleinen Wägezelle. Die Steuerung kennt damit
das Gewicht am Handgelenk und die Armhaltung und gibt Schulter und Ellbogen das Moment, das diese Last braucht, mal einem
Anteil je Stufe (sanft 50 %, normal 67 %, kräftig 80 %). Den Rest spürst du: So bleibt das Gefühl für den Baum
erhalten, und das Exo kann nie „mehr wollen“ als die Last.

**c) Vorsteuerung im Stand (Beine).** Am belasteten Bein tragen Hüfte und Knie automatisch:
Exo-Oberteil (22,1 kg laut Bauteilliste der technischen Ansicht; vorher mit 11 kg viel zu knapp angesetzt) + 10/20/30 % deines Oberkörpers je Stufe + der Lastanteil aus den Schlaufen. Die Hüfte gleicht
die Vorneigung des Rumpfs aus (Lagesensor), das Knie die Beugung. Stehen beide Füße, teilen sich die Beine die Last.

**c2) Der Rucksack zieht nach hinten.** Der Schwerpunkt des Exo-Oberteils liegt 22 cm über und 8 cm **hinter** der
Hüftachse (Anzeige „Exo-Oberteil“ in der technischen Ansicht). Aufrecht will er nach hinten kippen; ohne Ausgleich zögen
dich die Schultergurte zurück. Die Hüft-Vorsteuerung rechnet Mensch und Exo deshalb getrennt: aufrecht halten die
Hüftmotoren je Seite ≈ 8 Nm gegen das Kippen nach hinten, vorgebeugt (35°) tragen sie ≈ 14 Nm nach oben. So spürst du den
Rucksack weder aufrecht noch beim Bücken.

**d) Standbein und Schwungbein.** Die Fußlast aus den Wägezellen im Steigbügel entscheidet (geglättet über 80 ms, damit beim Aufsetzen
nichts springt):

| | Standbein | Schwungbein |
|---|---|---|
| Gewicht tragen (Vorsteuerung) | ja | nein |
| Schwerkraft der Exo-Beinschiene ausgleichen | nein (der Boden trägt) | ja |
| Folgen bei Druck am Oberschenkel | strecken voll, beugen kaum | beide Richtungen |
| Knie aktiv beugen | **nie** | bis 10 Nm |
| Hüfte aktiv beugen | höchstens 5 Nm | bis 35 Nm |

So läuft das Bein beim Gehen frei mit, hilft beim Anheben aus Matsch und Schnee, und am belasteten Bein kann das Exo
das Knie nie einknicken lassen. Zusätzlich begrenzt die **Federpatrone** im Knie-Beugezug die Beugekraft mechanisch auf
etwa 11 Nm, auch wenn die Software versagt.

**e) Feinheiten, die das Gefühl ausmachen** (in der Simulation getestet, [firmware/README.md](../firmware/README.md)):

| Baustein | Wert | Warum |
|---|---|---|
| Totband | 2 N | Rauschen (≈ 0,6 N) löst nichts aus; darüber setzt die Hilfe stetig ein, ohne Sprung |
| Tiefpass | 8 Hz | ruhig, aber schnell genug für Armbewegungen |
| Reibungsausgleich | 0,25 Nm (Knie 0,4) | Lager und Bowdenzüge verschwinden fast; weich um den Stillstand, damit nichts zittert |
| Dämpfung | 0,3 Nm·s/rad | klein: mehr machte das Exo in der Simulation zäh |
| Momentänderung | höchstens 400–600 Nm/s | nichts ruckt |
| Weiche Wände | 0,15 rad vor jedem Anschlag | Hilfe läuft vor dem Anschlag aus |
| Geschwindigkeitsgrenze | 4 rad/s | schnelle Bewegung wird nicht weiter verstärkt |
| Aufwachen | 2 s nullen, 3 s Rampe | nichts springt beim Einschalten |
| Verstärkung K | 0,5 / 1 / 2 (Stufen) | stabil bis K ≈ 5,5 mit HX711: große Reserve |
| Reaktionszeit | ≈ 35 ms (HX711 mit 80 Messungen/s, Filter, CAN) | nach Haptik-Studien fühlt sich eine Last bei 25–50 ms Verzögerung 10–20 % schwerer an ([schaetzwerte.md](schaetzwerte.md)); Upgrade: ADS1220 (2 Kanäle je Chip, 5 Chips am SPI-Bus mit eigenem CS-Pin) senkt sie auf ≈ 10 ms |

## 3. Weitere Sensoren

- **Motor-Encoder:** Gelenkwinkel und -geschwindigkeit (eingebaut).
- **Lagesensor am Rücken (BNO085):** Vorneigung des Rumpfs für die Hüfte, Kippgeschwindigkeit für die Sturzerkennung.
- **Schlaufenwinden:** Motorstrom = Zuziehkraft, Endschalter „ganz offen“.

## 4. Bedienung

| Element | Ort | Funktion |
|---|---|---|
| Ein/Aus | linker Schultergurt | kurz: ein/aus. Doppeltipp: Arbeitslicht |
| Not-Aus (Pilz) mit LED-Ring | linker Schultergurt | trennt Motoren hart vom Akku; Ring zeigt Zustand und Akku |
| Daumentaster: rechts 2, links 1 | an der Hand | je Schlaufe: kurz = zu, lang = auf. Beide rechten 2 s: Stufe wechseln |
| Anzeige | rechter Oberarm außen | Uhrzeit, Akku mit Restzeit, Stufe, Fehler im Klartext |
| Vibration | Box | bestätigt Schlaufe und Stufenwechsel |

## 5. Zustände

```
AUS ──Ein──▶ NULLEN (2 s ruhig stehen) ──▶ AKTIV (3 s sanft hochfahren) ──Aus──▶ AUSLAUF ──▶ AUS
                                              │
Not-Aus, Sensor- oder Motorfehler, Akku leer ─┴──▶ FEHLER: Gelenkmotoren stromlos, Schlaufen halten, Ring rot
Sturz erkannt ──▶ AUSLAUF in 0,1 s     Parkstütze unten ──▶ keine Unterstützung, Ring gelb
```

## 6. Elektronik

```
Stihl AP 500 S (36 V) ── Hauptsicherung ── Not-Aus ── Schütz ──┬── 8 Gelenkmotoren (CAN-Bus): 2 an den Hüften, 6 in der Box
                                                               ├── DC/DC 24 V ── 3 Motortreiber ── 3 Schlaufenwinden
                                                               └── DC/DC 5 V ── Steuerung, Display
Teensy 4.1 + CAN-Transceiver + Knopfzelle (Uhr)
   ├── 9 Wägezellen-Verstärker (6 Manschetten, 3 Schlaufenköpfe)
   ├── 2 × 4 Wägezellen in den Steigbügeln (je ein HX711)
   ├── Lagesensor am Rücken
   ├── Taster, LED-Ring, Vibration, Licht
   ├── Statuszeile → ESP32-C3 mit E-Paper am rechten Oberarm (und Bluetooth zum Handy)
   └── Watchdog: hängt die Software, fällt das Schütz ab
```

Pinplan: [verdrahtung.md](verdrahtung.md).

## 6a. Schutzfunktionen (Entwurf 4)

| Funktion | Auslöser | Was passiert |
|---|---|---|
| **Wärmeschutz** | Wärmemodell je Motor: Strom² über die Zeit (Zeitkonstante 3 min) | ab 100 % Wärme sinkt die Momentgrenze stufenlos bis auf 40 %; das Exo wird schwächer statt abzuschalten, Anzeige „Motor warm“ |
| **Sturzdämpfung** | Sturz erkannt (Kippgeschwindigkeit) | Arme stromlos, Beine 1,5 s mit Dämpfung (−4 Nm·s/rad): das Exo sackt langsam statt zu treten oder zu klappen |
| **Lendensperre** | > 5 kg in den Schlaufen für 0,3 s | Hubmagnet (Pin 13) sperrt das Lendengelenk, der Rahmen trägt den Baum; nach dem Absetzen frei |
| **Rückneigung** | Rumpf mehr als 9° nach hinten | Vibrationspulse: „lehn dich nach vorne“, bevor der Baum dich umzieht |
| **Winden nacheinander** | mehrere Schlaufen gleichzeitig gedrückt | eine Winde nach der anderen (24-V-Wandler 2,5 A), Zeitgrenze 12 s je Bewegung |

## 7. Was schiefgehen kann, und was dagegen hilft

| Problem | Ursache | Abhilfe |
|---|---|---|
| Exo zittert oder schaukelt | K zu hoch, Polster zu weich | Stufe senken, Manschetten straffer, dünner gepolstert |
| Exo „läuft weg“ | Nullpunkt verschoben (Kälte, Gurt nachgezogen) | Nullen bei jedem Einschalten |
| Baum fühlt sich zu leicht oder zu schwer an | Lastanteil der Stufe | Stufe wechseln (beide rechten Taster 2 s) |
| Bein streckt beim Gehen | Fußsensor meldet falsch | Schwelle am Prüfstand einstellen, Sensor prüfen |
| Bein knickt beim Aufsetzen | Standbein zu spät erkannt | Glättung verkürzen; Federpatrone begrenzt ohnehin |
| Schulter hilft beim Hebeln zu viel | Kopplung vom Unterarm | Hebel im Abgleich prüfen (Oberarmlänge) |
| Unterstützung wird nach langem Halten schwächer | Wärmeschutz | gewollt: Last kurz absetzen, Motoren kühlen in wenigen Minuten ab |
| Rumpf lässt sich nicht beugen | Lendensperre aktiv (Last in den Schlaufen) | Baum absetzen; Hebel prüfen |

## 8. So baut man es auf

1. **Am Tisch:** eine Wägezelle an den Teensy, mit Gewichten kalibrieren, Rauschen anschauen.
2. **Prüfstand Ellbogen:** Holzarm, Seilscheibe, Bowdenzug-Paar, ein Motor, eine Manschette ([montage.md](montage.md), Abschnitt 6).
3. **Am Körper:** erst ein Arm, dann Schulter, dann Hüfte im Stand (Fußsensor), dann Knie, zuletzt Gehen.
4. **Unter Last:** Sandsack in den Schlaufen (prüft die Last-Vorsteuerung), dann leichte, dann schwerere Bäume.

## 9. Firmware

Umgesetzt und in der Simulation getestet: [firmware/README.md](../firmware/README.md). Die Kopplung der Gelenke, die
Vorsteuerung und die Standbein-Logik stehen in `firmware/src/exo_app.h`, der Gelenkregler in `firmware/src/exo_core.h`.
