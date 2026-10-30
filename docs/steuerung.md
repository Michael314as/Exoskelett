# Steuerung: die Manschetten als Schnittstelle

**Idee:** Die blauen Manschetten, die das Exo mit deinem Körper verbinden, sind gleichzeitig die Bedienung.
Hebst du den Arm, drückt dein Oberarm von unten gegen die Manschette. Ein Kraftsensor misst diesen Druck und das Exo
bewegt das Schultergelenk in dieselbe Richtung. Je stärker du drückst, desto mehr hilft es. Hörst du auf zu drücken,
bleibt es stehen. Das Exo folgt dir, du führst.

Das ist das Prinzip der großen Verstärker-Exoskelette aus der Forschung, hier in einer Form, die eine Person mit
Bastelwerkzeug, Teilen für ein paar Euro und einem Mikrocontroller bauen kann.

---

## 1. Die Sensor-Manschette

```
      Schiene (Alu)                    Manschette (PETG + Polster + Klett)
   ┌──────────────┐                ┌───────────────────────┐
   │  Halter      │═══ Wägezelle ══│  Schale               │   ← dein Arm liegt hier
   │  (fest)      │   (Biegebalken)│                       │
   └──────────────┘                └───────────────────────┘
          ▲  Überlast-Puffer (TPU), 1 mm Spalt  ▲
```

- **Wägezelle:** Einpunkt-Biegebalken aus Alu, 10 oder 20 kg, etwa 80 × 13 × 13 mm, wie sie in Küchen- und
  Paketwaagen stecken. 3–8 € das Stück, als IP65-Ausführung etwas teurer. Sie misst Kraft in **einer** Richtung, und
  zwar quer zur Schiene in der Bewegungsebene (Arm vor/zurück bzw. Bein vor/zurück). Kräfte in anderen Richtungen und
  außermittige Lasten stören sie kaum, dafür ist diese Bauform gemacht.
- **Einbau:** Ein Ende an einen gedruckten oder gefrästen Halter an der Schiene, das andere Ende an die Manschettenschale.
  Die Manschette „schwimmt“ also einen Hauch auf der Zelle.
- **Überlastschutz:** Zwei TPU-Puffer mit etwa 1 mm Spalt. Bei mehr als etwa 15 kg liegen sie an und die Kraft geht an der
  Zelle vorbei. Stürzt du oder stößt irgendwo an, geht die Zelle nicht kaputt.
- **Verstärker:** je Zelle ein kleiner Wägezellen-Verstärker (HX711 im 80-Hz-Modus, 2–3 €; besser NAU7802 mit bis zu
  320 Messungen pro Sekunde) in einem vergossenen Gehäuschen direkt an der Manschette. Vier dünne Adern zur Steuerung.
- **Wo:** 8 Stück.

| Manschette | steuert | Richtung |
|---|---|---|
| Oberarm links/rechts | Schulter | heben **und** senken |
| Unterarm links/rechts | Ellbogen | beugen **und** strecken |
| Oberschenkel links/rechts | Beinseil (Hüfte) | nur strecken |
| Unterschenkel links/rechts | Beinseil (Knie) | nur strecken |

**Warum die Baumlast die Messung nicht stört:** Der Stamm hängt in den Schlaufen am Schlaufenkopf, und der ist fest mit
der Unterarmschiene verschraubt. Die Last geht über die Schienen in den Rahmen. Durch die Manschette geht nur, was **dein
Körper** gegen das Exo drückt. Wenn das Exo unter der Baumlast etwas nachgibt, drückt die Manschette auf deinen Arm, du
hältst dagegen, die Zelle misst das und das Exo drückt stärker. Du spürst dadurch einen Teil der Last und merkst, wenn
etwas schwer wird. Das ist gewollt.

---

## 2. Regelung in einfachen Worten

Für jedes Gelenk, etwa 200-mal pro Sekunde:

```
Motormoment = Schwerkraftausgleich + K × (Manschettenkraft × Hebelarm) − Dämpfung × Gelenkgeschwindigkeit
```

1. **Schwerkraftausgleich:** Die Steuerung kennt den Gelenkwinkel (Encoder) und weiß, wie schwer die Exo-Schiene ist.
   Sie hält das Eigengewicht, damit sich das Exo leer „schwerelos“ anfühlt.
2. **Verstärkung K:** wie viel das Exo zu deiner Kraft dazugibt. K = 1 heißt doppelte Kraft. Start mit K = 0,5,
   langsam steigern, Obergrenze etwa 3.
3. **Dämpfung:** bremst schnelle Bewegungen leicht ab. Das verhindert Aufschaukeln.
4. **Totband:** Kräfte unter etwa 5 N werden ignoriert (Rauschen, Gurt sitzt etwas schief).
5. **Filter:** Sensorwerte werden geglättet (Tiefpass um 8 Hz). Deine Bewegungen sind langsamer, Vibrationen schneller.
6. **Rampen:** Unterstützung wird über etwa 0,3 s ein- und ausgeblendet, nie schlagartig geschaltet.
7. **Grenzen:** maximales Moment, maximale Geschwindigkeit und Winkelbereich je Gelenk, in der Software **und** mit
   mechanischen Anschlägen.

