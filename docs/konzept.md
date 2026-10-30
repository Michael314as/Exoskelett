# Konzept: Christbaum-Exo (Entwurf 1)

Ziel: ein umsetzbares, robustes Exoskelett für zwei Arbeiten: Bäume am Stamm greifen und **schleifen** (Use Case 2)
und Bäume am Stumpf greifen, fast senkrecht **aufrichten und aufladen** (Use Case 1). Es unterstützt Beine,
Oberkörper und Arme und soll die eigene Kraft etwa verdoppeln. Budget: einige tausend Euro, verteilt über die Zeit.
Keine Spezialteile für zehntausende Euro, sondern Serienteile, die man einzeln kaufen und tauschen kann.

## 1. Grundidee in einem Satz

**Motoren sitzen hinten im Rückenrahmen, Seile übertragen die Kraft zu Hüfte, Knie, Schultern und Ellbogen,
ein Metallrahmen leitet die Last bis in die Fußplatten, und Federn übernehmen das reine Halten.**

Warum so:

| Entscheidung | Begründung |
|---|---|
| Motoren am Rücken statt in jedem Gelenk | Arme und Beine bleiben leicht, schwere Teile sitzen hinten (hilft auch beim Gleichgewicht, siehe Use Case 1). Motoren sind gut geschützt vor Nässe und Stößen. |
| Seilzug statt Zahnräder an den Gelenken | Seile übertragen hohe Zugkräfte leicht und günstig, Übersetzung kommt über Seilrollen fast gratis. Ein Seil kann nur ziehen: fällt etwas aus, lässt es los statt zu drücken. Das ist sicherer. |
| Zug nur in Lastrichtung | Beim Heben, Aufrichten und Ziehen braucht es fast nur eine Richtung: Hüfte und Knie strecken, Arme heben bzw. beugen. Die Gegenrichtung macht der Körper selbst. Das halbiert die Zahl der Antriebe. |
| Federn für Dauerhaltung | Arme oben halten und Rumpf halten kostet beim Motor Strom, bei einer Feder nichts. Gasfedern wie bei Industrie-Armstützen sind billig und bewährt. |
| Starrer Rahmen bis in die Fußplatten | Baumgewicht soll über Metall in den Boden, nicht über Knochen und Gelenke. |

## 2. Grundstruktur

```
           Schulterbügel ── Schulterrolle (Gasfeder + Seil) ── Oberarmschiene ── Ellbogen (Seil) ── Unterarmschiene
                 │
  Rückenrahmen (Kraxe-Prinzip, Alu) ── Antriebsbox: 6 Motoren, Akku, Steuerung
                 │
           Hüftgurt + Hüftbügel ── Zuggreifer hinten (Use Case 2)
                 │
  Hüftgelenk (Lager + Seilrolle) ── Oberschenkelschiene ── Kniegelenk (Lager + Seilrolle) ── Unterschenkelschiene
                                                                                                │
                                                                         Sprunggelenk (frei, mit Anschlag) ── Fußplatte mit Bindung
```

- **Rückenrahmen:** Aluminium-Vierkantrohr 25 × 25 × 2 mm, Bauform wie eine Lastenkraxe. Er nimmt die Antriebsbox auf und
  verbindet Schultern und Hüfte. Als Start kann ein gekauftes Kraxengestell mit Hüftgurt dienen.
- **Hüfte:** breiter Hüftgurt mit Alu-Bügel. Seitlich je ein Hüftgelenk mit Kugellagern (z. B. 6202), Achse genau auf Höhe
  des eigenen Hüftgelenks, seitlich einstellbar.
- **Beine:** Schienen aus Alu-Flachprofil bzw. Rohr, Länge über Lochraster einstellbar (Steckbolzen), Kniegelenk als
  einachsiges Scharnier mit Lagern und mechanischem Anschlag gegen Überstrecken.
