# Exoskelett

Aktives Exoskelett aus Metall, das beim Aufladen frisch geschnittener Christbäume entlastet.

> **Stand:** Der Use Case steht, ein erster Entwurf der Anforderungen liegt vor.
> Es gibt noch keine Mechanik, Elektronik oder Firmware. Das Exo in der 3D-Darstellung ist ein Platzhalter-Konzept.

## Use Case 1: Christbaum aufladen

Der Use Case ist der Grundstein des Projekts. Gelenke, Motoren, Greifer und Akku werden daraus abgeleitet.

1. **Greifen:** In die Hocke gehen. Die rechte Hand greift am Stumpf (dort sitzt der Stammgreifer), die linke so weit Richtung Spitze wie möglich.
2. **Aufrichten bis fast senkrecht:** Zurücklehnen. Die rechte Hand ist der Drehpunkt, die linke bringt über den Hebel den Baum in die Senkrechte.
3. **Tragen:** Fast senkrecht (78°) und leicht zurückgelehnt zum Plateau gehen.
4. **Hochheben:** Den Baum fast lotrecht (84°) balancieren und anheben, bis das Schnittende auf Plateauhöhe ist. Dann das Schnittende über die Kante führen und knapp dahinter auf der Ladefläche absetzen.
5. **Umkippen lassen:** Der Greifer hält das Schnittende wie ein Scharnier, der Baum kippt 90° nach vorne aufs Plateau und darf dabei frei fallen. Sobald die untersten Äste aufliegen, öffnet der Greifer. Nichts wird geschoben, das schont den Baum.
6. **Zurücktreten:** Arme runter, zurücktreten.

Das Plateau ist eine Ladefläche (z. B. Anhänger oder LKW) mit einer Höhe zwischen **0 und 1,75 m**.

Warum fast senkrecht und nicht waagrecht: Das Kippmoment am Greifer ist Gewicht × waagrechter Abstand des
Schwerpunkts vom Greifer. Waagrecht gehalten ist dieser Abstand am größten (brutaler Hebel), fast senkrecht fast null.

## Festgelegt

| Thema | Festlegung |
|---|---|
| Ziel | Das Exo unterstützt die eigene Bewegung, im besten Fall mit doppelter Kraft. So kräftig wie möglich, Grenzen sind Stand der Technik und Budget. |
| Aufrichten | Rechte Hand am Stumpf als Drehpunkt, linke Hand weit Richtung Spitze als Hebel, zurücklehnen. Die meisten Bäume gehen so schon ohne Exo. |
| Weiterer Einsatz | Das Exo soll auch beim Ziehen schwerer Lasten helfen (z. B. Bäume schleifen), siehe Use Case 2. |
| Bäume | 1 bis 4 m lang, nicht eingenetzt, sehr unterschiedlich schwer (das Gewicht folgt nicht sicher aus der Länge) |
| Umkippen | Der Baum darf frei aufs Plateau fallen |
| Person | 1,75 m groß, etwa 75 kg |
| Material | Rahmen aus stabilem Metall, dazu Teile aus dem 3D-Drucker |
| 3D-Drucker | Bambu Lab A1 mini |

## 3D-Darstellung

