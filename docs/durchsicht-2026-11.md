# Kritische Durchsicht, November 2026

Das ganze Projekt wurde noch einmal gegengelesen: Firmware, Simulation, Festigkeit, Schaltplan, Fertigungsdateien und
Dokumente gegeneinander. Die Frage war jedes Mal: **Was passiert, wenn genau das ausfällt, und merkt es jemand?**
Hier steht, was gefunden wurde, was davon behoben ist und was du entscheiden musst.

Kurz: Das Konzept trägt. Die meisten Fehler lagen an den Nahtstellen (Firmware ↔ echte Hardware, Zeichnung ↔
Rechnung, Schaltplan ↔ Bauteil) und wären erst am Prüfstand oder am Körper aufgefallen.

## 1. Behoben

### Firmware (Rev. S1)

| Nr | Was war falsch | Folge am Körper | Jetzt |
|---|---|---|---|
| 1 | Alle 17 Wägezellen hatten dieselbe Grenze von 150 N. Die Fußzellen tragen aber dein Gewicht, die Schlaufen den Baum. | Beim ersten Schritt oder beim ersten Baum über 15 kg: „Sensorfehler“, alles stromlos. | Grenze je Zelle: Manschetten 150 N, Schlaufen 650 N, Füße 2600 N |
| 2 | Ob ein Messwert neu ist, wurde nicht geprüft. Hängt der Messwandler, blieb der letzte Wert stehen. | Beim Mitgehen folgt das Gelenk der Kraft. Ein eingefrorener Wert von 20 N heißt: **Das Gelenk fährt weiter, bis zum Anschlag.** | Jede Zelle muss frische Werte liefern (20 ms bzw. 0,3 s), der Messwandler wird mit Prüfsumme gelesen, ein Neustart des Wandlers wird erkannt |
| 3 | Das „Nullen beim Einschalten“ stand in der Doku, war aber nicht programmiert. | Jeder nachgezogene Gurt hätte als Dauerkraft gezählt, das Exo wäre „weggelaufen“. | Die 2 s nach dem Einschalten werden gemittelt und abgezogen; mit zu viel Vorlast startet es nicht |
| 4 | Bei Sturz sollten die Beine 1,5 s bremsen. Nach 0,1 s fiel aber das Schütz ab, und die Bremswirkung war sehr klein. Ein einziger falscher Messwert des Lagesensors hätte „Sturz“ ausgelöst. | Dämpfung wirkungslos; Fehlauslösung mitten in der Arbeit. | Schütz bleibt für die Bremszeit zu, Dämpfung 15/20 Nm·s/rad, Sturz muss 20 ms anliegen |
| 5 | Jeder Fehler hieß: sofort alles stromlos. | In der Hocke oder mit Baum knicken die Exo-Beine weg. | Abgestuft: Sensorfehler nehmen die Hilfe sofort weg, bremsen aber die Beine; nur Not-Aus und Motorausfall schalten hart |
| 6 | „Akku leer“ kam ohne Vorwarnung als harter Fehler. Bricht die Spannung unter Last kurz ein, ebenso. | Kraft weg im ungünstigsten Moment (beim Aufstehen ist der Strom am höchsten). | Warnung, dann Auslaufen in 2 s; bei Einbruch unter Last wird nur die Kraft zurückgenommen |
| 7 | Der Gelenk-Nullpunkt (wo ist „gestreckt“?) ließ sich nirgends einstellen. Der Motorwinkel wurde ungeprüft verwendet. | Schwerkraftausgleich und Anschläge in der Software hätten für eine falsche Stellung gerechnet. | Befehl `gelenk0`, Winkelprüfung, Umrechnung des Einschaltwinkels (0–360°) |
| 8 | Ob ein Motor wirklich eingeschaltet hat, wurde nicht geprüft. Fiel der Teensy bei geschlossenem Schütz aus, hielt der Motor seinen letzten Befehl. | Ein Gelenk ohne Kraft bleibt unbemerkt; ein Gelenk mit altem Befehl drückt weiter. | Betriebsart jedes Motors wird geprüft; jeder Motor bekommt einen eigenen Wächter (0,1 s ohne Befehl = aus) |
| 9 | Lagesensor-Ausfall blieb unbemerkt (Sturzerkennung und Rumpf-Vorsteuerung blind); Überlastung des Regeltakts ebenso. | Stille Verschlechterung. | Beides wird überwacht und führt zum weichen Abschalten |
| 10 | Ein langer Druck auf den Daumentaster öffnete die Schlaufe auch mit Baum. | Baum fällt bei einem Versehen. | Unter Last erst Vorwarnung, dann weiter halten |
| 11 | Am Prüfstand mussten alle 17 Zellen angeschlossen sein. | Der erste Aufbau (ein Motor, eine Zelle) wäre gar nicht gestartet. | `pruefstand` aktiviert nur die nötige Zelle, `zelle <nr> 1` nimmt weitere dazu |
| 12 | Anzeigen „Akku schwach“ und „Motor warm“ standen in der Doku, wurden aber nie gesendet. | – | werden gesendet |

