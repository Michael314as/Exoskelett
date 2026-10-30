# Konzept: Christbaum-Exo (Entwurf 2)

Ziel: ein umsetzbares, robustes Exoskelett für zwei Arbeiten: Bäume am Stamm greifen und **schleifen** (Use Case 2)
und Bäume am Stumpf greifen, fast senkrecht **aufrichten und aufladen** (Use Case 1). Es unterstützt Beine,
Oberkörper und Arme und soll die eigene Kraft etwa verdoppeln. Budget: einige tausend Euro, verteilt über die Zeit.
Keine Spezialteile für zehntausende Euro, sondern Serienteile, die man einzeln kaufen und tauschen kann.

**Rahmenbedingungen (geklärt):** Schleifen bis etwa 100 m. Arbeiten bei Frostregen und nassem Schnee. Akku von vorhandenen
Stihl-Geräten. Bauen mit einfacher Werkstatt: bohren, schrauben, ein wenig schweißen und löten, keine Drehbank.
Bedienung nur Ein/Aus. Das Exo wird für eine Person (1,75 m, 75 kg) gebaut, Längen bleiben trotzdem einstellbar.

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

- **Bauweise:** komplett **geschraubt**, keine Drehteile und keine tragenden Schweißnähte. Lager sitzen in gekauften
  Flanschlagergehäusen, Achsen sind Passschrauben M10/M12, Seilrollen werden fertig mit Lager gekauft.
- **Rückenrahmen:** Aluminium-Vierkantrohr 25 × 25 × 2 mm, Bauform wie eine Lastenkraxe. Er nimmt die Antriebsbox auf und
  verbindet Schultern und Hüfte. Als Start kann ein gekauftes Kraxengestell mit Hüftgurt dienen.
- **Einstellbar (kostet fast nichts):** Länge von Ober- und Unterschenkel, Ober- und Unterarm über Lochraster, Hüftbreite
  über Langlöcher. Alles andere wird fest auf dich angepasst.
- **Hüfte:** breiter Hüftgurt mit Alu-Bügel. Seitlich je ein Hüftgelenk mit Kugellagern (z. B. 6202), Achse genau auf Höhe
  des eigenen Hüftgelenks, seitlich einstellbar.
- **Beine:** Schienen aus Alu-Flachprofil bzw. Rohr, Länge über Lochraster einstellbar (Steckbolzen), Kniegelenk als
  einachsiges Scharnier mit Lagern und mechanischem Anschlag gegen Überstrecken.
- **Füße:** Alu-Platte unter dem Schuh, festgehalten mit einer **Snowboard-Bindung** (günstig, schnell an- und ausziehbar,
  für Winter gemacht). Die Platte ragt etwa 18 cm nach vorne und 8 cm nach hinten über den Schuh hinaus. Vorne hilft das beim Aufladen
  (Baum vor dem Körper), hinten beim Schleifen (Last hinter dem Körper).
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

Grundsatz: **Greifen mechanisch, ohne Motor**, unter Last selbstklemmend. Fällt der Strom aus, fällt nichts herunter.

**Hand (Use Case 1), drei Möglichkeiten verglichen:**

| Ansatz | Vorteil | Nachteil |
|---|---|---|
| Schlaufe, die sich zuzieht (deine Idee) | sehr einfach, leicht, billig | Stamm muss durch die Schlaufe, am Boden liegend umständlich; lose Schlaufe kann sich verhaken |
| Selbstklemmende Zange (Rückezange) | hält sehr fest | schwer, muss man in die Hand nehmen, zwei Hände zum Ansetzen |
| **Klemmgabel mit Schnappbügel** an der Unterarmschiene | Gabel einfach von oben auf den Stamm drücken, Bügel schnappt zu; unter Zug keilt sich der Stamm in die V-Form; Lösen mit Daumenhebel | etwas mehr Bauaufwand |

**Entscheidung: Klemmgabel mit Schnappbügel an der rechten Unterarmschiene**, direkt hinter der Hand, V-Form mit
TPU-Belag und kleinen Zähnen, passt für etwa 5 bis 18 cm. Die Kraft geht über die Unterarmschiene ins Exo, nicht über die
Finger. Die linke Hand greift weiter oben ganz normal mit gutem Handschuh. Eine Würgeschlaufe als Ergänzung für sehr
glatte oder dünne Stämme ist einfach nachrüstbar.

**Zuggreifer hinten an der Hüfte (Use Case 2):** Querträger am Hüftbügel auf etwa 55 bis 60 cm Höhe, daran bis zu vier
Klemmgabeln mit Schnappbügel nach dem gleichen Prinzip. Man geht leicht in die Knie, drückt sie rückwärts auf die Stümpfe,
richtet sich auf: Die Stümpfe heben sich an und keilen sich fest. Lösen über einen gemeinsamen Zug am Hüftgurt.
Die Kraft geht über Hüftbügel und Beinschienen in die Fußplatten, die Beinmotoren helfen beim Vorwärtsschieben.

