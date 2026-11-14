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

## 6. Technische Zeichnung zum Durchfliegen (Stand 14. Nov 2026)

[visualisierung/exo-technik.html](../visualisierung/exo-technik.html), Knopf „Fliegen“ (Taste F): Ego-Perspektive wie im
Kreativmodus, W A S D, Leertaste hoch, Umschalt runter, Strg schneller, Mausrad Tempo, Klick wählt das Teil im Fadenkreuz.

Neu gezeichnet und anklickbar (je Gelenk und Seite eine eigene ID):

- **Motoren aus den STEP-Dateien des Herstellers** (`visualisierung/cad/robstride.js`): RS04 Ø 120 × 56 mm, RS03 Ø 98 × 57 mm.
  Der Abtrieb ist ein Flansch am Planetenträger, es gibt keine vorstehende Welle. Darauf: Abtriebsadapter (AS-01) mit
  9 × M5 (AS-02) und 2 Passstiften Ø 5 (AS-04), darauf Kurbel oder Gelenkflansch. Hinten 10 × M4 durch Motorträger und
  Kühlkörper (AS-03).
- **Anschlüsse laut Handbuch Kap. 2.3–2.5:** auf der Treiberplatine hinten, Ausgang oben am Rand: XT30APW-M mit Pin 1 Plus
  (VBAT+, rot) und Pin 2 Minus (GND, schwarz), GH1.25-2PWT mit CAN_L (braun) und CAN_H (blau). Werkskabel 310 mm in einen
  Anschlussdom auf dem Kapselmantel, dort Trennstelle XT30U / GH1.25 und Kabelverschraubung M16 (AS-05 … AS-10). Jeder Motor
  ist so gedreht, dass der Dom zum ankommenden Kabelbaum zeigt.
- **Kabel:** Leistung und CAN zu Schulter und Ellbogen fehlten in der alten Zeichnung (neu KB-05), ebenso zu den Rumpfkapseln
  (KB-06). Kabelkanäle mit Rastdeckel auf Ober-/Unterschenkel und Ober-/Unterarm (KB-07).
- **Verkleidung gegen Äste (VK):** Gitter über den Kühlstiften, PE-HD-Schalen über Kurbel und Schubstange an Knie und
  Ellbogen, Gelenkkappen über den Hebeln, Rückenschale, Kanaldeckel. Zusammen ≈ 1,05 kg.

Beim Zeichnen gefunden:

| Befund | Folge | Geändert |
|---|---|---|
| Hüft- und Kniekapsel saßen 120 mm auseinander, die Kühlkörper haben Ø 140 mm: Sie überlappten um ≈ 25 mm. Ebenso Schulter- und Ellbogenkapsel. | wäre beim Bau aufgefallen | Kniemotor 160 mm, Ellbogenmotor 145 mm unter der Gelenkachse (Parallelogramm bleibt 1:1) |
| Motorlänge war mit 52 mm (RS04) bzw. 54 mm (RS03) angesetzt, die STEP-Dateien haben 56 bzw. 57 mm | Kurbel saß im Abtrieb | Kapseln 4 mm breiter |
| Anschlussdom am Kniemotor hinten wäre mit der hinteren Wange des Motorträgers kollidiert | | Kabelausgang hinten unten (225°) an Hüfte, Knie, Ellbogen |

Gewicht laut Stückliste jetzt **46,9 kg** (vorher ≈ 44 kg): + 1,05 kg Verkleidung, + 1,6 kg Schrauben, Adapter, Stecker,
Kanäle und die fehlenden Kabel. Ohne Funktionsverlust ≈ 2,7 kg sparbar. Sparvorschläge (Reiter „Gewicht“, Schätzungen):

| Vorschlag | spart | Bewertung |
|---|---|---|
| Rumpfmotoren weglassen (offene Entscheidung 3) | ≈ 3,8 kg | ändert die Funktion, Michael entscheidet |
| Feste Schienenlängen statt Teleskop | (≈ 0,7 kg) | verworfen: Größe soll verstellbar bleiben |
| Rahmen und Schienen aus CFK-Rohr | ≈ 1,9 kg | teurer, Klebung bei Kälte prüfen |
| Taschen in den Laserteilen | ≈ 0,5 kg | ohne Funktionsverlust |
| Schubstangen Alu 7075 Ø 14 × 2 statt Stahl Ø 12 × 2 | ≈ 0,25 kg | Knicklast 9 kN bei 3,7 kN Last |
| Kühlstifte kürzen | – | nicht empfohlen, Halten ist schon knapp (Abschnitt 2) |

## 7. Regen, Schmutz, Bedienung, Wartung (Stand 14. Nov 2026)

Reiter „Schutz und Wartung“ in der technischen Zeichnung. Kurz:

- **Dicht (IP65):** Gelenkkapseln (O-Ringe, V-Ring, Druckausgleich, Dom mit O-Ring-Deckel, Kabelverschraubung IP68) und
  Elektronikbox (EPDM-Dichtung, Kabel nur von unten mit Tropfschlaufe; vorher IP54).
- **Kühlung bleibt offen:** Der Kühlkörper sitzt bewusst außerhalb der Dichtung und darf nass werden. Gegen Äste nur ein Gitter
  (≈ 20 mm Maschen), keine geschlossene Haube, weil das Halten thermisch ohnehin knapp ist (Abschnitt 2).
- **Neu gezeichnet:** Regenklappe über dem Akkuschacht (VK-07, Akku ist nur IPX4), Faltenbälge über den Gelenkköpfen der
  Schubstangen (VK-08), Vierteldrehverschlüsse an den Abdeckungen (VK-09, ohne Werkzeug, mit Handschuhen).
- **Sonst:** Ablaufbohrungen Ø 4 und Endkappen in den Schienen (kein Eis im Rohr), Kanaldeckel mit Ablaufschlitz,
  gedichtete Lager (2RS) und schmierfreie Gleitlager.
- **Wartung:** nach jedem Einsatz abspülen (kein Hochdruck), jede Saison Schrauben nachziehen und Spannband tauschen;
  Motor tauschen ≈ 20 min mit Innensechskant 3 und 4 mm. Bordwerkzeug: Innensechskant 3, 4, 5 mm, Gabelschlüssel 10 und 13 mm.
