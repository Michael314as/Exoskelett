# Exoskelett

Aktives Exoskelett aus Metall, das beim Aufladen frisch geschnittener Christbäume entlastet.

> **Stand:** Der Use Case wird gerade abgestimmt. Es gibt noch keine Mechanik, Elektronik oder Firmware.
> Das Exo in der 3D-Darstellung ist ein Platzhalter-Konzept, kein Entwurf.

## Use Case 1: Christbaum aufladen

Der Use Case ist der Grundstein des Projekts. Gelenke, Motoren, Greifer und Akku werden daraus abgeleitet.

1. **Greifen:** In die Hocke gehen und den Stammgreifer am astfreien Stammende (Schnittende) schließen.
2. **Aufrichten bis fast senkrecht:** Aus der Hocke hochkommen, den Baum hochschwenken und sich zurücklehnen.
3. **Tragen:** Fast senkrecht (78°) und leicht zurückgelehnt zum Plateau gehen.
4. **Ankippen:** Den Baum nach vorne kippen, bis er auf der Plateaukante bzw. mit der Spitze auf dem Plateau aufliegt.
5. **Aufschieben:** Das Schnittende anheben und den Baum aufs Plateau schieben. Das Plateau trägt dabei den größten Teil.
6. **Loslassen:** Greifer öffnen, zurücktreten.

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
| Aufladen | Beim Ankippen und Aufschieben liegt der Baum auf Kante bzw. Plateau auf, das Plateau trägt mit. |
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
| Ankippen auf die Kante | 147 N | 33 Nm | 80 Nm | 86 Nm |
| Aufschieben, Plateau trägt mit | höchstens 81 N, fällt auf 0 | 0 Nm | 26 Nm | 29 Nm |
| *Zum Vergleich: waagrecht tragen (erste Version)* | *147 N* | *73 Nm* | *135 Nm* | *142 Nm* |

**Kippmoment am Greifer nach Baumgröße**, Plateau 1,75 m:

| Baum | Aufrichten (kurze Spitze) | Tragen | Ankippen |
|---|---|---|---|
| 1,5 m (≈ 7 kg) | 25 Nm | 5 Nm | 12 Nm |
| 2,0 m (≈ 15 kg) | 73 Nm | 15 Nm | 33 Nm |
| 2,5 m (≈ 26 kg) | 164 Nm | 34 Nm | 72 Nm |
| 3,0 m (≈ 41 kg) | 309 Nm | 64 Nm | 132 Nm |

Was daraus folgt:

- **Fast senkrecht tragen nimmt den Hebel raus.** Das Kippmoment beim Tragen sinkt auf etwa ein Fünftel (2 m Baum: 15 statt 73 Nm), Schultern und Hüfte werden etwa halb so stark belastet wie beim waagrechten Tragen.
- **Ganz ohne Hebel geht das Aufrichten nicht.** Wer nur am Schnittende greift, schwenkt den Baum kurz durch die flache Lage. Dort wirkt für etwa eine Sekunde das volle Moment (2 m: 73 Nm, 3 m: 309 Nm). Das Exo muss diese Spitze aus der Hocke, nah am Körper abfangen. Bei niedrigen Plateaus (unter etwa 1 m) gilt das auch beim Ankippen, weil der Baum fast flach wird, bevor er aufliegt.
- **Hohe Plateaus sind beim Aufladen sogar leichter.** Die Kante fängt den Baum früh ab (bei 1,75 m schon bei etwa 63°), danach trägt das Plateau.
- **Über Kopf nur am Schluss und fast ohne Last.** Beim Aufschieben auf 1,75 m geht das Schnittende auf etwa 2 m, dann liegt der Baum aber schon zum größten Teil oben. Bei 2,5 bis 3 m Bäumen bleibt das Schnittende 5 bis 11 cm unter der Ladefläche und steht über, der Baum liegt trotzdem sicher oben.
- **Fast senkrecht vor dem Körper nimmt ein ungenetzter Baum die Sicht.** Eingenetzt ist er schmal genug, sonst müsste er etwas seitlich getragen werden.

## Offene Fragen

1. Stimmt der neue Ablauf (aufrichten, zurückgelehnt tragen, auf die Kante kippen, aufschieben)?
2. Ist es in Ordnung, dass der Baum beim Aufrichten kurz durch die flache Lage geht? Ohne diese Spitze ginge es nur, wenn der Baum zuerst an der Spitze angehoben und wie eine Leiter aufgestellt wird. Danach würde man unten greifen.
3. Wie lang und schwer sind die Bäume (kleinster, typischer, größter)? Sind sie beim Aufladen schon eingenetzt?
4. Was genau ist das Plateau (Anhänger, LKW, Palette)? Wie weit ist der Weg vom Schnitt zum Plateau, und wie ist der Boden (Wiese, Hang, Schnee, Matsch)?
5. Wie viele Bäume pro Stunde bzw. pro Tag?
6. Wie groß bist du?
7. Welcher 3D-Drucker steht zur Verfügung (Bauraum, Material)?

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
