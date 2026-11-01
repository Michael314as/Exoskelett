# Konzept: Christbaum-Exo (Entwurf 4)

Ziel: ein umsetzbares, robustes Exoskelett für zwei Arbeiten: Bäume am Stamm greifen und **schleifen** (Use Case 2)
und Bäume am Stumpf greifen, fast senkrecht **aufrichten und aufladen** (Use Case 1). Es unterstützt Beine,
Oberkörper und Arme. Budget: einige tausend Euro, verteilt über die Zeit. Serienteile, die man einzeln kaufen und
tauschen kann; gebaut ohne Drehbank und ohne Schweißen.

**Rahmenbedingungen (geklärt):** Schleifen bis etwa 100 m. Arbeiten bei Frostregen, nassem Schnee, **Schlamm und unebenem
Gelände**. Akku von vorhandenen Stihl-Geräten. Eine Person (1,75 m, 75 kg), Längen einstellbar. Die meisten Bäume
schaffst du selbst; das Exo soll **entlasten**, nicht ersetzen.

**Neu in Entwurf 4** (nach deiner Durchsicht der 3D-Konstruktion):

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
 Schulterbügel ── Schulter (Seilscheibe, offenes Seilpaar) ── Oberarm (Anzeige rechts) ── Ellbogen (Bowden-Paar) ── Unterarm ── Schlaufenträger
       │                                                                                                         (rechts unten + außen, links unten)
 Rückenrahmen ── Antriebsbox: 6 Motoren (Knie, Schulter, Ellbogen), Steuerung; Akku unten hinten
       │            └── Gurtsystem: Hüftgurt, Schultergurte, Lageverstellriemen, Brustgurt, Polster
 Gabel des Abspreizgelenks (Achse nach vorne durchs Hüftgelenk, Zentrierfedern ±20°)
       │
 Hüftschwinge mit Hüftmotor (Seilgetriebe 1:6) ── Hüftlager ── Oberschenkelschiene (Manschette mit Sensor) ── Kniegelenk (Kurvenscheibe + Beugescheibe)
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
- **Hüftmotor:** sitzt auf der Hüftschwinge hinter dem Hüftlager. Trommel Ø 2 cm, offene Seilschleife (Dyneema 3 mm) auf
  eine Scheibe Ø 12 cm am Oberschenkel: Übersetzung 1:6, bis etwa 140 Nm, Wirkungsgrad etwa 95 %. Er spreizt mit, also
  ändert Spreizen nichts am Antrieb, und er schwingt beim Gehen nicht mit. Haube gegen Äste und Schnee.

## 4. Beine und Füße

- **Schienen:** Außenrohr 25 × 25, innen 20 × 20 teleskopierbar. Die Oberschenkelschiene läuft von der Hüfte (23 cm
  neben der Körpermitte) schräg nach innen zum Knie (16 cm), die Unterschenkelschiene weiter zum Knöchel (15 cm): Knie-
  und Knöchellager sitzen dicht am Bein, die Schienen gehen **durchgehend bis in die Gelenke**.
- **Knie:** Flanschlager auf Höhe deines Kniegelenks, außen zwei Scheiben fest am Unterschenkel. Streckzug über die
  Kurvenscheibe (Hebel 5 cm gestreckt, 8 cm in der Hocke), Beugezug über die Beugescheibe Ø 9 cm. Beide kommen als
  Bowdenzug-Paar von hinten aus der Box. Der Beugezug endet in einer **Federpatrone**: Sie begrenzt die Beugekraft
  mechanisch auf etwa 11 Nm.
- **Fuß:** Knöchellager (Fußspitze hoch/runter) → Fersenbügel, der hinter die Ferse führt → **Kippgelenk mit Achse nach
  vorne durch den Knöchel**, hinter der Ferse. Der Fuß kippt seitlich mit, beim Spreizen und am Hang. Gummibuchsen lassen
  etwas Drehung zu (Fußspitze nach außen).
