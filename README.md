# Exoskelett

Aktives Exoskelett aus Metall, das beim Aufladen frisch geschnittener Christbäume entlastet.

> **Stand:** Der Use Case wird gerade abgestimmt. Es gibt noch keine Mechanik, Elektronik oder Firmware.
> Das Exo in der 3D-Darstellung ist ein Platzhalter-Konzept, kein Entwurf.

## Use Case 1: Christbaum aufladen

Der Use Case ist der Grundstein des Projekts. Gelenke, Motoren, Greifer und Akku werden daraus abgeleitet.

1. **Greifen:** In die Hocke gehen und den Stammgreifer am astfreien Stammende (Schnittende) schließen.
2. **Aufrichten bis fast senkrecht:** Aus der Hocke hochkommen, den Baum hochschwenken und sich zurücklehnen.
3. **Tragen:** Fast senkrecht (78°) und leicht zurückgelehnt zum Plateau gehen.
4. **Hochheben:** Den Baum fast lotrecht (84°) balancieren und anheben, bis das Schnittende auf Plateauhöhe ist. Dann das Schnittende über die Kante führen und knapp dahinter auf der Ladefläche absetzen.
5. **Umkippen lassen:** Der Greifer hält das Schnittende wie ein Scharnier, der Baum kippt 90° nach vorne aufs Plateau. Sobald die untersten Äste aufliegen, öffnet der Greifer. Nichts wird geschoben, das schont den Baum.
6. **Zurücktreten:** Arme runter, zurücktreten.

Das Plateau ist eine Ladefläche (z. B. Anhänger oder LKW) mit einer Höhe zwischen **0 und 1,75 m**.

Warum fast senkrecht und nicht waagrecht: Das Kippmoment am Greifer ist Gewicht × waagrechter Abstand des
Schwerpunkts vom Greifer. Waagrecht gehalten ist dieser Abstand am größten (brutaler Hebel), fast senkrecht fast null.

## 3D-Darstellung

