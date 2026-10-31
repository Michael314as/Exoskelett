# Exoskelett

Aktives Exoskelett aus Metall, das beim Aufladen und Schleifen frisch geschnittener Christbäume entlastet.

> **Stand: Entwurf 3.2.** Konzept, Steuerung, Kraftverteilung und Entwicklungsplan stehen, dazu zwei 3D-Ansichten.
> Gebaut ist noch nichts.

## Kurz gesagt

- Motoren und Stihl-Akku sitzen hinten im Rückenrahmen.
- **Beine:** ein Dyneema-Seil pro Bein streckt Hüfte und Knie (nur nach oben, Beugen macht man selbst). Es läuft um Scheiben auf den Gelenkachsen, am Knie eine Kurvenscheibe mit mehr Hebel in der Hocke.
- **Hüfte:** Beine lassen sich frei seitlich abspreizen und drehen; angetrieben wird nur vor/zurück.
- **Arme:** Seilpaare auf Seilscheiben bewegen Schultern (offenes Seil) und Ellbogen (Bowdenzug) in **beide Richtungen**.
- **Greifen:** Schlaufen kommen am Handgelenk nach unten heraus, rechts zwei, links eine; der Stamm liegt längs als Verlängerung des Unterarms. Jede Schlaufe hat eine eigene kleine Winde.
- **Steuerung:** Die blauen Manschetten sitzen auf Kraftsensoren. Drückst du gegen eine Manschette, bewegt das Exo das Gelenk mit.
- **Füße:** Steigbügel mit Spikes nach dem Prinzip eines Steigeisens, für Schlamm, Schnee und unebenes Gelände.

- **Bedienung und Komfort:** Parkstütze zum Einsteigen in unter 90 s, Arbeitslicht, LED-Ring und Display, Baumzähler, Handy-Anzeige, Heizgriffe, Magnet-Schnallen, in 3 Teile zerlegbar, eloxierte Oberfläche. Alles baubar ohne eigene Maschinen (Laserteile bestellen, 3D-Druck, Bohrständer).

Details: [docs/konzept.md](docs/konzept.md) · [docs/bedienung-und-features.md](docs/bedienung-und-features.md) · [docs/stueckliste.md](docs/stueckliste.md) · [docs/steuerung.md](docs/steuerung.md) ·
[docs/kraftverteilung.md](docs/kraftverteilung.md) · [docs/entwicklungsplan.md](docs/entwicklungsplan.md)

## Use Case 1: Christbaum aufladen

1. **Greifen:** In die Hocke gehen. Die rechte Hand fasst den Stumpf wie einen Stiel, beide rechten Schlaufen am Handgelenk ziehen zu. Die linke Hand greift so weit Richtung Spitze wie möglich.
2. **Aufrichten bis fast senkrecht:** Zurücklehnen. Rechts ist der Drehpunkt, links bringt der Hebel den Baum in die Senkrechte.
3. **Tragen:** Fast senkrecht und leicht zurückgelehnt zum Plateau gehen.
4. **Hochheben:** Den Baum fast lotrecht anheben, bis das Schnittende auf Plateauhöhe ist, und knapp hinter der Kante absetzen.
5. **Umkippen lassen:** Die Schlaufen halten das Schnittende wie ein Scharnier, der Baum kippt 90° aufs Plateau und darf frei fallen. Nichts wird geschoben.
6. **Zurücktreten:** Schlaufen auf, Arme runter, zurück.

Das Plateau ist eine Ladefläche mit einer Höhe zwischen **0 und 1,75 m**.

## Use Case 2: Bäume schleifen

Ein bis drei Bäume über den Boden zum Plateau ziehen, bis etwa 100 m, **vorwärts gehend**. Die Stümpfe hängen in den
Schlaufen am Handgelenk (je Schlaufe ein Stumpf: rechts zwei, links einer), die Arme hängen, die Spitzen schleifen hinterher. Die Last geht über
Schultergelenke und Rahmen in Hüftgurt und Exo-Beine statt in die Arme.

## Festgelegt

| Thema | Festlegung |
|---|---|
| Ziel | Entlasten: Das Exo unterstützt die eigene Bewegung, etwa mit doppelter Kraft. Die meisten Bäume schaffst du ohnehin selbst. |
| Bäume | 1 bis 4 m lang, nicht eingenetzt, sehr unterschiedlich schwer |
| Gelände | Wiese, Schlamm, nasser Schnee, Frostregen, uneben |
| Person | 1,75 m groß, etwa 75 kg |
| Akku | Stihl AP 500 S (vorhanden) |
| Bauweise | Alu-Profile, geschraubt; Teile aus dem 3D-Drucker (Bambu Lab A1 mini) |
| Budget | einige tausend Euro, über die Zeit verteilt |
| Bedienung | Ein/Aus, Not-Aus, Daumentaster für die Schlaufen; alles andere über die Sensor-Manschetten |

## 3D-Darstellung

