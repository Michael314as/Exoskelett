# Kraftverteilung (Entwurf 5)

Grobe statische Abschätzung: Welche Kräfte entstehen beim Aufladen und Schleifen, über welche
Teile laufen sie in den Boden und wie viel davon kann das Exo übernehmen.
Gerechnet mit [`tools/kraftverteilung.py`](../tools/kraftverteilung.py) (Seitenansicht, ohne
Schwung, Baumgewichte „typische Nordmanntanne“; echte Bäume streuen stark).

Die Zahlen sind **Größenordnungen** für die Auslegung, keine Vorhersage für jeden Baum.

---

## 1. Kraftfluss: welcher Weg die Last nimmt

```
Stamm
 └─ Zahnleiste + Schlaufen am Handgelenk (rechts 2, links 1)
     └─ Schlaufenkopf (Alu, an der Unterarmschiene verschraubt)
         └─ Unterarmschiene ── Ellbogengelenk (Motor über Bowdenzug-Paar)
             └─ Oberarmschiene ── Schultergelenk (Motor über offenes Seilpaar)
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

„Kraft je Arm“ ist die Kraft am stärker belasteten Arm, „je Schlaufe“ teilt sie rechts auf zwei
Schlaufen, links auf eine (beim Aufrichten hebelt links die Hand). Momente sind die **Zusatzmomente durch den Baum** (das Eigengewicht von Körper und
Exo kommt dazu). „Schwerpunkt vor Knöchel“: Liegt der Wert über etwa 20 cm, kippt man ohne
Ausfallschritt nach vorne (Schuh reicht etwa 20 cm vor den Knöchel).

| Schritt | Baum | Kraft je Arm (N) | je Schlaufe (N) | Ellbogen je Arm (Nm) | Schulter je Arm (Nm) | Hüfte beide (Nm) | Knie beide (Nm) | Schwerpunkt vor Knöchel (cm) |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Aufrichten 2 m | 15 kg | 92 | 92 | 27 | 28 | 54 | 33 | 9 |
| Aufrichten 3 m | 41 kg | 361 | 361 | 104 | 108 | 212 | 129 | 21 |
| Aufrichten 4 m | 85 kg | 960 | 960 | 277 | 288 | 564 | 343 | 35 |
| Tragen 2 m | 15 kg | 147 | 74 | 15 | 37 | 21 | 6 | -3 |
| Tragen 3 m | 41 kg | 402 | 201 | 42 | 101 | 57 | 17 | -1 |
| Tragen 4 m | 85 kg | 834 | 417 | 87 | 208 | 118 | 35 | 0 |
| Hochheben 2 m | 15 kg | 147 | 74 | 30 | 47 | 29 | 17 | -2 |
| Hochheben 3 m | 41 kg | 402 | 201 | 83 | 129 | 78 | 47 | 1 |
| Hochheben 4 m | 85 kg | 834 | 417 | 171 | 267 | 162 | 97 | 3 |
| Schleifen 2 × 2 m | 15 kg | 107 | 53 | 19 | 11 | 13 | 23 | -4 |
| Schleifen 3 × 2 m | 15 kg | 213 | 107 | 39 | 23 | 19 | 35 | -6 |
| Schleifen 2 × 3 m | 41 kg | 296 | 148 | 52 | 28 | 35 | 60 | -9 |

Annahmen der Schritte:
- **Aufrichten:** Schnittende steht am Boden (Drehpunkt), linker Unterarm hebt etwa 90 cm weiter
  oben, Baum bei 15°. Hocke, Rumpf vorgeneigt.
- **Tragen:** Baum fast senkrecht, rechter Unterarm steil nach oben wie eine Fackel, linker stützt nur seitlich.
- **Hochheben:** Schnittende auf etwa Brusthöhe.
- **Schleifen:** Arme hängen, Stümpfe am Handgelenk (rechts bis zu zwei, links einer), Spitzen schleifen über nassen Schnee
  oder Schlamm (Reibwert 0,7, eher hoch angesetzt).

---

## 3. Was die Antriebe schaffen

Seit Entwurf 5 sitzen die Motoren direkt am Gelenk (Übersetzung 1 : 1, an Knie, Ellbogen und Rumpf über eine
Schubstange). Es gibt keine Seile und Bowdenzüge mehr und damit keine Reibungsverluste auf dem Weg.
Die Werte sind die Grenzen in der Firmware (`firmware/src/exo_app.h`); Dauer = was die Kapsel auf Dauer wegkühlt
(geschätzt, wird in P33 gemessen).

| Gelenk | Motor | Dauer (Nm) | Spitze in der Firmware (Nm) | Bedarf beim Aufrichten eines 3-m-Baums, halbe Last |
| --- | --- | --- | --- | --- |
| Schulter | RS04 direkt | ≈ 28 | 100 | ≈ 50–100 |
| Ellbogen | RS03 über Schubstange | ≈ 16 | 56 | ≈ 30–55 |
| Hüfte | RS04 direkt | ≈ 28 | 110 | ≈ 75 |
| Knie | RS04 über Schubstange | ≈ 28 | 110 (strecken), 10 (beugen) | ≈ 50 |
| Rumpf beugen | RS04 über Schubstange | ≈ 28 | 80 (aufrichten) | mit Last trägt die Lendensperre |
| Rumpf drehen | RS03 über Kurbel 1 : 1,8 | ≈ 29 | 30 | – |

Bedarf je Seite in tiefer Hocke beim Aufrichten eines 3-m-Baums: Hüfte etwa 45 Nm (Körper) + 106 Nm (Baum) ≈ 150 Nm,
Knie etwa 33 + 65 ≈ 100 Nm. Die Spitze reicht also für mehr als die Hälfte. **Der Engpass ist die Wärme:** Mit
≈ 28 Nm Dauermoment kann eine Kapsel die Spitze nur etwa 10–15 s halten, dann nimmt der Wärmeschutz die Kraft weich
zurück. Für kurzes Aufrichten und Umsetzen passt das; langes Halten eines schweren Baums in der Hocke nicht. Der Wert
28 Nm ist geschätzt (der Hersteller nennt 40 Nm mit einer Kühlplatte 345 × 345 mm, die die Kapsel nicht hat) und der
wichtigste Messwert am Prüfstand (P33).

Die frühere Rechnung für den Seilantrieb (Entwurf 3 und 4) steht in der Git-Historie dieses Dokuments.

### Bewertung

| Bereich | 2 m | 3 m | 4 m |
| --- | --- | --- | --- |
| Schultern | Exo kann alles übernehmen | Exo übernimmt fast alles bis drei Viertel | Exo hilft spürbar (rund ein Drittel bis die Hälfte), Rest du |
| Ellbogen | Exo kann alles übernehmen | beim Tragen fast alles, beim Aufrichten gut die Hälfte | ein Fünftel (Aufrichten) bis zwei Drittel (Tragen) |
| Hüfte, Knie | locker | Exo übernimmt die Hälfte oder mehr | beim Aufrichten etwa die Hälfte, beim Tragen das meiste |
| Schleifen | kaum Moment, Last hängt an den Schultern | dito | dito |

Daraus folgt:
1. **Die Schultern sind der Engpass**, sobald der Baum vor dem Körper hängt. Weil der Stamm jetzt als
   Verlängerung des Unterarms am Handgelenk hängt, trägt man ihn am besten mit **steil aufgerichtetem
   Unterarm, wie eine Fackel**: Dann drückt die Last fast längs durch Unterarm und Ellbogen, und der
   Ellbogen muss kaum Moment halten. Möglichst nah am Körper; die linke Hand stützt weiter oben.
   Beim Aufrichten (Unterarm flach) ist dagegen der Ellbogen stark gefordert, dort helfen die Beine.
2. **Beine und Hüfte** sind gut versorgt. Das ist gewollt: Aufrichten und Hochheben kommen am
   besten aus den Beinen, wie beim richtigen Heben.
3. **Schleifen ist für das Exo leicht:** Mit hängenden Armen zieht die Last fast nur in
   Längsrichtung an den Schienen, die Gelenke müssen kaum Moment halten. Die Last landet über
   den Rahmen auf Hüftgurt und Exo-Beinen statt auf deinen Schultern.
4. **Gleichgewicht:** Beim Aufrichten schwerer Bäume (3–4 m) liegt der Schwerpunkt vor der
   Schuhspitze. Ohne verlängerte Fußplatten heißt das: **Ausfallschritt**, also einen Fuß nach vorne,
   und mit den Beinen hochdrücken. Das macht man ohnehin intuitiv. Beim Tragen, Hochheben und
   Schleifen bleibt der Schwerpunkt über dem Fuß.
5. **Schlaufen:** typisch 50–200 N je Schlaufe, im Extremfall rund 500 N (rechts verteilt auf zwei
   Schlaufen). Mit Zahnleiste im Schlaufenkopf hält der Stamm formschlüssig und nicht nur über Reibung.

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

- Motoren **direkt auf den Gelenkachsen** (Hüfte, Schulter) oder über eine starre Schubstange mit Parallelogramm
  (Knie, Ellbogen, Rumpf): konstanter Hebel, kein Spiel, nichts kann vereisen oder abspringen.
- Kurbelzapfen werden einseitig gebogen: Passschrauben 12.9 mit glattem Schaft ([festigkeit.md](festigkeit.md)).
- Schlaufenkopf und Unterarmschiene **biegesteif** (Alu 25 × 25 × 3), denn bis zu rund 1.000 N greifen
  hier außermittig an.
- Schulterbügel und Rahmen-Eckverbindungen mit **Doppelplatten**, dort sammelt sich die Last
  beider Arme.
- Hüftgurt wie bei einer Lastenkraxe: Er muss beim Schleifen 20–45 kg zusätzlich aufnehmen
  können, wenn die Beinmotoren aus sind.
- Steigbügel unter der **Schuhmitte** (nicht unter der Ferse), damit die Last der Schienen dort ankommt,
  wo du ohnehin stehst.