[`visualisierung/use-case-christbaum.html`](visualisierung/use-case-christbaum.html) herunterladen und im Browser öffnen.
Die Seite braucht Internet für die 3D-Bibliothek [three.js](https://threejs.org) und die Schriften.

- Ablauf abspielen, anhalten oder an jede Stelle springen
- Plateau-Höhe (0 bis 1,75 m), Baumlänge (1 bis 3 m) und Körpergröße (1,50 bis 2,00 m) einstellen
- „Baum ist schon eingenetzt“ umschalten
- Kamera: schräg, von der Seite oder mitlaufend; ziehen zum Drehen, scrollen oder zwei Finger zum Zoomen
- Lastanzeige: grobe statische Schätzung der Zusatzlast durch den Baum an Greifer, Schultern, Hüfte/Rücken und Knien

## Annahmen (bitte prüfen)

| Thema | Annahme |
|---|---|
| Ausgangslage | Der Baum liegt nach dem Schneiden am Boden, die Spitze zeigt Richtung Plateau. |
| Greifen | Nur am astfreien Stammende, mit einem Stammgreifer, der fest zum Exo gehört. |
| Tragen | Fast senkrecht (78°), Greifer auf Hüfthöhe vor dem Körper, die Person lehnt sich bis etwa 10° zurück. |
| Aufladen | Baum fast lotrecht anheben, bis das Schnittende auf Plateauhöhe ist, absetzen und 90° aufs Plateau kippen lassen. Kein Schieben. |
| Greifer | Kann beim Umkippen frei drehen (Scharnier), damit das Schnittende nicht über die Kante rutscht und das Exo kein Moment abbekommt. |
| Plateau | Ladefläche, 0 bis 1,75 m hoch. |
| Baum | Nordmanntanne. Gewicht aus der Länge geschätzt (2 m ≈ 15 kg), Schwerpunkt bei etwa 31 % der Länge ab Schnittende. |
| Person | Körpergröße 1,80 m (in der Darstellung einstellbar). |
| Exo | Aktiv: Motoren an Hüfte, Knie, Schulter und Ellbogen. Rahmen aus Metall, Teile aus dem 3D-Drucker, Akku und Steuerung am Rücken. |

## Erste Kennzahlen (grobe Schätzung)

Statisch gerechnet, nur die Zusatzlast durch den Baum (ohne Körpergewicht, ohne Schwung). Person 1,80 m, Baum nicht eingenetzt.

**Spitzenwerte je Schritt**, 2 m Baum (≈ 15 kg), Plateau 1,75 m:

| Schritt | Greifer Kraft | Greifer Kippmoment | Schultern | Hüfte / Rücken |
|---|---|---|---|---|
| Aufrichten (nur kurz, solange der Baum noch flach ist) | 147 N | 73 Nm | 94 Nm | 147 Nm |
| Tragen, fast senkrecht | 147 N | 15 Nm | 78 Nm | 70 Nm |
| Hochheben und an der Kante absetzen (Greifer bis 1,92 m, über Kopf) | 147 N | 8 bis 15 Nm | 78 Nm | 70 Nm |
| Umkippen lassen (Schnittende steht auf, Greifer dreht als Scharnier) | 0 N | 0 Nm | 0 Nm | 0 Nm |
| *Zum Vergleich: waagrecht tragen (erste Version)* | *147 N* | *73 Nm* | *135 Nm* | *142 Nm* |

**Nach Baumgröße**, Plateau 1,75 m, Baum nicht eingenetzt:

| Baum | Kippmoment beim Aufrichten (kurz) | Kippmoment beim Tragen | Gewicht über Kopf | Abstand zur Kante beim Hochheben |
|---|---|---|---|---|
| 1,5 m (≈ 7 kg) | 25 Nm | 5 Nm | 7 kg | 0,68 m |
| 2,0 m (≈ 15 kg) | 73 Nm | 15 Nm | 15 kg | 0,83 m (eingenetzt 0,46 m) |
| 2,5 m (≈ 26 kg) | 164 Nm | 34 Nm | 26 kg | 0,97 m |
| 3,0 m (≈ 41 kg) | 309 Nm | 64 Nm | 41 kg | 1,11 m |

Was daraus folgt:

- **Fast senkrecht tragen nimmt den Hebel raus.** Das Kippmoment beim Tragen sinkt auf etwa ein Fünftel (2 m Baum: 15 statt 73 Nm), Schultern und Hüfte werden etwa halb so stark belastet wie beim waagrechten Tragen.
- **Ganz ohne Hebel geht das Aufrichten nicht.** Wer nur am Schnittende greift, schwenkt den Baum kurz durch die flache Lage. Dort wirkt für etwa eine Sekunde das volle Moment (2 m: 73 Nm, 3 m: 309 Nm). Das Exo muss diese Spitze aus der Hocke, nah am Körper abfangen.
- **Kein Schieben, kein Moment beim Umkippen.** Sobald das Schnittende auf der Ladefläche steht, trägt das Plateau das Gewicht. Der Greifer dreht beim Umkippen als Scharnier mit und hält nur den Fuß an seinem Platz.
- **Der Preis: bei hohem Plateau das volle Gewicht über Kopf.** Bei 1,75 m geht der Greifer auf gut 1,9 m, mit dem ganzen Baum (2 m: 15 kg, 3 m: 41 kg). Das Moment ist klein, weil der Baum fast lotrecht steht, aber die Arme sind über dem Kopf. Die Schultermotoren des Exos müssen das tragen und die Last über den Rahmen in den Boden leiten. Mit 1,80 m Körpergröße geht es, mit 1,65 m gerade noch, mit 1,50 m nicht mehr.
- **Ungenetzt braucht beim Hochheben Abstand.** Sonst stoßen die unteren Äste an die Kante. Beim 2 m Baum etwa 0,8 m vor der Kante anheben und den letzten Schritt mit dem Baum über Kopf gehen. Eingenetzt reicht knapp ein halber Meter.
- **Aufprall beim Umkippen.** Der 2 m Baum schlägt etwa so auf, als würde man ihn flach aus 40 cm Höhe fallen lassen, der 3 m Baum wie aus gut 60 cm. Die Äste federn das ab. Ist es zu hart, kann der Greifer das Umkippen bremsen.
- **Fast senkrecht vor dem Körper nimmt ein ungenetzter Baum die Sicht.** Eingenetzt ist er schmal genug, sonst müsste er etwas seitlich getragen werden.

## Offene Fragen

1. Stimmt der Ablauf jetzt (aufrichten, zurückgelehnt tragen, fast lotrecht hochheben bis Plateauhöhe, absetzen, 90° umkippen lassen)?
2. Darf der Baum beim Umkippen frei fallen, oder soll der Greifer das Umkippen bremsen?
3. Ist es in Ordnung, dass der Baum beim Aufrichten kurz durch die flache Lage geht? Ohne diese Spitze ginge es nur, wenn der Baum zuerst an der Spitze angehoben und wie eine Leiter aufgestellt wird. Danach würde man unten greifen.
4. Wie lang und schwer sind die Bäume (kleinster, typischer, größter)? Sind sie beim Aufladen schon eingenetzt?
5. Was genau ist das Plateau (Anhänger, LKW, Palette)? Wie weit ist der Weg vom Schnitt zum Plateau, und wie ist der Boden (Wiese, Hang, Schnee, Matsch)?
6. Wie viele Bäume pro Stunde bzw. pro Tag?
7. Wie groß bist du?
8. Welcher 3D-Drucker steht zur Verfügung (Bauraum, Material)?

## Sicherheit

Ein aktives Exo mit diesen Momenten kann schwer verletzen. Deshalb von Anfang an mitplanen:
Not-Aus, mechanische Endanschläge an jedem Gelenk, Begrenzung von Drehmoment und Geschwindigkeit
und erste Tests auf einem Prüfstand ohne Person.

## Nächste Schritte

1. Use Case bestätigen oder korrigieren
2. Anforderungen festlegen: Lasten, Hubhöhe, Gewicht des Exos, Akkulaufzeit, Sicherheit
3. Konzept: welche Gelenke aktiv sein müssen, Aufbau des Stammgreifers, Metallrahmen und 3D-Druckteile
4. Erster Prototyp: der Stammgreifer, weil dort die größte Last sitzt

## Projektstruktur

```
visualisierung/
  use-case-christbaum.html   3D-Darstellung des Use Cases
```
