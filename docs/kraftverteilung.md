# Kraftverteilung (Entwurf 3)

Grobe statische Abschätzung: Welche Kräfte entstehen beim Aufladen und Schleifen, über welche
Teile laufen sie in den Boden und wie viel davon kann das Exo übernehmen.
Gerechnet mit [`tools/kraftverteilung.py`](../tools/kraftverteilung.py) (Seitenansicht, ohne
Schwung, Baumgewichte „typische Nordmanntanne“; echte Bäume streuen stark).

Die Zahlen sind **Größenordnungen** für die Auslegung, keine Vorhersage für jeden Baum.

---

## 1. Kraftfluss: welcher Weg die Last nimmt

```
Stamm
 └─ Zahnleiste + 2 Schlaufen (Unterseite Unterarm)
     └─ Schlaufenträger (Alu-U-Profil)
         └─ Unterarmschiene ── Ellbogengelenk (Motor über Bowdenzug-Paar)
             └─ Oberarmschiene ── Schultergelenk (Motor über Bowdenzug-Paar)
                 └─ Schulterbügel ── Rückenrahmen
                     └─ Hüftbügel ── Hüftgelenk (Motor über Beinseil)
                         └─ Oberschenkelschiene ── Kniegelenk (gleiches Beinseil)
                             └─ Unterschenkelschiene ── Sprunggelenk (frei)
                                 └─ Steigbügel unter dem Schuh ── Boden
```

Wichtig für die Steuerung: **Die blauen Manschetten liegen nicht in diesem Weg.** Die Last des
Baums geht über die Schlaufen direkt in die Schienen. Die Manschetten spüren nur, was dein
Körper gegen das Exo drückt – also deine Absicht. Dazu mehr in [steuerung.md](steuerung.md).

Wo die Last am Körper ankommt:
- **Hüftgurt:** nimmt das Gewicht von Rahmen, Akku und Motoren (rund 10 kg) und, wenn die
  Beinmotoren nicht schieben, auch die Last aus den Armen.
- **Exo-Beine:** bei fast gestreckten Knien tragen die Schienen die Last fast ohne Motorkraft
  in den Steigbügel. In der Hocke trägt nur, was die Beinmotoren an Moment aufbringen.
- **Deine Arme** spüren nur den Teil, den das Exo nicht übernimmt.

---

## 2. Lasten je Arbeitsschritt

„Kraft je Arm“ ist die Kraft am stärker belasteten Arm, „je Schlaufe“ bei zwei geschlossenen
Schlaufen. Momente sind die **Zusatzmomente durch den Baum** (das Eigengewicht von Körper und
Exo kommt dazu). „Schwerpunkt vor Knöchel“: Liegt der Wert über etwa 20 cm, kippt man ohne
Ausfallschritt nach vorne (Schuh reicht etwa 20 cm vor den Knöchel).

| Schritt | Baum | Kraft je Arm (N) | je Schlaufe (N) | Ellbogen je Arm (Nm) | Schulter je Arm (Nm) | Hüfte beide (Nm) | Knie beide (Nm) | Schwerpunkt vor Knöchel (cm) |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Aufrichten 2 m | 15 kg | 92 | 46 | 15 | 28 | 54 | 33 | 9 |
| Aufrichten 3 m | 41 kg | 361 | 180 | 58 | 108 | 212 | 129 | 21 |
| Aufrichten 4 m | 85 kg | 960 | 480 | 153 | 288 | 564 | 343 | 35 |
| Tragen 2 m | 15 kg | 147 | 74 | 23 | 49 | 35 | 21 | -1 |
| Tragen 3 m | 41 kg | 402 | 201 | 64 | 133 | 97 | 57 | 2 |
| Tragen 4 m | 85 kg | 834 | 417 | 132 | 275 | 201 | 119 | 5 |
| Hochheben 2 m | 15 kg | 147 | 74 | 15 | 56 | 38 | 27 | -1 |
| Hochheben 3 m | 41 kg | 402 | 201 | 42 | 153 | 104 | 73 | 3 |
| Hochheben 4 m | 85 kg | 834 | 417 | 86 | 317 | 215 | 150 | 6 |
| Schleifen 2 × 2 m | 15 kg | 107 | 53 | 7 | 7 | 4 | 32 | -5 |
| Schleifen 4 × 2 m | 15 kg | 213 | 107 | 13 | 13 | 7 | 65 | -9 |
| Schleifen 2 × 3 m | 41 kg | 296 | 148 | 18 | 16 | 10 | 85 | -10 |

Annahmen der Schritte:
- **Aufrichten:** Schnittende steht am Boden (Drehpunkt), linker Unterarm hebt etwa 90 cm weiter
  oben, Baum bei 15°. Hocke, Rumpf vorgeneigt.
- **Tragen:** Baum fast senkrecht, rechter Unterarm trägt, linker stützt nur seitlich.
- **Hochheben:** Schnittende auf etwa Brusthöhe.
- **Schleifen:** Arme hängen, Stümpfe auf Oberschenkelhöhe, Spitzen schleifen über nassen Schnee
  oder Schlamm (Reibwert 0,7, eher hoch angesetzt).

