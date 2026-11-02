# Konzept: Christbaum-Exo (Entwurf 5: Gelenkkapseln)

Ziel: ein umsetzbares, robustes Exoskelett für zwei Arbeiten: Bäume am Stamm greifen und **schleifen** (Use Case 2)
und Bäume am Stumpf greifen, fast senkrecht **aufrichten und aufladen** (Use Case 1). Es unterstützt Beine,
Oberkörper und Arme. Budget: einige tausend Euro, verteilt über die Zeit. Serienteile, die man einzeln kaufen und
tauschen kann; gebaut ohne Drehbank und ohne Schweißen.

**Rahmenbedingungen (geklärt):** Schleifen bis etwa 100 m. Arbeiten bei Frostregen, nassem Schnee, **Schlamm und unebenem
Gelände**. Akku von vorhandenen Stihl-Geräten. Eine Person (1,75 m, 75 kg), Längen einstellbar. Die meisten Bäume
schaffst du selbst; das Exo soll **entlasten**, nicht ersetzen.

**Neu in Entwurf 5: Gelenkkapseln.** Die Motoren sitzen direkt an den Gelenken, jeder in einer dichten, beheizten Kapsel.
Seile, Bowdenzüge, Umlenkrollen, Seilscheiben und die Antriebsbox am Rücken entfallen. Grund: Das waren die Teile, die
vereisen, sich dehnen und reißen konnten (Zugkräfte bis 2,4 kN, Bowdenzug-Wirkungsgrad 0,7). Details in Abschnitt 5.

| Punkt | Entwurf 4 | Entwurf 5 |
|---|---|---|
| Hüfte | RS06 auf der Hüftschwinge, Seilschleife 1:6 | **RobStride RS04 direkt auf der Hüftachse** (120 Nm Spitze) |
| Knie | RS06 in der Box, Bowdenzug-Paar, Kurvenscheibe | **RS04 oben am Oberschenkel, Schubstange 1:1** zum Kniehebel, Federpaket begrenzt das Beugen |
| Schulter | RS06 in der Box, offenes Seilpaar über 4 Rollen | **RS04 direkt auf der Schulterachse** |
| Ellbogen | RS06 in der Box, Bowdenzug-Paar | **RS03 oben am Oberarm, Schubstange 1:1** |
| Wetter | Seile und Rollen können vereisen | Kapsel mit Stiftkühlkörper, O-Ringen, V-Ring, Druckausgleich, Heizband 4 W für Pausen |
| Rücken | Box mit 6 Motoren, Akkus seitlich | Akkus stehend oben im Schutzbügel, flache Elektronikbox darunter, Rucksack ≈ 12 cm tief; Rumpfdrehung ±15° |
| Gewicht | 31,9 kg | **43,3 kg** (Motoren schwerer, Kapseln mit Kühlkörpern, Rumpf beugen und drehen mit Motor) |
| Material | 3.800–4.400 € | ≈ 5.700–6.800 € |

**Entwurf 4 im Vergleich zu Entwurf 3** (zur Nachverfolgung):

| Punkt | Entwurf 3.2 | Entwurf 4 |
|---|---|---|
| Hüfte seitlich | Scharnier am Hüftbügel, Teile überschnitten sich, Mechanik nicht erkennbar | **Abspreizgelenk hinter dir**: Achse nach vorne, genau in Verlängerung deines Hüftgelenks, mit Hüftschwinge und Zentrierfedern |
| Beinantrieb | ein Seil pro Bein für Hüfte und Knie, nur strecken | **Mischweg:** Hüftmotor direkt auf der Hüftschwinge (Seilgetriebe 1:6), Knie per Bowdenzug-Paar von hinten. Beide Gelenke getrennt, **beide Richtungen** |
| Beinsteuerung | Manschetten an Ober- und Unterschenkel | Oberschenkel-Manschette + **Fußkontakt** (Stand/Schwung) + automatisches Tragen im Stand |
| Beinschienen | senkrecht, Lücke am Fußgelenk | laufen schräg nach innen und durchgehend bis in Knie- und Knöchellager |
| Fuß | Sohlenrahmen, Sprunggelenk gummigedämpft | schmaler Steigbügel unter der Sohlenmitte, **Kippgelenk hinter der Ferse** (Fuß kippt beim Spreizen mit) |
| Oberkörper | Hüftgurt und zwei Schultergurte angedeutet | **Gurtsystem** nach Rucksack-Prinzip: Hüftgurt, S-Schultergurte, Lageverstellriemen, Brustgurt, Polster |
| Schlaufen rechts | zwei hintereinander unter dem Handgelenk | **unten + schräg außen**, auf Höhe des Handgelenks, um 4 cm versetzt; zweite hält einen zweiten Stamm oder greift mit |
| Anzeige | E-Paper am rechten Schultergurt mit Baumzähler | **rechter Oberarm außen**: Uhrzeit, Akku mit Restzeit, Stufe, Fehler |
| Steuerung | jede Manschette nur für ihr Gelenk | Druck über die Hebel auf **alle** Gelenke, **Baumlast-Vorsteuerung** aus Wägezellen in den Schlaufenköpfen |

