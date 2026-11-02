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
| AK70-10: Nennmoment / Nennstrom | 8,3 Nm / 7,2 A (CubeMars-Shop nennt 10 Nm) | [RobotShop](https://www.robotshop.com/products/cubemars-ak70-10-kv100-ak-series-robotic-actuator), [CubeMars](https://www.cubemars.com/product/ak70-10-kv100-robotic-actuator.html) |
| Spitze | 24,8 Nm / 23,2 A | [CubeMars](https://www.cubemars.com/product/ak70-10-kv100-robotic-actuator.html) |
| Wicklungswiderstand, Isolation | 0,272 Ω (Phase-Phase), Klasse H (180 °C), Umgebung −20 … 50 °C, Temperatursensor eingebaut | [AIFit Lab](https://aifitlab.com/products/cubemars-ak70-10-motor), [MAB Robotics](https://www.mabrobotics.pl/product-page/ak70-10-cubemars-t-motor) |
| Kupferverlust | ≈ 14 W bei Nennstrom, ≈ 145 W bei Spitze | I² · R |

Was das für die Arbeit heißt (Motormoment / Nennmoment, Firmware-Wärmemodell mit Zeitkonstante 3 min):

| Situation | Motor | Last | Einschätzung |
|---|---|---|---|
| Hocke halten, Exo + 20 % Oberkörper | Knie | ≈ 6,8 Nm ≈ 82 % Nenn | dauerhaft möglich |
| aufrecht stehen | Hüfte | ≈ 1,4 Nm (8 Nm / 6) | kalt |
| Baum 15 kg aufrichten, 5 s | Schulter | bis 24 Nm ≈ 3 × Nenn | kurz ok; das Modell erlaubt etwa 20 s, dann sinkt die Hilfe |
| Baum 100 m schleifen | Schulter | ≈ 4–6 Nm | dauerhaft |

Die Kälte im Einsatz (Frost) hilft: Ein Kelvin weniger Umgebung ist ein Kelvin mehr Reserve. Die tatsächliche
Zeitkonstante fehlt im Datenblatt; P33 misst sie, dann wird die Zeitkonstante (180 s, `firmware/src/exo_app.h`) angepasst.

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
| Exo gesamt | 31,1 kg, davon 21,5 kg Oberteil, ≈ 4,8 kg je Bein | Bauteilliste der technischen Ansicht |
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
