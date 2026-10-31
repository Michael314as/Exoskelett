# Entwicklungsplan: vom Konzept zum tragbaren Exo

Dieses Dokument beschreibt, **wie man ein aktives Exoskelett real entwickelt**, was man dafür
wissen und können muss und in welcher Reihenfolge man vorgeht. Es gehört zu
[konzept.md](konzept.md) (was gebaut wird) und zum 3D-Modell
[exo-konstruktion.html](../visualisierung/exo-konstruktion.html) (wie es aussieht).

Grundhaltung: **Du kannst die meisten Bäume schon selbst bewegen.** Das Exo muss dich also
nicht ersetzen, sondern **entlasten** – weniger Rücken, weniger Ermüdung, mehr Ausdauer über
eine ganze Saison. Das senkt die Anforderungen stark: Ein Exo, das nur 20 bis 40 % hilft und
dabei nie stört, ist besser als ein starkes, das klemmt oder dich aus dem Gleichgewicht bringt.

---

## 1. Das wichtigste Grundverständnis

### Mechanik
- **Hebel und Drehmoment.** Kraft mal Hebelarm. Jedes Gelenk des Exos ist ein Hebel; ein Seil,
  das weiter vom Gelenk entfernt angreift, braucht weniger Kraft. Das ist der Kern, warum
  Umlenkrollen und Ankerpunkte da sitzen, wo sie sitzen.
- **Kraftfluss.** Sich immer fragen: Wo geht die Kraft hin? Beim Exo soll sie über Schienen in
  die Steigbügel und in den Boden gehen – nicht über Haut, Knie oder Wirbelsäule.
- **Gelenkachsen und Fluchtung.** Die Achse des Exo-Gelenks muss möglichst genau auf der
  Achse deines Gelenks liegen. Liegt sie daneben, rutschen die Manschetten bei jeder Bewegung
  und scheuern. Das ist der häufigste Fehler bei Exos überhaupt.
- **Festigkeit, Knicken, Lochleibung.** Alu-Rohre versagen selten auf Zug, eher an Bohrungen
  (Loch reißt aus) oder durch Knicken. Faustregeln: Bohrungen nicht zu nah am Rand,
  Passschrauben statt Gewindeteil im Lager, Doppelplatten an Gelenken.
- **Lager und Spiel.** Ein bisschen Spiel an der Manschette ist gewollt, Spiel im Gelenk nie.
- **Seile.** Dyneema ist extrem fest, aber rutschig (Knoten halten schlecht → Spleiß oder
  Klemmanker) und mag keine scharfen Kanten. Umlenkradius mindestens 8–10 × Seildurchmesser.

### Elektrik
- **Akku sicher nutzen.** Strom, Spannung, Leistung. Jede Leitung ab dem Akku braucht eine
  Sicherung, die passend zum Kabelquerschnitt ausgelegt ist.
- **Not-Aus richtig.** Ein echter Not-Aus trennt die Motoren hart (Schütz oder Leistungsschalter),
  nicht nur per Software.
- **CAN-Bus.** Wie die Motoren angesprochen werden: zwei verdrillte Leitungen, Abschluss-
  widerstände an beiden Enden, jeder Motor eine eigene ID.
- **Stecker und Feuchtigkeit.** Kontakte korrodieren bei Nässe und Salz; abgedichtete Stecker
  (z. B. Deutsch DT oder wasserdichte M12) statt Breadboard-Kabel.

### Steuerung und Programmierung
- **Sensoren.** Motor-Encoder (Winkel), IMUs (Neigung von Rumpf und Schenkeln), evtl.
  Fußschalter. Verstehen, wie verrauscht diese Werte sind und wie man sie filtert.
- **Regelung.** Motoren nicht auf Position regeln, sondern auf **Kraft/Moment** (Strom). Das Exo
  soll mitschieben, nicht führen. Ein Seil kann nur ziehen – das macht die Regelung einfacher
  und sicherer.