Dazu: [steuerung.md](steuerung.md), [kraftverteilung.md](kraftverteilung.md), [stueckliste.md](stueckliste.md),
[montage.md](montage.md), 3D-Modell: [visualisierung/exo-konstruktion.html](../visualisierung/exo-konstruktion.html).

## 1. Grundidee in einem Satz

**Ein Rückenrahmen mit Motoren trägt über eine Hüftschwinge zwei angetriebene Beine und zwei angetriebene Arme; Schlaufen
am Handgelenk halten die Stämme; Manschetten, Fußsensoren und Schlaufen-Wägezellen sagen dem Exo, was du willst und was
es tragen muss.**

| Entscheidung | Begründung |
|---|---|
| Alle Achsen durch deine Gelenke | Sonst verschieben sich Exo und Körper gegeneinander und die Manschetten drücken. Wo die Achse im Körper liegt (Abspreizen der Hüfte, seitliches Kippen am Fuß), sitzt das Gelenk **auf derselben Linie dahinter**. |
| Hüftmotor an der Hüfte, übrige Motoren hinten | Die Hüfte liegt nah am Schwerpunkt; ein Motor auf der Hüftschwinge spreizt mit und schwingt beim Gehen nicht mit. Knie, Schulter, Ellbogen bleiben leicht, ihre Motoren sitzen in der Box. |
| Jedes angetriebene Gelenk in beide Richtungen | Arme heben und senken; Beine strecken (stark) und beugen (schwach, mechanisch begrenzt). So läuft das Exo beim Gehen frei mit und hilft beim Anheben des Beins. |
| Steuern über Druck, Tragen über Messung | Die Manschetten zeigen deine Absicht, die Wägezellen in den Schlaufen das Gewicht. Das Exo trägt seinen Anteil der Last von selbst, du führst die Bewegung. |
| Starrer Rahmen bis in den Steigbügel | Baumgewicht geht über Metall in den Boden, nicht über deine Knochen. |

## 2. Grundstruktur

```
 Schulterbügel ── Schulterkapsel (RS04 direkt) ── Oberarm (Ellbogenkapsel RS03 + Schubstange, Anzeige rechts) ── Ellbogen ── Unterarm ── Schlaufenträger
       │                                                                                                         (rechts unten + außen, links unten)
 Rückenrahmen ── flache Elektronikbox (Steuerung, Wandler), Akkus seitlich
       │            └── Gurtsystem: Hüftgurt, Schultergurte, Lageverstellriemen, Brustgurt, Polster
 Gabel des Abspreizgelenks (Achse nach vorne durchs Hüftgelenk, Zentrierfedern ±20°)
       │
 Hüftschwinge mit Hüftkapsel (RS04 direkt) ── Hüftlager ── Oberschenkelschiene (Kniekapsel RS04 oben, Manschette) ── Schubstange ── Kniegelenk (Kniehebel)
                                                                                                                 │
                                                       Unterschenkelschiene (Schale) ── Knöchellager ── Fersenbügel ── Kippgelenk hinter der Ferse ── Steigbügel
```

- **Bauweise:** komplett geschraubt, Lager in gekauften Flanschlagergehäusen, Achsen als Passschrauben, Bleche gelasert,
  Schalen und Gehäuse gedruckt.
- **Einstellbar:** Ober-/Unterschenkel, Ober-/Unterarm über Lochraster mit Steckbolzen; Rückenlänge der Schultergurte
  über eine Lochleiste.

## 3. Hüfte: Abspreizgelenk, Hüftschwinge, Hüftmotor

- **Problem in Entwurf 3.2:** Das Scharnier saß am Hüftbügel neben der Hüfte. Beim Spreizen drehte sich das Bein um einen
  falschen Punkt, Teile überschnitten sich, und das Beinseil änderte seine Länge.
- **Lösung:** Die Achse fürs Abspreizen zeigt nach vorne und muss durch dein Hüftgelenk gehen. Dort ist Körper, also sitzt
  das Gelenk **auf derselben Linie hinter dir**: eine Gabel unten am Rückenrahmen (Alu 8 mm), darin eine Nabe mit zwei
  Gleitlagern. Vorne hält ein Kugelsperrbolzen die Achse; Ziehen am roten Ring nimmt das Bein ab.
