# Optimierung: Kraft, Gewicht, Herstellkosten (Entwurf 5 → Entwurf 6, Stand 16. Nov 2026)

Ziel von Michael: in allen drei Disziplinen **doppelt so gut** wie Entwurf 5, in einzelnen Schritten. Jede Zahl hier
rechnet [`tools/optimierung.py`](../tools/optimierung.py) aus der Stückliste der technischen Zeichnung, den Preisen aus
[stueckliste.md](stueckliste.md) und den Herstellerdaten der Motoren ([realismus-check.md](realismus-check.md)).
`python3 tools/optimierung.py --schritt 1` zeigt den Stand nach Schritt 1 usw., `--stueckliste` die neue Teileliste.

Die anschauliche Fassung mit Zeichnung: [visualisierung/exo-entwurf6.html](../visualisierung/exo-entwurf6.html).

## Ergebnis in einem Satz

**Kraft ×2,2 und Kosten ×2 sind mit echten Kaufteilen erreichbar, Gewicht nur ×1,6 bis ×1,8.** Halbes Gewicht ginge
nur, wenn auch die Schultermotoren wegfallen; dann kann das Exo den Baum nicht mehr selbst auf die Ladefläche heben.

| | Entwurf 5 | nach Schritt 1–3 | mit Optionen (Schritt 4) | Ziel | erreicht? |
|---|---|---|---|---|---|
| Masse getragen | 47,2 kg | **29,4 kg** (× 1,61) | **25,9 kg** (× 1,83) | ≤ 23,6 kg | **nein**, siehe unten |
| Material ein Exo | 5.885 € | **3.148 €** (× 1,87) | 2.872 € ohne CFK (× 2,05), 3.112 € mit CFK | ≤ 2.943 € | **ja**, ohne CFK-Rohre |
| Kraft (schwerster Baum, Mittel über 7 Arbeitsschritte) | 1,0 | **× 2,24** | × 2,25 | × 2 | **ja**, aber nicht bei jedem Schritt |

Der Kostenwert 5.885 € ist die Summe der Einzelzeilen der Stückliste (Mitte der Spannen); die Kopfzeile dort nennt
6.000–7.100 €. Ich rechne mit dem kleineren Wert, damit der Faktor nicht geschönt ist.

## Was „Kraft“ hier heißt

Nicht das Motormoment, sondern was du damit schaffst: **der schwerste Baum, den du in einem Arbeitsschritt so lange
halten oder aufrichten kannst.** Gerechnet wird je Gelenk: deine Kraft (Harbo 2012, Ausdauer nach Rohmert) + was das Exo
liefert − was das Exo für sich selbst braucht − das Gewicht deines eigenen Arms bzw. Oberkörpers. Das schwächste Gelenk
begrenzt. Momente je kg Baum aus [kraftverteilung.md](kraftverteilung.md) (Baum 3 m).

## Ausgangswerte Entwurf 5

- 47,2 kg getragen (ein Akku), davon 17,2 kg Gelenkkapseln mit 10 Motoren.
- 5.885 € Material, davon 2.605 € Motoren.
- Kraft: siehe Tabelle in Schritt 2. Der Realismus-Check hatte schon gezeigt: **An den Armen wirkt das Exo, an Beinen,
  Hüfte und Rumpf kaum** (× 1,0 bis × 1,3), weil dort deine eigenen Muskeln stark sind (250 Nm) und der Motor viel
  davon nur für das Exo selbst verbraucht.

