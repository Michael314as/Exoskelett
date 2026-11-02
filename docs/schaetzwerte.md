# Schätzwerte bis zur Messung am Prototyp

Was sich erst am gebauten Exo messen lässt, steht hier als bestmögliche Schätzung aus Datenblättern und Studien.
Jede Zahl wird ersetzt, sobald das Log vom Prüfstand (Stufe 1 und 2, [bestellung-prototyp.md](bestellung-prototyp.md))
vorliegt: Log mit `log 1` aufnehmen, ins Repository legen, Auswertung mit `tools/auswertung.py`.

## 1. Schlaufe: Schließ- und Haltekraft (Prüfpunkt P20)

| Größe | Schätzung | Grundlage |
|---|---|---|
| Nennmoment Bandantrieb 5840-31ZY, 24 V, 27 U/min (290:1) | 46 kg·cm ≈ 4,5 Nm bei 1,2 A | Händler-Datenblätter ([NFP](https://nfpshop.com/product/12v-24v-dc-permanent-magnet-dc-turbo-worm-reduction-motor-model-nfp-5840-31zy), [Precision Mini Drives](https://precisionminidrives.com/product/31mm-worm-reduction-motor-with-encoder-115mm-type-model-nfp-5840-31zy-en)) |
| Blockiermoment | 70 kg·cm ≈ 6,9 Nm bei 4,4 A | dieselben Tabellen; Angaben 100/150 kg·cm einzelner Händler sind unplausibel |
| Kraft am Stachelrad (r 16 mm) | 280 N Nenn, 430 N blockiert | Moment / Radius |
| mit 2,5-A-Grenze des 24-V-Wandlers | ≈ 200 N | Moment wächst etwa linear mit dem Strom über dem Leerlaufstrom (0,2 A) |
| **am Stamm, nach Bandreibung (≈ 75 %)** | **≈ 150 N Schließkraft** | Reibung Lochband im PTFE-Kanal, Erfahrungswert |
| **Haltekraft stromlos** | **> 400 N** | Schneckengetriebe 290:1 ist selbsthemmend; Rückdrehen erst über dem Blockiermoment |
| Bedarf | Baum 15–30 kg zieht 150–300 N **längs** am Kopf und Schäkel, das Band muss nur gegen Herausrutschen klemmen | Zahnleiste formschlüssig |

**Fazit:** Die frühere Schätzung von 100 N war zu niedrig angesetzt. Mit ≈ 150 N anlegen und > 400 N halten reicht für
Stämme bis 12 cm. Risiko: Die Werte stammen von Händlern, nicht vom Hersteller. P20 mit 20-kg-Sandsack klärt das.

## 2. Motorwärme (Prüfpunkt P33)

| Größe | Wert | Quelle |
|---|---|---|
| RobStride RS04 (Hüfte, Knie, Schulter): Nennmoment | 40 Nm mit Kühlplatte 345 × 345 mm, 35 Nm mit 220 × 200 mm; **in der Kühlkörper-Kapsel angesetzt 28 Nm** | [RobStride-Handbücher](https://github.com/RobStride/Product_Information) |
| RobStride RS03 (Ellbogen) | 20 Nm Nenn, 60 Nm Spitze; in der Kühlkörper-Kapsel 16 Nm | dito |
| Kühlkörper Fischer ICK S R 140 × 50 | 1,85 K/W ruhend, 0,4 K/W mit Luftstrom | [DigiKey](https://www.digikey.de/en/products/detail/fischer-elektronik/ICK-S-R-140-x-50/26281855) |
| Spitze RS04 | 120 Nm / 90 A Phasenstrom | RS04-Handbuch |
| Wicklung, Schutz | Isolationsklasse B, Grenze 145 °C, Thermistor; die Temperatur kommt in jeder CAN-Rückmeldung, die Firmware regelt ab 100 °C zurück | RS06-Handbuch |

Was das für die Arbeit heißt (Gelenkmoment = Motormoment, Übersetzung 1:1; Wärmemodell mit Zeitkonstante 3 min, Dauermoment in der Kapsel):

| Situation | Motor | Last | Einschätzung |
|---|---|---|---|
| Hocke halten, Exo + 20 % Oberkörper | Knie (RS04) | ≈ 38 Nm ≈ 1,36 × Kapsel-Dauermoment | ≈ 2,5 min voll, dann wird die Hilfe weicher |
| aufrecht stehen | Hüfte (RS04) | ≈ 2 Nm | kalt |
| Baum 15 kg aufrichten, 5 s | Schulter (RS04) | bis 100 Nm ≈ 3,6 × Dauermoment | ok; volle Kraft ≈ 13–15 s (Firmware-Test: 13 s), dann sinkt die Hilfe weich |
| Baum 100 m schleifen | Schulter | ≈ 20–30 Nm | dauerhaft |

Das Dauermoment in der Kühlkörper-Kapsel (28 Nm) ist eine Schätzung: Stiftkühlkörper Ø 140 mit 1,85 K/W (ruhend) statt der
Prüfplatte 345 × 345 mm des Herstellers, dafür kalte Winterluft. Am Prüfstand messen (P33), dann in der Firmware anpassen (`motorRatedNm` in `firmware/src/exo_app.h`).
Kälte hilft: jedes Kelvin weniger Umgebung ist Reserve.

## 3. Reaktionszeit

| Größe | Wert | Quelle |
|---|---|---|
| eigene Kette | ≈ 35 ms: HX711 80 Messungen/s (12,5 ms) + 8-Hz-Filter (≈ 20 ms) + CAN und Regeltakt (≈ 2 ms) | Firmware |
| Zielwert in der Literatur | ≤ 10 ms Regelkreis-Verzögerung für stabile, sichere Admittanzregelung | [Patent US 11 337 881](https://mtec-sc.org/patents/US-11337881-B2) |
| Wahrnehmung | 25 ms Verzögerung → Masse fühlt sich ≈ 10 % schwerer an, 50 ms → ≈ 20 % | [Haptik-Studie (PMC4567313)](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC4567313/) |
| Gleichgewicht | Reflexe beginnen ≈ 100 ms nach einer Störung; Exo-Moment in dieser Zeit verbesserte das Gleichgewicht nicht | [Beck et al., Sci. Robotics](http://sites.edb.utexas.edu/wp-content/uploads/sites/181/2024/02/Beck-et-al_SciRobotics.pdf) |

**Fazit:** Mit 35 ms funktioniert es, fühlt sich aber etwas träge an: Der Exo-Arm wirkt etwa 15 % schwerer, als er ist.
Für das Gefühl „das Exo ist ein Teil von mir“ lohnt der ADS1220 (≈ 10 ms). Er bleibt als Upgrade, damit der Prüfstand
erst zeigt, ob es nötig ist (Kosten ≈ 5 × 8 €, Firmware-Treiber nötig).

## 4. Gewicht und Tragegefühl

| Größe | Wert | Quelle |
|---|---|---|
| Exo gesamt | 43,3 kg, davon 29,1 kg Oberteil (mit Rumpfkapseln), ≈ 7,2 kg je Bein (mit Hüft- und Kniekapsel) | Bauteilliste der technischen Ansicht |
| Gewicht auf dem Körper | im Stand fast null: Exo-Beine und Motoren tragen es in den Boden | Firmware-Vorsteuerung |
| Mehraufwand beim Gehen, falls nicht getragen | steigt etwa linear mit der Last (Rucksack) | [Legg & Ramsey](https://www.tandfonline.com/doi/abs/10.1080/00140139208967382), [Army LCDA-Modell](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC8919998/) |
| Masse am Bein | kostet pro kg deutlich mehr als am Rücken; ein Knöchel-Exo mit 2,1 kg Beinmasse brauchte gute Unterstützung, um überhaupt zu sparen | [Mooney et al.](https://link.springer.com/article/10.1186/1743-0003-11-80) |
| frühe passive Bein-Exos | verbrauchten mehr Energie als ein normaler Rucksack | [DTIC-Studie](https://apps.dtic.mil/sti/tr/pdf/ADA481701.pdf) |

**Fazit:** Spürbar sind vor allem die 4,8 kg je Bein beim Gehen und die Trägheit beim Drehen. Für kurze Wege
(bis 100 m) und Heben, Aufrichten und Tragen von Bäumen überwiegt der Nutzen. Für lange Strecken ohne Last ist ein
Exo dieser Klasse ein Nachteil: Parkstütze nutzen, Exo abstellen.

## 5. Passform

Keine Literaturzahl ersetzt das Anprobieren. Ausgelegt ist auf 1,75 m: Teleskopschienen ±5 cm, Lochleiste der
Schultergurte ±5 cm. Bei Stufe 3 (Rahmen ohne Motoren) zuerst P1–P3 prüfen.