- **Hüftschwinge:** Alu-Vierkant 25 × 25 von der Nabe nach außen und seitlich nach vorne zum Hüftlager (Flanschlager).
  Sie liegt unter dem Hüftgurt und berührt ihn auch bei 20° Spreizen nicht.
- **Zentrierfedern:** Ein Hebel an der Nabe sitzt zwischen zwei kleinen Druckfedern: Das Bein steht ohne Kraft in der
  Mitte und pendelt beim Gehen nicht; seitliche Schritte bleiben leicht. Gummipuffer begrenzen auf ±20°.
- **Hüftkapsel:** RobStride RS04 direkt auf der Hüftachse, außen neben dem Hüftlager. Das Gehäuse hält ein Motorträger an
  der Hüftschwinge, der Abtrieb ist mit der Oberschenkelplatte verschraubt: 110 Nm strecken, ohne Seil. Der Motor spreizt
  mit und schwingt beim Gehen nicht mit. Dichte, beheizte Haube (Abschnitt 5).

## 4. Beine und Füße

- **Schienen:** Außenrohr 25 × 25, innen 20 × 20 teleskopierbar. Die Oberschenkelschiene läuft von der Hüfte (23 cm
  neben der Körpermitte) schräg nach innen zum Knie (16 cm), die Unterschenkelschiene weiter zum Knöchel (15 cm): Knie-
  und Knöchellager sitzen dicht am Bein, die Schienen gehen **durchgehend bis in die Gelenke**.
- **Knie:** Flanschlager auf Höhe deines Kniegelenks, außen der Kniehebel (60 mm). Die Kniekapsel (RS04) sitzt oben am
  Oberschenkel, ihre Kurbel treibt über eine Schubstange den Hebel an (1:1). In der Stange sitzt ein **Federpaket**: Es gibt
  beim Beugen ab etwa 11 Nm nach und begrenzt die Beugekraft mechanisch, Strecken bleibt starr.
- **Fuß:** Knöchellager (Fußspitze hoch/runter) → Fersenbügel, der hinter die Ferse führt → **Kippgelenk mit Achse nach
  vorne durch den Knöchel**, hinter der Ferse. Der Fuß kippt seitlich mit, beim Spreizen und am Hang. Gummibuchsen lassen
  etwas Drehung zu (Fußspitze nach außen).
- **Steigbügel:** schmaler Edelstahl-Bügel unter der Sohlenmitte mit 6 Spikes, Fersenschale, Ratschenriemen über den Rist.
  Passt zu jedem Arbeitsstiefel, nichts saugt sich im Schlamm fest. Ein vergossener **Drucksensor** zwischen Bügel und
  Sohle meldet, ob das Bein belastet ist.

## 5. Antriebe: Gelenkkapseln

| Gelenk | Motor | sitzt | Übertragung | Grenze in der Firmware | Motor kann |
|---|---|---|---|---|---|
| Hüfte L/R | RobStride RS04 | auf der Hüftachse, Gehäuse an der Hüftschwinge | direkt | strecken 110 Nm, beugen 35 Nm | 116 Nm |
| Knie L/R | RobStride RS04 | oben am Oberschenkel (y −12 cm) | Kurbel 60 mm → Schubstange Ø 12 → Kniehebel 60 mm (1:1) | strecken 110 Nm, beugen 10 Nm (+ Federpaket ≈ 11 Nm mechanisch) | 114 Nm |
| Schulter L/R | RobStride RS04 | auf der Schulterachse, Gehäuse an der Schulterschwinge | direkt | ±100 Nm | 116 Nm |
| Ellbogen L/R | RobStride RS03 | oben am Oberarm (y −12 cm) | Kurbel 50 mm → Schubstange Ø 10 → Hebel 50 mm (1:1) | ±56 Nm | 57 Nm |
| Rumpf beugen | RobStride RS04 | hinter dem Kreuzbein am unteren Rahmen | Kurbel 50 mm → Schubstange → Hebel am Lendengelenk (1:1) | aufrichten 80 Nm, vorbeugen 20 Nm | 114 Nm |
| Rumpf drehen | RobStride RS03 | hinter dem Kreuzbein, Achse senkrecht | Kurbel 40 mm → Stange → Wagen der Bogenführung (≈ 1 : 1,8) | ±30 Nm | ≈ 100 Nm |

**Schubstange statt Seil:** Kurbel und Hebel bilden ein Parallelogramm, die Stange läuft parallel zur Schiene. Der Hebel
arbeitet im Bereich ±55° um die Querlage, die Stange kommt nie in eine Strecklage. Größte Stangenkraft ≈ 3,7 kN (Knie, 110 Nm
bei 60° Hebelstellung); Stahlrohr Ø 12 × 2 knickt erst bei ≈ 16 kN. Die Gelenkköpfe M8 gleichen 8° Schräglauf aus.