[`visualisierung/use-case-christbaum.html`](visualisierung/use-case-christbaum.html) herunterladen und im Browser öffnen.
Die Seite braucht Internet für die 3D-Bibliothek [three.js](https://threejs.org) und die Schriften.

- Ablauf abspielen, anhalten oder an jede Stelle springen
- Plateau-Höhe, Baumlänge und **Baumgewicht unabhängig voneinander** einstellen
- Körpergröße und Körpergewicht, Gewicht des Exos und **verlängerte Fußplatten** einstellen
- Kamera: schräg, von der Seite oder mitlaufend; ziehen zum Drehen, scrollen oder zwei Finger zum Zoomen
- Lastanzeige: grobe statische Schätzung der Zusatzlast durch den Baum an Greifer, Schultern, Hüfte/Rücken und Knien
- Hinweise, wenn es über Kopf geht, die Arme nicht reichen oder das Gleichgewicht kippt

## Annahmen (bitte prüfen)

| Thema | Annahme |
|---|---|
| Ausgangslage | Der Baum liegt nach dem Schneiden am Boden, die Spitze zeigt Richtung Plateau. |
| Greifen | Nur am astfreien Stammende, mit einem Stammgreifer, der fest zum Exo gehört. |
| Greifer | Kann beim Umkippen frei drehen (Scharnier), damit das Schnittende nicht über die Kante rutscht und das Exo kein Moment abbekommt. |
| Baumgewicht | Als Anhaltspunkt eine Nordmanntanne (2 m ≈ 15 kg, 3 m ≈ 41 kg, 4 m ≈ 85 kg). Für die Auslegung bis 120 kg gerechnet. |
| Schwerpunkt | Bei etwa 30 % der Länge ab Schnittende. |
| Stamm | Durchmesser am Schnitt etwa 5 cm (1 m Baum) bis 18 cm (4 m Baum). |
| Exo | Etwa 30 kg, je zur Hälfte am Rücken (Akku, Steuerung) und an den Beinen. |

## Auslegung: so kräftig wie möglich

Gerechnet für dich (1,75 m, 75 kg), Exo 30 kg, Plateau 1,75 m, statisch, nur die Zusatzlast durch den Baum.
Momente sind die Summe beider Körperseiten. „Fußplatte länger“ heißt: um so viel muss die Standfläche nach vorne
wachsen, damit der gemeinsame Schwerpunkt nicht vor die Füße wandert.

| Baum | Greifer | Schultern | Hüfte | Knie | Fußplatte länger |
|---|---|---|---|---|---|
| 2 m, 15 kg | 73 / 15 Nm | 93 / 76 Nm | 144 / 69 Nm | 86 / 63 Nm | 0 / 0 cm |
| 2 m, 30 kg (schwerer Baum) | 146 / 30 Nm | 187 / 152 Nm | 288 / 137 Nm | 171 / 127 Nm | 6 / 0 cm |
| 3 m, 41 kg | 306 / 64 Nm | 364 / 229 Nm | 488 / 209 Nm | 343 / 199 Nm | 19 / 3 cm |
| 3 m, 60 kg | 449 / 93 Nm | 533 / 335 Nm | 714 / 307 Nm | 502 / 291 Nm | 28 / 7 cm |
| 4 m, 85 kg | 844 / 175 Nm | 998 / 517 Nm | 1201 / 477 Nm | 933 / 452 Nm | 48 / 14 cm |
| 4 m, 120 kg | 1191 / 248 Nm | 1408 / 730 Nm | 1695 / 673 Nm | 1317 / 637 Nm | 59 / 19 cm |

Erster Wert: beim Hochschwenken aus der flachen Lage (Schritt 2). Zweiter Wert: alles andere (Tragen, Hochheben, Absetzen).

Was daraus folgt:

- **Das Hochschwenken aus der flachen Lage ist der teuerste Moment.** Es treibt jeden Wert um das Zwei- bis Fünffache hoch. Mit deiner Technik nimmt die linke Hand das Kippmoment als Hebelkraft auf (Hebel etwa 0,73 m), ein Greifer muss es nicht halten. Nötige Hebelkraft links: 2 m / 15 kg etwa 10 kg, 3 m / 41 kg etwa 43 kg, 4 m / 85 kg etwa 118 kg, 4 m / 120 kg etwa 166 kg. Verdoppelt das Exo deine Kraft, schaffst du ungefähr doppelt so schwere Bäume wie heute. Für die ganz schweren 4 m Bäume reicht auch das nicht, dort hilft die Leiter-Methode (Spitze anheben, unter dem Baum zum Stamm gehen).
- **Mehr Kraft allein hilft beim Gleichgewicht nicht.** Der Baum hängt vor dem Körper, die Standfläche ist zu klein. Dagegen helfen nach vorne verlängerte Fußplatten (bis etwa 20 cm), eine ausklappbare Stütze vorne für das Hochheben an der Kante und schwere Teile wie den Akku hinten am Rücken.
- **Die Last muss über den Rahmen in den Boden, nicht durch den Körper.** Das Exo trägt den Baum über Greifer, Rahmen und Beine bis zu den Fußplatten, die Person führt nur. Gelenke, die eine Last nur halten, sollten sich sperren lassen (Bremse), dann müssen die Motoren nur bewegen.
- **Zum Vergleich mit dem Stand der Technik:** Übliche Exo-Gelenkantriebe liegen bei etwa 50 bis 200 Nm. Eines der stärksten tragbaren Ganzkörper-Exos (Sarcos Guardian XO) hebt rund 90 kg. Der 4 m Baum mit 120 kg liegt also an der Grenze dessen, was es gibt, und geht nur mit Leiter-Methode, Lastpfad in den Boden und größerer Standfläche.
- **Für das Hochheben über Kopf ist ein Hubmast eine Option.** Statt sehr starker Schultermotoren könnte ein senkrechter Mast am Rückenrahmen mit Spindelantrieb den Greifer heben. Spindelantriebe mit über 1 kN Kraft sind günstig und robust.

## Anforderungen (Entwurf)

Für den schwersten angenommenen Fall: 4 m Baum, 120 kg, Plateau 1,75 m, mit Leiter-Methode beim Aufrichten.

| Nr. | Anforderung | Zielwert |
|---|---|---|
| A1 | Baumgewicht | bis 120 kg (nach dem Wiegen echter Bäume anpassen) |
| A2 | Stammdurchmesser im Greifer | etwa 5 bis 18 cm |
| A3 | Greifer: Haltekraft entlang des Stamms | mindestens 1,2 kN, mit Sicherheitsfaktor 2 also 2,4 kN |
| A4 | Greifer (rechte Hand) | hält den Stumpf, Kippmoment übernimmt die linke Hand; beim Umkippen frei drehbar (Scharnier) |
| A4b | Unterstützung | jede Bewegung mit bis zu gleich viel Kraft wie die Person unterstützen (Kraft verdoppeln) |
| A5 | Hubhöhe des Greifers | bis etwa 2,1 m (Schnittende knapp über 1,75 m) |
| A6 | Hüfte, Knie, Schultern | je Seite etwa 320 bis 370 Nm halten, Gelenke sperrbar |
| A7 | Standfläche | Fußplatten etwa 20 cm nach vorne verlängert oder ausklappbare Stütze |
| A8 | Lastpfad | Baumgewicht über Rahmen und Beine in die Fußplatten, nicht über den Körper |
| A9 | Gewicht des Exos | so leicht wie möglich, schwere Teile hinten am Rücken |
| A10 | Sicherheit | Not-Aus, Endanschläge, Drehmoment- und Geschwindigkeitsbegrenzung, zuerst Prüfstand ohne Person |

## Use Case 2: Bäume schleifen

3D: in [`visualisierung/use-case-christbaum.html`](visualisierung/use-case-christbaum.html) oben auf „Schleifen“ umschalten.

Ein bis vier Bäume (je nach Größe) über den Boden zum Plateau ziehen. Ohne Seil und Gurt: Etwas am Exo fasst die Stämme direkt.
Am liebsten vorwärts gehen. Ohne Exo geht man in der Praxis rückwärts, je schwerer, desto eher.

**Kräfte (grobe Schätzung, Reibung Äste auf Gras etwa 0,6, auf Schnee etwa 0,3):**

| Last | Gesamt | Stumpf angehoben: Gewicht am Griff | Zug auf Gras | Zug auf Schnee | Ganz am Boden: Zug auf Gras |
|---|---|---|---|---|---|
| 4 × 1 m | 12 kg | 9 kg | 2 kg | 1 kg | 7 kg |
| 4 × 2 m | 60 kg | 44 kg | 9 kg | 5 kg | 36 kg |
| 2 × 3 m | 82 kg | 60 kg | 13 kg | 7 kg | 49 kg |
| 1 × 4 m | 85 kg | 63 kg | 13 kg | 7 kg | 51 kg |
| 1 × 4 m oder 4 × 2 m schwer | 120 kg | 89 kg | 19 kg | 9 kg | 72 kg |

Was daraus folgt:

- **Hebt man die Stümpfe an, ist der Zug klein, aber das Gewicht am Griff groß.** Die Äste gleiten dann nur noch, der Großteil des Gewichts hängt am Griff.
- **Darum geht man ohne Exo rückwärts:** Die Hände sind vorne, man lehnt sich mit dem Körpergewicht zurück. Vorwärts müssten die Hände 40 bis 90 kg hinter dem Rücken halten, das können die Arme am schlechtesten.
- **Vorschlag: Zuggreifer hinten an der Hüfte des Exos.** Eine Klemme auf Hüfthöhe hinter dem Körper fasst ein bis vier Stümpfe und hebt sie leicht an. Das Gewicht geht über den Hüftrahmen und die Beine des Exos in die Fußplatten, die Beine schieben vorwärts, die Hände bleiben frei. Man lehnt sich dabei leicht nach vorne, die Last hinten gleicht das aus.
- **Gleicher Lastpfad wie beim Aufladen:** Rahmen, Hüfte und Beine werden für beide Use Cases gebraucht, der Zuggreifer ist ein zusätzliches Teil am Hüftrahmen.

**Anforderungen (Entwurf):**

| Nr. | Anforderung | Zielwert |
|---|---|---|
| Z1 | Zuggreifer fasst | 1 bis 4 Stämme, je etwa 5 bis 18 cm Durchmesser, schnell lösbar |
| Z2 | Gewicht am Zuggreifer | bis etwa 90 kg, über Rahmen und Beine in die Fußplatten |
| Z3 | Zugkraft | bis etwa 20 kg mit angehobenen Stümpfen, bis 70 kg wenn sie schleifen |
| Z4 | Gehen | vorwärts, Hände frei |

## 3D-Druck (Bambu Lab A1 mini)

- Bauraum 18 × 18 × 18 cm: größere Teile in Segmente teilen und verschrauben.
- Offener Drucker: PLA, PETG und TPU gehen gut, ABS und ASA eher nicht.
- Für draußen im Winter PETG statt PLA, weil es zäher ist.
- TPU als Belag in den Greifbacken: schont die Rinde und rutscht weniger.
- Tragende Teile (Rahmen, Gelenkachsen, Greiferarme) aus Metall. Gedruckt werden Gehäuse, Abdeckungen, Manschetten, Kabelführungen, Sensorhalter und Bohrschablonen für die Metallteile.

## Offene Fragen

1. Passt der Zuggreifer hinten an der Hüfte für Use Case 2? Wie weit schleifst du die Bäume ungefähr, und geht es bergauf?
2. Wie hoch ist das Budget ungefähr? Davon hängt ab, welche Antriebe infrage kommen (z. B. Wellgetriebe gegen Spindelantriebe).
3. Kannst du die schwersten Bäume wiegen (z. B. mit einer Kofferwaage oder Kranwaage)? Dann wird A1 aus einer Annahme ein Messwert.
4. Was genau ist das Plateau (Anhänger, LKW, Palette)? Wie weit ist der Weg vom Schnitt zum Plateau, und wie ist der Boden (Wiese, Hang, Schnee, Matsch)?
5. Wie viele Bäume pro Stunde bzw. pro Tag? Das bestimmt den Akku.

## Sicherheit

Ein aktives Exo mit diesen Kräften kann schwer verletzen. Deshalb von Anfang an mitplanen:
Not-Aus, mechanische Endanschläge an jedem Gelenk, Begrenzung von Drehmoment und Geschwindigkeit
und erste Tests auf einem Prüfstand ohne Person.

## Nächste Schritte

1. Leiter-Methode und Budget klären, schwerste Bäume wiegen
2. Anforderungen festziehen
3. Konzept: Lastpfad, welche Gelenke aktiv und welche sperrbar, Hubmast oder Schultermotoren, Aufbau des Stammgreifers
4. Erster Prototyp: der Stammgreifer, weil dort die größte Last sitzt

## Projektstruktur

```
docs/
  konzept.md                 Konzept: Aufbau, Antriebe, Greifer, Steuerung, Kosten
visualisierung/
  use-case-christbaum.html   3D: beide Use Cases (Aufladen, Schleifen) mit dem Exo laut Konzept
```