- **Absichtserkennung.** Woran erkennt das Exo, dass du hebst, trägst oder gehst? Am Anfang:
  einfache Regeln aus Winkeln und Winkelgeschwindigkeiten. Keine KI nötig.
- **Zustandsmaschine.** Klar definierte Zustände (Aus, Bereit, Unterstützen, Fehler) mit sicheren
  Übergängen. Jeder unbekannte Zustand führt zu „Motoren stromlos“.

### Ergonomie und Körper
- Wo darf Druck auf den Körper, wo nicht (Knochenvorsprünge, Kniekehle, Leiste).
- Wie ein Hüftgurt Last trägt (Beckenkamm), warum Schultergurte nur stabilisieren sollen.
- Dass sich Gelenke beim Bewegen leicht verschieben (das Knie rollt und gleitet).

### Sicherheit
- Was passiert bei Seilriss, Akku leer, Software hängt, Sturz? Jede dieser Fragen braucht eine
  Antwort, bevor das Exo am Körper eingeschaltet wird.
- Mechanische Anschläge an jedem Gelenk, damit kein Motor ein Gelenk überstrecken kann.

---

## 2. Was du schon mitbringst und was du dazulernen solltest

| Bereich | Stand | Was hilft |
| --- | --- | --- |
| Bauen, Schrauben, Bohren | vorhanden | Ständerbohrmaschine oder Bohrschablonen für genaue Löcher |
| Schweißen | schwach | **nicht nötig**: alles geschraubt |
| Drehen | keine Drehbank | Teile kaufen (Lager, Rollen) oder online fertigen lassen (Trommeln) |
| 3D-Druck (A1 mini) | vorhanden | Halter, Kappen, Seilführungen, Gehäuse; PETG/ASA außen, TPU für Polster und Zähne |
| Löten | etwas | Crimpen lernen (Aderendhülsen, wasserdichte Stecker) |
| Programmieren | etwas | Arduino/ESP32-Grundlagen, CAN-Bibliothek, einfache Zustandsmaschine |
| Mechanik-Grundlagen | lernen | Hebelgesetz, Drehmoment, Kraftfluss (YouTube/Technik-Schulbücher reichen) |
| CAD | lernen | Fusion 360 (privat kostenlos) oder Onshape: Teile zeichnen, Bohrbilder drucken |

---

## 3. Phasen der Entwicklung

Jede Phase endet mit einem **Test, den man bestehen muss**, bevor die nächste beginnt. So
merkt man früh, wenn etwas nicht funktioniert, und hat jederzeit etwas Nutzbares.

### Phase 0 – Messen und Verstehen (1–2 Wochen)
- Eigene Maße nehmen: Hüft-, Knie-, Knöchelachse, Oberschenkel-/Unterschenkellänge,
  Schulterbreite, Unterarm. Gelenkachsen mit Klebepunkten markieren und von der Seite fotografieren.
- Echte Arbeit filmen (Aufladen, Schleifen). Wo ermüdest du? Rücken, Oberschenkel, Unterarme?
  Das entscheidet, welche Gelenke zuerst Unterstützung brauchen.
- **Ergebnis:** Maßblatt + kurze Liste „Was tut am meisten weh / wo wird es zäh“.

### Phase 1 – Schlaufengreifer und Steigbügel ohne Exo (2–4 Wochen, ~150–250 €)
- **Schlaufengreifer als Handwerkzeug:** Schlaufenkopf mit Zahnleiste am Handgelenk einer festen Unterarm-Manschette
  mit Handgriff, zwei Schlaufen, zwei Schneckengetriebe-Winden, Daumentaster, ein Akku-Pack.
- **Steigbügel** mit Spikes für die eigenen Arbeitsschuhe.
- **Test:** Eine Saison-Woche mit echten Bäumen. Schließen die Schlaufen schnell genug? Halten sie bei
  Nässe und Frost? Lassen sie sich mit Handschuhen bedienen und notfalls von Hand öffnen? Wie läuft es
  sich mit den Steigbügeln im Schlamm?