- **Steigbügel:** schmaler Edelstahl-Bügel unter der Sohlenmitte mit 6 Spikes, Fersenschale, Ratschenriemen über den Rist.
  Passt zu jedem Arbeitsstiefel, nichts saugt sich im Schlamm fest. Ein vergossener **Drucksensor** zwischen Bügel und
  Sohle meldet, ob das Bein belastet ist.

## 5. Antriebe

| Kanal | Motor sitzt | Richtung | Übertragung | Spitze je Seite |
|---|---|---|---|---|
| Hüfte L/R | auf der Hüftschwinge | strecken stark, beugen bis 35 Nm | offene Seilschleife 1:6 | ~140 Nm |
| Knie L/R | Box | strecken stark, beugen bis 10 Nm | Bowdenzug-Paar, Kurvenscheibe 5→8 cm | ~95–115 Nm |
| Schulter L/R | Box | heben und senken | offenes Seilpaar über Rollen auf Scheibe Ø 14 cm | ~100 Nm |
| Ellbogen L/R | Box | beugen und strecken | Bowdenzug-Paar auf Scheibe Ø 10 cm | ~58 Nm |

8 bürstenlose Robotergelenk-Motoren (QDD-Klasse, CAN, 9 Nm dauernd / 25 Nm Spitze), von Hand rückdrehbar: Ohne Strom
laufen alle Gelenke frei mit. Die Rechnung zum Mischweg: [kraftverteilung.md](kraftverteilung.md), Abschnitt 3.

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

## 6a. Steuerung (kurz)

Details: [steuerung.md](steuerung.md).

- **Manschetten mit Wägezelle** an Oberarm, Unterarm, Oberschenkel: Drückst du, folgt das Exo. Der Druck wird über die
  Hebel auf alle Gelenke umgerechnet, die ihn bewegen können.
- **Wägezellen in den Schlaufenköpfen:** Das Exo kennt das Baumgewicht und trägt seinen Anteil von selbst (je Stufe
  50 / 67 / 80 %).
- **Fußkontakt:** Das Standbein trägt automatisch einen Teil von Oberkörper, Exo und Baum; das Schwungbein läuft frei und
  hilft beim Anheben; am Standbein beugt das Knie nie aktiv.

## 6b. Greifer: Schlaufen am Handgelenk

**Prinzip Fangschlinge:** Dyneema-Seil in einem Gummischlauch mit Federstahl-Einlage bleibt offen wie ein Ring. Ein Ende
hängt am Schnappschäkel, das andere läuft zu einer kleinen Winde mit Schneckengetriebe (hält ohne Strom).

- **Schlaufenträger:** gebogenes Alu-Band von der Unterarmschiene außen um das Handgelenk herum nach unten.
- **Rechts zwei Köpfe auf Höhe des Handgelenks:**
  - **unten** (Handflächenseite): Hauptschlaufe zum Laden und Schleifen;
  - **schräg außen-unten**, 4 cm Richtung Ellbogen versetzt, damit sich die Schlaufen nie verhaken: hält beim Schleifen
    einen zweiten Stamm neben dem ersten, oder greift beim Laden zusätzlich um denselben Stamm (doppelter Halt).
- **Links ein Kopf unten.**
- Jeder Kopf hat eine Zahnleiste (formschlüssiger Halt) und eine Wägezelle 50 kg (Baumgewicht für die Steuerung).
- Der Stamm liegt längs als Verlängerung des Unterarms, das Schnittende am Handgelenk.
- Daumentaster an der Hand: kurz = zu, lang = auf. Notöffnung: rote Lasche am Schnappschäkel, auch ohne Strom.

## 6c. Anzeige am rechten Oberarm

1,54"-E-Paper außen am rechten Oberarm (bei Kälte und Sonne lesbar, stromsparend): groß die **Uhrzeit**, darunter **Akku
in Prozent mit Restzeit** und die **Stufe**, unten **Fehler und Warnungen im Klartext** (z. B. „Akku schwach“,
„Sensor-Manschette gestört“, „Parkstütze unten“). Keine Produktionsdaten. Die Uhr läuft auf dem Teensy mit Knopfzelle.

