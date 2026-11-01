# Festigkeit und Proportionen des Gerüsts (Entwurf 4)

Gerechnet mit `python3 tools/festigkeit.py`. Werkstoff EN AW-6082 T6 (Streckgrenze 250 MPa). Ziel: Spitzenlast mit
mindestens zweifacher Sicherheit (≤ 125 MPa), Dauerlast ≤ 50 MPa, damit das Gerüst viele Lastwechsel aushält.
Lasten: Spitzenmomente der Motoren und Aufrichten eines 3-m-Baums ([kraftverteilung.md](kraftverteilung.md)).

| Bauteil | Belastung | Moment Spitze / Dauer | vorher | σ Spitze / Dauer | neu | σ Spitze / Dauer | ok |
|---|---|---|---|---|---|---|---|
| Rückenrahmen senkrecht (je) | beide Schultern 2 × 100 Nm + Box 11 kg, auf 2 Rohre | 115 / 40 Nm | 25 × 25 × 2 | 88 / 31 MPa | **30 × 30 × 2** | 59 / 20 MPa | ja |
| Hüftschwinge | Hüftmoment 140 Nm + Last 510 N × 0,2 m (Stand auf einem Bein) | 240 / 80 Nm | 25 × 25 × 2 | 184 / 61 MPa | **35 × 35 × 3** | 64 / 21 MPa | ja |
| Oberschenkelschiene außen | Hüft- bzw. Kniemoment, größter Wert | 140 / 47 Nm | 25 × 25 × 2 | 107 / 36 MPa | **30 × 30 × 2.5** | 60 / 20 MPa | ja |
| Oberschenkelschiene innen (Teleskop) | wie außen, Überlappung >= 8 cm | 140 / 47 Nm | 20 × 20 × 2 | 178 / 60 MPa | **25 × 25 × 2.5** | 91 / 31 MPa | ja |
| Unterschenkelschiene außen | Kniemoment | 115 / 31 Nm | 25 × 25 × 2 | 88 / 24 MPa | **30 × 30 × 2** | 59 / 16 MPa | ja |
| Unterschenkelschiene innen | Kniemoment, unten kleiner | 90 / 25 Nm | 20 × 20 × 2 | 114 / 32 MPa | **25 × 25 × 2** | 69 / 19 MPa | ja |
| Schulterbügel | Schultermoment 100 Nm + Armlast 400 N × 0,1 m | 140 / 45 Nm | 20 × 20 × 2 | 178 / 57 MPa | **30 × 30 × 3** | 53 / 17 MPa | ja |
| Oberarmschiene | Ellbogenmoment 58 Nm + Baum am Handgelenk | 110 / 35 Nm | 22 × 22 × 2 | 112 / 36 MPa | **25 × 25 × 3** | 63 / 20 MPa | ja |
| Unterarmschiene | Schlaufenlast 400 N × 0,26 m | 105 / 35 Nm | 22 × 22 × 2 | 107 / 36 MPa | **25 × 25 × 3** | 61 / 20 MPa | ja |
| Querrohre Rücken | Torsion/Biegung aus einseitigem Armzug | 50 / 15 Nm | 20 × 20 × 2 | 64 / 19 MPa | **20 × 20 × 2** | 64 / 19 MPa | ja |

alle Teile ausreichend

**Ergebnis:** In Entwurf 4 waren Hüftschwinge, inneres Teleskoprohr am Oberschenkel und Schulterbügel zu schwach
(178–184 MPa bei Spitze); die übrigen Teile hatten wenig Reserve gegen Ermüdung. Mit den neuen Profilen liegt jedes Teil bei
Spitze unter 92 MPa und bei Dauerlast unter 31 MPa. Das 3D-Modell ist angepasst.

**Proportionen:** Am stärksten belastet sind Hüftschwinge (Hüftmoment und Gewicht laufen hindurch) und Schulterbügel (beide
Schultermomente). Dort sitzen die größten Profile. Querrohre und Unterschenkel unten bleiben schlanker.

**Lastlinie an der Hüfte (wichtig):** Die Abspreiz-Achse ist frei, sie überträgt kein Moment. Das funktioniert nur, weil
die Bodenkraft unter der Sohlenmitte (Steigbügel) senkrecht durch die Abspreiz-Achse läuft (beide liegen auf derselben
Linie durch dein Hüftgelenk). Stünde der Steigbügel seitlich neben dem Fuß, würde das Exo-Bein bei jedem Schritt seitlich
gegen deinen Oberschenkel drücken (bei 50 kg Last etwa 170 N). Deshalb: Steigbügel immer unter der Sohlenmitte.

**Noch zu tun:** Gelenkplatten und Gabeln (Lochleibung, Biegung der Platten) mit den Zeichnungen nachrechnen, wenn die
Fertigungsdateien für Entwurf 4 erzeugt werden.