## 5. Steuerung und Bedienung

- **Rechner:** ein Mikrocontroller mit CAN-Bus (z. B. ESP32 oder Teensy), spricht alle Motoren direkt an.
- **Sensoren:** Lagesensoren (IMU) an Rücken, Oberschenkeln und Oberarmen, dazu die Encoder und der Motorstrom.
  So erkennt das Exo, ob man in die Hocke geht, sich aufrichtet oder vorwärts drückt.
- **Unterstützen statt fernsteuern:** Das Exo verstärkt die Bewegung, die man selbst macht. Bewegt man sich nicht, hält es nur.
- **Bedienung: nur Ein/Aus.** Ein großer Schalter am linken Schultergurt. Im Ein-Zustand erkennt das Exo selbst aus den
  Sensoren, was du tust (in die Hocke gehen, aufrichten, vorwärts schieben, Arm heben), und unterstützt genau diese Bewegung.
  Aus heißt: alle Seile locker, das Exo läuft frei mit.
- **Not-Aus:** großer Pilzschalter am Schultergurt, trennt die Motoren hart vom Akku. Seile werden dann locker.

## 6. Energie

- **Akku: Stihl AP-Akku** (36 V Akkusystem), den du schon hast. Er sitzt unten in der Antriebsbox in einer Halterung mit
  Kontakten (3D-gedruckter Adapter, solche Adapter gibt es als Bauvorlage und fertig zu kaufen). Die Motoren laufen in
  diesem Spannungsbereich. Vorher prüfen wir am Tisch, ob der Akku die nötigen Ströme ohne Abschalten liefert.
- **Laufzeit, grob:** Vorhanden ist ein **Stihl AP 500 S** mit etwa 337 Wh. Bei durchschnittlich 100 bis 150 W
  Unterstützung reicht das für etwa 2 bis 3 Stunden. Ein zweiter Akku zum Wechseln deckt einen Arbeitstag ab.
- **Kälte und Nässe:** Akku in der gedämmten Box, wasserdichte Rundstecker, Elektronik vergossen bzw. im dichten Gehäuse.

## 7. Gewicht und Gleichgewicht

- **Gewichtsabwägung:** Motoren 6 × 0,5 bis 1 kg, Stihl-Akku etwa 2 kg, Alu-Rahmen etwa 5 kg, Fußplatten mit Bindung
  etwa 2 kg, Rest etwa 2 kg. **Ziel: 15 bis 18 kg.** Mehr Gewicht lohnt sich nur, wo es tragend ist (Hüfte, Beine).
  Die Last geht über die Beinschienen in den Boden, darum spürt man das Gewicht des Exos kaum.
- Ein großer Teil (Akku, Motoren) sitzt hinten und gleicht den Baum vorne aus.
- Verlängerte Fußplatten vorne (15 bis 20 cm) vergrößern die Standfläche, das wirkt direkt gegen das Vornüberkippen.

## 8. Wetterfestigkeit

Abgedichtete Lager (2RS), Seilrollen mit Abdeckung gegen Vereisen, Dyneema nimmt kein Wasser auf, Alu und
Edelstahlschrauben rosten nicht, keine offene Elektronik an Armen und Beinen.

## 9. Sicherheit

Seile ziehen nur, Greifer klemmen mechanisch, Gelenke haben feste Endanschläge in Körpergrenzen, Drehmomente sind in der
Software begrenzt, Not-Aus trennt hart. Erste Tests auf einem Prüfstand und mit Leergewicht, erst dann mit Bäumen.
**Nicht mit Motorsäge in der Hand im unterstützten Modus arbeiten.**

## 10. Kosten, grob

| Posten | etwa |
|---|---|
| 6 Robotergelenk-Motoren | 900 bis 3 000 € (je nach Typ) |
| Steuerung, CAN, Sensoren, Not-Aus, Kabel | 150 bis 250 € |
| Akku-Adapter für Stihl AP (Akku vorhanden) | 20 bis 80 € |
| Alu-Profile, Lager, Bolzen, Rollen, Dyneema-Seil | 250 bis 400 € |
| Snowboard-Bindungen, Hüftgurt, Kraxengestell, Gasfedern | 150 bis 300 € |
| Klemmgabeln (Alu, Federn, TPU) | 50 bis 120 € |
| Filament (PETG, TPU) | 50 bis 100 € |
| **Summe** | **etwa 1 600 bis 4 300 €** |

## 11. Bauen in Stufen

1. **Beine + Rücken + Hüfte, passiv:** Rahmen, Fußplatten, Lastpfad, Zuggreifer. Schon ohne Motor nützlich beim Schleifen.
2. **Beinmotoren:** Unterstützung beim Hochkommen und Vorwärtsschieben.
3. **Schultern mit Gasfedern**, dann Schulter- und Ellbogenmotoren.
4. **Steuerung verfeinern**: Bewegungserkennung, Unterstützungsstärke, Akkulaufzeit messen.

Jede Stufe ist für sich testbar und verteilt die Kosten über die Zeit.