**Die Kapsel:** außen ein gekaufter Stiftkühlkörper direkt auf dem Motorgehäuse, seitlich ein dünner Mantel aus PETG-CF mit
zwei O-Ringen, am drehenden Abtrieb ein V-Ring, unten ein Druckausgleichselement M12 gegen Kondenswasser. An der
Abtriebsseite ein Heizband 4 W für Pausen bei Frost. Das Wärmemanagement steht in Abschnitt 5c.

### 5d. Rumpf: beugen und drehen mit Motor

Beide Rumpfmotoren sitzen in Kühlkörper-Kapseln **hinter dem Kreuzbein am unteren Rahmen**: Sie bewegen sich beim Bücken
und Drehen nicht mit, machen den Rücken nicht breiter und stoßen nicht an die schwingenden Arme.

- **Beugen (RS04):** Kurbel 50 mm → Schubstange → Hebel am Lendengelenk, Parallelogramm 1:1. Hilft beim Aufrichten bis 80 Nm,
  beim Vorbeugen bis 20 Nm. Die Gasfedern tragen weiter das Exo-Oberteil, der Motor trägt einen Anteil deines Oberkörpers
  (wie an der Hüfte: 10/20/30 % je Stufe) und der Baumlast. Hängt ein Baum in den Schlaufen, rastet die Lendensperre ein und
  der Motor wird stromlos: Er kühlt ab, während die Sperre trägt.
- **Drehen (RS03):** Kurbel 40 mm in der waagerechten Ebene → Stange → Zapfen am Wagen der Bogenführung (≈ 1 : 1,8). Bis 30 Nm,
  die Zentrierfedern bleiben. Vor den Anschlägen (±15°) läuft die Hilfe weich aus.
- **Woher das Exo deine Absicht kennt:** vier Wägezellen, je eine in den oberen Gurtankern und hinter dem Schulterblatt-
  polster. Beugst du dich vor, ziehen die Gurte: Das Exo beugt mit. Richtest du dich auf, drückst du ins Polster: Es streckt.
  Drehst du dich, wird es links und rechts unterschiedlich: Daraus folgt die Drehrichtung.
- **Ohne Strom** laufen beide frei mit (rückdrehbar), die Federn zentrieren.

### 5c. Wärmemanagement

Langes Halten unter Last kostet bei jedem Elektromotor dauernd Strom, und die Wärme wächst mit dem Quadrat des Moments.
Entscheidend ist deshalb, wie gut die Wärme aus der Kapsel herauskommt.

| Weg | Lösung | Warum |
|---|---|---|
| Wicklung → Gehäuse | im Motor (Thermistor, Firmware liest die Temperatur in jedem Takt) | – |
| Gehäuse → Kühlkörper | Wärmeleitpad 1 mm (6 W/mK), Kühlkörper mit 10 × M4 direkt aufs Gehäuse | kein Kunststoff, keine Dämmung im Wärmeweg |
| Kühlkörper → Luft | **Fischer ICK S R 140** (Stifte auf 30 mm gekürzt; RS03: Ø 120): 1,85 K/W ruhend, 0,4 K/W bei Luftstrom (ungekürzt) | Stifte statt Rippen: kühlen in jeder Gelenklage gleich; Schnee schmilzt ab und läuft weg |
| Umgebung | −10 … +5 °C im Winter | kältere Luft = mehr Reserve als im Datenblatt (dort 25 °C) |
| Pausen bei Frost | Heizband 4 W an der Abtriebsseite (V-Ring, Lager), nicht am Kühlkörper | die Kühlkörperseite soll kühlen, nur die Dichtung muss eisfrei bleiben |

**Ergebnis (Schätzung, Prüfstand P33 misst es):** Dauermoment in der Kapsel ≈ 28 Nm (RS04) bzw. 16 Nm (RS03) statt 25 / 14 Nm
mit der gedämmten Haube.

| Situation | Moment | volle Kraft für |
|---|---|---|
| Baum aufrichten, Schulter | 100 Nm | ≈ 13–15 s, danach wird die Hilfe weich schwächer (nicht aus) |
| Ellbogen am Anschlag | 56 Nm | ≈ 15 s |
| Hocke mit Last halten, Knie | ≈ 38 Nm | ≈ 2,5 min |
| Schleifen, Gehen, Stehen | 2–30 Nm | dauerhaft |

Die Firmware schützt doppelt: ein Wärmemodell (Zeitkonstante 3 min) und die gemessene Wicklungstemperatur ab 100 °C.
Der Motor selbst schaltet erst bei 135–145 °C ab. Eine Haltesperre ist damit nicht nötig.
**Heizen in Pausen:** an unter +3 °C, wenn die Gruppe kaum arbeitet, aus über +8 °C; bei −10 °C im Schnitt ≈ 10–15 W.

