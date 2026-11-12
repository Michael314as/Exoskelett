# Montage und Inbetriebnahme

Reihenfolge so, dass jeder Schritt für sich prüfbar ist. Prüfpunkte mit Nummer (P…) stehen in [pruefung.md](pruefung.md).
Teile: [fertigung/teileliste.md](../fertigung/teileliste.md), Kaufteile: [stueckliste.md](stueckliste.md),
Kabel: [verdrahtung.md](verdrahtung.md).

## 1. Vorbereiten
1. Maßblatt (Phase 0 im [Entwicklungsplan](entwicklungsplan.md)): Gelenkachsen markieren, Längen messen.
2. Manschetten-Radien im Skript `tools/fertigung.py` eintragen, Druckteile drucken.
3. Laserteile entgraten, Kanten brechen. Spike-Gewinde M5 schneiden.
4. Rohre ablängen, mit Bohrschablone bohren, Rohrenden mit Kappen schließen.

## 2. Rahmen und Beine (passiv)
1. Rückenrahmen mit Eck-Doppelplatten verschrauben (M6, Sicherungsmutter, 8 Nm).
2. Hüftgurt und Schultergurte montieren, Magnet-Schnallen anbringen.
3. Gabel des Abspreizgelenks unten am Rahmen, Nabe mit Hüftschwinge einsetzen, vorne Kugelsperrbolzen. Die Achse muss von hinten genau auf die Mitte deines Hüftgelenks zeigen (Klebepunkt). Zentrierfedern und Gummipuffer (±20°). Hüftlager an der Schwinge (Passschraube M10, 20 Nm).
4. Ober- und Unterschenkelschienen (laufen schräg nach innen zum Knie und Knöchel), Kniegelenk mit Anschlag, Knöchellager, Fersenbügel mit Kippgelenk hinter der Ferse, Steigbügel mit Fersenschale und Ristriemen.
5. Länge über Steckbolzen so einstellen, dass Exo-Achsen und Körperachsen fluchten (Klebepunkte).
6. **Prüfen P1–P4** (Passform, Gehen, Hocke, Aussteigen bei Sturz).

## 3. Elektrik
1. Leistungsverdrahtung laut [Verdrahtungsplan](verdrahtung.md): zuerst Hauptsicherung, Schütz und Not-Aus (der Pilz schaltet die Schützspule, nicht die dicke Leitung), Motoren zuletzt.
2. **Prüfen P5–P7** (Isolation, Not-Aus, Schütz fällt beim Reset ab), **noch ohne Motoren**.
3. Teensy flashen (`firmware/tools/build_teensy.sh`, dann Teensy Loader), ESP32-C3 flashen.
4. Motoren einzeln am Tisch: CAN-ID setzen (RobStride-Software), anschließen, einschalten. Meldet sich ein Motor nicht
   oder geht er nicht in den Betrieb, bleibt der Ring rot und die Statuszeile nennt den Grund.

## 4. Gelenkkapseln, Kurbeln und Schubstangen
1. Motorträger und Wangen verschrauben, Motor einsetzen (Schraubensicherung mittelfest), Lochbild vorher am echten
   Motor nachmessen.
2. Kurbel auf den Abtrieb: erst die zwei Passstifte, dann die Schrauben über Kreuz anziehen (M5: 6 Nm, M4: 3 Nm).
3. Zapfenbohrungen in Kurbel und Hebel auf **8 H7 bzw. 6 H7 reiben** (sie sind 0,2 mm kleiner gelasert). Passschraube
   ISO 7379 (12.9) einsetzen: Der glatte Schaft trägt den Gelenkkopf, das Gewinde liegt hinter der Kurbel, Mutter mit
   Schraubensicherung. Distanzhülse so wählen, dass der Gelenkkopf höchstens 7 mm vor der Kurbel sitzt.
   **Keine normale Schraube mit durchgehendem Gewinde verwenden**, sie bricht auf Dauer ([festigkeit.md](festigkeit.md)).
4. Schubstange einhängen. Länge so einstellen, dass Kurbel und Hebel parallel stehen, wenn das Gelenk in der Mitte
   seines Bereichs ist. Kontermuttern fest, Farbmarkierung.