Geprüft mit: `make -C firmware test` (über 100 Prüfungen, davon rund 30 neu), einem neuen Test, in dem der echte
Teensy-Code auf dem PC gegen nachgebildete Motoren und Messwandler läuft (`firmware/test/teensy_host.cpp`), und dem
Bau mit dem ARM-Compiler. **Nicht geprüft** ist alles, was nur der echte Motor zeigt, siehe Abschnitt 3.

### Mechanik und Fertigung

| Nr | Was war falsch | Jetzt |
|---|---|---|
| 13 | Die Zapfen, an denen die Schubstangen an den Kurbeln hängen, waren als Gewinde M8/M6 gezeichnet. Die Stange greift einseitig an, das Gewinde wird gebogen: 841 MPa im Gewindegrund, das bricht nach einiger Zeit. | Passschrauben ISO 7379 in 12.9 (glatter Schaft trägt), Bohrungen zum Reiben; in Zeichnungen, Teileliste, Bestellliste, Montage |
| 14 | Die Kurbeln sitzen nur mit Schrauben am Motorabtrieb. Am RS03 reicht die Reibung nicht sicher, am RS04 knapp. | je zwei Passstifte |
| 15 | Oberschenkelrohr außen 30 × 30 × 2,5 hat innen genau 25,0 mm. Das Innenrohr 25 × 25 lässt sich nicht schieben. | 30 × 30 × 2 (1 mm Spiel, Spannung bleibt klein) |
| 16 | Die Bohrschablonen waren für Rohre 25 und 20 mm, gebaut wird mit 30 und 25 mm. Die Stückliste nannte noch die alten, zu schwachen Rohre. | Schablonen 30 / 25, Stückliste angepasst |
| 17 | `tools/festigkeit.py` rechnete noch Entwurf 4 und nur die Rohre. | rechnet Entwurf 5, dazu Schubstangen, Gelenkköpfe, Zapfen, Kurbelsitz, Lendensperre, Teleskop-Spiel |

### Elektrik (Schaltplan und Bestellliste; gebaut ist noch nichts)

| Nr | Was war falsch | Jetzt |
|---|---|---|
| 18 | Der Not-Aus-Pilz lag in der Hauptleitung. Sein Kontakt ist für wenige Ampere gebaut, nicht für 40 A Gleichstrom. Er verschweißt oder brennt ab, und dann ist der Not-Aus wirkungslos. | Der Pilz unterbricht die Schützspule; das Schütz trennt |
| 19 | Als Schütz war ein Kfz-Relais für 24 V eingetragen. Der Akku hat voll 42 V, beim Bremsen 46 V. | Gleichstromschütz für ≥ 60 V / 100 A |
| 20 | Im Schaltbild der Steuerung hing die 5-V-Versorgung hinter dem Schütz: Der Teensy hätte sich nie selbst einschalten können. | Steuerung vor dem Schütz (so war es im Verdrahtungsplan schon gemeint) |
| 21 | Sicherungen 40 A / 15 A ohne Rechnung; Stromschiene mit 8 statt 10 Abgängen. | Startwerte 60 A / 20 A / 15 A, 10 Abgänge, Messung als Prüfpunkt P43 |
| 22 | Bremswiderstand 10 Ω nimmt nur 210 W auf. | 4,7 Ω empfohlen, Messung in P37 |
| 23 | Übliche HX711-Platinen messen nur 10-mal je Sekunde. | Hinweis zum Umlöten auf 80/s, Firmware überwacht |

### Dokumente

Verdrahtung, Stückliste, Bestellliste, Montage (Abschnitte 3–4 beschrieben noch Seile und Trommeln), Wartung,
Kraftverteilung, Bedienung, Steuerung, Prüfprotokoll (neu P40–P46), Risikobeurteilung (8 neue Zeilen) sind auf
Entwurf 5 und den neuen Stand gebracht. Der ESP32-Sketch wird jetzt bei jedem Push mit dem echten Compiler gebaut.

## 2. Offen: Das musst du entscheiden

Diese Punkte ändern dein Konzept oder kosten Geld. Ich habe sie **nicht** umgesetzt.

