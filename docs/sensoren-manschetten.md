# Sensoren und Manschetten (Stand 14. Nov 2026)

Anschaulich in der technischen Zeichnung [exo-technik.html](../visualisierung/exo-technik.html), Reiter „Sensoren und
Manschetten“; jedes Teil dort anklickbar (Baugruppe SE).

## Was das Exo misst

Gelenkwinkel, Geschwindigkeit, Moment und Temperatur liefern die RobStride-Motoren selbst (zwei 14-Bit-Absolutencoder,
Phasenstrom × Momentkonstante, Temperatur von Wicklung und Treiber, alles über CAN). Zusätzliche Winkelsensoren braucht es
nicht. Was fehlt, ist vor allem die **Kraft zwischen Mensch und Exo**: Daraus erkennt die Steuerung, was du willst.

| Was | Sensor | Ort | Anschluss | Rate | Wozu |
|---|---|---|---|---|---|
| Absicht Arme und Beine | Wägezelle YZC-133, 20 kg (4 je Seite, SE-02) | Manschetten Oberarm, Unterarm, Oberschenkel, Schienbein | M8-Stecker (binder 718) → LiYCY 4 × 0,14 geschirmt im Kabelkanal → ADS131M04 in der Box | 1 000/s | Mitgehen (≈ 3 ms Verzug) |
| Absicht Rumpf | 4 × Wägezelle 20 kg (SE-09) | Gurtanker, Schulterblattpolster | ADS131M04 | 1 000/s | Vorbeugen, Aufrichten, Drehen |
| Gelenkwinkel, -moment, -temperatur | im Motor | jede Kapsel | CAN 1 Mbit/s | 500/s | Haltung, Last, Wärmemodell |
| Rumpflage, Sturz | BNO085 (EL-07) | Box | I²C | 400/s | Neigung, Sturzerkennung |
| Fußlast | 4 × Halbbrücke 50 kg je Steigbügel, HX711 (SE-06) | Steigbügel | KB-02 | 80/s | Standbein / Schwungbein |
| Baumgewicht | YZC-1B 50 kg (SE-07) | Schlaufenkopf | KB-03 | 80/s | Vorsteuerung |
| Akkustrom und -spannung | TI INA228 + Shunt 0,2 mΩ (SE-13, **Vorschlag zu offener Entscheidung 4**) | Box, Anfang der Kupferschiene | I²C | 1 000/s | Leistungsgrenze 2,5 kW, Restlaufzeit |
| Außentemperatur | TI TMP117 (SE-14) | unter der Box | I²C | 1/s | Kapselheizung, Frostwarnung |
| Kapseltemperatur | NTC in der Heizfolie (AN-03) | jede Kapsel | im Kabelbaum | 1/s | Heizung, kein Kondenswasser |

Messleitungen laufen getrennt von den Leistungsleitungen (≥ 2 cm), Schirm einseitig an der Box. Jede Manschettenzelle ist
über einen M8-Stecker steckbar (Tausch ohne Löten).

## Manschetten Rev. M3

Von innen nach außen: Bezug aus Abstandsgewirke (Klett, waschbar) → Polster **Plastazote LD45 4 mm** (geschlossenzelliger
PE-Schaum: bleibt bei Frost weich und nimmt kein Wasser auf; das bisherige Viskopolster wird in der Kälte hart) → Schale
(PETG-CF 4 mm für den Prototyp, später PP-Copolymer 3 mm tiefgezogen, kältezäh und warm nachformbar) → zwei BOA-Bänder →
Längsschlitten ±12 mm (gleicht den Achsversatz Mensch/Exo aus) → Wägezelle mit Überlastpuffer → **Klemmhalter mit
Flügelschraube, auf der Schiene ±40 mm verschiebbar** → M8-Stecker → Kabelkanal.

| Glied | Breite | Größen (Umfang, Richtwerte) | Sitz |
|---|---|---|---|
| Oberarm | 87 mm | S 26–30 · M 30–34 · L 34–38 cm | unter dem Deltamuskel |
| Unterarm | 73 mm | S 22–25 · M 25–28 · L 28–31 cm | mit Drehring ±60° |
| Oberschenkel | 102 mm | S 44–50 · M 50–56 · L 56–63 cm | mittleres Drittel |
| Schienbein | 87 mm | S 32–36 · M 36–40 · L 40–44 cm | unter dem Knie |

Größe verstellen: Gliedlängen über die Teleskopschienen, Umfang über 3 Schalengrößen plus BOA, Lage über den Klemmhalter.

## Behoben: Zittern beim Start der Zeichnung

Die Simulation neigt den Rumpf vor, bis das Lot ≈ 3 cm vor der Knöchelmitte liegt (Ausgleich des Rucksacks). Der Regler
dafür war zu scharf eingestellt und startete bei 0°: Beim Öffnen kippte der Rumpf um ≈ 15° vor und schaukelte nach. Jetzt
pendelt sich die Haltung vor dem ersten Bild ein, und der Regler führt mit kleiner Verstärkung und 4 mm Totband nach.
