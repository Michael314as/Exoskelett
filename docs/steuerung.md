# Steuerung: so merkt das Exo, was du willst (Entwurf 5, Manschetten M2)

Das wichtigste Gefühl beim Tragen: **Du bewegst dich, das Exo folgt, und schwere Lasten werden leichter.** Kein
Joystick, keine Knöpfe zum Bewegen. Dafür braucht das Exo drei Arten von Information:

| Frage | Sensor | Wofür |
|---|---|---|
| Wohin willst du? | **Sensor-Manschetten** (Wägezellen) an Oberarm, Unterarm, Oberschenkel, Schienbein; 4 Rumpf-Wägezellen | Bewegung: Das Exo geht mit deinem Druck mit |
| Was muss es tragen? | **Wägezellen in den Schlaufenköpfen**, Gelenkwinkel, Rumpfneigung | Last: Das Exo trägt seinen Anteil von selbst |
| Welches Bein steht? | **Wägezellen im Steigbügel** | Standbein trägt, Schwungbein läuft frei |

Die Antwort darauf rechnet der Teensy 500-mal pro Sekunde und schickt jedem der 10 Motoren ein Moment, eine
Sollgeschwindigkeit und eine Dämpfung. Die Dämpfung auf die Sollgeschwindigkeit rechnet der Motor selbst, ohne Umweg über
den Bus.

> **Rev. M2 – warum neu?** Mit der bisherigen Regelung (Druck × Verstärkung = Moment, HX711 mit 80 Messungen/s) hättest du
> gegen die Trägheit der Exo-Glieder anschieben müssen: In der Simulation bis 12 N an Schulter und Knie bei zügiger Bewegung.
> Das fühlt sich an wie ein Arm im Wasser. Ziel ist, dass du dich **spielend leicht** bewegst und das Exo mitgeht.
> Drei Änderungen schaffen das: schnelle Messung (Abschnitt 1), **Mitgehen** statt nur Verstärken (Abschnitt 2f) und
> eine straffe, mitgleitende Manschette (Abschnitt 1). Ergebnis in der Simulation: **2,5–4 N** an jeder Manschette, egal ob
> sie straff über dem Pulli oder weich über der Winterjacke sitzt.

---

## 1. Die Sensor-Manschette (M2)

```
   Arm / Bein
        │  Viskopolster 4 mm (dicht, perforiert, Wechselbezug)
   ┌────┴────────────┐   PETG-CF-Schale 4 mm, 45 % breiter als vorher, umschließt 210°
   │     Schale      │   hinten 2 Bänder mit BOA-Drehverschluss (statt Klett)
   └────┬────────────┘
        │  Steg
   ═════╪═════  Längsschlitten: 2 Stangen Ø 5 längs zum Glied, iglidur-Buchsen, Zentrierfedern, ±12 mm
        │
   ╔════╧════╗    Wägezelle (Biegebalken 20 kg), misst nur quer zur Schiene
   ╚════╤════╝    zwei TPU-Puffer mit 1 mm Spalt: Überlastschutz
        │
   Halter an der Schiene          → geschirmtes Kabel → ADS131M04 in der Elektronikbox (1000 Messungen/s)
```

| Was | Vorher (bis Rev. T2) | Jetzt (M2) | Warum |
|---|---|---|---|
| Messung | HX711 an jeder Zelle, 80/s, Filter 8 Hz, ≈ 35 ms Verzug | **3 × ADS131M04** (je 4 Kanäle gleichzeitig), 1000/s, Filter 40 Hz, ≈ 3 ms | Mitgehen braucht frische Kraftwerte; bei 35 ms läuft das Exo deiner Bewegung hinterher |
| Verschluss | Klettgurt | **BOA-Drehverschluss**, 2 Bänder | gleichmäßig straff in feinen Rasten, mit Handschuhen bedienbar, Ziehen = sofort auf; Klett gibt nach, sammelt Schnee und vereist |
| Schale | 5 mm PETG, schmal, 6 mm Polster | **4 mm PETG-CF, 45 % breiter, 4 mm dichtes Viskopolster**, 3 Größen | breiter = weniger Druck je cm² bei gleicher Kraft; dünn und dicht = kaum Nachgeben zwischen dir und der Zelle |
| Längsrichtung | starr | **Längsschlitten ±12 mm** | Die Achsen von Mensch und Exo liegen nie genau übereinander (± 1–2 cm). Beim Beugen wandert die Manschette sonst über die Haut (Scheuern, Druckstellen) und drückt längs auf die Zelle; das las die Steuerung als „du willst“ |
| Schienbein | Schale ohne Sensor | **Sensor-Manschette** | Das Knie geht jetzt selbst mit. Früher hätte eine Zelle dort vor allem das Schwingen des Exo-Beins gemessen; mit Mitgehen ist genau diese Kraft das, was die Regelung klein hält |
| Unterarm | Drehring | bleibt | Hand drehen, ohne gegen das Exo zu arbeiten |

