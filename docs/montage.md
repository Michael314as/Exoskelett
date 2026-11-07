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

## 3. Antriebsbox und Elektrik
1. Motoren an die Motorplatte (M3, Schraubensicherung mittelfest), Trommeln mit Bordscheiben.
2. Leistungsverdrahtung laut Verdrahtungsplan; Not-Aus und Schütz zuerst, Motoren zuletzt.
3. **Prüfen P5–P7** (Isolation, Not-Aus, Schütz fällt beim Reset ab), **noch ohne Seile**.
4. Teensy flashen (`firmware/tools/build_teensy.sh`, dann Teensy Loader), ESP32-C3 flashen.
5. Motoren einzeln: CAN-ID setzen, Richtung prüfen (Konsole `status`).

## 4. Seile und Seilpaare
1. Seilscheiben aus Lagen verschrauben (M4), Seilklemmen einsetzen.
2. Hüfte: Hüftmotor auf die Motorplatte der Schwinge, offene Seilschleife Trommel → Scheibe Ø 12 cm (oben und unten je ein Seil, an beiden Enden geklemmt), Spannschraube bis kein Spiel mehr fühlbar ist.
   Knie: Bowdenzug-Paar von der Box über Abspreizgelenk und Schwinge hinten am Oberschenkel entlang (Radius ≥ 15 cm), Streckzug über die Kurvenscheibe nach vorne, Beugezug durch die Federpatrone unter der Beugescheibe nach vorne, beide klemmen.
3. Schulter: offenes Seilpaar über die Rollen auf der Schwenkachse; Ellbogen: Bowdenzug-Paar. Spannschrauben stellen
   die Vorspannung ein, bis kein Spiel mehr fühlbar ist.
4. **Prüfen P8–P9** (Seilwege frei, kein Kontakt zu Gelenken in voller Hocke und voller Armbewegung).

## 5. Sensor-Manschetten und Kalibrierung
1. Wägezellen in die Halter (M3), Überlastpuffer mit 1 mm Spalt (Fühlerlehre).
2. USB anschließen, Exo **aus**, Konsole 115200 Baud:
   - `null <zelle>` mit unbelasteter Manschette bzw. leerem Schlaufenkopf (Zellen 0–8),
   - Manschette waagrecht halten, bekanntes Gewicht (z. B. 5 kg Hantelscheibe) anhängen: `kal <zelle> 5000`,
   - für jede Zelle wiederholen, dann `speichern`.
3. Fußsensoren: Multimeter an Pin 38/39 gegen Masse. Unbelastet unter 0,5 V, auf diesem Bein stehend über 1 V (Schwelle 300 ≈ 1 V; bei Bedarf in `main_teensy.cpp` anpassen).
4. Uhr stellen: `uhr <stunde> <minute>`.
5. **Prüfen P10–P12** (Vorzeichen jeder Zelle, Rauschen, Überlast).

## 6. Prüfstand und Parameter (Mitgehen einstellen, P39)
1. Holzarm mit Gelenkkapsel (Ellbogen: RS03, ID 8, Schubstange) und der Unterarm-Manschette M2 (Zelle 5).
2. Konsole: `pruefstand` (oder `pruefstand <gelenk>`, z. B. `pruefstand 5` für die Schulter rechts), `stufe 0`, `speichern`.
   Dann ist nur dieser Motor aktiv.
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
6. Prüfstand-Abnahme **P13–P20**, **P39**.

## 7. Erste Bewegung am Körper
1. Konsole `voll`, Stufe sanft: `stufe 0`, `speichern`.
2. Körper-Abnahme **P21–P30** laut Prüfprotokoll.
3. Erst danach Stufe normal, dann leichte Bäume.
