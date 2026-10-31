# Prüf- und Abnahmeprotokoll

Jeder Punkt hat ein messbares Kriterium. Ein Exo gilt als abgenommen, wenn alle Punkte bestanden sind.
Nach jeder Änderung an Mechanik oder Firmware die betroffenen Punkte wiederholen.
Die Punkte F1, F2 und F4 laufen automatisch im GitHub-Workflow (`make -C firmware test`, `make -C firmware ident`, Teensy-Build); F3 mit `python3 tools/fertigung.py`.

| Nr | Prüfung | Vorgehen | bestanden, wenn | ✓ |
|---|---|---|---|---|
| F1 | Simulation und Abnahmetests | `make -C firmware test` | „ALLE TESTS BESTANDEN“ | ☐ |
| F2 | Teensy-Build | `firmware/tools/build_teensy.sh …` | `exo.hex` entsteht, keine Warnung im eigenen Code | ☐ |
| F3 | Fertigungsdateien | `python3 tools/fertigung.py` | keine FEHLER-Zeile | ☐ |
| F4 | Auswertungsskript | `make -C firmware ident` | „alle innerhalb 20 %“ | ☐ |
| P1 | Passform | 30 min gehen, Treppe, Hocke | keine Druckstelle, Manschetten rutschen < 1 cm | ☐ |
| P2 | Gelenkachsen | Klebepunkte Körper vs. Exo von der Seite fotografieren | Abstand < 1,5 cm in Stand und Hocke | ☐ |
| P3 | Freiheitsgrade | Bein abspreizen, Fußspitze drehen, unebener Boden | ±20° Abspreizen, ±15° Drehen ohne Anschlag im Normalgang | ☐ |
| P4 | Aussteigen | Schnallen auf, mit Helfer rückwärts auf Matte setzen | in < 20 s frei, ohne Werkzeug | ☐ |
| P5 | Isolation | Multimeter Akku+ / Akku− gegen Rahmen | > 1 MΩ | ☐ |
| P6 | Not-Aus | Exo aktiv, Not-Aus drücken | Schütz offen, Ring rot, Motoren stromlos (Multimeter) | ☐ |
| P7 | Watchdog | Test-Firmware mit Endlosschleife | Neustart in < 0,6 s, Schütz offen | ☐ |
| P8 | Seilwege | volle Hocke, Arm ganz hoch und runter, Ellbogen ganz | Seil berührt kein Gelenk, keine Kante | ☐ |
| P9 | Vorspannung | Gelenk von Hand hin und her | kein fühlbares Spiel, kein Ruck an den Umkehrpunkten | ☐ |
| P10 | Zellen-Vorzeichen | jede Manschette in +Richtung drücken | Konsole zeigt positive Kraft an der richtigen Zelle | ☐ |
| P11 | Rauschen | Exo aus, ruhig, 10 s Rohwerte | Schwankung < 1 N | ☐ |
| P12 | Überlast | 20 kg an eine Manschette | Puffer liegt an, Zelle unbeschädigt, Wert nach Entlasten wieder 0 ± 1 N | ☐ |
| P13 | Einschalten | kurz Ein, 2 s ruhig | Ring weiß, dann grün; kein Fehler | ☐ |
| P14 | Sanftes Aufwachen | Kraftmesser an Manschette | volle Unterstützung erst nach ~3 s, kein Sprung | ☐ |
| P14b | Parameter bestimmt | Prüfstand-Log, `tools/auswertung.py` | Modellgüte R² ≥ 0,8; Werte in Simulation und Firmware übernommen, F1 grün | ☐ |
| P15 | Leichtgängigkeit | Holzarm im Prüfstand frei bewegen, Kraftmesser | Kraft mit Regelung < 60 % von ausgeschaltet | ☐ |
| P16 | Verstärkung | 10 kg am Holzarm halten, Stufe 1/2/3 | gemessene Handkraft 67/50/33 % (±10 %) | ☐ |
| P17 | Stabilität | Stufe 3, Arm anstoßen | kein Schwingen, Ruhe in < 0,5 s | ☐ |
| P18 | Grenzen | Gelenk Richtung Anschlag führen | Unterstützung läuft vor dem Anschlag aus | ☐ |
| P19 | Sensorfehler | Wägezellenstecker ziehen | sofort stromlos, Ring rot, Grund in der Statuszeile | ☐ |
| P20 | Schlaufen | Taster kurz/lang, 20 kg Sandsack | zu in < 6 s, hält stromlos 10 min, öffnet; Notöffnung von Hand | ☐ |
| P21 | Körper, leer | Gehen 10 min, Hocke 20× | nichts zieht oder bremst spürbar | ☐ |
| P22 | Arme | heben, senken, beugen, strecken | Exo folgt ohne Verzögerung, die man merkt | ☐ |
| P23 | Beine | aus der Hocke hoch, 20× | spürbare Hilfe, beim Gehen keine | ☐ |
| P24 | Parkstütze | Stütze runter während aktiv | Unterstützung aus, Ring gelb | ☐ |
| P25 | Sturz | auf Matte fallen lassen (Helfer) | Motoren in < 0,2 s weich | ☐ |
| P26 | Licht, Anzeige | Doppeltipp, Display | Licht schaltet, Display zeigt Akku/Stufe/Bäume | ☐ |
| P27 | Sandsack 20 kg tragen | 50 m, Stufe 2 | Handkraft spürbar kleiner, keine Erwärmung > 60 °C an Motoren | ☐ |
| P28 | Echte Bäume | 10 leichte, dann 10 mittlere | ohne Zwischenfall | ☐ |
| P29 | Wetter | 1 h bei Nässe/Schnee | keine Aussetzer, Box innen trocken | ☐ |
| P30 | Akkulaufzeit | typischer Arbeitstag | ≥ 2 h mit einem Akku | ☐ |

Datum, Firmware-Version (Git-Commit), Prüfer: ____________________