- **Gemessen wird nur die Kraft quer zur Schiene**, in der Bewegungsrichtung des Gelenks. Längs nimmt der Schlitten,
  Verdrehen der Drehring bzw. die Schale.
- **Straff, aber nicht eng:** Die Regelung ist für weiche (2 kN/m, Winterjacke) bis straffe (15 kN/m, dünner Pulli)
  Ankopplung ausgelegt und getestet. Über der Jacke wirkt sie nur etwas weicher; schwingen kann sie nicht (Schwingungswächter,
  Abschnitt 2f).
- **Wo die Manschetten sitzen:** Oberarm (Schulter), Unterarm kurz vor dem Handgelenk (Ellbogen), Oberschenkel über dem
  Knie (Hüfte, mit 60 % auch Knie), Schienbein unter dem Knie (Knie). Dazu 4 Rumpf-Wägezellen (Gurtanker, Rückenpolster).
- **Die Baumlast geht nicht durch die Manschetten**, sondern über die Schlaufen direkt in die Unterarmschiene. Sonst
  würde das Exo jeden Baum als „du willst nach unten“ deuten.
- **Nullen:** Beim Einschalten (2 s ruhig stehen) nullt das Exo; die Zellen kalibriert man in der Werkstatt
  (Konsole `null` / `kal`, Zellen 0–16).

## 2. Das Regelgesetz

Für jedes Gelenk:

```
Moment = Schwerkraft der Exo-Schiene                    (das Exo trägt sich selbst)
       + Vorsteuerung: Baumlast, Körpergewicht im Stand  (das Exo trägt seinen Anteil, ohne dass du drückst)
       + K · Druck an den Manschetten · Hebel            (das Exo folgt dir, verstärkt)
       + Reibungsausgleich (nach der Sollgeschwindigkeit)
       + Kd · (Sollgeschwindigkeit − Geschwindigkeit)    (MITGEHEN: rechnet der Motor selbst, ≈ 10 kHz)
Sollgeschwindigkeit = admit · Druck an den Manschetten  (0,9 rad/s je N an den Gliedern, bis 8 rad/s)
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
Exo-Oberteil (29,5 kg laut Bauteilliste der technischen Ansicht; vorher mit 11 kg viel zu knapp angesetzt) + 10/20/30 % deines Oberkörpers je Stufe + der Lastanteil aus den Schlaufen. Die Hüfte gleicht
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

| Baustein | Wert (M2) | Warum |
|---|---|---|
| Regeltakt | 500 Hz, Befehl ≤ 1 ms auf dem Bus | darunter wird Mitgehen an Schulter und Knie unruhig (Studie [tools/transparenz.cpp](../tools/transparenz.cpp)) |
| Messung | 1000/s, Tiefpass 40 Hz, Totband 0,3 N | schnell genug fürs Mitgehen, Rauschen (≈ 0,05–0,15 N) löst nichts aus |
| Mitgehen (admit) | 0,9 rad/s je N (Rumpf beugen 0,25, drehen 0,35) | 3 N Druck = ≈ 2,7 rad/s: zügige Armbewegung mit einem Fingerdruck |
| Dämpfung im Motor (Kd) | Ellbogen 0,75 · Schulter 2,3 · Knie 2,9 · Hüfte 4 · Rumpf 10 / 7 Nm·s/rad | ≈ 0,75 × Trägheit des Exo-Glieds / 0,065 s; doppelt so viel geht noch, halb so viel auch (Simulation) |
| Reibungsausgleich | 0,1–0,2 Nm + 0,25–0,5 Nm·s/rad | nach der Sollgeschwindigkeit, nicht der gemessenen: zittert nicht im Stillstand |
| Schwingungswächter | Anteil > 6 Hz in der Kraft: über 2 N → Mitgehen schnell bis 25 % zurück, unter 1 N langsam wieder frei | fängt zu straffe Manschette, Busverzug, Kälte ab; Gehen (1–2 Hz) löst ihn nicht aus |
| Momentänderung | höchstens 300–600 Nm/s (Vorsteuer-Anteil) | nichts ruckt |
| Weiche Wände | 0,15 rad vor jedem Anschlag, quadratisch | Mitgehen und Verstärkung laufen vor dem Anschlag aus |
| Geschwindigkeitsgrenze | 8 rad/s an den Gliedern, 2 rad/s am Rumpf | zügiges Gehen und Armschwung erreichen 5–7 rad/s; mit der alten Grenze von 4 rad/s blieb das Exo bei schnellen Schritten zurück (Simulation: 21–23 N statt 5 N) |
| Aufwachen | 2 s nullen, 3 s Rampe | nichts springt beim Einschalten |
| Verstärkung K | 0,5 / 1 / 2 (Stufen) | wie bisher; dazu kommt beim Halten der Mitgeh-Anteil (siehe 2f) |
| Rückfallebene | admit = 0 → reiner Momentbetrieb wie bis Rev. T2 | stabil bis K = 5,5 auch mit langsamer Messung |

**f) Mitgehen (Admittanz), Rev. M2.** Bisher galt: Du drückst, das Exo gibt ein Moment dazu. Das Exo-Glied selbst
(Ellbogenkapsel, Schiene, Schubstange) musst du dabei trotzdem beschleunigen und abbremsen, und weil die Messung langsam
war, lief das Exo immer etwas hinterher. Jetzt rechnet die Steuerung aus deinem Druck eine **Geschwindigkeit**, mit der
das Gelenk mitgehen soll, und der Motor regelt diese Geschwindigkeit selbst mit seiner internen Dämpfung (RobStride
„Operation Control“: Moment, Sollgeschwindigkeit und Kd in einem Befehl). So reagiert er sofort auf sein eigenes
Geschwindigkeitssignal, ohne auf den Bus zu warten.

| Simulation (Firmware-Test, Manschette 2–15 kN/m) | bisher (T2) | Mitgehen (M2) |
|---|---|---|
| Ellbogen 0,5 Hz ±29° / 1,2 Hz ±20° | 3,0 / 4,9 N | **1,3 / 1,9 N** |
| Schulter 1,2 Hz ±20° | bis 12,2 N | **bis 2,7 N** |
| Knie (Schwungbein) 1,2 Hz ±20° | bis 12,8 N | **bis 3,0 N** |
| Schulter, schneller Schwung 1,4 Hz ±34° | 24 N | **4,8 N** |
| Knie, schneller Schwung 1,4 Hz ±34° | 26 N | **9,7 N** (Rest = Sicherheitsgrenze 10 Nm beim Knie-Beugen; ohne sie 4,9 N) |
| Trägheit und Reibung des Exo-Glieds halb oder doppelt so groß wie geschätzt | – | **bis 3,6 N (Schulter), 5,0 N (Knie)** |

Werte: Effektivwert der Kraft an der Manschette; 3 N ≈ das Gewicht von 300 g. Die Studie
([tools/transparenz.cpp](../tools/transparenz.cpp)) mit anderem Menschmodell kommt auf dieselbe Größenordnung. Störfälle (Dämpfung doppelt, Busverzug 2 ms, Rauschen 0,6 N, Reibung doppelt bei Kälte, Schwerkraft ±30 % falsch
geschätzt) bleiben unter 4 N, mit Schwingungswächter im schlimmsten Fall unter 11 N, nie aufschaukelnd.

**Beim Halten** (ruhig, Last am Arm) wirkt der Mitgeh-Anteil zusätzlich als Verstärkung: Motor = (K + Kd · admit / r) ×
dein Moment, aber nur bis zur Geschwindigkeitsgrenze (Kd × 8 rad/s, am Ellbogen 6 Nm). Kleine Lasten trägt das Exo
damit fast ganz, große wie bisher nach der Stufe K und der Baumlast-Vorsteuerung. Simulation Ellbogen, 20 Nm Last: du
trägst 40 / 30 / 20 % in den Stufen sanft / normal / kräftig (vorher 67 / 50 / 33 %).

**Knie-Beugen bleibt auf 10 Nm begrenzt** (Software und Federpaket 11 Nm), damit das Exo nie kräftig ins Knie tritt.
Bei sehr schnellen Schritten braucht der Exo-Unterschenkel allein etwa 12 Nm zum Beschleunigen; den Rest (≈ 10 N an der
Schienbein-Manschette) spürst du. Beim normalen Gehen im Wald liegt das weit darunter. Wer es leichter will, kann die
Grenze nach dem Prüfstand auf 15 Nm anheben (Federpaket entsprechend), das ist eine Sicherheitsabwägung.
**Entscheidung (3. November 2026):** 10 Nm bleiben. Die Simulation gilt für die Planung als Nachweis, der echte
Nachweis kommt mit P39 (Log mit `log 1` aufnehmen, auswerten, nachstellen).

**Was bewiesen ist und was nicht:** Die Zahlen stammen aus der Simulation (Firmware-Tests, `make test`) mit geschätzten
Trägheiten. Die Tests zeigen, dass es auch bei Faktor 2 falsch geschätzten Trägheiten, weicher oder straffer Manschette,
Busverzug und Kälte leicht und stabil bleibt. Wie es sich anfühlt, zeigt erst der Prüfstand (P39) und dann der Körper.

**Was das für dich heißt:** Arm heben, umgreifen, Bein über einen Ast steigen: Das Exo ist praktisch nicht zu spüren.
Wenn du stehen bleibst, bleibt es stehen (kein Nachlaufen: Sollgeschwindigkeit 0 = Motor bremst). Drückst du gegen
einen Anschlag, laufen Mitgehen und Verstärkung vorher weich aus.

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
| Vibration | Elektronikbox | bestätigt Schlaufe und Stufenwechsel |

## 5. Zustände

```
AUS ──Ein──▶ NULLEN (2 s ruhig stehen) ──▶ AKTIV (3 s sanft hochfahren) ──Aus──▶ AUSLAUF ──▶ AUS
                                              │