**Warum nicht wasserdichte Motoren?** Von RobStride gibt es nur das kleine RS02 als IP67 (17 Nm), zu schwach. Die stärkeren
Modelle haben keine Schutzart. Industriemotoren mit IP65/67 kosten mehrere tausend Euro je Gelenk. Also dichtet die Kapsel
(Ziel IP65: Schnee, Regen, Spritzwasser). Eine Anfrage an RobStride nach IP67-Versionen: [anfrage-robstride.md](anfrage-robstride.md).

**Rückdrehbar:** Die Planetengetriebe 9:1 lassen sich von Hand drehen; ohne Strom laufen alle Gelenke frei mit.

## 6. Oberkörper: Gurtsystem

Die Last läuft über den Rahmen in die Exo-Beine. Die Gurte **tragen kaum**, sie halten das Exo in Lage. Aufbau wie bei
einem guten Trekkingrucksack, angepasst an die Kräfte des Exos:

| Gurt | Lage | Aufgabe | Komfort |
|---|---|---|---|
| Hüftgurt, 10 cm breit | auf dem Beckenkamm, vorne Magnet-Schnalle | führt das Exo in Höhe und Seite, nimmt das Kippen auf, wenn ein Arm zieht | gepolsterte Flossen, sitzt über der Hüftschwinge, nie auf ihr |
| S-Schultergurte | über die Schultern, oben an der Lochleiste, unten am Hüftgurt | halten das Exo, damit es nicht nach hinten kippt | S-Form an der Brust vorbei, Rückenlänge einstellbar |
| Lageverstellriemen | Schulter → schräg nach oben zum Rahmen | ziehen den Rahmen oben an den Rücken | nur anziehen, bis der Rahmen anliegt |
| Brustgurt | höhenverstellbar zwischen den Schultergurten | hält die Schultergurte zusammen, wenn ein Arm seitlich zieht | Magnet-Schnalle, einhändig |
| Lenden- und Schulterblattpolster | zwischen Rahmen und Rücken | Druck verteilen, Luftspalt | Abstandsgewirke, abnehmbar, waschbar |

Anziehen: rückwärts ins Exo auf der Parkstütze, Hüftgurt, Schultergurte, Lageverstellriemen, Brustgurt, Manschetten,
Ristriemen. Der Not-Aus mit LED-Ring sitzt am linken Schultergurt.

## 5a. Rucksack: kompakt, Akkus oben geschützt

```
        Bogen des Schutzbügels
     ┌──────────────────────────┐
     │  Akku L  │  Akku R       │  stehend zwischen den Schulterblättern, von oben eingesteckt
     │ (Stihl)  │ (Stihl)       │  Kontakte unten (Wasser läuft ab), Rastung oben vorne
     ├──────────────────────────┤
     │  Elektronikbox (6 cm tief)│  Lendenhöhe: Steuerung, Wandler, Sicherungen, Bremswiderstand
     └──────────────────────────┘
        Bogenführung der Rumpfdrehung, Lendengelenk, Hüftgabeln
```

- **Akkus oben:** Zwei Stihl AP 500 S stehen nebeneinander zwischen den Schulterblättern, direkt hinter dem Rahmen.
  Rundum der Schutzbügel (CFK, Gummipuffer), oben drüber sein Bogen: Fällst du nach hinten, trifft zuerst der Bügel,
  Akkus und Box liegen innen. Einstecken von oben wie aus einem Köcher (Griff über die Schulter); der zweite Akku
  ist optional, beide parallel über Ideal-Dioden, im Betrieb einzeln wechselbar.
- **Kompakt:** Ohne Motorbox ist der Rucksack nur noch etwa 12 cm tief (vorher gut 20 cm) und so breit wie dein Rücken.
  Der Schwerpunkt des Exo-Oberteils liegt fast über der Hüfte (3 cm dahinter), der Rucksack zieht kaum nach hinten.
- **Elektronikbox:** flach (60 × 140 × 220 mm), Stecker unten für die Beine, seitlich oben für die Arme.
- **Gasfedern** des Lendengelenks sitzen jetzt seitlich neben der Box.
- **Festigkeit:** Rohrgrößen nach [festigkeit.md](festigkeit.md).

## 5b. Schulter: seitlich heben

Wie an der Hüfte sitzt das Gelenk fürs seitliche Heben hinter dir, die Achse zeigt nach vorne durch dein Schultergelenk.
Die Schulterschwinge führt über die Schulter zum Schulterlager; dort sitzt die Schulterkapsel (RS04) direkt auf der Achse.
Sie hebt beim seitlichen Heben mit, deshalb ändert seitliches Heben nichts am Antrieb. Eine Feder trägt den Exo-Arm.