Beide Seiten herunterladen und im Browser öffnen; sie brauchen Internet für [three.js](https://threejs.org) und die Schriften.

**Konstruktion:** [`visualisierung/exo-konstruktion.html`](visualisierung/exo-konstruktion.html)
- 17 Baugruppen zum Antippen oder als automatischer **Rundgang**: was sie tun, wie sie gebaut sind, welche Teile, worauf achten
- Umgebung: verschneite Plantage in der Dämmerung (mit Arbeitslicht und Schneefall) oder Werkstatt mit zerlegten Teilen
- Parkstütze ausklappen, Licht schalten, Oberfläche roh oder eloxiert, Beine spreizen
- LED-Ring und Display am Schultergurt zeigen live Zustand, Akku und gezählte Bäume
- Arm heben/Ellbogen: Der Arm des Menschen führt, das Exo folgt; Pfeile an den Manschetten und eine Live-Anzeige zeigen, was die Sensoren messen
- Schlaufen zuziehen, mit und ohne Stamm
- **Kraftfluss:** zeigt farbig, wie die Last eines 3 m Baums vom Stamm bis in den Boden läuft
- Werkbank mit zerlegtem Arm-Antrieb, Sensor-Manschette und Schlaufenwinde

**Ablauf:** [`visualisierung/use-case-christbaum.html`](visualisierung/use-case-christbaum.html)
- Aufladen und Schleifen abspielen, anhalten oder an jede Stelle springen
- Plateau-Höhe, Baumlänge, Baumgewicht, Anzahl Bäume, Körper- und Exo-Gewicht einstellen
- Grobe Lastanzeige und Hinweise, wenn die Arme nicht reichen oder das Gleichgewicht kippt; bei Bedarf macht die Figur automatisch einen Ausfallschritt
- Dämmerung in der verschneiten Plantage mit Arbeitslicht, LED-Ring und Baumzähler (oder Tag)

## Kräfte, kurz

Ausführlich in [docs/kraftverteilung.md](docs/kraftverteilung.md). Das Wichtigste:

- Die **Schultern** sind der Engpass, wenn der Baum vor dem Körper hängt. Tragen mit steilem Unterarm (wie eine Fackel) hält den Ellbogen fast lastfrei.
- **Beine und Hüfte** sind gut versorgt; Aufrichten und Hochheben kommen am besten aus den Beinen.
- **Schleifen** ist für das Exo leicht: Die Last zieht fast nur längs an den Schienen.
- Ohne lange Fußplatten hilft beim Aufrichten schwerer Bäume ein **Ausfallschritt**.

## 3D-Druck (Bambu Lab A1 mini)

- Bauraum 18 × 18 × 18 cm: größere Teile in Segmente teilen und verschrauben.
- Für draußen im Winter PETG statt PLA; TPU für Polster, Puffer und Zahnleisten.
- Tragende Teile (Rahmen, Achsen, Schlaufenkopf, Seilscheiben) aus Metall. Gedruckt werden Manschetten, Gehäuse, Kabel- und Seilführungen, Sensorhalter und Bohrschablonen.

## Offene Fragen

1. Kannst du ein paar schwere Bäume wiegen (Kofferwaage, Kranwaage)? Dann werden die Lastannahmen Messwerte.
2. Wie viele Bäume pro Tag? Das bestimmt, ob ein zweiter Akku nötig ist.

## Sicherheit

Ein aktives Exo kann schwer verletzen. Von Anfang an: Not-Aus, der die Motoren hart trennt, mechanische Endanschläge an
jedem Gelenk, Begrenzung von Moment und Geschwindigkeit, Notöffnung der Schlaufen von Hand, erste Tests auf einem
Prüfstand ohne Person. Nicht mit Motorsäge in der Hand im unterstützten Modus arbeiten.

## Nächste Schritte

Der Weg vom Konzept zum tragbaren Exo steht in [`docs/entwicklungsplan.md`](docs/entwicklungsplan.md). Kurz:

1. Maße nehmen und echte Arbeit filmen
2. Schlaufengreifer als Handwerkzeug bauen, Steigbügel testen
3. Passiver Rahmen mit Gelenken, ohne Motoren
4. Prüfstand mit einem Motor und einer Sensor-Manschette, dann Beine, dann Arme

## Projektstruktur

```
docs/
  konzept.md                 Konzept Entwurf 3: Aufbau, Antriebe, Greifer, Füße, Kosten
  steuerung.md               Sensor-Manschetten, Regelung, Elektronik, Sicherheit
  bedienung-und-features.md  Bedienung, Komfort-Features, Fertigung ohne Maschinen, Kosten
  stueckliste.md             Jedes Teil mit Herstellweg (Kauf, Laser, Druck, Bohren), Druckgrößen, Werkzeug
  kraftverteilung.md         Lasten je Arbeitsschritt, Kraftfluss, was die Motoren schaffen
  entwicklungsplan.md        Grundwissen, Phasen, Tests, Werkzeug, Stolpersteine
tools/
  kraftverteilung.py         Rechnung zu kraftverteilung.md
visualisierung/
  exo-konstruktion.html      3D: Konstruktion im Detail, Baugruppen erklärt, Kraftfluss
  use-case-christbaum.html   3D: beide Use Cases (Aufladen, Schleifen) mit dem Exo laut Konzept
```