Not-Aus, Sensor- oder Motorfehler, Akku leer ─┴──▶ FEHLER: Gelenkmotoren stromlos, Schlaufen halten, Ring rot
Sturz erkannt ──▶ AUSLAUF in 0,1 s     Parkstütze unten ──▶ keine Unterstützung, Ring gelb
```

## 6. Elektronik

```
Stihl AP 500 S (36 V) ── Hauptsicherung ── Not-Aus ── Schütz ──┬── 10 Gelenkkapseln an 2 CAN-Bussen (CAN1: Hüften, Knie, Rumpf beugen; CAN3: Schultern, Ellbogen, Rumpf drehen)
                                                               │   RS04 an Hüften, Knien, Schultern, Rumpf beugen; RS03 an Ellbogen, Rumpf drehen; Bremswiderstand ab 46 V
                                                               ├── DC/DC 24 V ── 3 Motortreiber ── 3 Schlaufenwinden
                                                               └── DC/DC 5 V ── Steuerung, Display
Teensy 4.1 + 2 CAN-Transceiver + Knopfzelle (Uhr), Regeltakt 500 Hz
   ├── 3 × ADS131M04 an SPI0 (1000/s): 8 Manschetten (Oberschenkel, Schienbein, Oberarm, Unterarm) + 4 Rumpf-Wägezellen
   ├── 5 × HX711: 3 Schlaufenköpfe, 2 Steigbügel (je 4 Zellen)
   ├── Lagesensor am Rücken
   ├── Taster, LED-Ring, Vibration, Licht, Heizbänder
   ├── Statuszeile → ESP32-C3 mit E-Paper am rechten Oberarm (und Bluetooth zum Handy)
   └── Watchdog: hängt die Software, fällt das Schütz ab
