# Exoskelett

Aktives Exoskelett aus Metall, das beim Aufladen frisch geschnittener Christbäume entlastet.

> **Stand:** Der Use Case steht. Als Nächstes wird festgelegt, bis zu welcher Baumgröße das Exo ausgelegt wird.
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
| Bäume | 1 bis 4 m lang, nicht eingenetzt, unterschiedlich schwer |
| Umkippen | Der Baum darf frei aufs Plateau fallen |
| Person | 1,75 m groß |
| Material | Rahmen aus stabilem Metall, dazu Teile aus dem 3D-Drucker |
| 3D-Drucker | Bambu Lab A1 mini |

## 3D-Darstellung

[`visualisierung/use-case-christbaum.html`](visualisierung/use-case-christbaum.html) herunterladen und im Browser öffnen.
Die Seite braucht Internet für die 3D-Bibliothek [three.js](https://threejs.org) und die Schriften.

- Ablauf abspielen, anhalten oder an jede Stelle springen
- Plateau-Höhe (0 bis 1,75 m), Baumlänge (1 bis 4 m) und Körpergröße (1,50 bis 2,00 m) einstellen
- Kamera: schräg, von der Seite oder mitlaufend; ziehen zum Drehen, scrollen oder zwei Finger zum Zoomen
- Lastanzeige: grobe statische Schätzung der Zusatzlast durch den Baum an Greifer, Schultern, Hüfte/Rücken und Knien
- Hinweise, wenn es über Kopf geht, die Arme nicht reichen oder das Gleichgewicht kippt

## Annahmen (bitte prüfen)

| Thema | Annahme |
|---|---|
| Ausgangslage | Der Baum liegt nach dem Schneiden am Boden, die Spitze zeigt Richtung Plateau. |
| Greifen | Nur am astfreien Stammende, mit einem Stammgreifer, der fest zum Exo gehört. |
| Greifer | Kann beim Umkippen frei drehen (Scharnier), damit das Schnittende nicht über die Kante rutscht und das Exo kein Moment abbekommt. |
| Baumgewicht | Nordmanntanne, aus der Länge geschätzt (2 m ≈ 15 kg, 3 m ≈ 41 kg, 4 m ≈ 85 kg). Schwerpunkt bei etwa 30 % der Länge ab Schnittende. |
| Körpergewicht | Etwa 71 kg. Es geht ins Gleichgewicht ein, das Gewicht des Exos selbst ist noch nicht berücksichtigt. |
| Exo | Aktiv: Motoren an Hüfte, Knie, Schulter und Ellbogen. Akku und Steuerung am Rücken. |

## Was das für dich heißt (1,75 m, grobe Schätzung)

Statisch gerechnet, nur die Zusatzlast durch den Baum, Plateau 1,75 m. „Kippt“ heißt: Der gemeinsame Schwerpunkt
von dir und dem Baum liegt vor dem Fußballen, ohne Ausfallschritt würdest du nach vorne kippen.

| Baum | Gewicht | Kippmoment am Greifer beim Aufrichten (kurz) | Hüfte (Spitze) | Gleichgewicht | Über Kopf bei 1,75 m |
|---|---|---|---|---|---|
| 1 m | 3 kg | 6 Nm | 19 Nm | sicher | 3 kg |
| 1,5 m | 7 kg | 25 Nm | 61 Nm | sicher | 7 kg |
| 2 m | 15 kg | 73 Nm | 144 Nm | beim Aufrichten knapp (3 cm) | 15 kg |
| 2,5 m | 26 kg | 164 Nm | 283 Nm | kippt beim Aufrichten (15 cm), beim Tragen knapp | 26 kg |
| 3 m | 41 kg | 309 Nm | 492 Nm | kippt beim Aufrichten (29 cm) und Tragen (9 cm) | 41 kg |
| 4 m | 85 kg | 842 Nm | 1198 Nm | kippt beim Aufrichten (61 cm) und Tragen (21 cm) | 85 kg |

Beim Tragen fast senkrecht ist das Kippmoment am Greifer etwa ein Fünftel davon (2 m: 15 Nm). Beim Umkippen ist es null.
Bei 1,75 m Plateau geht der Greifer auf 1,9 bis 2 m, der Baum ragt dann bis 3,8 m (2 m Baum) bzw. 6 m (4 m Baum) hoch.

Was daraus folgt:

- **Bis 2 m funktioniert der Ablauf.** Beim Aufrichten wird es beim 2 m Baum knapp, ein Ausfallschritt nach vorne gleicht das aus.
- **Ab 2,5 m kippt man beim Aufrichten nach vorne, ab 3 m auch beim Tragen.** Das liegt nicht an der Motorkraft, sondern daran, dass der Baum vor dem Körper hängt und die Füße eine zu kleine Standfläche haben.
- **3 bis 4 m Bäume sind für ein tragbares Exo nicht sinnvoll.** 41 bis 85 kg, bis über 800 Nm am Greifer und über 1000 Nm an der Hüfte, das ganze Gewicht über Kopf und ein Baum, der beim Hochheben bis 6 m aufragt. Das wäre auch mit Exo gefährlich.
- **Ungenetzt braucht beim Hochheben Abstand zur Kante,** damit die unteren Äste nicht anstoßen: 2 m Baum etwa 0,8 m, 3 m Baum 1,1 m. Den letzten Schritt geht man mit dem Baum über Kopf.
- **Aufprall beim Umkippen:** Der 2 m Baum schlägt etwa so auf, als würde man ihn flach aus 40 cm fallen lassen, der 3 m Baum wie aus gut 60 cm. Die Äste federn das ab.
- **Ein ungenetzter Baum vor dem Körper nimmt beim Tragen die Sicht.**

**Vorschlag:** Das Exo für Bäume bis 2,5 m auslegen, beim Aufrichten mit Ausfallschritt. Bäume ab 3 m zu zweit oder mit
einem Hilfsmittel am Fahrzeug laden (z. B. Seilwinde oder kleiner Kran). Ganz ohne Kippmoment ginge das Aufrichten,
wenn man den Baum wie eine Leiter von der Spitze her aufstellt und erst dann unten greift.

## 3D-Druck (Bambu Lab A1 mini)

- Bauraum 18 × 18 × 18 cm: größere Teile in Segmente teilen und verschrauben.
- Offener Drucker: PLA, PETG und TPU gehen gut, ABS und ASA eher nicht.
- Für draußen im Winter PETG statt PLA, weil es zäher ist.
- TPU als Belag in den Greifbacken: schont die Rinde und rutscht weniger.
- Tragende Teile (Rahmen, Gelenkachsen, Greiferarme) aus Metall. Gedruckt werden Gehäuse, Abdeckungen, Manschetten, Kabelführungen, Sensorhalter und Bohrschablonen für die Metallteile.

## Offene Fragen

1. Passt der Vorschlag, das Exo für Bäume bis 2,5 m auszulegen und größere anders zu laden? Wie viele deiner Bäume sind größer als 2,5 m?
2. Kannst du drei oder vier Bäume unterschiedlicher Länge wiegen (z. B. mit einer Kofferwaage)? Das Gewicht ist bisher nur geschätzt.
3. Wie schwer bist du ungefähr? Das geht ins Gleichgewicht ein.
4. Was genau ist das Plateau (Anhänger, LKW, Palette)? Wie weit ist der Weg vom Schnitt zum Plateau, und wie ist der Boden (Wiese, Hang, Schnee, Matsch)?
5. Wie viele Bäume pro Stunde bzw. pro Tag?

## Sicherheit

Ein aktives Exo mit diesen Momenten kann schwer verletzen. Deshalb von Anfang an mitplanen:
Not-Aus, mechanische Endanschläge an jedem Gelenk, Begrenzung von Drehmoment und Geschwindigkeit
und erste Tests auf einem Prüfstand ohne Person.

## Nächste Schritte

1. Größte Baumgröße für das Exo festlegen, ein paar Bäume wiegen
2. Anforderungen festlegen: Lasten, Hubhöhe, Gewicht des Exos, Akkulaufzeit, Sicherheit
3. Konzept: welche Gelenke aktiv sein müssen, Aufbau des Stammgreifers, Metallrahmen und 3D-Druckteile
4. Erster Prototyp: der Stammgreifer, weil dort die größte Last sitzt

## Projektstruktur

```
visualisierung/
  use-case-christbaum.html   3D-Darstellung des Use Cases
```