**Beine (Seil zieht nur):** Zeigt die Manschettenkraft „strecken“, zieht der Motor. Zeigt sie „beugen“, gibt er Seil nach
und hält nur eine kleine Vorspannung (10–20 N), damit das Seil nicht von den Rollen springt. Hüfte und Knie hängen an
einem Seil: Die Steuerung nimmt den größeren der beiden Wünsche (Oberschenkel- oder Unterschenkel-Manschette).
Beim normalen Gehen (Knie fast gestreckt, Rumpf aufrecht) bleiben die Beine durchsichtig und unterstützen nicht, erst ab
einer Kniebeugung von etwa 25° oder beim Vorwärtsdrücken unter Last.

**Arme (beide Richtungen):** Das Seilpaar kann in beide Richtungen ziehen. Heben, Senken, Beugen und Strecken werden gleich
behandelt. Senken unter Last heißt dabei „kontrolliert nachgeben“: Das Exo hält gegen und lässt so schnell nach, wie du
nach unten drückst.

---

## 3. Weitere Sensoren

- **Motor-Encoder:** Gelenkwinkel, Geschwindigkeit, Motorstrom (eingebaut).
- **Lagesensoren (IMU)** am Rücken und an den Oberschenkeln: Wie geneigt ist der Rumpf? Gehst du, hockst du, stehst du?
  Sie entscheiden, ob die Beine unterstützen, und erkennen Stürze.
- **Schlaufenwinden:** Motorstrom = Zugkraft der Schlaufe, Endschalter für „ganz offen“.

---

## 4. Bedienung

| Element | Ort | Funktion |
|---|---|---|
| Ein/Aus (Kippschalter mit Schutzkappe) | linker Schultergurt | Aus: alles stromlos, Exo läuft frei mit |
| Not-Aus (Pilz) | linker Schultergurt, mit beiden Händen erreichbar | trennt Motoren hart vom Akku |
| Daumentaster: rechts 2, links 1 | Handschlaufe am Unterarm | je Schlaufe: kurz = zuziehen, lang = öffnen |
| Summer + Vibration | Schultergurt | Bereit, Grenze erreicht, Akku schwach, Fehler |

Die Verstärkung K wird nicht im Feld verstellt, sondern einmal am Laptop oder Handy eingestellt.

---

## 5. Zustände

```
AUS ──Ein──▶ NULLEN (2 s ruhig stehen, Sensoren auf null) ──▶ BEREIT (nur Schwerkraftausgleich)
                                                              │ ▲
                                    Manschettenkraft > Totband ▼ │ Kraft weg
                                                          UNTERSTÜTZEN
Jeder Fehler ──▶ FEHLER: Gelenkmotoren stromlos, Schlaufen halten, Summer
```

Fehler sind zum Beispiel: Sensor liefert keinen Wert oder klebt am Anschlag, Gelenkwinkel außerhalb des Bereichs,
Motor meldet Übertemperatur, CAN-Nachricht fehlt, Akkuspannung zu niedrig, Sturz erkannt.

---

## 6. Elektronik

```
Stihl AP 500 S (36 V) ── Hauptsicherung ── Not-Aus ── Schütz ──┬── 6 Gelenkmotoren (CAN-Bus)
                                                               ├── DC/DC 24 V ── 3 Motortreiber ── 3 Schlaufenwinden
                                                               └── DC/DC 5 V ── Steuerung
Steuerung: Teensy 4.1 (oder ESP32-S3) + CAN-Transceiver
           ├── 8 Wägezellen-Verstärker
           ├── 3 IMUs
           ├── 3 Daumentaster, Ein/Aus, Summer, Vibrationsmotor
           └── Watchdog: hängt die Software, fällt das Schütz ab
```

Die Motoren regeln ihren Strom selbst (eingebaut). Die Steuerung schickt nur das gewünschte Moment über CAN.

---

## 7. Was schiefgehen kann, und was dagegen hilft

| Problem | Ursache | Abhilfe |
|---|---|---|
| Exo zittert oder schaukelt auf | K zu hoch, Polster zu weich, Filter zu langsam | K senken, Dämpfung erhöhen, Manschetten straffer und dünner gepolstert |
| Exo „läuft weg“ | Sensor-Nullpunkt verschoben (Kälte, Gurt nachgezogen) | Nullen bei jedem Einschalten, Nullpunkt-Nachführung im Stillstand |
| Exo reagiert träge | Verstärker zu langsam, Bowden-Reibung | NAU7802 statt HX711, Reibung im Modell ausgleichen |
| Arm wird beim Bücken mitgezogen | Manschette zu eng, Gurt drückt schief | Manschetten mit etwas Spiel, Sensor nur quer zur Schiene |
| Bein streckt beim Gehen ungewollt | Gehen nicht erkannt | Beinunterstützung erst ab Kniebeugung, IMU-Gangerkennung |

---

## 8. So baut man es auf

1. **Am Tisch:** eine Wägezelle mit Verstärker an den Mikrocontroller, mit Gewichten kalibrieren, Rauschen anschauen.
2. **Sensor-Manschette** drucken, auf eine Holzlatte schrauben, mit der Hand drücken und die Werte beobachten.
3. **Prüfstand Ellbogen:** Holzarm mit Scharnier, Seilscheibe, Bowdenzug-Paar, ein Motor. Mit der Hand an der Manschette
   führen. Erst K = 0,3, dann steigern, Schwingungsgrenze suchen.
4. **Am Körper:** erst ein Arm, nur Ellbogen, mit Not-Aus in der anderen Hand. Dann Schulter, dann Beine.
5. **Unter Last:** zuerst Sandsack in den Schlaufen, dann leichte Bäume, dann schwerere.

Grundwissen dafür steht in [entwicklungsplan.md](entwicklungsplan.md).