## 7. Energie

- **Akku: Stihl AP 500 S** (36 V, etwa 337 Wh), unten in der Antriebsbox in einem gedruckten Adapter.
- **Laufzeit, grob:** bei durchschnittlich 100 bis 150 W etwa 2 bis 3 Stunden. Schlaufenwinden brauchen fast nichts.
- **Kälte und Nässe:** Akku in der gedämmten Box, wasserdichte Rundstecker, Elektronik im dichten Gehäuse.

## 8. Gewicht

Motoren 8 × 0,5–1 kg, Schlaufenwinden 3 × 0,3 kg, Akku etwa 2 kg, Alu-Rahmen mit Hüftschwingen etwa 5,5 kg,
Steigbügel und Fersenbügel etwa 1 kg, Gurte etwa 1,5 kg, Rest etwa 2 kg. **Ziel: 17 bis 20 kg.** Die zwei Hüftmotoren
sitzen nah am Schwerpunkt und schwingen nicht mit, das Bein bleibt so leicht wie bisher.

## 9. Wetterfestigkeit

Abgedichtete Lager (2RS), Seilrollen mit Abdeckung gegen Vereisen, Bowdenhüllen mit Endkappen, Dyneema nimmt kein
Wasser auf, Alu und Edelstahl rosten nicht, Wägezellen in IP65-Ausführung oder vergossen, Stecker nach unten.

## 10. Sicherheit

Not-Aus trennt die Motoren hart. Ohne Strom laufen alle Gelenke frei, die Schlaufen halten (Schneckengetriebe) und lassen
sich über die Schnappschäkel von Hand öffnen. Gelenke haben feste Endanschläge, Momente und Geschwindigkeiten sind in der
Software begrenzt. Erst Prüfstand, dann Leergewicht, dann Bäume.
**Nicht mit Motorsäge in der Hand im unterstützten Modus arbeiten.**

## 11. Kosten, grob

| Posten | etwa |
|---|---|
| 8 Robotergelenk-Motoren | 1 200 bis 4 000 € |
| 3 Schlaufenwinden (Schneckengetriebemotor, Treiber) | 80 bis 130 € |
| Steuerung, CAN, 9 Wägezellen + Verstärker, 2 Fußsensoren, Not-Aus, Kabel, Anzeige | 230 bis 360 € |
| Akku-Adapter für Stihl AP (Akku vorhanden) | 20 bis 80 € |
| Alu-Profile, Lager, Bolzen, Rollen, Seilscheiben, Dyneema, Bowdenzüge | 300 bis 450 € |
| Steigbügel, Fersenbügel, Spikes, Gurtsystem | 160 bis 280 € |
| Filament (PETG, TPU) | 50 bis 100 € |
| **Summe** | **etwa 2 100 bis 5 500 €** |

## 12. Bauen in Stufen

1. **Schlaufengreifer als Handwerkzeug** (an einer Unterarm-Manschette, ohne Exo) und Steigbügel testen.
2. **Rahmen, Beine, Hüfte passiv**, mit Steigbügeln: Lastpfad und Passform prüfen.
3. **Prüfstand:** ein Motor, ein Seilpaar, eine Sensor-Manschette auf einem Holzarm.
4. **Beine:** Hüftmotoren auf den Schwingen, dann Knie per Bowdenzug, Oberschenkel-Manschetten und Fußsensoren.
5. **Arme:** Schulter mit offenem Seilpaar, Ellbogen mit Bowdenzug-Paar, dann Schlaufen ans Exo.
6. **Feinschliff:** Verstärkung, Filter, Akkulaufzeit, eine Saison Praxis.

Jede Stufe ist für sich testbar und verteilt die Kosten über die Zeit.
