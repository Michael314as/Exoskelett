# Realismus-Check und Kraftgewinn (Entwurf 5, Stand 14. Nov 2026)

Was das Exo wirklich kann, gerechnet mit Herstellerdaten echter Referenzbauteile. Anschaulich als 3D-Gesamtbild:
[visualisierung/exo-gesamtbild.html](../visualisierung/exo-gesamtbild.html). Baut auf der Durchsicht
[durchsicht-2026-11.md](durchsicht-2026-11.md) auf.

## 1. Referenzbauteile

| Teil | Referenz | Werte aus dem Datenblatt | Quelle |
|---|---|---|---|
| Hüfte, Knie, Schulter, Rumpf beugen | RobStride RS04 | 48 V (24–60 V), Leerlauf 200 U/min, 120 Nm bei ≈ 95 U/min, Nenn 40 Nm drehend (Platte 345 × 345 mm), 28,5 Nm im Stillstand, Kt 2,1 Nm/Arms, 27 / 90 A Phasenstrom, 1,42 kg, Isolationsklasse B, Wicklungsgrenze 145 °C | [RS04-Handbuch](https://github.com/RobStride/Product_Information), S. 9–11 |
| Ellbogen, Rumpf drehen | RobStride RS03 | 48 V, 60 Nm Spitze, Nenn 20 Nm drehend (Platte 215 × 220 mm), 13 Nm im Stillstand, Kt 2,36 Nm/Arms, 0,88 kg, 106 × 106 × 56 mm, IP52 (Händler) | RS03-Handbuch, [Händler](https://openelab.io/products/robstride03-qdd-60n-m-integrated-joint-motor-module) |
| Akku | Stihl AP 500 S | 36 V, 8,8 Ah, 337 Wh, 2,0 kg | [Stihl](https://stihl.co.uk/en/ap/ap-500-s-battery-124181) |
| Vergleichslast am selben Akku | Stihl MSA 300 C-O | 2,5 kW (Datenblatt), 3,0 kW (Text) bei 36 V ≈ 70–85 A | [Stihl](https://www.stihl.co.za/en/ap/msa-300-c-o-without-battery-and-charger-144325) |
| Kühlkörper | Fischer ICK S R 140 × 50 | 1,85 K/W ruhend, 0,4 K/W mit Luftzug | [DigiKey](https://www.digikey.de/en/products/detail/fischer-elektronik/ICK-S-R-140-x-50/26281855) |
| Vergleichsgerät | Sarcos Guardian XO | ≈ 68 kg, 24 Gelenke, 90 kg Nutzlast, ≈ 500 W, ≈ 2 h mit 2 × 500 Wh, ≈ 450 Nm Spitze | [IEEE Spectrum](https://spectrum.ieee.org/amp/sarcos-guardian-xo-powered-exoskeleton-2650279406) |
| Muskelkraft | Harbo et al. 2012, 178 Gesunde | Männer isometrisch: Knie strecken 247 ± 56 Nm, Hüfte beugen 156 ± 33 Nm, Ellbogen beugen 51 ± 14 Nm, Schulter abspreizen 61 ± 14 Nm (Alter 15–83, Mittel 53) | [Eur J Appl Physiol](https://link.springer.com/doi/10.1007/s00421-011-1975-3) |

## 2. Die wichtigste neue Zahl: Halten im Stillstand

Beide Handbücher haben zwei Überlasttabellen, eine drehend und eine **im Stillstand**. Steht der Motor, erwärmt sich
eine Phase allein (laut Hersteller 1,414-mal mehr), und der Motor-eigene Schutz greift viel früher.

| Moment RS04 | drehend | Stillstand | bisheriges Wärmemodell der Firmware (28 Nm, 3 min) |
|---|---|---|---|
| 100 Nm | 15 s | **1,5 s** | 15 s |
| 80 Nm | 38 s | **6 s** | 24 s |
| 60 Nm | 108 s | **33 s** | 44 s |
| 40 Nm | dauerhaft | **130 s** | 121 s |
| Dauer | 40 Nm | **28,5 Nm** (große Platte) | 28 Nm |

| Moment RS03 | drehend | Stillstand |
|---|---|---|
| 60 Nm | 13 s | 1 s |
| 40 Nm | 67 s | 9 s |
| 30 Nm | 393 s | 25 s |
| Dauer | 20 Nm | 13 Nm |

Ein Baum wird meist gehalten, nicht bewegt. Für die Auslegung zählt deshalb die Stillstand-Spalte. In der Kühlkörper-Kapsel
(kleiner als die Prüfplatte, dafür kalte Luft) setze ich **≈ 20 Nm (RS04) und ≈ 11 Nm (RS03)** Dauer an.

## 3. Ampel

| Punkt | Status | Begründung | Vorschlag |
|---|---|---|---|
| Spitzenmoment beim Halten | **rot** | Plan: Schulter 100 Nm ≈ 13–15 s. Hersteller: 1,5 s. Der Motor-Schutz schaltet ab, bevor die Firmware weich zurücknimmt. | Wärmemodell mit zwei Zeitkonstanten (Wicklung Sekunden, Gehäuse Minuten) nach der Stillstand-Tabelle; Halten mit 60–70 Nm planen |
| Dauermoment in der Kapsel | gelb | `motorRatedNm` 28 / 16 Nm liegt über dem Herstellerwert im Stillstand | vorläufig 20 / 11 Nm, P33 misst |
| Spitzenstrom | gelb | Aufrichten mit 4 Bein- und 2 Schultermotoren ≈ 2,5–3,5 kW für 1–2 s (≈ 60–95 A); die Kettensäge MSA 300 zieht 2,5–3 kW aus demselben Akku | Stromsensor (offene Entscheidung 4), Leistungsgrenze ≈ 2,5 kW |
| Gewicht | gelb | 44 kg + 2 kg zweiter Akku; 17 kg davon Gelenkkapseln. In der tiefen Hocke braucht jedes Knie ≈ 30 Nm nur fürs Exo | Rumpfmotoren zurückstellen (offene Entscheidung 3) |
| Spannung 36 V statt 48 V | grün | ≈ 150 U/min am Gelenk ≈ 900 °/s; Gehen braucht 300–400 °/s am Knie | – |
| Laufzeit | grün | ≈ 260 Wh nutzbar bei 0 °C, Mischbetrieb ≈ 120 W → ≈ 2 h je Akku | zweiten Akku mitnehmen |
| Stromlos | grün (neu) | Handbuch 4.2.5: Der Motor dämpft schnelles Rückdrehen nach dem Ausschalten (`damper`) | Prüfstand: wie schnell gibt ein stromloses Knie unter 30 kg nach? (offene Entscheidung 1) |
| Schubstangen, Rahmen | grün | Knicklast 16 kN gegen 3,7 kN | – |
| Dichtheit | grün | Händler nennen IP52; reicht nicht für Schnee und Schlamm | Kapsel bleibt, P35/P36 |

## 4. Wie viel stärker?

Vergleich je Gelenk: du (Maximalkraft × Anteil, den man so lange halten kann, Ermüdungskurve nach Rohmert:
2 s 95 %, 10 s 80 %, 30 s 60 %, 2 min 40 %, dauerhaft 15 %) gegen den Motor (Stillstand-Tabelle, Firmware-Grenze),
abzüglich des Moments, das der Motor fürs Exo selbst braucht. Muskelwerte für einen trainierten Mann um 75 kg
(Schulter und Ellbogen 65 Nm, Hüfte, Knie, Rumpf 250 Nm; Richtwerte, Streuung ±25 %).

| Gelenk | Exo braucht für sich | 10 s | 2 min | dauerhaft |
|---|---|---|---|---|
| Schulter | 7 Nm (Exo-Arm) | × 2,2 | × 2,3 | × 2,3 |
| Ellbogen | 1,5 Nm | × 1,7 | × 1,8 | × 2,0 |
| Rumpf aufrichten | 0 (Gasfedern) | × 1,4 | × 1,4 | × 1,5 |
| Hüfte, 30° vorgebeugt | 25 Nm | × 1,2 | × 1,2 | × 1,0 |
| Knie, halbe Hocke | 17 Nm | × 1,3 | × 1,2 | × 1,1 |

Alltagsnah:
- **Hantel mit gestrecktem Arm vor dir** (Hebel 0,55 m): 10 s lang ohne Exo ≈ 10 kg je Arm, mit Exo ≈ 21 kg.
- **Baum 3 m (41 kg) wie eine Fackel**: Die rechte Schulter braucht ≈ 108 Nm. Ohne Exo einarmig unmöglich, mit Exo
  etwa 10 Sekunden lang, wenn du kräftig mithältst; für längeres Tragen den Stamm näher an den Körper nehmen.
- **Beine**: Der Gewinn liegt im Stand und bei fast gestreckten Knien, wo die Schienen Exo und Baum ohne Motorkraft in den
  Boden leiten. In der tiefen Hocke trägt der Kniemotor vor allem das Exo selbst.

Kurz: **An den Armen etwa doppelt so stark, kurz wie dauerhaft. An Beinen, Hüfte und Rumpf etwa ein Drittel stärker für
Sekunden, auf Dauer trägt das Exo dort vor allem sich selbst.**

## 5. Energie

| Tätigkeit | mittlere Leistung (Schätzung) | ein Akku (≈ 260 Wh) |
|---|---|---|
| bereit stehen, Frost (Elektronik, Treiber, Heizung) | ≈ 45 W | ≈ 5,7 h |
| Mischbetrieb Christbaum | ≈ 120 W | ≈ 2,1 h |
| Gehen ohne Last | ≈ 150 W | ≈ 1,7 h |
| Schleifen 100 m am Stück | ≈ 170 W | ≈ 1,5 h |
| Baum halten, 4 Gelenke à 40 Nm | ≈ 420 W | ≈ 37 min |

Wärme beim Halten wächst mit dem Quadrat des Moments. Der Wicklungswiderstand steht nicht im Handbuch; aus T-N-Kurve und
üblichem Wirkungsgrad geschätzt ≈ 0,5–0,8 kW Wärme je RS04 bei 100 Nm, ≈ 100 W bei 40 Nm, ≈ 25–40 W bei 20 Nm.
Der Stromsensor (Entscheidung 4) würde diese Zahlen am Prüfstand messbar machen.