---

## 3. Was die Antriebe schaffen

Motor-Klasse „Robotergelenk mittel“ (etwa 9 Nm dauernd, 25 Nm Spitze am Abtrieb), Seiltrommel
Ø 3 cm. Je Körperseite, nach Reibungsverlusten (Bowdenzug 30 %, offenes Seil 10 %):

| Gelenk | Übersetzung | Dauer (Nm) | Spitze (Nm) |
| --- | --- | --- | --- |
| Schulter | Seilscheibe Ø 14 cm | 29 | 82 |
| Ellbogen | Seilscheibe Ø 10 cm | 21 | 58 |
| Hüfte | Rolle 9,5 cm hinter der Achse | 51 | 143 |
| Knie | Rolle 6 cm vor der Achse | 32 | 90 |

Seilkraft bei Motorspitze: etwa 1.700 N. Dyneema 3 mm hält ein Vielfaches; Bowden-Innenzug
deshalb **2 mm Edelstahl 7 × 7 oder Dyneema in PTFE-Hülle**, Software begrenzt auf etwa 1.000 N.

### Bewertung

| Bereich | 2 m | 3 m | 4 m |
| --- | --- | --- | --- |
| Schultern | Exo kann fast alles übernehmen | Exo übernimmt gut die Hälfte | Exo hilft spürbar (rund ein Viertel bis ein Drittel), Rest du |
| Ellbogen | Exo kann fast alles übernehmen | drei Viertel bis fast alles | etwa ein Drittel bis die Hälfte |
| Hüfte, Knie | locker | Exo übernimmt die Hälfte oder mehr | beim Aufrichten etwa die Hälfte, beim Tragen das meiste |
| Schleifen | kaum Moment, Last hängt an den Schultern | dito | dito |

Daraus folgt:
1. **Die Schultern sind der Engpass**, sobald der Baum vor dem Körper hängt. Deshalb trägt
   man schwere Bäume mit **beiden Unterarmen** am Stamm (linke Schlaufen auch zu): Das halbiert die
   Last je Schulter. Und möglichst nah am Körper.
2. **Beine und Hüfte** sind gut versorgt. Das ist gewollt: Aufrichten und Hochheben kommen am
   besten aus den Beinen, wie beim richtigen Heben.
3. **Schleifen ist für das Exo leicht:** Mit hängenden Armen zieht die Last fast nur in
   Längsrichtung an den Schienen, die Gelenke müssen kaum Moment halten. Die Last landet über
   den Rahmen auf Hüftgurt und Exo-Beinen statt auf deinen Schultern.
4. **Gleichgewicht:** Beim Aufrichten schwerer Bäume (3–4 m) liegt der Schwerpunkt vor der
   Schuhspitze. Ohne verlängerte Fußplatten heißt das: **Ausfallschritt**, also einen Fuß nach vorne,
   und mit den Beinen hochdrücken. Das macht man ohnehin intuitiv. Beim Tragen, Hochheben und
   Schleifen bleibt der Schwerpunkt über dem Fuß.
5. **Schlaufen:** typisch 50–200 N je Schlaufe, im Extremfall rund 500 N. Mit Zahnleiste im
   Schlaufenträger hält der Stamm formschlüssig und nicht nur über Reibung.

---

## 4. Wie viel spürst du noch?

Die Steuerung verstärkt deine Kraft mit einem einstellbaren Faktor K (siehe
[steuerung.md](steuerung.md)). Solange die Motoren nicht an ihrer Grenze sind, gilt:

| K | Exo übernimmt | du spürst |
| --- | --- | --- |
| 0,5 | ein Drittel | zwei Drittel |
| 1 | die Hälfte („doppelte Kraft“) | die Hälfte |
| 2 | zwei Drittel | ein Drittel |
| 3 | drei Viertel | ein Viertel |

Start mit K = 0,5, dann steigern. Über K = 3 wird das Exo unruhig und man spürt die Last
kaum noch, was gefährlich ist: Man merkt nicht mehr, wenn etwas zu schwer wird.

---

## 5. Folgen für die Konstruktion

- Schulter- und Ellbogengelenke mit **großer Seilscheibe** (Ø 10–14 cm), weil dort am meisten
  Moment gebraucht wird und die Bowdenzüge Reibung kosten.
- Schlaufenträger und Unterarmschiene **biegesteif** (Alu 25 × 25 × 2 oder U-Profil 30 × 20 × 3),
  denn bis zu rund 1.000 N greifen hier außermittig an.
- Schulterbügel und Rahmen-Eckverbindungen mit **Doppelplatten**, dort sammelt sich die Last
  beider Arme.
- Hüftgurt wie bei einer Lastenkraxe: Er muss beim Schleifen 20–45 kg zusätzlich aufnehmen
  können, wenn die Beinmotoren aus sind.
- Steigbügel unter der **Schuhmitte** (nicht unter der Ferse), damit die Last der Schienen dort ankommt,
  wo du ohnehin stehst.
