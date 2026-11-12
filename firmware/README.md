# Firmware: Steuerung des Christbaum-Exos

Die Steuerung ist geschrieben und **in einer Simulation getestet**. Damit ist nachgewiesen, dass das Regelkonzept aus
[docs/steuerung.md](../docs/steuerung.md) funktioniert und sich so verhält, wie ein hochwertiges Exo sich anfühlen soll:
leicht, weich, ohne Zittern, mit spürbarer Verstärkung und sicherem Abschalten.

| Datei | Inhalt |
|---|---|
| `src/exo_core.h` | Gelenkregler (Manschettenkraft → Moment), Taster-Erkennung, Schlaufenwinde, Sicherheits-Zustandsmaschine |
| `src/exo_app.h` | Gesamtsteuerung: 8 Gelenke, Kopplung der Manschetten über die Hebel, Vorsteuerung aus Baumlast und Körpergewicht, Standbein/Schwungbein, 3 Winden, Bedienung, LED-Ring |
| `src/drivers.h` | Treiber: CAN-Protokoll der Motoren (RobStride RS04/RS03, 29-Bit-ID, Betriebsmodus Typ 1 / Rückmeldung Typ 2 mit Temperatur), Typ 1 mit Mitgehen (`packMitCommand`: Moment, Sollgeschwindigkeit, Kd), Wägezellen-Wandler ADS131M04 (Manschetten) und HX711 (Schlaufen, Steigbügel), Kalibrierung, Übersetzung |
| `src/settings.h` | Einstellungen und Kalibrierung im EEPROM (mit Prüfsumme), Werkstatt-Konsole, Wartungshinweise |
| `src/main_teensy.cpp` | Teensy 4.1: Pinplan, 200-Hz-Takt, Schütz, Motoren über CAN, 8 Wägezellen, Lagesensor, LED-Ring, Watchdog, Statuszeile an den ESP32-C3 |
| `esp32/` | ESP32-C3: E-Paper-Anzeige und Bluetooth zum Handy |
| `tools/build_teensy.sh` | baut `exo.hex` für den Teensy 4.1 mit ARM-GCC (ohne IDE) |
| `test/esp32_host.cpp` | baut den ESP32-C3-Sketch mit Platzhaltern auf dem PC und prüft Anzeige und Bluetooth-Verhalten |
| `test/teensy_host.cpp` | baut den Teensy-Code mit Platzhaltern auf dem PC und lässt 1000 Regeltakte laufen |
| `test/sim_test.cpp` | Physik-Simulation (Mensch, Weichteile, Manschette, Wägezelle, Exo-Gelenk, Motor) und Abnahmetests |
| `test/log_demo.cpp` | erzeugt ein Prüfstand-Log mit bekannten Werten; `make ident` prüft damit `tools/auswertung.py` |

Starten auf dem PC (Linux/Mac, g++ oder clang++), führt Simulation, Treibertests und den Teensy-Testbuild aus:

```
make -C firmware test
```

Auf GitHub laufen die Tests bei jeder Änderung automatisch (`.github/workflows/firmware.yml`).

---

## Was die Simulation nachbildet

- **Mensch:** Unterarm mit Trägheit, Muskel folgt einer Wunschbewegung, Kraft begrenzt.
- **Weichteile und Polster** zwischen Arm und Manschette als Feder und Dämpfer.
- **Wägezelle:** 1000 Messungen/s (ADS131M04, Rev. M2) bzw. 80/s (HX711, alter Momentbetrieb zum Vergleich), mit Rauschen.
- **Regler:** 500 Hz, wie auf dem Teensy; Befehle kommen 1 ms später beim Motor an (Bus).
- **Motor:** rechnet t_ff + Kd·(v_soll − v) selbst mit frischer Geschwindigkeit (RobStride Typ 1), Stromregler 0,4 ms.
- **Exo-Gelenk:** Trägheit, Lagerreibung, Schwerkraft der Schiene; Schulter und Knie mit eigenen Werten (Abschnitt 17).
- **Baumlast:** greift über die Schlaufen am Exo an, nicht an der Manschette.

## Ergebnisse (aktueller Stand)

