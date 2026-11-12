# Prüf- und Abnahmeprotokoll

Jeder Punkt hat ein messbares Kriterium. Ein Exo gilt als abgenommen, wenn alle Punkte bestanden sind.
Nach jeder Änderung an Mechanik oder Firmware die betroffenen Punkte wiederholen.
Die Punkte F1, F2 und F4 laufen automatisch im GitHub-Workflow (`make -C firmware test`, `make -C firmware ident`, Teensy-Build, dazu der ESP32-Build); F3 mit `python3 tools/fertigung.py`.

| Nr | Prüfung | Vorgehen | bestanden, wenn | ✓ |
|---|---|---|---|---|
| F1 | Simulation und Abnahmetests | `make -C firmware test` | „ALLE TESTS BESTANDEN“ | ☐ |
| F2 | Teensy-Build | `firmware/tools/build_teensy.sh …` | `exo.hex` entsteht, keine Warnung im eigenen Code | ☐ |
| F3 | Fertigungsdateien | `python3 tools/fertigung.py` | keine FEHLER-Zeile | ☐ |
| F4 | Auswertungsskript | `make -C firmware ident` | „alle innerhalb 20 %“ | ☐ |
| P1 | Passform | 30 min gehen, Treppe, Hocke | keine Druckstelle, Manschetten rutschen < 1 cm | ☐ |
| P2 | Gelenkachsen | Klebepunkte Körper vs. Exo von der Seite fotografieren | Abstand < 1,5 cm in Stand und Hocke | ☐ |
| P3 | Freiheitsgrade | Beine spreizen, Fuß seitlich kippen, Hocke | ±20° Abspreizen ohne Berührung Hüftgurt/Schwinge, Fuß kippt mit, Zentrierfeder holt das Bein zurück | ☐ |
| P4 | Aussteigen | Schnallen auf, mit Helfer rückwärts auf Matte setzen | in < 20 s frei, ohne Werkzeug | ☐ |
| P5 | Isolation | Multimeter Akku+ / Akku− gegen Rahmen | > 1 MΩ | ☐ |
| P6 | Not-Aus | Exo aktiv, Not-Aus drücken; danach dasselbe mit abgezogenem Teensy (nur 24 V an der Spule) | Schütz offen, Ring rot, Motoren stromlos (Multimeter); das Schütz fällt auch ohne Teensy ab (der Pilz unterbricht die Spule); Display zeigt „Not-Aus“ | ☐ |
| P7 | Watchdog | Test-Firmware mit Endlosschleife | Neustart in < 0,6 s, Schütz offen | ☐ |
| P8 | Schubstangen | volle Hocke, Arm ganz hoch und runter, Ellbogen ganz | Stange und Kurbel berühren nichts, Hebel bleibt im Bereich ±55° | ☐ |
| P9 | Vorspannung | Gelenk von Hand hin und her | kein fühlbares Spiel, kein Ruck an den Umkehrpunkten | ☐ |
| P10 | Zellen-Vorzeichen | jede Manschette in +Richtung drücken | Konsole zeigt positive Kraft an der richtigen Zelle | ☐ |
| P11 | Rauschen | Exo aus, ruhig, 10 s Rohwerte | Schwankung < 1 N | ☐ |
| P12 | Überlast | 20 kg an eine Manschette | Puffer liegt an, Zelle unbeschädigt, Wert nach Entlasten wieder 0 ± 1 N | ☐ |
| P13 | Einschalten, Nullen | kurz Ein, 2 s ruhig; dann noch einmal und dabei mit 5 kg gegen eine Manschette drücken | Ring weiß, dann grün; kein Fehler. Mit Druck: startet nicht, Anzeige „Nullen: Manschetten entlasten“; nach dem Loslassen startet es und die Manschette zeigt 0 ± 1 N | ☐ |
| P14 | Sanftes Aufwachen | Kraftmesser an Manschette | volle Unterstützung erst nach ~3 s, kein Sprung | ☐ |
| P14b | Parameter bestimmt | Prüfstand-Log, `tools/auswertung.py` | Modellgüte R² ≥ 0,8; Werte in Simulation und Firmware übernommen, F1 grün | ☐ |
| P15 | Leichtgängigkeit | Holzarm im Prüfstand frei bewegen, Kraftmesser | Kraft mit Regelung < 60 % von ausgeschaltet | ☐ |
| P16 | Verstärkung | 10 kg am Holzarm halten, Stufe 1/2/3 | gemessene Handkraft 67/50/33 % (±10 %) | ☐ |
| P17 | Stabilität | Stufe 3, Arm anstoßen | kein Schwingen, Ruhe in < 0,5 s | ☐ |
| P18 | Grenzen | Gelenk Richtung Anschlag führen | Unterstützung läuft vor dem Anschlag aus | ☐ |
| P19 | Sensorfehler | Wägezellenstecker ziehen (Manschette, Schlaufe, Fuß je einmal); am Prüfstand zusätzlich eine Ader der Zelle kurzschließen | Unterstützung sofort 0, Gelenk bremst weich (kein Schlag), nach 1,5 s stromlos, Ring rot, Grund „Sensor-Manschette gestört“ in der Statuszeile | ☐ |
| P20b | Fußkontakt | auf einem Bein stehen, dann das andere | Standbein wird erkannt; Schwungbein läuft frei, Standbein beugt nicht aktiv | ☐ |
| P20c | Last-Vorsteuerung | 10 kg Sandsack in Schlaufe R1, Arm ruhig halten | gemessene Handkraft ≈ ein Drittel (Stufe normal), ohne Drücken an der Manschette | ☐ |
| P20 | Schlaufen | Taster kurz/lang, 20 kg Sandsack | zu in < 6 s, hält stromlos 10 min; lang drücken mit Last: erst nur Vibration, öffnet nach einer weiteren Sekunde Halten; Notöffnung von Hand; bei gedrücktem Not-Aus läuft keine Winde | ☐ |
| P21 | Körper, leer | Gehen 10 min, Hocke 20× | nichts zieht oder bremst spürbar | ☐ |
| P22 | Arme | heben, senken, beugen, strecken | Exo folgt ohne Verzögerung, die man merkt | ☐ |
| P23 | Beine | aus der Hocke hoch, 20× | spürbare Hilfe, beim Gehen keine | ☐ |
| P24 | Parkstütze | Stütze runter während aktiv | Unterstützung aus, Ring gelb | ☐ |
| P25 | Sturz | erst leer am Ständer kippen, dann auf Matte fallen lassen (Helfer); zusätzlich kräftig auf den Rahmen klopfen und zügig bücken | Arme in < 0,2 s weich, Beine 1,5 s gedämpft (sacken langsam, treten nicht), Schütz fällt erst danach; Klopfen und Bücken lösen nicht aus | ☐ |
| P25b | Rückneigung | mit Helfer langsam nach hinten lehnen | Vibration ab etwa 9°, hört beim Aufrichten auf | ☐ |
| P26 | Licht, Anzeige | Doppeltipp, Display | Licht schaltet, Display zeigt Akku/Stufe/Bäume | ☐ |
| P27 | Sandsack 20 kg tragen | 50 m, Stufe 2 | Handkraft spürbar kleiner, keine Erwärmung > 60 °C an Motoren | ☐ |
| P28 | Echte Bäume | 10 leichte, dann 10 mittlere | ohne Zwischenfall | ☐ |
| P29 | Wetter | 1 h bei Nässe/Schnee | keine Aussetzer, Elektronikbox und Kapseln innen trocken | ☐ |
| P30 | Akkulaufzeit | typischer Arbeitstag | ≥ 2 h mit einem Akku | ☐ |
| P31 | Lendengelenk | bücken leer, dann 10 kg in Schlaufe | leer frei ±25°, Gasfedern tragen das Oberteil; mit Last rastet die Sperre in < 0,5 s, löst nach dem Absetzen | ☐ |
| P32 | Kurbelschutz, Quetschstellen | Finger/Jackenärmel an jede Kurbel, Lendengelenk, Schulterschlitten | nichts wird eingezogen oder eingeklemmt | ☐ |
| P33 | Wärme der Kapsel | Holzarm mit 100 Nm halten, Wicklungstemperatur (CAN) und Kühlkörper messen; dann 30 Nm dauerhaft | ≥ 12 s volle Kraft, danach weich weniger; bei 30 Nm Wicklung stabil < 100 °C → Dauermoment in der Firmware (`motorRatedNm`) eintragen | ☐ |
| P34 | Komfort-Freiheitsgrade | Fuß drehen, Unterarm drehen, Schultern hochziehen, Oberkörper drehen | Drehteller ±20° mit Rückstellung, Drehring frei, Schulter folgt ±3 cm, Rumpf dreht ±15° ohne dass Gurte verrutschen, Federn holen zurück | ☐ |
| P35 | Kapsel dicht | 10 min Gartenschlauch auf die Kapsel, Gelenk dabei bewegen; Haube öffnen | innen trocken, kein Wasser am Motor | ☐ |
| P36 | Kapsel bei Frost | über Nacht in den Gefrierschrank (−18 °C), dann einschalten | Heizung springt an, Gelenk läuft sofort leicht, nach 10 min Wicklung > +3 °C | ☐ |
| P38 | Rumpf | vorbeugen, aufrichten, drehen ohne Last; dann 10 kg in der Schlaufe | folgt ohne Verzögerung, die man merkt; mit Last rastet die Sperre und der Beugemotor wird stromlos; Drehen läuft vor ±15° weich aus | ☐ |
| P39 | Mitgehen einstellen (Prüfstand, dann am Körper) | 1) admit = 0 (Momentbetrieb), Rauschen der Zelle messen; 2) admit 0,2 → 0,6, Kd ab 50 % des Startwerts in Schritten; Log mit `log 1 <gelenk>`, auswerten mit `tools/auswertung.py` ([montage.md](montage.md) Abschnitt 6); 3) Arm 1 Hz frei schwingen, über Pulli und über Winterjacke; 4) Schlitten: Gelenk 90° beugen, Manschette darf längs gleiten | Rauschen < 0,15 N; kein Zittern, Wächter löst bei normaler Bewegung nicht aus; Manschettenkraft rms < 4 N (Ziel aus der Simulation 2,5–4 N); keine Scheuerstelle nach 30 min; Kd-Werte in `exo_app.h` eintragen | ☐ |
| P37 | Bremsenergie | 20 kg am Holzarm schnell absenken, Spannung am Bus messen (Oszilloskop) | bleibt unter 50 V, kein Motor schaltet ab; sonst kleineren Bremswiderstand (4,7 Ω) | ☐ |
| P40 | Gelenk-Nullpunkt, Winkel nach dem Einschalten | `gelenk0` einmessen; dann je Gelenk 5× in verschiedenen Stellungen aus- und einschalten, bei ausgeschaltetem Exo das Gelenk auch von Hand verstellen; Winkel im Log mit Winkelmesser vergleichen | Winkel stimmt jedes Mal auf ± 2°; nie „Gelenkwinkel unplausibel“ in einer erlaubten Stellung. Stimmt er nach dem Verstellen im ausgeschalteten Zustand nicht, braucht das Gelenk einen Referenzschalter (Rücksprache) | ☐ |
| P41 | Motor-Wächter | Prüfstand, Motor hält 10 Nm: CAN-Stecker am Motor ziehen; danach USB-Test-Firmware, die keine Befehle mehr schickt | Motor wird in < 0,2 s von selbst weich (Bus-Wächter im Motor), Teensy meldet „Motor meldet sich nicht“ und öffnet das Schütz | ☐ |
| P42 | Eingefrorener Sensor | Prüfstand: DRDY-Leitung des Messwandlers trennen bzw. 3,3 V des Wandlers kurz wegnehmen, während der Arm bewegt wird | in < 0,05 s Unterstützung 0 und Fehler „Sensor-Manschette gestört“; der Arm läuft nicht weiter | ☐ |
| P43 | Strom und Spannung unter Last | Stromzange (DC) und Oszilloskop am Akku: Kniekapsel am Prüfstand mit 110 Nm gegen Anschlag, 3 s; später am Körper Aufstehen aus der Hocke mit 20 kg | Strom je RS04 bei Spitze gemessen und in [verdrahtung.md](verdrahtung.md) eingetragen; Sicherungen ≥ 1,5 × gemessen; Akku schaltet nicht ab; Spannung bleibt > 30 V; unter 33 V nimmt die Firmware Kraft zurück („Akku unter Last“) | ☐ |
| P44 | Regeltakt | 10 min Betrieb mit allen Motoren und Sensoren, Konsole `status` | kein „Regeltakt überlastet“; Zähler für Übertragungsfehler der Messwandler bleibt bei 0 (höchstens einzelne) | ☐ |
| P45 | Kurbelzapfen und Kurbelsitz | nach 5 h und nach 40 h: Farbmarkierung an Passschrauben, Kurbeln und Kontermuttern ansehen, an der Stange wackeln | nichts hat sich gedreht, kein Spiel, keine Risse | ☐ |
| P46 | Belastungsprobe Rahmen | Exo am Ständer, Gelenke blockiert: 1,5-fache Spitzenlast (60 kg) 1 min an jede Unterarmschiene, 150 kg auf jeden Steigbügel | keine bleibende Verformung, keine Risse an Schweißnähten (Farbeindringprüfung) | ☐ |

Datum, Firmware-Version (Git-Commit), Prüfer: ____________________