5. Hauben mit O-Ring und V-Ring aufsetzen, Druckausgleichselement nach unten, Kurbelschutz montieren.
6. **Prüfen P8–P9** (nichts streift in voller Hocke und voller Armbewegung, kein Spiel).
7. **Gelenk-Nullpunkte einmessen (einmal, und nach jedem Motortausch):** Exo aus, USB-Konsole: `gelenk0 alle`.
   Alle Gelenke in die gestreckte Grundstellung bringen (aufrecht, Arme hängen, am besten am Montageständer), Exo
   einschalten, 2 s ruhig. Die Firmware übernimmt die Motorwinkel, schaltet selbst wieder aus und speichert.
   Danach **P40** (Aus- und Einschalten in verschiedenen Stellungen: Der angezeigte Winkel muss stimmen). Ohne diesen
   Schritt kennt die Regelung die Gelenkstellung nicht; liegt ein Winkel außerhalb des möglichen Bereichs, schaltet
   sie mit „Gelenkwinkel unplausibel“ ab.

## 5. Sensor-Manschetten und Kalibrierung
1. Wägezellen in die Halter (M3), Überlastpuffer mit 1 mm Spalt (Fühlerlehre).
2. USB anschließen, Exo **aus**, Konsole 115200 Baud:
   - `null <zelle>` mit unbelasteter Manschette bzw. leerem Schlaufenkopf (Zellen 0–8),
   - Manschette waagrecht halten, bekanntes Gewicht (z. B. 5 kg Hantelscheibe) anhängen: `kal <zelle> 5000`,
   - für jede Zelle wiederholen, dann `speichern`.
3. Fußsensoren (Zellen 9/10) wie die anderen Zellen nullen und mit dem eigenen Körpergewicht kalibrieren (`kal 9 <gramm>`, auf einem Bein stehend).
4. Uhr stellen: `uhr <stunde> <minute>`.
5. **Prüfen P10–P12** (Vorzeichen jeder Zelle, Rauschen, Überlast).

## 6. Prüfstand und Parameter (Mitgehen einstellen, P39)
1. Holzarm mit Gelenkkapsel (Ellbogen: RS03, ID 8, Schubstange) und der Unterarm-Manschette M2 (Zelle 5).
2. Konsole: `pruefstand` (oder `pruefstand <gelenk>`, z. B. `pruefstand 5` für die Schulter rechts), `stufe 0`, `speichern`.
   Dann sind nur dieser Motor und seine Manschetten-Zelle aktiv; die anderen Zellen müssen nicht angeschlossen sein.
   Weitere Zellen meldet `zelle <nr> 1` an (z. B. den Schlaufenkopf), `voll` schaltet wieder alles ein.
   Beim Einschalten nullt die Firmware die Manschette: Sie darf dabei nicht belastet sein (sonst „Nullen: Manschetten entlasten“).
3. `log 1` (bzw. `log 1 <gelenk>`), Ausgabe mitschreiben, Exo einschalten, Holzarm 30 s an der Manschette führen:
   langsam und schnell, über den ganzen Bereich, einmal über dünnem Pulli, einmal über der Winterjacke.
4. `python3 tools/auswertung.py log.csv --hebel 0.15`. Die Auswertung liefert:
   - Trägheit, Reibung und Schwerkraft des echten Gelenks (aus dem Moment, das der Motor zurückmeldet),
   - die Manschettenkraft in Bewegung (Ziel < 4 N) und wie oft der Schwingungswächter eingegriffen hat,
   - einen Vorschlag als Zeile für `firmware/src/exo_app.h` (`kdJoint`, `admit`, `frictionComp`, `viscComp`,
     `gravCoef`): „passt“, „Kd senken“ (Wächter greift ein) oder „admit höher“ (zu viel Kraft, kein Schwingen).
5. Vorschlag eintragen, Trägheit und Reibung auch in `firmware/test/sim_test.cpp` (struct Plant), `make -C firmware test`,
   neu bauen, flashen, Schritt 3–4 wiederholen, bis die Auswertung „passt“ meldet. Die Auswertung selbst ist mit
   simulierten Logs geprüft (`make -C firmware ident`: findet die eingebauten Werte und schlägt in allen drei Fällen das
   Richtige vor).
6. Prüfstand-Abnahme **P13–P20**, **P39** und die Ausfallprüfungen **P41–P44** (Kabel ziehen, eingefrorener Sensor, Strom, Regeltakt).

## 7. Erste Bewegung am Körper
1. Konsole `voll`, Stufe sanft: `stufe 0`, `speichern`.
2. Körper-Abnahme **P21–P30** laut Prüfprotokoll.
3. Erst danach Stufe normal, dann leichte Bäume.