```

Buslast: 5 Motoren × 500 Hz × (Befehl + Antwort) ≈ 5000 Rahmen/s × ≈ 140 Bit ≈ 70 % je Bus (1 Mbit/s). Wird es eng,
geht der Takt auf 400 Hz (Simulation: noch stabil) oder ein dritter Bus (CAN2, Pins 0/1, dann Display an Serial8).

Pinplan: [verdrahtung.md](verdrahtung.md).

## 6a. Schutzfunktionen (Entwurf 4)

| Funktion | Auslöser | Was passiert |
|---|---|---|
| **Wärmeschutz** | Wärmemodell je Motor: Strom² über die Zeit (Zeitkonstante 3 min) | ab 100 % Wärme sinkt die Momentgrenze stufenlos bis auf 40 %; das Exo wird schwächer statt abzuschalten, Anzeige „Motor warm“ |
| **Sturzdämpfung** | Sturz erkannt (Kippgeschwindigkeit) | Arme stromlos, Beine 1,5 s mit Dämpfung (−4 Nm·s/rad): das Exo sackt langsam statt zu treten oder zu klappen |
| **Lendensperre** | > 5 kg in den Schlaufen für 0,3 s | Hubmagnet (Pin 9, ab Rev. M2) sperrt das Lendengelenk, der Rahmen trägt den Baum; nach dem Absetzen frei |
| **Rückneigung** | Rumpf mehr als 9° nach hinten | Vibrationspulse: „lehn dich nach vorne“, bevor der Baum dich umzieht |
| **Winden nacheinander** | mehrere Schlaufen gleichzeitig gedrückt | eine Winde nach der anderen (24-V-Wandler 2,5 A), Zeitgrenze 12 s je Bewegung |

## 6b. Gelenkkapseln (Entwurf 5)

| Funktion | Wie |
|---|---|
| Motoren | RobStride RS04 (Hüfte, Knie, Schulter), RS03 (Ellbogen), Übersetzung 1:1 (direkt bzw. Schubstange); Wertebereiche des CAN-Protokolls je Modell |
| Wärmeschutz | wie 6a, mit dem Dauermoment in der Kühlkörper-Kapsel (RS04 28 Nm, RS03 16 Nm) statt dem Datenblattwert; Schulter hält 100 Nm ≈ 13 s mit voller Kraft, dann weich weniger |
| Frostschutz | Heizbänder an der Abtriebsseite je Gruppe (Pin 42 Beine, Pin 43 Arme): an, wenn die kälteste gemessene Wicklung unter +3 °C fällt und die Gruppe kaum arbeitet; aus über +8 °C |
| Rumpf beugen / drehen | Absicht aus 4 Rumpf-Wägezellen (Gurtanker ziehen = vor, Polster drücken = auf, Unterschied = drehen); Vorsteuerung beim Beugen wie an der Hüfte; Grenzen 80/20 Nm bzw. ±30 Nm; bei aktiver Lendensperre ist der Beugemotor stromlos |
| Knie beugen | Software begrenzt auf 10 Nm, zusätzlich gibt das Federpaket in der Schubstange ab ≈ 11 Nm mechanisch nach |

## 7. Was schiefgehen kann, und was dagegen hilft

| Problem | Ursache | Abhilfe |
|---|---|---|
| Exo zittert oder schaukelt | Bus überlastet / verzögert, Kd zu hoch, Polster durchgesessen | Schwingungswächter nimmt Mitgehen zurück (Anzeige im Log); Buslast prüfen, Kd am Prüfstand (P39) senken, Polster tauschen |
| Exo fühlt sich zäh an | Wächter hat zurückgeregelt, Messwandler verrauscht, Kälte (Reibung) | 2 s still halten → Wächter erholt sich; Schirmung der Manschettenkabel prüfen; Kapselheizung |
| Manschette scheuert beim Beugen | Schlitten klemmt (Schmutz, Eis) | Stangen abwischen; iglidur läuft trocken, kein Fett (bindet Schmutz) |
| Exo „läuft weg“ | Nullpunkt verschoben (Kälte, Gurt nachgezogen) | Nullen bei jedem Einschalten |
| Baum fühlt sich zu leicht oder zu schwer an | Lastanteil der Stufe | Stufe wechseln (beide rechten Taster 2 s) |
| Bein streckt beim Gehen | Fußsensor meldet falsch | Schwelle am Prüfstand einstellen, Sensor prüfen |
| Bein knickt beim Aufsetzen | Standbein zu spät erkannt | Glättung verkürzen; Federpatrone begrenzt ohnehin |
| Schulter hilft beim Hebeln zu viel | Kopplung vom Unterarm | Hebel im Abgleich prüfen (Oberarmlänge) |
| Unterstützung wird nach langem Halten schwächer | Wärmeschutz | gewollt: Last kurz absetzen, Motoren kühlen in wenigen Minuten ab |
| Rumpf lässt sich nicht beugen | Lendensperre aktiv (Last in den Schlaufen) | Baum absetzen; Hebel prüfen |

## 8. So baut man es auf

1. **Am Tisch:** ADS131M04-Platine und eine Wägezelle an den Teensy, mit Gewichten kalibrieren, Rauschen anschauen (Ziel < 0,15 N).
2. **Prüfstand:** eine Gelenkkapsel (RS04) mit Kurbel und Schubstange an einem Holzarm, eine Manschette M2. Zuerst Dichtheit und Frost testen (P35, P36), dann Mitgehen einstellen (P39: erst Momentbetrieb, dann admit und Kd schrittweise).
3. **Am Körper:** erst ein Arm, dann Schulter, dann Hüfte im Stand (Fußsensor), dann Knie, zuletzt Gehen.
4. **Unter Last:** Sandsack in den Schlaufen (prüft die Last-Vorsteuerung), dann leichte, dann schwerere Bäume.

## 9. Firmware

Umgesetzt und in der Simulation getestet: [firmware/README.md](../firmware/README.md). Die Kopplung der Gelenke, die
Vorsteuerung und die Standbein-Logik stehen in `firmware/src/exo_app.h`, der Gelenkregler mit Mitgehen und
Schwingungswächter in `firmware/src/exo_core.h`, der Motorbefehl mit Sollgeschwindigkeit und Kd in `firmware/src/drivers.h`
(`packMitCommand`). Die Studie, aus der die Werte stammen: [tools/transparenz.cpp](../tools/transparenz.cpp).
