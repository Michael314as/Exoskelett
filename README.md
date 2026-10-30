# Exoskelett

Aktives Exoskelett aus Metall, das beim Aufladen frisch geschnittener Christbäume entlastet.

> **Stand:** Der Use Case steht, ein erster Entwurf der Anforderungen liegt vor.
> Es gibt noch keine Mechanik, Elektronik oder Firmware. Das Exo in der 3D-Darstellung ist ein Platzhalter-Konzept.

## Use Case 1: Christbaum aufladen

Der Use Case ist der Grundstein des Projekts. Gelenke, Motoren, Greifer und Akku werden daraus abgeleitet.

1. **Greifen:** In die Hocke gehen und den Stammgreifer am astfreien Stammende (Schnittende) schließen.
2. **Aufrichten bis fast senkrecht:** Aus der Hocke hochkommen, den Baum hochschwenken und sich zurücklehnen.
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
| Ziel | Das Exo so kräftig wie möglich machen, für möglichst viele Fälle. Grenzen sind Stand der Technik und Budget, nicht die Baumgröße. |
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

- **Das Hochschwenken aus der flachen Lage ist der teuerste Moment.** Es treibt jeden Wert um das Zwei- bis Fünffache hoch. Schwere Bäume sollten deshalb wie eine Leiter von der Spitze her aufgestellt werden: Schnittende am Boden gegen die Fußplatte, Spitze anheben und unter dem Baum zum Stamm gehen. Dafür braucht es selbst beim 4 m, 120 kg Baum nur etwa 35 kg Kraft an den Händen, die das Exo über Kopf unterstützen kann. Erst danach greift der Stammgreifer unten. Leichte Bäume kann das Exo weiter hochschwenken.
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
| A4 | Greifer: Kippmoment | etwa 250 Nm, beim Umkippen frei drehbar (Scharnier) |
| A5 | Hubhöhe des Greifers | bis etwa 2,1 m (Schnittende knapp über 1,75 m) |
| A6 | Hüfte, Knie, Schultern | je Seite etwa 320 bis 370 Nm halten, Gelenke sperrbar |
| A7 | Standfläche | Fußplatten etwa 20 cm nach vorne verlängert oder ausklappbare Stütze |
| A8 | Lastpfad | Baumgewicht über Rahmen und Beine in die Fußplatten, nicht über den Körper |
| A9 | Gewicht des Exos | so leicht wie möglich, schwere Teile hinten am Rücken |
| A10 | Sicherheit | Not-Aus, Endanschläge, Drehmoment- und Geschwindigkeitsbegrenzung, zuerst Prüfstand ohne Person |

## 3D-Druck (Bambu Lab A1 mini)

- Bauraum 18 × 18 × 18 cm: größere Teile in Segmente teilen und verschrauben.
- Offener Drucker: PLA, PETG und TPU gehen gut, ABS und ASA eher nicht.
- Für draußen im Winter PETG statt PLA, weil es zäher ist.
- TPU als Belag in den Greifbacken: schont die Rinde und rutscht weniger.
- Tragende Teile (Rahmen, Gelenkachsen, Greiferarme) aus Metall. Gedruckt werden Gehäuse, Abdeckungen, Manschetten, Kabelführungen, Sensorhalter und Bohrschablonen für die Metallteile.

## Offene Fragen

1. Passt die Leiter-Methode für schwere Bäume (Spitze anheben, unter dem Baum zum Stamm gehen, dann unten greifen)?
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
visualisierung/
  use-case-christbaum.html   3D-Darstellung des Use Cases
```
