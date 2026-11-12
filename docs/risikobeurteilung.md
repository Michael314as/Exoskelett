# Risikobeurteilung (Eigenbau, Entwurf 5)

Vorgehen angelehnt an DIN EN ISO 12100 (Risikobeurteilung von Maschinen). Für Exoskelette gibt es zusätzlich
DIN EN ISO 13482 (Roboter für die persönliche Assistenz). Dieses Exo ist ein **Eigenbau für die eigene Nutzung**.
Soll es verkauft, verliehen oder von Angestellten benutzt werden, gelten die Maschinenverordnung (EU) 2023/1230 bzw.
bis Januar 2027 die Maschinenrichtlinie 2006/42/EG mit CE-Kennzeichnung. Dafür ist eine Prüfung durch eine Fachstelle nötig.

Bewertung: S = Schwere (1 leicht … 4 schwer), W = Wahrscheinlichkeit vor Maßnahme (1 selten … 4 häufig).

| Gefährdung | Ursache | S | W | Maßnahme | Prüfung | Restrisiko |
|---|---|---|---|---|---|---|
| Gelenk überstreckt | Motor treibt über den Körperbereich | 4 | 2 | mechanische Anschläge, weiche Wand in Software, Momentgrenze | P8, P18 | gering |
| Ungewollte Bewegung | Sensorfehler, Kabelbruch | 3 | 2 | Messbereich je Zelle, Unterstützung sofort 0, Beine gedämpft, Rückmeldung rot | P19, F1 | gering |
| Gelenk läuft weiter (Mitgehen folgt einem alten Wert) | Messwandler hängt, Übertragungsfehler, Wert eingefroren | 4 | 2 | **Frische-Überwachung** jeder Zelle (20 ms / 0,3 s), Prüfsumme des Messwandlers, Neustart-Erkennung; Sollgeschwindigkeit begrenzt | P42, F1 | gering |
| Falsche Gelenkstellung in der Regelung | Nullpunkt nicht eingemessen, Motor getauscht, Winkel nach dem Einschalten versetzt | 4 | 2 | Nullpunkt einmessen (`gelenk0`), Winkelprüfung gegen den möglichen Bereich, **mechanische Anschläge bleiben die letzte Sicherung** | P40, P8 | mittel bis P40 bestanden ist |
| Motor treibt weiter, obwohl die Steuerung ausgefallen ist | CAN-Kabel ab, Teensy hängt mit geschlossenem Schütz | 4 | 1 | Bus-Wächter im Motor (0,1 s), Hardware-Watchdog öffnet das Schütz | P41, P7 | gering |
| Beine knicken weg bei Abschaltung | Fehler oder leerer Akku in der Hocke / mit Baum | 3 | 3 | weiche Fehler bremsen die Beine 1,5 s; Akku warnt früh und läuft in 2 s aus; Spannungseinbruch nimmt nur Kraft zurück. **Bei Not-Aus, Motorausfall und Sicherungsfall bleibt es beim sofortigen Wegfall** (die Motoren sind dann leicht drehbar, der Körper trägt das Exo über die Gurte) | F1, P19, P43 | mittel |
| Sicherung oder Akku schaltet unter Spitzenlast ab | Strom beim Aufstehen höher als angenommen | 3 | 3 | Strom messen, Sicherungen danach wählen, Firmware regelt bei Spannungseinbruch ab | P43 | mittel bis P43 bestanden ist |
| Start mit verspannter Manschette | Arm oder Bein drückt beim Einschalten | 2 | 3 | Nullen prüft die Vorlast und startet sonst nicht | P13, F1 | gering |
| Software hängt | Fehler, Störung | 3 | 2 | Hardware-Watchdog 0,5 s, Schütz fällt ab, Pulldown | P7 | gering |
| Exo schwingt oder läuft nach (Mitgehen, Rev. M2) | Busverzug, zu hohe Dämpfung, sehr straffe Manschette, Rauschen | 3 | 2 | Schwingungswächter (> 6 Hz → bis 25 % zurück), Sollgeschwindigkeit ≤ 8 rad/s (Rumpf 2) und weich vor Anschlägen, Gesamtmoment inkl. Kd-Anteil begrenzt, Kp = 0 (keine Positionsfeder, die zieht), v_soll 0 bei Stillstand = Motor bremst | P39, P8 | gering |
| Not-Aus wirkungslos | nur Software; Kontakt verschweißt, weil er den Motorstrom schalten muss | 4 | 2 | Not-Aus unterbricht die **Schützspule** (kleiner Strom), Gleichstromschütz für ≥ 60 V / 100 A trennt die Motoren, zweiter Kontakt meldet; vor jedem Einsatz testen | P6 | gering |
| Sturz mit Exo | Gelände, Kippen nach vorne | 3 | 3 | Ausfallschritt, Steigbügel mit Spikes, Sturzerkennung → Motoren weich, **Beine 1,5 s gedämpft** (kein Zusammensacken, kein Treten), Aussteigen ohne Werkzeug | P4, P25 | mittel |
| Kippen nach hinten | Baum zieht, Rumpf zu weit zurück | 3 | 2 | **Warnung per Vibration** ab 9° Rückneigung (Firmware), Schutzbügel hinten mit Gummipuffern trifft zuerst, Akkus im Schutzbügel | P25b | gering |
| Rumpf knickt unter Last ein | Lendengelenk frei beweglich; **Sperre löst bei Stromausfall** (Hubmagnet stromlos = frei) | 3 | 2 | Gasfedern tragen das Oberteil, **Sperre rastet automatisch**, sobald > 5 kg in den Schlaufen hängen (0,3 s), Hebel zum Sperren von Hand | P31 | mittel (Vorschlag: Sperre umdrehen, Feder sperrt, Magnet löst; siehe [durchsicht-2026-11.md](durchsicht-2026-11.md)) |
| Schlaufe öffnet mit Baum in der Hand | Taster versehentlich lang gedrückt | 3 | 2 | unter Last erst Vorwarnung, öffnet nur bei weiterem Halten | P20, F1 | gering |
| Baum fällt aus der Schlaufe | Schlaufe rutscht | 2 | 2 | Zahnleiste formschlüssig, Zugkraft über Motorstrom, Schneckengetriebe hält stromlos | P20 | gering |
| Baum lässt sich nicht lösen | Winde defekt | 3 | 1 | Schnappschäkel mit Zuglasche (Notöffnung von Hand) | P20 | gering |
| Stufenwechsel öffnet Schlaufe | Bedienfehler | 3 | 2 | gesperrt in der Firmware (Test) | F1 | gering |
| Quetschen an Kurbel und Schubstange | Finger, Kleidung zwischen Kurbel und Schiene | 2 | 2 | **Kurbelschutz aus transparentem PETG** an Knie und Ellbogen, Stange außen an der Schiene | P8, P32 | gering |
| Quetschen am Lendengelenk, Schulterschlitten | Haut, Jacke zwischen den Laschen | 2 | 2 | Spalt ≥ 25 mm oder ≤ 5 mm, Polster darüber, Gasfedern hinter dem Rahmen | P32 | gering |
| Bruch Schubstange / Gelenkkopf / Kurbelzapfen | Überlast, Verschleiß, Zapfen auf Biegung | 3 | 2 | Rohr knickt erst bei ≈ 17 kN (Betrieb ≤ 3,2 kN), Gelenkköpfe mit vierfacher Reserve, **Zapfen als Passschraube 12.9** (normale Schraube bräche im Gewinde), Passstifte in den Kurbeln, alle 40 h prüfen | P45, Wartung | gering |
| Wasser oder Eis in der Gelenkkapsel | Dichtung undicht, Kondenswasser | 2 | 2 | O-Ring, V-Ring, Druckausgleichselement, Heizfolie; Motortemperatur und Motorfehler werden überwacht (Fehler = stromlos) | P35, P36 | gering |
| Überspannung beim Bremsen | Motoren speisen zurück, Akku nimmt nichts auf | 3 | 2 | Bremswiderstand ab 46 V; sonst schalten die Motoren mitten in der Bewegung ab | P37 | gering |
| Druckstellen, Scheuern | Achsen fluchten nicht | 2 | 3 | Maßblatt, Langlöcher, Polster, freie Abspreiz-/Drehgelenke | P1–P3 | mittel |
| Überhitzung Motoren/Akku | Dauerlast | 2 | 2 | **Wärmemodell je Motor** (I²t, Zeitkonstante 3 min) senkt die Grenze stufenlos auf bis 40 %, Anzeige meldet „Motor warm“; Motor-Temperatur, Akku-BMS | P27, F1 | gering |
| Schlaufenwinden überlasten Wandler | drei Winden gleichzeitig | 2 | 2 | Winden laufen nacheinander (Firmware), Zeitgrenze 12 s je Bewegung | F1, P20 | gering |
| Akku-Kurzschluss | Nässe, Scheuerstelle | 4 | 1 | Hauptsicherung, IP67-Stecker, Isolationsprüfung | P5, P29 | gering |
| Überlastung des Körpers | Exo verdeckt, wie schwer es ist | 3 | 2 | Verstärkung höchstens 3 (Mensch spürt immer ≥ 25 %), Stufen | P16 | gering |
| Arbeiten mit Motorsäge | Kettensäge im unterstützten Modus | 4 | 2 | **verboten**: Bedienungsanleitung, Exo vorher aus | – | Nutzer |
| Dritte in der Nähe | Baum kippt aufs Plateau | 3 | 2 | Gefahrenbereich 4 m freihalten | – | Nutzer |

**Fazit:** Mit den Maßnahmen ist das Restrisiko für die eigene Nutzung nach bestandener Abnahme vertretbar.
Offen bleiben die Punkte mit „mittel“, vor allem: Wegfall der Beinkraft bei hartem Abschalten, Strom unter Spitzenlast (P43), Winkel nach dem Einschalten (P40), Sturz im Gelände (das Exo hat keinen Fußmotor und hält das Gleichgewicht nicht aktiv) und Passform: Sie werden in der Saison-Erprobung beobachtet und ins Protokoll eingetragen.