## 6a. Steuerung (kurz)

Details: [steuerung.md](steuerung.md).

- **Manschetten mit Wägezelle** an Oberarm, Unterarm, Oberschenkel: Drückst du, folgt das Exo. Der Druck wird über die
  Hebel auf alle Gelenke umgerechnet, die ihn bewegen können.
- **Wägezellen in den Schlaufenköpfen:** Das Exo kennt das Baumgewicht und trägt seinen Anteil von selbst (je Stufe
  50 / 67 / 80 %).
- **Fußkontakt:** Das Standbein trägt automatisch einen Teil von Oberkörper, Exo und Baum; das Schwungbein läuft frei und
  hilft beim Anheben; am Standbein beugt das Knie nie aktiv.

## 6b. Greifer: Schlaufen am Handgelenk

**Prinzip Spannband (wie eine Schlauchschelle):** Die Schlaufe ist ein Edelstahl-Lochband 20 × 0,3 mm mit Gummi innen.
Ein Ende ist am Kopf fest (Schnappschäkel), das andere läuft durch einen Schlitz im Kopf in einen Bandkanal zum Antrieb.
Ein Stachelrad am Schneckengetriebemotor greift in die Lochung. **Zuziehen:** Das Band wird aus der Schlaufe gezogen,
die Schlaufe wird kleiner (nichts wird zusammengedrückt); das freie Band (bis 25 cm) läuft in einen Speicherkanal an der
Unterarmschiene. Stopp bei etwa 400 N (Motorstrom + Wägezelle). **Öffnen:** Das Stachelrad schiebt das Band zurück, der
Federstahl drückt die Schlaufe rund auf (bis Ø 16 cm). Hält ohne Strom; Notöffnung über einen Freilaufhebel am Antrieb
oder den Schnappschäkel.

- **Schlaufenträger:** gebogenes Alu-Band von der Unterarmschiene außen um das Handgelenk herum nach unten.
- **Rechts zwei Köpfe auf Höhe des Handgelenks:**
  - **unten** (Handflächenseite): Hauptschlaufe zum Laden und Schleifen;
  - **schräg außen-unten**, 4 cm Richtung Ellbogen versetzt, damit sich die Schlaufen nie verhaken: hält beim Schleifen
    einen zweiten Stamm neben dem ersten, oder greift beim Laden zusätzlich um denselben Stamm (doppelter Halt).
- **Links ein Kopf unten.**
- Jeder Kopf hat eine Zahnleiste (formschlüssiger Halt) und eine Wägezelle 50 kg (Baumgewicht für die Steuerung).
- Der Stamm liegt längs als Verlängerung des Unterarms, das Schnittende am Handgelenk.
- **Bedienteil an der Handschlaufe, Daumenseite:** je Schlaufe eine Wippe. Nach vorne tippen = zuziehen bis Haltekraft,
  nach vorne halten = langsam nachziehen, nach hinten 1 s halten = öffnen (mit Last erst nach Vibrationswarnung).
  Rechts gelb = unten, orange = außen, fühlbar verschieden geformt; beide nach vorne = beide zu. Links eine Wippe und eine
  blaue Taste (kurz = Licht, lang = Stufe). Je Wippe eine LED: grün offen, gelb zieht, blau hält.
  *Die Firmware nutzt noch die alte Tastenlogik und wird darauf umgestellt.*

## 6c. Anzeige am rechten Oberarm

1,54"-E-Paper außen am rechten Oberarm (bei Kälte und Sonne lesbar, stromsparend): groß die **Uhrzeit**, darunter **Akku
in Prozent mit Restzeit** und die **Stufe**, unten **Fehler und Warnungen im Klartext** (z. B. „Akku schwach“,
„Sensor-Manschette gestört“, „Parkstütze unten“). Keine Produktionsdaten. Die Uhr läuft auf dem Teensy mit Knopfzelle.

## 6d. Tragekomfort: Lendengelenk und kleine Freiheitsgrade

Ein starrer Rücken zwingt dich, aus den Knien zu heben, und drückt bei jedem Bücken Hüftgurt und Schultergurte gegen
den Körper. Deshalb hat Entwurf 4 jetzt:

| Gelenk | Bewegung | Wie | Warum |
|---|---|---|---|
| **Rumpfdrehung** unter dem Lendengelenk (motorisiert, Abschnitt 5d) | Oberkörper drehen ±15° | Bogenführung: gebogene Edelstahlwelle Ø 10 (R 95 mm um die Wirbelsäule), Wagen mit 4 Profilrollen INA LFR 5201-10-2Z, 2 Zentrierfedern, Gummianschläge | Umschauen, Baum zur Seite ziehen, Hüfte drehen ohne dass Gurte scheuern; die Achse liegt in der Wirbelsäule, deshalb verschiebt sich nichts gegen den Körper |
| **Lendengelenk** im Rückenrahmen, 17 cm über den Hüftgelenken | Rumpf beugen ±25°, seitlich ±10° | Scharnier mit Elastomerfeder, **zwei Gasfedern 150 N** tragen das Oberteil (≈ 36 Nm) | Bücken fühlt sich leicht an, das Exo-Oberteil zieht dich nicht nach vorne |
| **Sperre** am Lendengelenk | rastet in 5 Stellungen (−25 … +25°) | Hubmagnet 24 V mit Sperrbolzen, Rastsegment Edelstahl | sobald > 5 kg in den Schlaufen hängen (0,3 s), sperrt die Firmware: dann trägt der Rahmen den Baum, nicht dein Kreuz. Hebel zum Sperren von Hand |
| **Schulterschlitten** | Schulter hebt und senkt sich ±3 cm | 2 Wellen Ø 8 mit Linearlagern, Druckfeder | Schulterblatt kann sich bewegen: Arm über Kopf und Schulterzucken ohne Zwang |
| **Drehring Unterarm** | Unterarm dreht frei | Ring-Lager an der Unterarm-Manschette | Handgelenk drehen (Stamm greifen, Säge halten) ohne die Schiene mitzunehmen |
| **Drehteller am Steigbügel** | Fußspitze ±20° | Axial-Nadellager, Zentrierfeder | Wenden und Fuß nach außen stellen, ohne das Knie zu verdrehen |

Dazu **Kurbelschutz** aus transparentem PETG über den Kurbeln an Knie und Ellbogen: Finger, Jackenärmel und Zweige geraten
nicht zwischen Kurbel und Schiene.

**Warum keine weiteren Motoren für Rumpf und Fuß?** Ein Motor am Lendengelenk müsste etwa 150 Nm aufbringen, wäre
schwer und würde dich beim Bücken bevormunden. Gasfeder + Sperre bringt den Nutzen (Last im Rahmen) ohne das Gewicht.
Einen Fußmotor für aktive Balance hat das Exo bewusst nicht: Das Gleichgewicht hältst du selbst über deine Füße, das
Exo läuft frei mit, warnt aber bei Rückneigung und dämpft die Beine bei einem Sturz ([steuerung.md](steuerung.md)).

## 7. Energie

- **Akku: Stihl AP 500 S** (36 V, etwa 337 Wh) in einem Schacht seitlich neben der Elektronikbox; ein **zweiter Schacht** auf der
  anderen Seite nimmt bei Bedarf einen zweiten Akku auf (parallel über Ideal-Dioden, im Betrieb einzeln wechselbar).
- **Laufzeit, grob:** bei durchschnittlich 100 bis 150 W etwa 2 bis 3 Stunden; bei Frost kommen 10–15 W Kapselheizung dazu.
- **Bremsenergie:** Beim Hinsetzen und Absenken speisen die Motoren zurück. Ein Bremswiderstand (ab 46 V, 10 Ω / 200 W)
  begrenzt die Spannung, sonst schalten die Motoren bei Überspannung ab (RobStride-Entlademodul passt nicht: 34 / 64 V).
- **Kälte und Nässe:** Akku in der gedämmten Box, wasserdichte Rundstecker, Elektronik im dichten Gehäuse.

## 8. Gewicht

**Bauteilliste der technischen Ansicht: 43,3 kg mit einem Akku** (Entwurf 4 mit Seilantrieb: 31,9 kg). Die Motoren allein
wiegen 10,3 kg (6 × RS04 je 1,42 kg, 2 × RS03 je 0,88 kg) statt 5,0 kg; Kühlkörper (8 × ≈ 0,28 kg), Mäntel, Dichtungen und Wangen kommen dazu.
Seile, Rollen, Scheiben, Bowdenzüge und die Motorbox fallen weg.

Was das für dich heißt:
- Fast das ganze Gewicht geht über die Exo-Beine und Steigbügel in den Boden. Die Steuerung trägt im Stand **29,1 kg
  Exo-Oberteil** (mit den Rumpfkapseln am Becken), Schwerpunkt 34 cm über und 5 cm hinter der Hüftachse: Der Rucksack zieht
  kaum nach hinten.
- Mehr Masse sitzt an den Gliedern: je Bein 1,4 kg Kniemotor oben am Oberschenkel, je Arm 1,4 kg an der Schulter und
  0,9 kg oben am Oberarm. Beim schnellen Schwingen spürst du das; im Stand gleicht die Steuerung das Eigengewicht aus.