- Warum zuerst: Der Greifer bringt sofort Nutzen, ist billig und zeigt, wie Kräfte wirklich wirken.

### Phase 2 – Passiver Rahmen (4–8 Wochen, ~400–800 €)
- Rückenrahmen, Hüftgurt, Hüftgelenke mit Abspreizgelenk und Drehlager, Bein-Schienen mit Knie, Steigbügel.
  **Noch ohne Motoren**, aber mit allen Gelenkanschlägen.
- Längen über Steckbolzen einstellbar; Gelenkachsen nach Maßblatt ausrichten.
- **Tests:**
  - 30 Minuten gehen, Treppen, Hocke, in die Knie gehen – scheuert etwas, rutscht etwas?
  - Rückwärts auf eine Matte fallen lassen (mit Helfer): Kommst du aus dem Rahmen raus?
  - Rahmen mit Gewichten belasten (Sandsäcke), Verbindungen auf Spiel prüfen.
- Hier passiert die meiste Anpassung. Rechne mit 2–3 Umbauten. Das ist normal.

### Phase 3 – Prüfstand: ein Antrieb, eine Sensor-Manschette (3–6 Wochen, ~400–900 €)
- **Ein** Motor mit Doppeltrommel, ein Bowdenzug-Paar, eine Seilscheibe an einem Holzarm mit Scharnier.
- **Eine Sensor-Manschette** (Wägezelle + Verstärker) auf dem Holzarm. Mit der Hand führen: Das Exo
  soll folgen. Verstärkung K langsam steigern, Schwingungsgrenze finden (siehe [steuerung.md](steuerung.md)).
- Steuerung: ESP32 oder Teensy, CAN-Transceiver, Not-Aus-Schütz, Sicherung, Akku-Adapter.
- Lernen: Motor ansprechen, Strom/Moment vorgeben, Encoder auslesen, Seil spannen ohne zu
  ziehen („Leerlauf mit leichter Vorspannung“), Fehlerfälle.
- **Tests:** Not-Aus trennt immer. Kabel ab → Motor stromlos. Software hängt → Watchdog
  schaltet ab. Seil schlaff → Motor wickelt nicht unkontrolliert.
- Warum vor dem Körper: Alles, was hier schiefgeht, tut nicht weh.

### Phase 4 – Beinantrieb (4–8 Wochen, ~800–1.500 €)
- Antriebsbox mit zwei Beinmotoren an den Rahmen, Seile über Hüft- und Knierolle.
- Sensor-Manschetten an Ober- und Unterschenkel.
- Zuerst mit **sehr kleinem K** und Totmannschalter in der Hand.
- Unterstützung erst ab Kniebeugung; beim normalen Gehen bleiben die Beine durchsichtig.
- **Tests:** Aufstehen aus der Hocke 50× mit und ohne Exo, Gefühl und Puls vergleichen.
  Treppen. Gehen auf Schnee. Wie fühlt sich das Abschalten mitten in der Bewegung an?

### Phase 5 – Schultern und Arme (4–8 Wochen, ~600–1.200 €)
- Schulter- und Ellbogenmotoren mit Bowdenzug-Paaren und Sensor-Manschetten, in beide Richtungen.
- Dann den Schlaufengreifer aus Phase 1 an die Unterarmschienen.
- **Tests:** Arme frei bewegen ohne Widerstand (Exo „schwerelos“); Sandsack in den Schlaufen heben und
  senken; Baum aufrichten; 2–4 Bäume schleifen.

