# Firmware: Steuerung des Christbaum-Exos

Die Steuerung ist geschrieben und **in einer Simulation getestet**. Damit ist nachgewiesen, dass das Regelkonzept aus
[docs/steuerung.md](../docs/steuerung.md) funktioniert und sich so verhält, wie ein hochwertiges Exo sich anfühlen soll:
leicht, weich, ohne Zittern, mit spürbarer Verstärkung und sicherem Abschalten.

| Datei | Inhalt |
|---|---|
| `src/exo_core.h` | Gelenkregler (Manschettenkraft → Moment), Taster-Erkennung, Schlaufenwinde, Sicherheits-Zustandsmaschine, Baumzähler |
| `src/exo_app.h` | Gesamtsteuerung: 6 Gelenke, 3 Winden, Bedienung, LED-Ring, Licht, Vibration |
| `src/drivers.h` | Treiber: CAN-Protokoll der Motoren (MIT-Modus, CubeMars AK70-10 oder baugleich), Wägezellen-Verstärker HX711, Kalibrierung, Seilübersetzung |
| `src/settings.h` | Einstellungen und Kalibrierung im EEPROM (mit Prüfsumme), Werkstatt-Konsole, Wartungshinweise |
| `src/main_teensy.cpp` | Teensy 4.1: Pinplan, 200-Hz-Takt, Schütz, Motoren über CAN, 8 Wägezellen, Lagesensor, LED-Ring, Watchdog, Statuszeile an den ESP32-C3 |
| `esp32/` | ESP32-C3: E-Paper-Anzeige und Bluetooth zum Handy |
| `tools/build_teensy.sh` | baut `exo.hex` für den Teensy 4.1 mit ARM-GCC (ohne IDE) |
| `test/teensy_host.cpp` | baut den Teensy-Code mit Platzhaltern auf dem PC und lässt 1000 Regeltakte laufen |
| `test/sim_test.cpp` | Physik-Simulation (Mensch, Weichteile, Manschette, Wägezelle, Exo-Gelenk, Motor) und Abnahmetests |

Starten auf dem PC (Linux/Mac, g++ oder clang++), führt Simulation, Treibertests und den Teensy-Testbuild aus:

```
make -C firmware test
```

Auf GitHub laufen die Tests bei jeder Änderung automatisch (`.github/workflows/firmware.yml`).

---

## Was die Simulation nachbildet

- **Mensch:** Unterarm mit Trägheit, Muskel folgt einer Wunschbewegung, Kraft begrenzt.
- **Weichteile und Polster** zwischen Arm und Manschette als Feder und Dämpfer.
- **Wägezelle:** 80 Messungen/s (HX711) oder 320/s (NAU7802), mit Rauschen.
- **Regler:** 200 Hz, wie auf dem Teensy.
- **Exo-Gelenk:** Trägheit, Lager- und Bowdenreibung, Schwerkraft der Schiene, Motor mit 2 ms Verzögerung.
- **Baumlast:** greift über die Schlaufen am Exo an, nicht an der Manschette.

## Ergebnisse (aktueller Stand)

| Test | Ergebnis |
|---|---|
| **Leichtgängigkeit:** Arm frei bewegen | Kraft an der Manschette: ohne Regelung 5,7 N (rms), mit Regelung 3,0 N. Das Exo fühlt sich fast doppelt so leicht an wie ungeregelt. |
| **Verstärkung:** 20 Nm Baumlast halten | K = 0,5: du trägst 67 %, K = 1: 51 %, K = 2: 34 %, K = 3: 26 % (erwartet 67 / 50 / 33 / 25 %) |
| **Stabilität:** ab wann schwingt es? | stabil bis K = 5,5 (HX711) bzw. 7,5 (NAU7802). In der Software ist K höchstens 3: große Reserve |
| **Weichheit:** plötzliche Bewegung | Moment ändert sich höchstens 2 Nm je Takt (nichts ruckt), Überschwingen 0,01 rad |
| **Beinseil** | Moment nie negativ (Seil kann nicht drücken), Vorspannung bleibt |
| **Aufwachen** | Nullen 2 s, dann steigt die Unterstützung über 3 s an |
| **Sensorfehler, Not-Aus** | im selben Regeltakt (5 ms) alle Momente 0, Ring rot |
| **Sturz** | Motoren in 0,1 s weich aus |
| **Parkstütze unten** | keine Unterstützung, Ring gelb |
| **Bedienung** | kurz/lang/doppelt sicher erkannt; Doppeltipp = Licht; Schlaufe zieht bis zur Zugkraft zu und hält stromlos; Stufenwechsel |
| **Baumzähler** | zählt jeden Baum genau einmal |
| **Treiber** | Momentbefehl auf 0,01 Nm genau verpackt, Motorantwort richtig gelesen, HX711-Werte inkl. negativer Zahlen richtig, Sensor am Anschlag erkannt |
| **Motorauswahl** | 100 Nm Spitze an der Schulter brauchen 24,3 Nm am Motor: passt zum AK70-10 (25 Nm) |
| **Teensy-Code** | baut mit ARM-GCC für den Teensy 4.1 (86 KB, keine Warnung im eigenen Code); zusätzlich PC-Lauf mit Platzhaltern |
| **Einstellungen, Konsole** | Prüfsumme erkennt kaputtes EEPROM, Kalibrierung mit Gewicht, Fehlbedienung abgewiesen |
| **Statuszeile** | Teensy → ESP32-C3 im vereinbarten Format, kaputte Zeilen werden verworfen |

## Was die Simulation gefunden hat

1. **Zu viel Dämpfung macht das Exo zäh.** Mit der ursprünglich geplanten Dämpfung war die Kraft beim freien Bewegen
   **höher** als ganz ohne Regelung. Jetzt gibt es einen Reibungsausgleich und nur noch eine kleine Dämpfung.
2. **Das Totband war zu groß.** Mit 5 N bekamen kleine Bewegungen gar keine Hilfe. Bei 0,6 N Sensorrauschen reichen 2 N.
3. **Bedienfehler:** Beide rechten Daumentaster 2 s gedrückt (Stufenwechsel) wurde zugleich als „lang gedrückt“ erkannt
   und hätte die Schlaufen **geöffnet**, mit Baum in der Hand. Behoben und mit einem eigenen Test abgesichert.
4. **Fehlendes `#include`:** Der Test-Build des Teensy-Codes hat einen Fehler gefunden, der auf dem PC zufällig nicht
   auffiel und auf dem Teensy den Bau verhindert hätte. Behoben.

## Was noch fehlt

- **Auf echter Hardware laufen lassen:** `exo.hex` auf einen Teensy 4.1 flashen, Motoren und Sensoren anschließen,
  Prüfprotokoll [docs/pruefung.md](../docs/pruefung.md) P5–P20.
- **ESP32-C3 bauen:** Der Code ist geschrieben, der ESP32-Build ist hier nicht gelaufen (kein ESP32-Compiler verfügbar).
- **Abgleich mit echter Hardware:** Steifigkeit der Polster, Reibung der Bowdenzüge und Trägheit sind geschätzt.
  Auf dem Prüfstand werden sie gemessen und hier eingetragen, danach wird die Verstärkung neu eingestellt.
- Ob es sich wirklich gut anfühlt, entscheidest du am Prüfstand und beim Tragen.