1. **Hartes Abschalten bleibt ein Risiko.** Bei Not-Aus, Sicherungsfall oder leerem Akku sind die Beine sofort
   kraftlos. Das Exo wiegt 44 kg; ohne Motoren trägst du davon den Teil, der über die Gurte kommt, plus den Baum.
   Vorschlag: am Prüfstand messen, wie schwer sich ein stromloses Knie (RS04, 1 : 1) von Hand beugen lässt. Wenn es zu
   leicht nachgibt, braucht das Knie eine mechanische Stütze (Gasfeder oder Freilauf-Sperre in Streckrichtung).
2. **Lendensperre umdrehen.** Jetzt gilt: Strom an = gesperrt. Fällt der Strom aus, während du einen Baum hältst,
   löst die Sperre. Sicherer wäre: Feder sperrt, Magnet löst. Dann bleibt der Rumpf bei jedem Fehler gestützt.
   Nachteil: Mit leerem Akku ist der Rumpf steif, bis du den Hebel von Hand ziehst.
3. **Rumpfmotoren später?** Die zwei Rumpfantriebe bringen 2 Motoren, etwa 3 kg und 800 € und die unsicherste
   Sensorik im Projekt: Die Absicht „vorbeugen“ wird an den Schultergurten gemessen, an denen auch das Gewicht des
   Exos und des Baums zieht. Das lässt sich in der Simulation nicht ehrlich prüfen. Vorschlag: Exo Nr. 1 zunächst mit
   Gasfedern und Sperre bauen (wie Entwurf 4), die Rumpfkapseln nachrüsten, wenn Arme und Beine laufen. Firmware und
   Zeichnungen bleiben, in der Firmware reicht `jointMask`.
4. **Strommessung einbauen.** Es gibt keinen Stromsensor. Ob Akku und Sicherungen den Spitzenstrom mitmachen, ist
   die größte unbekannte Zahl im Projekt (der Hersteller nennt 90 A Spitze **im Motor**; was davon am Akku ankommt,
   hängt von der Drehzahl ab). Ein Hall-Sensor (z. B. ACS758, ≈ 10 €) am freien Analogeingang würde eine echte
   Leistungsbegrenzung erlauben statt der jetzigen Schätzung über die Spannung.
5. **Seitliches Kippen.** Die Abspreiz-Achse der Hüfte ist frei. Beim Stehen auf einem Bein hält deine eigene
   Hüftmuskulatur Oberkörper, Exo-Oberteil und Baum seitlich. Mit 44 kg + Baum ist das spürbar. Eine stärkere
   Zentrierfeder hilft etwas; mehr geht nur mit einem weiteren Antrieb. Am Körper beobachten (P21).
6. **Gewicht 44 kg.** Seit Entwurf 4 (32 kg) sind 12 kg dazugekommen. Ein Exo, das mit leerem Akku nicht mehr zu
   tragen ist, braucht einen Plan für den Rückweg (zweiter Akku ist vorgesehen; Parkstütze am Fahrzeug).

## 3. Offen: Zeigt erst die Hardware

| Frage | Warum wichtig | Prüfpunkt |
|---|---|---|
| Stimmt der Motorwinkel nach dem Einschalten, auch wenn das Gelenk ausgeschaltet bewegt wurde? | Ohne das sind Schwerkraftausgleich und Software-Anschläge falsch. Laut Handbuch ja (0–360° am Abtrieb), nachgemessen hat es hier niemand. Falls nein: Referenzschalter je Gelenk. | P40 |
| Wirkt der Wächter im Motor (`canTimeout`)? | Zweite Sicherung, wenn die Steuerung ausfällt | P41 |
| Dauermoment der Kapsel (geschätzt 28 Nm) | Bestimmt, wie lange das Exo einen Baum wirklich hält | P33 |
| Strom und Spannung bei Spitzenlast | Sicherungen, Akku-Abschaltung | P43 |
| Querkraft auf das Abtriebslager (Schubstange am Knie: 3,2 kN) | Der Hersteller nennt keinen zulässigen Wert | Anfrage ([anfrage-robstride.md](anfrage-robstride.md), Frage 7) |
| Sturzerkennung erfasst nur Kippen nach vorn/hinten | Seitliches Fallen wird nicht erkannt | Erweiterung, wenn der Lagesensor am Körper läuft |
| Schweißnähte, Klemmschellen, Hüft-Gelenkplatten | nicht gerechnet | P46 |

## 4. Reihenfolge, die ich vorschlage

1. Anfrage an RobStride um die neuen Fragen ergänzt abschicken.
2. Stufe 1 der Bestellliste (Prüfstand) mit dem **richtigen Schütz und Not-Aus** bestellen, dazu den Stromsensor.
3. Am Prüfstand zuerst P40, P41, P42, P43, P33. Diese fünf entscheiden, ob Annahmen der Firmware stimmen.
4. Danach erst Laserteile für den ganzen Körper bestellen (Lochbilder am echten Motor nachgemessen).