### Phase 5b – Komfort und Finish (2–4 Wochen, ~330–400 €)
- Parkstütze, Arbeitslicht, LED-Ring, Display, Vibration, Magnet-Schnallen, Schnellkupplungen, Abdeckungen.
- Wenn alle Bohrungen sitzen: Alu-Teile gesammelt eloxieren oder pulverbeschichten lassen.
- **Tests:** Einsteigen in unter 90 Sekunden, mit Handschuhen. Alles im Dunkeln bedienbar? Ablesbar bei Kälte?
  Details in [bedienung-und-features.md](bedienung-und-features.md).

### Phase 6 – Wetter und Dauer (1 Saison)
- Abdichten, Kondenswasser, Frost an Gelenken, Seilverschleiß, Akkulaufzeit bei Kälte.
- Wartungsliste führen: Was ist nach 10, 50, 100 Stunden verschlissen?
- **Ergebnis:** Liste der Änderungen für Version 2.

---

## 4. Werkzeug und Arbeitsplatz

- Ständerbohrmaschine (oder gute Bohrschablonen aus dem 3D-Drucker), Metallsäge oder
  Kapp-/Bandsäge für Alu, Feilen, Entgrater, Gewindeschneider M5–M10.
- Drehmomentschlüssel (kleiner Bereich) für Gelenkschrauben.
- Multimeter, Crimpzange, Lötkolben, Labornetzteil mit Strombegrenzung (für erste Motortests).
- Federwaage oder Kofferwaage zum Messen von Seilkräften.
- Laptop mit CAD und Arduino/PlatformIO.

---

## 5. Typische Stolpersteine

| Problem | Woran man es merkt | Was hilft |
| --- | --- | --- |
| Gelenkachsen sitzen falsch | Manschetten rutschen, Druckstellen | Achsen nachmessen, Langlöcher für Feinjustage |
| Zu viel Gewicht | Ermüdung trotz Unterstützung | Akku und Motoren nah am Rücken, Schienen kürzen, Hohlprofile |
| Seil springt von Rolle | Unterstützung bricht plötzlich ab | Rollen mit tiefer Rille und Abdeckung, Seilführung kurz vor jeder Rolle |
| Motor zu träge oder zu ruppig | Exo „schiebt nach“ oder ruckt | Moment langsam ein- und ausblenden, nicht schalten |
| Elektronik bei Nässe | Aussetzer | dichte Box, Stecker nach unten, Silikagel |
| Frust durch zu viel auf einmal | Projekt stockt | Phasen einhalten, jede Phase muss schon nutzen |

---

## 6. Sicherheitsregeln während der Entwicklung

1. Kein Motor am Körper, bevor er auf dem Prüfstand 10 Stunden ohne Fehler lief.
2. Not-Aus immer mit der freien Hand erreichbar; zusätzlich Totmannschalter in der Testphase.
3. Mechanische Anschläge sind Pflicht, Software-Grenzen kommen dazu, nie statt dessen.
4. Neue Funktionen zuerst mit kleinem Moment, dann schrittweise mehr.
5. Erste Tests nie allein und nie mit echten schweren Bäumen.
6. Jede Änderung kurz notieren (Datum, was, warum). Dann weiß man später, was geholfen hat.

---

## 7. Zeit- und Kostenrahmen (grob)

| Phase | Zeit (Feierabend) | Kosten |
| --- | --- | --- |
| 0 Messen | 1–2 Wochen | ~0 € |
| 1 Schlaufengreifer, Steigbügel | 2–4 Wochen | 150–250 € |
| 2 Passiver Rahmen | 4–8 Wochen | 400–800 € |
| 3 Prüfstand | 3–6 Wochen | 400–900 € |
| 4 Beinantrieb | 4–8 Wochen | 800–1.500 € |
| 5 Arme | 4–8 Wochen | 600–1.200 € |
| 6 Saison-Test | 1 Saison | Verschleißteile |

Realistisch also **ein Jahr Freizeitprojekt** bis zu einer Version, die eine Saison mitarbeitet.
Nach Phase 1 und 2 hast du bereits Werkzeuge, die im Alltag helfen, auch wenn die Motoren
später kommen.
