# Festigkeit und Proportionen des Gerüsts (Entwurf 5)

Gerechnet mit `python3 tools/festigkeit.py` (die Tabellen unten sind die Ausgabe des Skripts). Werkstoff der Rohre
EN AW-6082 T6 (Streckgrenze 250 MPa). Ziel: Spitzenlast mit mindestens zweifacher Sicherheit (≤ 125 MPa), Dauerlast
≤ 50 MPa, damit das Gerüst viele Lastwechsel aushält. Lasten: Spitzenmomente der Motoren in der Firmware
(Hüfte, Knie 110 Nm, Schulter 100 Nm, Ellbogen 56 Nm, Rumpf beugen 80 Nm) und Aufrichten eines 3-m-Baums
([kraftverteilung.md](kraftverteilung.md)).

## Rahmenrohre

| Bauteil | Belastung | Moment Spitze / Dauer | Profil | σ Spitze / Dauer | ok |
|---|---|---|---|---|---|
| Rückenrahmen senkrecht (je) | Baum 400 N × 0,4 m vor dem Lendengelenk + Oberteil, auf 2 Rohre; Schultern 2 × 100 Nm | 130 / 45 Nm | **30 × 30 × 2** | 66 / 23 MPa | ja |
| Hüftschwinge | Hüftmoment 110 Nm + Last 824 N × 0,2 m (Stand auf einem Bein mit Baum) | 275 / 99 Nm | **35 × 35 × 3** | 73 / 26 MPa | ja |
| Oberschenkelschiene außen | Hüft- bzw. Kniemoment 110 Nm + Querkraft der Schubstange | 140 / 47 Nm | **30 × 30 × 2** | 71 / 24 MPa | ja |
| Oberschenkelschiene innen (Teleskop) | wie außen, Überlappung >= 8 cm | 140 / 47 Nm | **25 × 25 × 2.5** | 91 / 31 MPa | ja |
| Unterschenkelschiene außen | Kniemoment | 115 / 31 Nm | **30 × 30 × 2** | 59 / 16 MPa | ja |
| Unterschenkelschiene innen | Kniemoment, unten kleiner | 90 / 25 Nm | **25 × 25 × 2** | 69 / 19 MPa | ja |
| Schulterbügel | Schultermoment 100 Nm + Armlast 400 N × 0,1 m | 140 / 45 Nm | **30 × 30 × 3** | 53 / 17 MPa | ja |
| Oberarmschiene | Ellbogenmoment 56 Nm + Baum am Handgelenk | 110 / 35 Nm | **25 × 25 × 3** | 63 / 20 MPa | ja |
| Unterarmschiene | Schlaufenlast 400 N × 0,26 m | 105 / 35 Nm | **25 × 25 × 3** | 61 / 20 MPa | ja |
| Querrohre Rücken | Torsion/Biegung aus einseitigem Armzug | 50 / 15 Nm | **20 × 20 × 2** | 64 / 19 MPa | ja |

## Teleskope

| Teleskop | Außenrohr | Innenrohr | Spiel | ok |
|---|---|---|---|---|
| Oberschenkel | 30 × 30 × 2 (innen 26) | 25 × 25 × 2.5 | 1 mm | ja |
| Unterschenkel | 30 × 30 × 2 (innen 26) | 25 × 25 × 2 | 1 mm | ja |

## Gelenkteile des Antriebs

| Teil | Nachweis | Beanspruchung | zulässig / Grenze | Sicherheit | ok | Hinweis |
|---|---|---|---|---|---|---|
| Schubstange Knie Ø 12 × 2, L 310 | Knicken bei 110 Nm, Hebel 55° ausgelenkt | 3196 N | 17616 N | 5.5 | ja |  |
| Schubstange Ellbogen Ø 10 × 1,5, L 280 | Knicken bei 56 Nm | 1953 N | 9861 N | 5.1 | ja |  |
| Gelenkkopf M8 (Knie, Rumpf) | statische Tragzahl | 3196 N | 12900 N | 4.0 | ja | Katalogwert prüfen |
| Gelenkkopf M6 (Ellbogen) | statische Tragzahl | 1953 N | 8150 N | 4.2 | ja | Katalogwert prüfen |
| Zapfen Knie als Schraube M8 8.8 (Gewinde im Biegequerschnitt) | Biegung im Gewindegrund | 841 MPa | 640 MPa | 0.8 | NEIN | **so nicht bauen** (Gegenbeispiel): bricht auf Dauer im Gewinde |
| Zapfen Knie als Passschraube Ø 8 (ISO 7379, 12.9) | Biegung im glatten Schaft, Spitze | 445 MPa | 1080 MPa | 2.4 | ja | Bohrung 8 H7 reiben |
| … dieselbe Passschraube, typische Last (50 Nm) | schwellend, zulässig ≈ 400 MPa (Dauerfestigkeit Schaft 12.9) | 151 MPa | 400 MPa | 2.6 | ja |  |
| Zapfen Rumpf beugen, Passschraube Ø 8 (12.9) | 80 Nm, Hebel 50 mm | 246 MPa | 1080 MPa | 4.4 | ja |  |
| Zapfen Ellbogen als Passschraube Ø 6 (12.9) | Biegung im glatten Schaft, Spitze | 506 MPa | 1080 MPa | 2.1 | ja | Bohrung 6 H7 reiben |
| Lochleibung Kurbel Knie (Alu 10 mm, Ø 8) | Flächenpressung am Zapfen | 40 MPa | 375 MPa | 9.4 | ja |  |
| Lochleibung Kurbel Ellbogen (Alu 8 mm, Ø 6) | Flächenpressung am Zapfen | 41 MPa | 375 MPa | 9.2 | ja |  |
| Kurbel am Abtrieb RS04: 9 × M5 auf Ø 36 | Umfangskraft je Schraube gegen Reibschluss (6,4 kN × 0,15) | 679 N | 960 N | 1.4 | knapp | knapp: 2 Stifte Ø 6 setzen (in der Zeichnung vorgesehen), Lochbild am Motor prüfen |
| Kurbel am Abtrieb RS03: 6 × M4 auf Ø 30,4 | Umfangskraft je Schraube gegen Reibschluss (4,0 kN × 0,15) | 614 N | 600 N | 1.0 | NEIN | Reibschluss reicht nicht sicher: 2 Stifte Ø 5 setzen, sonst tragen die Schrauben auf Abscheren |
| Sperrbolzen Lendensperre Ø 8 | Abscheren bei 182 Nm am Lendengelenk | 52 MPa | 300 MPa | 5.8 | ja | Bolzen braucht eine eigene Führung; der Hubmagnet darf die Querkraft nicht tragen |
| Rastsegment Edelstahl 4 mm | Lochleibung | 81 MPa | 315 MPa | 3.9 | ja |  |
| Wägezellen im Steigbügel (4 × 50 kg), beim Auftreten | Mensch + selbst getragener Lastanteil auf einem Bein, × 1,5 | 1398 N | 1962 N | 1.4 | knapp | reicht; Zellen so einbauen, dass ein Anschlag bei ≈ 250 kg trägt (Sprung, Stolpern) |