- **Sparmöglichkeiten:** Kniemotor RS03 statt RS04 (−1,1 kg, aber nur 57 Nm am Knie: reicht für Gehen und leichtes Heben,
  nicht für die Hocke mit schwerem Baum), zweiter Akkuschacht weg (−0,5 kg), Parkstütze bleibt am Fahrzeug (schon so).
- **Bewusst nicht gespart:** Die tragenden Rohre liegen bei etwa 60 MPa Spitze gegen 125 MPa zulässig, also Sicherheit ≈ 2
  ([festigkeit.md](festigkeit.md)).

## 9. Wetterfestigkeit

Die Motoren sitzen in dichten, beheizten Gelenkkapseln (Abschnitt 5). Es gibt keine offenen Seile und Rollen mehr, die
vereisen können; die Schubstangen sind starr und laufen in Gelenkköpfen aus Edelstahl. Abgedichtete Lager (2RS), Alu und
Edelstahl rosten nicht, Wägezellen in IP65-Ausführung oder vergossen, Stecker nach unten, Elektronikbox IP54.

## 10. Sicherheit

Not-Aus trennt die Motoren hart. Ohne Strom laufen alle Gelenke frei, die Schlaufen halten (Schneckengetriebe) und lassen
sich über die Schnappschäkel von Hand öffnen. Gelenke haben feste Endanschläge, Momente und Geschwindigkeiten sind in der
Software begrenzt. Erst Prüfstand, dann Leergewicht, dann Bäume.
**Nicht mit Motorsäge in der Hand im unterstützten Modus arbeiten.**

## 11. Kosten, grob

| Posten | etwa |
|---|---|
| 7 × RobStride RS04 (≈ 295–320 $) + 3 × RS03 (≈ 225–245 $), mit Versand, Einfuhrumsatzsteuer, Zoll | 3 200 bis 3 600 € |
| Kapseln (10): Stiftkühlkörper, Wangen, Kurbeln, Hebel (Laser), Mäntel (Druck), Wärmeleitpads, V-Ringe, O-Ringe, Druckausgleich, Heizbänder, Schubstangen, Gelenkköpfe | 520 bis 640 € |
| 3 Schlaufenantriebe 5840-31ZY + 3 Treiber Pololu G2 | 180 bis 220 € |
| Steuerung, CAN, 11 Wägezellen + Verstärker, Not-Aus, Relais, Stecker, Kabel, Anzeige, Licht | 400 bis 500 € |
| Akku-Adapter für Stihl AP (Akku vorhanden) | 20 bis 80 € |
| Alu-Profile, Lager, Bolzen, Laserteile Rahmen und Gelenke | 450 bis 550 € |
| Lendengelenk, Gasfedern, Hubmagnet, Drehteller, Drehring, Schulterschlitten | 190 bis 220 € |
| Steigbügel, Fersenbügel, Spikes, Gurtsystem, Manschetten, Polster | 300 bis 380 € |
| Schutzbügel, Parkstütze, Haube Elektronikbox, Eloxieren, Filament | 230 bis 360 € |
| Rumpfantrieb: Kurbeln, Hebel, Stangen, Gelenkköpfe, 4 Rumpf-Wägezellen + HX711 | 90 bis 120 € |
| **Summe Material** | **etwa 5 700 bis 6 800 €** (dazu Werkzeug einmalig 280 bis 500 €) |

Über einen Einkaufsagenten direkt aus China (RS04 ¥ 1.199, RS03 ¥ 999) wären die Motoren etwa 1.000 € billiger;
Gewährleistung und Lieferung sind dann schwieriger.

## 12. Bauen in Stufen

1. **Schlaufengreifer als Handwerkzeug** (an einer Unterarm-Manschette, ohne Exo) und Steigbügel testen.
2. **Rahmen, Beine, Hüfte passiv**, mit Steigbügeln: Lastpfad und Passform prüfen.
3. **Prüfstand:** eine Gelenkkapsel mit RS04, Kurbel und Schubstange an einem Holzarm, eine Sensor-Manschette. Zuerst die
   Kapsel: 10 min Gartenschlauch, eine Nacht im Gefrierschrank, danach muss das Gelenk sofort leicht laufen.
4. **Beine:** Hüftkapseln, dann Kniekapseln mit Schubstange, Oberschenkel-Manschetten und Fußsensoren.
5. **Arme:** Schulterkapseln, Ellbogenkapseln mit Schubstange, dann Schlaufen ans Exo.
6. **Feinschliff:** Verstärkung, Filter, Akkulaufzeit, eine Saison Praxis.

Jede Stufe ist für sich testbar und verteilt die Kosten über die Zeit.