- **Füße:** Alu-Platte unter dem Schuh, festgehalten mit einer **Snowboard-Bindung** (günstig, schnell an- und ausziehbar,
  für Winter gemacht). Die Platte ragt etwa 15 bis 20 cm nach vorne über die Schuhspitze, das vergrößert die Standfläche.
- **Schultern:** Bügel vom Rückenrahmen über die Schultern, Schultergelenk außen mit Drehachse für Heben/Senken.
  Seitwärts schwenkbar, damit die Arme frei bleiben.
- **Arme:** leichte Schienen an Ober- und Unterarm mit Manschetten, Ellbogen als Scharnier.
- **Manschetten, Gehäuse, Seilführungen:** 3D-Druck (PETG, Polster und Greifbeläge in TPU), alles innerhalb 18 × 18 × 18 cm.

## 3. Antriebe

**Sechs Motorkanäle:**

| Kanal | wirkt auf | Richtung | Zweck |
|---|---|---|---|
| Bein links / rechts | Hüfte **und** Knie über ein gemeinsames Seil | strecken | aus der Hocke hochkommen, Last tragen, beim Ziehen vorwärts schieben |
| Schulter links / rechts | Oberarm | heben | Arme mit Last heben, Baum hochziehen, über Kopf |
| Ellbogen links / rechts | Unterarm | beugen | Baum heranziehen, Hebel der linken Hand beim Aufrichten |

- **Motoren:** bürstenlose Robotergelenk-Motoren mit eingebautem Planetengetriebe aus der Laufroboter-Szene
  (Quasi-Direct-Drive-Klasse). Sie werden in Stückzahlen gebaut, haben Encoder und CAN-Bus eingebaut und kosten je
  nach Größe etwa 150 bis 600 €. Genaue Typen wählen wir im nächsten Schritt nach Datenblatt aus.
- **Seiltrommel:** auf jedem Motor eine Trommel mit etwa 2 cm Radius. An der Gelenkrolle mit 6 bis 8 cm Radius ergibt sich
  noch einmal Faktor 3 bis 4 Übersetzung. Beispiel: 25 Nm am Motor → 1,25 kN Seilkraft → etwa 75 bis 100 Nm am Gelenk.
- **Seil:** Dyneema-Seil 3 bis 4 mm (sehr reißfest, leicht, dehnt sich kaum), über Rollen am Rahmen geführt statt in
  Bowdenzughüllen (weniger Reibung).
- **Beinseil über Hüfte und Knie:** ein Seil pro Bein läuft hinten über die Hüftrolle und vorne über die Knierolle. Zieht der
  Motor, strecken sich Hüfte und Knie gleichzeitig, genau die Bewegung beim Hochkommen aus der Hocke und beim Schieben.
  Spart zwei Motoren.
- **Federn:** je Schulter eine Gasfeder, die das Gewicht der Arme plus etwas Last ausgleicht. Rückenstrecker-Feder
  optional. Federn sind wegklappbar, falls sie stören.

**Unterstützung, Größenordnung:** Für einen schweren 2 m Baum (30 kg) braucht die Hüfte beim Aufrichten etwa 150 Nm pro Seite.
Übernimmt das Exo die Hälfte, also 75 Nm, ist das mit Motoren dieser Klasse und Seilübersetzung machbar. Das ist das Ziel
„Kraft verdoppeln“. Für die sehr schweren 4 m Bäume reicht auch das nicht ganz, dafür gibt es die Leiter-Methode.

## 4. Greifer

Grundsatz: **Greifen mechanisch, ohne Motor**, selbstklemmend unter Last. So fällt nichts herunter, wenn der Strom weg ist.

- **Stumpfgreifer rechte Hand (Use Case 1):** kleine selbstklemmende Stammzange (Prinzip Rückezange), mit Handgriff und an der
  rechten Unterarmschiene eingehängt. Zieht man, klemmt sie fester. Öffnen per Daumenhebel. Die linke Hand greift ganz normal
  mit Handschuh weiter oben am Stamm.