Daraus folgt die Richtung für alle drei Schritte: **Motoren nur dort, wo der Mensch schwach ist (Schulter), Halten
mit Sperren statt mit warmen Motoren, Beine als passiver Lastweg in den Boden.** So arbeiten auch erprobte Geräte:
Lockheed FORTIS ist ganz passiv und leitet 16 kg Werkzeug über die Beine in den Boden
([New Atlas](https://newatlas.com/fortis-exoskeleton-us-navy/33403)), Ekso EVO hebt die Arme mit Federn
(4,1 kg, 2,4–7,1 kg Hilfe je Arm, [Datenblatt](https://eksobionics.com/static/46a879fd6bdb04b7442754362c4e5e8d/Ekso-EVO-Technical-Sheet.pdf)).

## Schritt 1: Gewicht

| Maßnahme | Masse | Kosten |
|---|---|---|
| G1 Beine ohne Motoren: Hüfte und Knie laufen frei mit, Exo-Gewicht und Last gehen über die Schienen in den Steigbügel | −10,65 kg | −1.638 € |
| G2 Rumpfmotoren weg: Aufrichten tragen Gasfedern und Lendensperre, Drehen bleibt passiv (offene Entscheidung 3) | −4,00 kg | −588 € |
| G3 Ellbogenmotoren weg: den Ellbogen bewegst du selbst, halten übernimmt in Schritt 2 eine Sperre | −3,72 kg | −588 € |
| G4 Leichtbau ohne Funktionsverlust: Taschen in Laserteilen, ein Akkuschacht, kleinere Box und Schütz | −1,32 kg | 0 € |

**Stand: 27,5 kg (× 1,72), 3.070 € (× 1,92), Kraft × 0,74.** Ohne Ausgleich wäre das Exo jetzt schwächer: Ellbogen
und Halten fehlen. Das holt Schritt 2.

Was dabei bleibt: Teleskopschienen (Größe verstellbar), Steigbügel, Hüft-Abspreizgelenk, Lendengelenk mit Sperre und
Gasfedern, Rumpfdrehung, Manschetten, Schlaufengreifer, Kapseln mit Kühlkörper für die zwei Schultermotoren,
Dichtheit und Verkleidung. Was wegfällt: 8 von 10 Motoren mit Kapseln, Kurbeln, Schubstangen, Kabeln; Fußkontakt-
und Rumpf-Wägezellen; Wägezellen in den Beinmanschetten.

Geprüft: Die Hüftschwinge trägt ohne Motormoment nur noch Last × Hebel (824 N × 0,2 m ≈ 165 Nm) und reicht als
35 × 35 × 2 (≈ 62 MPa, Grenze 125 MPa). Eine leichtere RobStride am Ellbogen (RS06: 621 g, 36 / 11 Nm, ≈ 199 €,
[OpenELAB](https://openelab.io/fi/products/robstride06-qdd-joint-motor-module)) habe ich verworfen: Sperre plus dein
Arm hält mehr und wiegt weniger.

## Schritt 2: Kraft

| Maßnahme | Masse | Kosten |
|---|---|---|
| K1 Sperren an Schulter, Ellbogen und Knie (blockierbare Gasfedern): halten ohne Wärme; beim Aufrichten Arme sperren und aus den Beinen heben | +1,98 kg | +475 € |
| K2 Schulter auf 180 Nm gesperrtes Haltemoment (Federhebel 60 mm); Gerüst nachgerechnet | +0,12 kg | +20 € |

**Stand: 29,6 kg (× 1,59), 3.565 € (× 1,65), Kraft × 2,24.**

**Warum Sperren:** Der Realismus-Check hat gezeigt, dass ein stehender Motor nach Herstellerangabe 100 Nm nur 1,5 s
hält, 40 Nm gut 2 min, dauerhaft ≈ 20 Nm. Ein Baum wird aber meistens *gehalten*. Eine gesperrte Gasfeder hält
dagegen beliebig lange, ohne Strom und ohne Wärme. Der Motor ist dann nur noch zum Bewegen da.

**Referenzbauteile** (Herstellerangaben):

| Gelenk | Bauteil | Datenblatt | ausgelegt auf | Haltemoment |
|---|---|---|---|---|
| Schulter | blockierbare Gasfeder, starr auf Druck (SUSPA HY4 / Bansbach Typ P), Rohr 28 mm, F1 400 N | starr bis 10 kN, F1 80–1.000 N, Auslösekraft 0,25 × F1, Weg 2,5–3,5 mm, −20 bis +60 °C ([SUSPA](https://suspa.com/uk/products/locking-gas-struts/rigid-locking), [Bansbach](https://www.albertjagger.co.uk/contentfiles/redirects/Lockable_Gas_Springs.pdf)) | 3 kN × Hebel 60 mm | **180 Nm** |
| Ellbogen | blockierbare Gasfeder, starr auf Zug (SUSPA HY1), Rohr 22 mm, F1 80 N | starr auf Zug bis 3,5 kN | 2,5 kN × 32 mm | **80 Nm** |
| Knie | wie Ellbogen | | 2,5 kN × 40 mm | **100 Nm** |

- **Bedienung:** Daumenhebel an jedem Griff über Bowdenzug: drücken = Arm frei, loslassen = Arm hält; mit Raste für
  „immer frei“. Knie: Rasthebel am Hüftgurt „Gehen / Halten“. Alles mechanisch, funktioniert ohne Strom. Ein Mikroschalter
  je Hebel meldet der Firmware den Zustand (Motor arbeitet nicht gegen die Sperre, Warnton bei gesperrtem Knie im Gehen).
- **Federwirkung:** Die Schulterfeder (F1 400 N) hebt den Arm wie eine passive Armstütze mit ≈ 12 Nm, ohne Strom. Ellbogen-
  und Kniefeder sind auf die schwächste Füllung gesetzt und drücken nur ≈ 3 Nm gegen.
- **Aufrichten mit gesperrten Armen:** Arme sperren, aus den Beinen hochdrücken. Die Arme wirken dann als starrer Hebel,
  deine Beine (250 Nm je Seite) heben. So hebt man ohnehin richtig.
- **Gerüst bei 180 Nm an der Schulter** (nach [festigkeit.md](festigkeit.md)): Schulterbügel 30 × 30 × 3 ≈ 68 MPa,
  Oberarmschiene 25 × 25 × 3 ≈ 103 MPa (Grenze mit Sicherheit 2: 125 MPa). Die Federkraft (3 kN) läuft über Schultergabel
  und Hebel, nicht über das Motorlager. Am Ellbogen bleibt es bei 80 Nm, weil die Oberarmschiene sonst zu knapp wird.
- **Geschätzt, nicht aus dem Datenblatt:** Masse der Federn (0,20–0,33 kg) und Preis (≈ 60 € je Stück in Einzelmengen).
  Anfrage an SUSPA oder Bansbach mit Hub, F1, Einbaulänge stellen, bevor bestellt wird.

**Kraft je Arbeitsschritt** (schwerster Baum in kg, in Klammern was zuerst an die Grenze kommt):

| Arbeitsschritt | Dauer | ohne Exo | Entwurf 5 | nach Schritt 1 | nach Schritt 2 | Faktor |
|---|---|---|---|---|---|---|
| Aufrichten (Stamm steht am Boden) | 10 s | 17 | 41 (Ellbogen) | 19 (Ellbogen) | 45 (Gleichgewicht) | 1,1 |
| Aufladen (Arm hebt den Baum auf die Ladefläche) | 10 s | 15 | 43 (Ellbogen) | 24 (Ellbogen) | 49 (Schulter) | 1,1 |
| Hochheben und halten (Brusthöhe) | 10 s | 15 | 35 (Schulter) | 24 (Ellbogen) | 62 (Ellbogen) | 1,8 |
| Hochheben und halten (Brusthöhe) | 2 min | 6 | 17 (Schulter) | 11 (Ellbogen) | 50 (Ellbogen) | 2,9 |
| Tragen wie eine Fackel | 10 s | 19 | 44 (Schulter) | 45 (Schulter) | 90 (Schulter) | 2,0 |
| Tragen wie eine Fackel | 2 min | 8 | 22 (Schulter) | 23 (Ellbogen) | 79 (Schulter) | 3,6 |
| Tragen wie eine Fackel | dauerhaft | 2 | 7 (Schulter) | 7 (Ellbogen) | 42 (Hüfte) | 6,2 |

Ehrlich gelesen:
- **Halten und Tragen: × 2 bis × 6.** Das ist der eigentliche Gewinn. Einen 3-m-Baum (41 kg) wie eine Fackel tragen
  geht jetzt dauerhaft; vorher 10 Sekunden.
- **Aufrichten: × 1,1.** Hier begrenzt nicht die Kraft, sondern das **Gleichgewicht**: Ab ≈ 45 kg liegt der Schwerpunkt
  mehr als 20 cm vor dem Knöchel, man kippt nach vorn ([kraftverteilung.md](kraftverteilung.md)). Mehr Kraft nützt dort nichts.
- **Aufladen: × 1,1.** Hier dreht die Schulter unter Last, die Sperre hilft nicht, es zählt nur der Motor (RS04 drehend
  100 Nm für 15 s). Doppelt ginge nur mit einer Untersetzung 1 : 2 (Zahnriemen) an der Schulter, also mehr Gewicht und
  Kosten. Nicht gemacht.
- **Beine:** Sie haben keinen Motor mehr. Beim Aufrichten arbeiten deine Beine wie ohne Exo, tragen aber das Exo-Oberteil
  nicht (das geht über die Schienen in den Boden, solange die Knie nicht tief gebeugt sind). In der tiefen Hocke hängt am
  Knie ≈ 9 Nm Exo statt bisher 17 Nm (die vorher der Motor trug). Halten in der Hocke übernimmt die Kniesperre.

## Schritt 3: Herstellkosten

| Maßnahme | Masse | Kosten |
|---|---|---|
| C1 Motoren direkt im Hersteller-Shop statt beim Händler | 0 | −153 € |
| C2 Elektronik auf 2 Motoren zugeschnitten: ein Messwandler, kleineres Schütz, weniger Stecker | 0 | −65 € |
| C3 Beinmanschetten messen nichts mehr: Klettgurt statt BOA, kein Längsschlitten | −0,20 kg | −85 € |
| C4 Fertigung: Pulver statt Eloxal, Akku-Adapter gedruckt, weniger Lagerplatten, keine Cordura-Hauben | 0 | −115 € |

**Stand: 29,4 kg (× 1,61), 3.148 € (× 1,87), Kraft × 2,24.**

C1: RobStride verkauft den RS04 in China für ¥ 1.199 (stueckliste.md). Mit Versand, Einfuhrumsatzsteuer 19 % und Zoll
2,7 % ≈ 220 € statt ≈ 296 € beim Händler. Lieferzeit und Gewährleistung sind schlechter; für 2 Motoren vertretbar.

## Schritt 4: Optionen für den Rest bis Faktor 2

Jede Option ändert etwas, das du im Einsatz merkst. Deshalb einzeln:

| Option | Masse | Kosten | was du merkst |
|---|---|---|---|
| O1 Rumpfdrehung weglassen | −0,96 kg | −70 € | Rahmen dreht nur noch im Gurt mit (≈ ±5° statt ±15°) |
| O2 Schlaufen mit Handratsche statt Motor spannen | −0,54 kg | −176 € | Schlaufe mit der anderen Hand zuziehen, öffnen per Hebel |
| O3 Arbeitslicht weg | −0,24 kg | −30 € | Stirnlampe nehmen |
| O4 Rahmen, Schulter, Außenrohre aus CFK (Teleskop bleibt Alu) | −1,80 kg | +240 € | nichts, aber teurer; Klebung bei Frost prüfen ([R&G](https://www.r-g.de/en/art/600053): 20 × 20 ≈ 30 €/m) |

- **O1–O3:** 27,7 kg (× 1,70), **2.872 € (× 2,05)**. Kosten-Ziel erreicht.
- **O1–O4:** **25,9 kg (× 1,83)**, 3.112 € (× 1,89). Leichter, aber das Kosten-Ziel ist dann wieder knapp verfehlt.

**Warum nicht 23,6 kg:** Danach bleiben vor allem Teile, die die Funktion ausmachen: 2 Schultermotoren mit Kapsel
(3,4 kg), Akku (2 kg), Gurte (1,3 kg), Manschetten (1,0 kg), Sperren (2,1 kg), Schienen und Gelenke. Ein kleinerer Akku
(Stihl AP 300 S, 1,8 kg, 281 Wh, [Stihl](https://stihl.co.uk/en/ap/battery-ap-300-s-2023-version-164012)) bringt nur
0,2 kg. Die letzten 2,3 kg gehen nur mit dem Wegfall der Schultermotoren:

**Variante ganz passiv** (Schritte 1–3, dann auch ohne Schultermotoren, Akku und Elektronik): **19,2 kg (× 2,5),
1.838 € (× 3,2), Kraft × 1,95.** Halten und Tragen bleiben gleich gut (Sperren, Schulterfeder), aber **Aufladen fällt
auf × 0,4**: Den Baum auf die Ladefläche heben musst du dann mit deiner Schulter allein. Zum Vergleich: der passive
Chairless Chair von noonee (nur Kniesperre) wiegt 3,6 kg und kostet 2.290 € ([noonee](https://noonee.com/en/products/chairless-chair)).