| Test | Ergebnis |
|---|---|
| **Leichtgängigkeit Ellbogen** (Mitgehen, M2) | Kraft an der Manschette 0,5 Hz: ohne Regelung 5,7 N (rms), alter Momentbetrieb 3,0 N, **Mitgehen 1,3 N**; 1,2 Hz: 7,4 / 4,9 / **1,9 N** |
| **Leichtgängigkeit Schulter, Knie** (1,2 Hz, Manschette 2–15 kN/m) | alt bis 12,2 / 10,1 N, **Mitgehen bis 2,7 / 2,9 N** (Knie: Trägheit 0,18 kg·m² aus der Bauteilliste) |
| **Schneller Schwung** 1,4 Hz ±34° | Schulter alt 24 N → **4,8 N**; Knie alt 20 N → **4,7 N** (mit Beugegrenze 10 Nm) |
| **Trägheit/Reibung falsch geschätzt** (×0,5 … ×2) | Schulter und Knie bis 3,6 N: bleibt leicht und stabil |
| **Störfall** Kd doppelt + 2 ms Busverzug, straffe Manschette | Schwingungswächter regelt zurück: 5,7 / 6,7 N rms, kein Aufschaukeln |
| **Schwingungswächter** | 15-Hz-Zittern → Mitgehen auf 25 %; erholt sich nach Ruhe; normale Bewegung 1 Hz löst ihn nicht aus |
| **Verstärkung:** 20 Nm Baumlast halten | K = 0,5 / 1 / 2: du trägst 40 / 30 / 20 % (Stufe + gedeckelter Mitgeh-Anteil, erwartet 47 / 35 / 23 %) |
| **Stabilität** | Mitgehen: kein Schwingen bei Manschette 2 / 6 / 15 kN/m in allen Stufen. Rückfallebene Momentbetrieb: stabil bis K = 5,5 |
| **Weichheit:** plötzliche Bewegung | Vorsteuer-Moment ändert sich höchstens 0,8 Nm je Takt (nichts ruckt), kein Überschwingen |
| **Knie mit Schienbein-Manschette** | Schwungbein: geht beim Strecken und Beugen mit; Standbein: Schienbein-Druck beugt nie |
| **Aufwachen** | Nullen 2 s, dann steigt die Unterstützung über 3 s an |
| **Not-Aus, Motorausfall** | im selben Regeltakt (2 ms) alle Momente 0, Schütz offen, Ring rot |
| **Sensorfehler** (Stecker, eingefrorener Wert, NaN, unmöglicher Winkel) | Unterstützung im selben Takt 0, Hüften und Knie 1,5 s gedämpft, dann Fehler |
| **Sturz** | erst nach 20 ms bestätigt; Arme in 0,1 s weich aus, Beine 1,5 s gedämpft (Schütz bleibt so lange zu) |
| **Nullen** | Manschetten-Vorlast wird beim Einschalten abgezogen; mit mehr als 30 N Vorlast startet das Exo nicht |
| **Akku** | Warnung unter 32 V, Kraft läuft unter 30 V in 2 s aus; bei Spannungseinbruch unter Last sinkt die Momentgrenze stufenlos |
| **Schlaufe unter Last** | öffnet erst nach Vorwarnung und einer weiteren Sekunde Halten |
| **Nachgespielte Hardware** (`test/teensy_host.cpp`) | der echte Teensy-Code läuft auf dem PC gegen nachgebildete Motoren und Messwandler: Motoren werden einzeln eingeschaltet und bekommen den Bus-Wächter, ein stummer Motor, ein Übertragungsfehler oder ein eingefrorener Wandler führt zum richtigen Fehler |
| **Parkstütze unten** | keine Unterstützung, Ring gelb |
| **Bedienung** | kurz/lang/doppelt sicher erkannt; Doppeltipp = Licht; Schlaufe zieht bis zur Zugkraft zu und hält stromlos; Stufenwechsel |
| **Kopplung** | 40 N nur am Unterarm: Schulter hilft mit +14 Nm (vorher 0) |
| **Baumlast-Vorsteuerung** | 200 N in den Schlaufen: Ellbogen trägt genau zwei Drittel (31,7 Nm), ohne Drücken; schwerer Baum bleibt in der Momentgrenze |
| **Beine im Stand** | Hocke: Hüfte −14 Nm, Knie −20 Nm von selbst; mit Baum 300 N: −29 / −40 Nm |
| **Schwungbein** | keine Gewichtsvorsteuerung, Bein anheben: Hüfte +10 Nm, Knie +5 Nm (Beugen begrenzt 10 Nm); Standbein beugt nie aktiv |
| **Treiber** | Momentbefehl auf 0,01 Nm genau verpackt, Mitgeh-Befehl (v_soll, Kd, Kp = 0) richtig, Kd/Geschwindigkeit über die Übersetzung umgerechnet, Motorantwort richtig gelesen, ADS131M04-Register und -Rahmen (Vorzeichen) richtig, HX711-Werte inkl. negativer Zahlen richtig, Sensor am Anschlag erkannt |
| **Teensy-Code** | baut mit ARM-GCC für den Teensy 4.1 (`tools/build_teensy.sh`, 107 KB, keine Warnung im eigenen Code); zusätzlich PC-Lauf mit Platzhaltern (SPI, zwei CAN-Busse) |
| **Einstellungen, Konsole** | Prüfsumme erkennt kaputtes EEPROM, Kalibrierung mit Gewicht, Fehlbedienung abgewiesen |
| **Statuszeile** | Teensy → ESP32-C3: Modus, Akku, Stufe, Restzeit, Uhrzeit, Stunden, Grund; kaputte Zeilen werden verworfen |
| **ESP32-Sketch** | kompiliert auf dem PC mit Platzhaltern; jede gültige Zeile geht ans Handy, E-Paper zeichnet nur bei Änderung (auch bei neuem Fehlergrund), überlange Zeilen laufen nicht über |

