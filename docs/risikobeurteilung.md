# Risikobeurteilung (Eigenbau, Entwurf 3.2)

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
| Sturz mit Exo | Gelände, Kippen nach vorne | 3 | 3 | Ausfallschritt, Steigbügel mit Spikes, Sturzerkennung → Motoren weich, Aussteigen ohne Werkzeug | P4, P25 | mittel |
| Baum fällt aus der Schlaufe | Schlaufe rutscht | 2 | 2 | Zahnleiste formschlüssig, Zugkraft über Motorstrom, Schneckengetriebe hält stromlos | P20 | gering |
| Baum lässt sich nicht lösen | Winde defekt | 3 | 1 | Schnappschäkel mit Zuglasche (Notöffnung von Hand) | P20 | gering |
| Stufenwechsel öffnet Schlaufe | Bedienfehler | 3 | 2 | gesperrt in der Firmware (Test) | F1 | gering |
| Quetschen an Seilscheiben, Rollen | Finger, Kleidung | 2 | 2 | Abdeckungen, Seile in eigener Ebene außen | P8 | gering |
| Seilriss | Verschleiß, scharfe Kante | 2 | 2 | Seil zieht nur (Ausfall = locker), Dyneema mit 4-facher Reserve, Prüfung alle 40 h | Wartung | gering |
| Druckstellen, Scheuern | Achsen fluchten nicht | 2 | 3 | Maßblatt, Langlöcher, Polster, freie Abspreiz-/Drehgelenke | P1–P3 | mittel |
| Überhitzung Motoren/Akku | Dauerlast | 2 | 2 | Motor-Temperatur aus dem Motor, Strombegrenzung, Akku-BMS | P27 | gering |
| Akku-Kurzschluss | Nässe, Scheuerstelle | 4 | 1 | Hauptsicherung, IP67-Stecker, Isolationsprüfung | P5, P29 | gering |
| Überlastung des Körpers | Exo verdeckt, wie schwer es ist | 3 | 2 | Verstärkung höchstens 3 (Mensch spürt immer ≥ 25 %), Stufen | P16 | gering |
| Arbeiten mit Motorsäge | Kettensäge im unterstützten Modus | 4 | 2 | **verboten**: Bedienungsanleitung, Exo vorher aus | – | Nutzer |
| Dritte in der Nähe | Baum kippt aufs Plateau | 3 | 2 | Gefahrenbereich 4 m freihalten | – | Nutzer |

**Fazit:** Mit den Maßnahmen ist das Restrisiko für die eigene Nutzung nach bestandener Abnahme vertretbar.
Offen bleiben Sturz im Gelände und Passform: Sie werden in der Saison-Erprobung beobachtet und ins Protokoll eingetragen.
