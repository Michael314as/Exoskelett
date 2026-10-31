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
3. Hüftbügel, Abspreizgelenk (Gummipuffer einstellen: ±20°), Hüftlager (Passschraube M10, 20 Nm), Drehlager.
4. Ober- und Unterschenkelschienen, Kniegelenk mit Anschlag, Steigbügel mit Seitenbügel.
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
2. Beinseil: Box → Rolle Becken → hinten um die Hüftscheibe → Rollen Oberschenkel → vorne um die Knie-Kurvenscheibe → Klemme.
   Vorspannung von Hand so, dass das Seil gerade nicht durchhängt.
3. Schulter: offenes Seilpaar über die Rollen auf der Schwenkachse; Ellbogen: Bowdenzug-Paar. Spannschrauben stellen
   die Vorspannung ein, bis kein Spiel mehr fühlbar ist.
4. **Prüfen P8–P9** (Seilwege frei, kein Kontakt zu Gelenken in voller Hocke und voller Armbewegung).

## 5. Sensor-Manschetten und Kalibrierung
1. Wägezellen in die Halter (M3), Überlastpuffer mit 1 mm Spalt (Fühlerlehre).
2. USB anschließen, Exo **aus**, Konsole 115200 Baud:
   - `null <zelle>` mit unbelasteter Manschette (Zellen 0–7),
   - Manschette waagrecht halten, bekanntes Gewicht (z. B. 5 kg Hantelscheibe) anhängen: `kal <zelle> 5000`,
   - für jede Zelle wiederholen, dann `speichern`.
3. **Prüfen P10–P12** (Vorzeichen jeder Zelle, Rauschen, Überlast).

## 6. Erste Bewegung
1. Stufe sanft: `stufe 0`, `speichern`.
2. Prüfstand-Abnahme **P13–P20**, dann Körper-Abnahme **P21–P30** laut Prüfprotokoll.
3. Erst danach Stufe normal, dann leichte Bäume.
