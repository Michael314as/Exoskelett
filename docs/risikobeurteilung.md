# Risikobeurteilung (Eigenbau, Entwurf 5)

Vorgehen angelehnt an DIN EN ISO 12100 (Risikobeurteilung von Maschinen). Für Exoskelette gibt es zusätzlich
DIN EN ISO 13482 (Roboter für die persönliche Assistenz). Dieses Exo ist ein **Eigenbau für die eigene Nutzung**.
Soll es verkauft, verliehen oder von Angestellten benutzt werden, gelten die Maschinenverordnung (EU) 2023/1230 bzw.
bis Januar 2027 die Maschinenrichtlinie 2006/42/EG mit CE-Kennzeichnung. Dafür ist eine Prüfung durch eine Fachstelle nötig.

Bewertung: S = Schwere (1 leicht … 4 schwer), W = Wahrscheinlichkeit vor Maßnahme (1 selten … 4 häufig).

| Gefährdung | Ursache | S | W | Maßnahme | Prüfung | Restrisiko |
|---|---|---|---|---|---|---|
| Gelenk überstreckt | Motor treibt über den Körperbereich | 4 | 2 | mechanische Anschläge, weiche Wand in Software, Momentgrenze | P8, P18 | gering |
| Ungewollte Bewegung | Sensorfehler, Kabelbruch | 3 | 2 | Plausibilitätsprüfung, sofort stromlos, Rückmeldung rot | P19, F1 | gering |
| Software hängt | Fehler, Störung | 3 | 2 | Hardware-Watchdog 0,5 s, Schütz fällt ab, Pulldown | P7 | gering |
| Not-Aus wirkungslos | nur Software | 4 | 1 | Not-Aus trennt elektrisch hart, zweiter Kontakt meldet | P6 | gering |
| Sturz mit Exo | Gelände, Kippen nach vorne | 3 | 3 | Ausfallschritt, Steigbügel mit Spikes, Sturzerkennung → Motoren weich, **Beine 1,5 s gedämpft** (kein Zusammensacken, kein Treten), Aussteigen ohne Werkzeug | P4, P25 | mittel |
| Kippen nach hinten | Baum zieht, Rumpf zu weit zurück | 3 | 2 | **Warnung per Vibration** ab 9° Rückneigung (Firmware), Schutzbügel hinten mit Gummipuffern trifft zuerst, Akkus seitlich geschützt | P25b | gering |
| Rumpf knickt unter Last ein | Lendengelenk frei beweglich | 3 | 2 | Gasfedern tragen das Oberteil, **Sperre rastet automatisch**, sobald > 5 kg in den Schlaufen hängen (0,3 s), Hebel zum Sperren von Hand | P31 | gering |
| Baum fällt aus der Schlaufe | Schlaufe rutscht | 2 | 2 | Zahnleiste formschlüssig, Zugkraft über Motorstrom, Schneckengetriebe hält stromlos | P20 | gering |
| Baum lässt sich nicht lösen | Winde defekt | 3 | 1 | Schnappschäkel mit Zuglasche (Notöffnung von Hand) | P20 | gering |
| Stufenwechsel öffnet Schlaufe | Bedienfehler | 3 | 2 | gesperrt in der Firmware (Test) | F1 | gering |
| Quetschen an Kurbel und Schubstange | Finger, Kleidung zwischen Kurbel und Schiene | 2 | 2 | **Kurbelschutz aus transparentem PETG** an Knie und Ellbogen, Stange außen an der Schiene | P8, P32 | gering |
| Quetschen am Lendengelenk, Schulterschlitten | Haut, Jacke zwischen den Laschen | 2 | 2 | Spalt ≥ 25 mm oder ≤ 5 mm, Polster darüber, Gasfedern hinter dem Rahmen | P32 | gering |
| Bruch Schubstange / Gelenkkopf | Überlast, Verschleiß | 3 | 1 | Rohr knickt erst bei ≈ 16 kN (Betrieb ≤ 3,7 kN), Gelenkköpfe Edelstahl mit Reserve, jährlich auf Spiel prüfen | Wartung | gering |
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
Offen bleiben Sturz im Gelände (das Exo hat keinen Fußmotor und hält das Gleichgewicht nicht aktiv) und Passform: Sie werden in der Saison-Erprobung beobachtet und ins Protokoll eingetragen.