## Was die Simulation gefunden hat

1. **Zu viel Dämpfung macht das Exo zäh.** Mit der ursprünglich geplanten Dämpfung war die Kraft beim freien Bewegen
   **höher** als ganz ohne Regelung. Jetzt gibt es einen Reibungsausgleich und nur noch eine kleine Dämpfung.
2. **Das Totband war zu groß.** Mit 5 N bekamen kleine Bewegungen gar keine Hilfe. Bei 0,6 N Sensorrauschen reichen 2 N.
3. **Bedienfehler:** Beide rechten Daumentaster 2 s gedrückt (Stufenwechsel) wurde zugleich als „lang gedrückt“ erkannt
   und hätte die Schlaufen **geöffnet**, mit Baum in der Hand. Behoben und mit einem eigenen Test abgesichert.
4. **Fehlendes `#include`:** Der Test-Build des Teensy-Codes hat einen Fehler gefunden, der auf dem PC zufällig nicht
   auffiel und auf dem Teensy den Bau verhindert hätte. Behoben.

## Was noch fehlt

- **Annahmen über die Motoren, die erst am echten Motor geprüft werden können** (Prüfpunkte P40, P41): Der Winkel
  nach dem Einschalten ist absolut über eine Umdrehung des Abtriebs (laut Handbuch 0–2π); der Parameter `canTimeout`
  (0x7028) wird nach jedem Einschalten neu geschrieben und stoppt den Motor ohne Befehle; die Betriebsart steht in
  den Bits 22–23 der Rückmeldung.

- **Auf echter Hardware laufen lassen:** `exo.hex` auf einen Teensy 4.1 flashen, Motoren und Sensoren anschließen,
  Prüfprotokoll [docs/pruefung.md](../docs/pruefung.md) P5–P20.
- **ESP32-C3:** Der Sketch wird im GitHub-Workflow mit dem echten ESP32-Compiler gebaut (Job `esp32`); auf einem
  echten Board ist er noch nicht gelaufen.
- **Abgleich mit echter Hardware:** Reibung, Trägheit und Schwerkraft sind geschätzt. Am Prüfstand (Konsole `pruefstand`,
  `log 1`) aufnehmen und mit `python3 tools/auswertung.py log.csv` bestimmen, dann eintragen ([docs/montage.md](../docs/montage.md), Abschnitt 6).
- Ob es sich wirklich gut anfühlt, entscheidest du am Prüfstand und beim Tragen.
