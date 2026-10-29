# Exoskelett

Aktives Exoskelett aus Metall, das beim Aufladen frisch geschnittener Christbäume entlastet.

> **Stand:** Der Use Case wird gerade abgestimmt. Es gibt noch keine Mechanik, Elektronik oder Firmware.
> Das Exo in der 3D-Darstellung ist ein Platzhalter-Konzept, kein Entwurf.

## Use Case 1: Christbaum aufladen

Der Use Case ist der Grundstein des Projekts. Gelenke, Motoren, Greifer und Akku werden daraus abgeleitet.

1. **Greifen:** In die Hocke gehen und den Stammgreifer am astfreien Stammende (Schnittende) schließen.
2. **Anheben bis waagrecht:** Das Schnittende geht hoch, die Spitze schleift noch am Boden. Dann stellt das Exo den Baum waagrecht und die Spitze hebt ab.
3. **Tragen:** Den Baum waagrecht vor dem Körper zum Plateau tragen.
4. **Hochheben:** Den Stamm knapp über die Plateaukante bringen.
5. **Aufschieben:** Mit der Spitze voran aufs Plateau schieben.
6. **Ablegen und loslassen:** Absenken, Greifer öffnen, zurücktreten.

Das Plateau ist eine Ladefläche (z. B. Anhänger oder LKW) mit einer Höhe zwischen **0 und 1,75 m**.

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
| Halten | Der Baum wird frei gehalten. Nur das Exo trägt ihn, an der Spitze stützt nichts. |
| Plateau | Ladefläche, 0 bis 1,75 m hoch. |
| Baum | Nordmanntanne. Gewicht aus der Länge geschätzt (2 m ≈ 15 kg), Schwerpunkt bei etwa 31 % der Länge ab Schnittende. |
| Person | Körpergröße 1,80 m (in der Darstellung einstellbar). |
| Exo | Aktiv: Motoren an Hüfte, Knie, Schulter und Ellbogen. Rahmen aus Metall, Teile aus dem 3D-Drucker, Akku und Steuerung am Rücken. |

## Erste Kennzahlen (grobe Schätzung)

Statisch gerechnet, nur die Zusatzlast durch den Baum (ohne Körpergewicht, ohne Schwung). Person 1,80 m, Baum nicht eingenetzt. Werte sind Spitzenwerte über den ganzen Ablauf.

| Baum | Plateau | Greifer Kraft | Greifer Kippmoment | Schultern | Hüfte / Rücken | Knie | Höchster Griffpunkt |
|---|---|---|---|---|---|---|---|
| 1,5 m (≈ 7 kg) | 1,75 m | 72 N | 25 Nm | 55 Nm | 59 Nm | 55 Nm | 2,00 m, über Kopf |
| 2,0 m (≈ 15 kg) | 1,20 m | 147 N | 73 Nm | 135 Nm | 142 Nm | 133 Nm | 1,51 m |
| 2,0 m (≈ 15 kg) | 1,75 m | 147 N | 73 Nm | 135 Nm | 142 Nm | 134 Nm | 2,06 m, über Kopf |
| 2,5 m (≈ 26 kg) | 1,75 m | 257 N | 164 Nm | 272 Nm | 284 Nm | 270 Nm | 2,13 m, 4 cm zu hoch für die Arme |
| 3,0 m (≈ 41 kg) | 1,75 m | 405 N | 309 Nm | 479 Nm | 498 Nm | 479 Nm | 2,16 m, 9 cm zu hoch für die Arme |

Was daraus folgt:

- **Das Kippmoment am Greifer ist der Kern des Problems.** Einen Baum nur am Schnittende waagrecht zu halten, braucht bei 2 m etwa 70 Nm, bei 3 m etwa 300 Nm. Mit bloßen Händen geht das kaum.
- **Bei 1,75 m Plateau ist es Arbeit über Kopf.** Der Stamm muss auf etwa 2 m und höher. Ab etwa 2,5 m Baumlänge reichen die Arme einer 1,80 m großen Person nicht mehr.
- **Eingenetzte Bäume sind schmaler** und müssen weniger hoch gehoben werden (2 m Baum auf 1,75 m Plateau: Stamm auf 1,98 statt 2,06 m).

## Offene Fragen

1. Stimmt der Ablauf in der 3D-Darstellung?
2. Muss der Baum frei waagrecht gehalten werden, oder darf die Spitze zuerst aufs Plateau gelegt und der Baum dann nachgeschoben werden? Das zweite braucht deutlich weniger Kraft.
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