Stangenkraft: Knie 3196 N (Spitze), 1088 N (typisch); Ellbogen 1953 N; Rumpf beugen 1765 N.

Rahmen: alle Teile ausreichend; Teleskope: passen; Gelenkteile: offen: Kurbel am Abtrieb RS03: 6 × M4 auf Ø 30,4

## Was daraus folgt

**Kurbelzapfen (neu, wichtig):** Die Schubstange greift einseitig an der Kurbel an, der Zapfen wird also gebogen. Als
normale Schraube M8 mit Gewinde im Biegequerschnitt bricht er auf Dauer (841 MPa im Gewindegrund). Deshalb sind die
Zapfen jetzt **Passschrauben ISO 7379, Festigkeit 12.9** (Ø 8 an Knie und Rumpf, Ø 6 am Ellbogen): Der glatte Schaft
trägt, das Gewinde liegt hinter der Kurbel. Die Bohrungen werden 0,2 mm kleiner gelasert und auf H7 gerieben. Der
Gelenkkopf sitzt mit einer Distanzhülse so nah wie möglich an der Kurbel (Abstand Mitte Gelenkkopf bis Kurbel ≤ 7 mm).

**Kurbel am Motorabtrieb:** Die Schrauben allein (Reibschluss) reichen am RS03 nicht sicher und am RS04 nur knapp.
In beiden Kurbeln sitzen deshalb zwei Passstifte. Das Lochbild am Motor muss vor dem Lasern am echten Teil
nachgemessen werden (im Handbuch stehen keine Toleranzen).

**Teleskope:** Das Oberschenkel-Außenrohr war mit 30 × 30 × 2,5 geplant. Innen bleiben dann genau 25,0 mm, das
Innenrohr 25 × 25 ließe sich nicht schieben. Jetzt 30 × 30 × 2 (innen 26 mm, 1 mm Spiel, mit Gleitband 0,5 mm
ausgefüttert). Die Spannung steigt dadurch von 60 auf 71 MPa und bleibt weit unter der Grenze.

**Lendensperre:** Der Sperrbolzen trägt bei gesperrtem Rumpf bis zu 182 Nm am Lendengelenk. Er braucht eine eigene
Führungsbuchse im Rahmen. Der Hubmagnet zieht den Bolzen nur, er darf keine Querkraft bekommen (sein Anker würde
klemmen oder sich verbiegen).

**Lastlinie an der Hüfte:** Die Abspreiz-Achse ist frei, sie überträgt kein Moment. Das funktioniert nur, weil
die Bodenkraft unter der Sohlenmitte (Steigbügel) senkrecht durch die Abspreiz-Achse läuft (beide liegen auf derselben
Linie durch dein Hüftgelenk). Stünde der Steigbügel seitlich neben dem Fuß, würde das Exo-Bein bei jedem Schritt seitlich
gegen deinen Oberschenkel drücken (bei 50 kg Last etwa 170 N). Deshalb: Steigbügel immer unter der Sohlenmitte.
Beim Stehen auf einem Bein mit Baum hält trotzdem **dein** Körper das Becken seitlich waagerecht, das Exo hilft dabei
nicht (siehe [durchsicht-2026-11.md](durchsicht-2026-11.md), offene Punkte).

**Nicht gerechnet (noch offen):** Schweißnähte und Klemmschellen der Rohre, Gelenkplatten der Hüfte (Lagersitz),
Querkraft auf die Abtriebslager der Motoren (RobStride nennt keinen Wert, Frage steht in
[anfrage-robstride.md](anfrage-robstride.md)), Ermüdung der Schweißnähte. Vor der ersten Fahrt mit Last am Körper:
Belastungsprobe am Prüfstand mit 1,5-facher Spitzenlast und Rissprüfung (Farbeindringmittel) an den Nähten.