- **Zuggreifer hinten an der Hüfte (Use Case 2):** Querträger am Hüftbügel, daran bis zu vier kurze selbstklemmende Zangen.
  Man setzt sie an den Stümpfen an, die Last hebt die Stümpfe leicht an und klemmt sie fest. Lösen über einen gemeinsamen
  Zug am Hüftgurt. Die Kraft geht über Hüftbügel und Beinschienen in die Fußplatten, die Beinmotoren helfen beim Vorwärtsschieben.

## 5. Steuerung und Bedienung

- **Rechner:** ein Mikrocontroller mit CAN-Bus (z. B. ESP32 oder Teensy), spricht alle Motoren direkt an.
- **Sensoren:** Lagesensoren (IMU) an Rücken, Oberschenkeln und Oberarmen, dazu die Encoder und der Motorstrom.
  So erkennt das Exo, ob man in die Hocke geht, sich aufrichtet oder vorwärts drückt.
- **Unterstützen statt fernsteuern:** Das Exo verstärkt die Bewegung, die man selbst macht. Bewegt man sich nicht, hält es nur.
- **Betriebsarten:** *Aus* (alles locker), *Heben/Aufrichten*, *Tragen*, *Ziehen*. Umschalten über zwei große Tasten am
  linken Schultergurt, mit Handschuhen bedienbar.
- **Not-Aus:** großer Pilzschalter am Schultergurt, trennt die Motoren hart vom Akku. Seile werden dann locker.

## 6. Energie

- **Akku:** handelsüblicher E-Bike-Akku 36 oder 48 V, etwa 500 bis 700 Wh, in der Antriebsbox. Günstig, robust, mit
  Ladegerät und Schutzschaltung. Reicht nach grober Schätzung für einen Arbeitstag mit Pausen.
- Elektronik spritzwassergeschützt in der Box, Kabel nur am Rücken.

## 7. Gewicht und Gleichgewicht

- Ziel: unter 20 kg gesamt. Ein großer Teil (Akku, Motoren) sitzt hinten und gleicht den Baum vorne aus.
- Verlängerte Fußplatten vorne (15 bis 20 cm) vergrößern die Standfläche, das wirkt direkt gegen das Vornüberkippen.

## 8. Sicherheit

Seile ziehen nur, Greifer klemmen mechanisch, Gelenke haben feste Endanschläge in Körpergrenzen, Drehmomente sind in der
Software begrenzt, Not-Aus trennt hart. Erste Tests auf einem Prüfstand und mit Leergewicht, erst dann mit Bäumen.
**Nicht mit Motorsäge in der Hand im unterstützten Modus arbeiten.**

## 9. Kosten, grob

| Posten | etwa |
|---|---|
| 6 Robotergelenk-Motoren | 900 bis 3 000 € (je nach Typ) |
| Steuerung, CAN, Sensoren, Not-Aus, Kabel | 150 bis 250 € |
| E-Bike-Akku mit Ladegerät | 300 bis 500 € |
| Alu-Profile, Lager, Bolzen, Rollen, Dyneema-Seil | 250 bis 400 € |
| Snowboard-Bindungen, Hüftgurt, Kraxengestell, Gasfedern | 150 bis 300 € |
| Zangen für die Greifer | 100 bis 200 € |
| Filament (PETG, TPU) | 50 bis 100 € |
| **Summe** | **etwa 1 900 bis 4 750 €** |

## 10. Bauen in Stufen

1. **Beine + Rücken + Hüfte, passiv:** Rahmen, Fußplatten, Lastpfad, Zuggreifer. Schon ohne Motor nützlich beim Schleifen.
2. **Beinmotoren:** Unterstützung beim Hochkommen und Vorwärtsschieben.
3. **Schultern mit Gasfedern**, dann Schulter- und Ellbogenmotoren.
4. **Steuerung verfeinern**, Betriebsarten, Akkulaufzeit messen.

Jede Stufe ist für sich testbar und verteilt die Kosten über die Zeit.
