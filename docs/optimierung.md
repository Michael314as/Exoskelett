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

