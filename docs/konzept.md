# Konzept: Christbaum-Exo (Entwurf 3.2)

Ziel: ein umsetzbares, robustes Exoskelett für zwei Arbeiten: Bäume am Stamm greifen und **schleifen** (Use Case 2)
und Bäume am Stumpf greifen, fast senkrecht **aufrichten und aufladen** (Use Case 1). Es unterstützt Beine,
Oberkörper und Arme und soll die eigene Kraft etwa verdoppeln. Budget: einige tausend Euro, verteilt über die Zeit.
Keine Spezialteile für zehntausende Euro, sondern Serienteile, die man einzeln kaufen und tauschen kann.

**Rahmenbedingungen (geklärt):** Schleifen bis etwa 100 m. Arbeiten bei Frostregen, nassem Schnee, **Schlamm und unebenem
Gelände**. Akku von vorhandenen Stihl-Geräten. Bauen mit einfacher Werkstatt: bohren, schrauben, ein wenig löten, keine
Drehbank. Das Exo wird für eine Person (1,75 m, 75 kg) gebaut, Längen bleiben trotzdem einstellbar. Die meisten Bäume
schaffst du selbst; das Exo soll **entlasten**, nicht ersetzen.

**Neu in Entwurf 3:** Schlaufengreifer an beiden Unterarmen statt Klemmgabel, Zuggreifer hinten gestrichen,
Sensor-Manschetten als Steuerung, Arme in beide Richtungen angetrieben (Bowdenzug-Paar), Steigbügel mit Spikes statt
langer Fußplatten, Gasfedern gestrichen.

**Neu in Entwurf 3.1:** Schlaufen kommen am Handgelenk nach unten heraus (rechts 2, links 1), der Stamm liegt längs als
Verlängerung des Unterarms. Hüfte mit freiem Abspreizgelenk und Drehlager, damit du die Füße seitlich bewegen kannst.
Beinseil über eine Umlenkscheibe auf der Hüftachse und eine Kurvenscheibe am Knie (das Seil kommt nie mehr ans Gelenk,
mehr Hebel in der Hocke). Schulter mit offenem Seilpaar statt Bowdenzug (weniger Reibung).

Dazu: [steuerung.md](steuerung.md) (wie das Exo merkt, was du willst), [kraftverteilung.md](kraftverteilung.md)
(welche Kräfte wo auftreten), [entwicklungsplan.md](entwicklungsplan.md) (wie man es baut),
[bedienung-und-features.md](bedienung-und-features.md) (Bedienung, Komfort-Features, Fertigung ohne Maschinen).

## 1. Grundidee in einem Satz

**Motoren sitzen hinten im Rückenrahmen, Seile übertragen die Kraft zu Hüfte, Knie, Schultern und Ellbogen, Schlaufen an
den Unterarmen halten die Stämme, und Kraftsensoren in den Manschetten sagen dem Exo, wohin du dich bewegen willst.**

| Entscheidung | Begründung |
|---|---|
| Motoren am Rücken statt in jedem Gelenk | Arme und Beine bleiben leicht, schwere Teile sitzen hinten (gleicht den Baum vorne aus). Motoren sind gut geschützt vor Nässe und Stößen. |
| Beine: offenes Seil, zieht nur | Beine brauchen nur Strecken (aus der Hocke hoch, schieben). Beugen macht der Körper selbst. Seil über Scheiben auf den Gelenkachsen: wenig Reibung, gleichbleibender bzw. in der Hocke wachsender Hebel. |
| Arme: Seilpaare, beide Richtungen | Arme sollen heben **und** senken bzw. beugen **und** strecken. Zwei Seile auf einer Trommel, gegenläufig gewickelt: ein Motor, zwei Richtungen. Schulter mit offenem Seilpaar über Rollen, Ellbogen mit Bowdenzug-Paar (der Weg führt über die Schulter). |
| Sensor-Manschetten | Die blauen Manschetten sind die Schnittstelle: Drückst du gegen eine Manschette, bewegt das Exo das Gelenk in diese Richtung. Intuitiv, keine Knöpfe. |
| Schlaufen am Handgelenk | Eine Schlaufe passt sich jedem Stammdurchmesser an, ist leicht und hält mehrere Bäume getrennt. Der Stamm als Verlängerung des Unterarms liegt wie ein Werkzeugstiel in der Hand. |
| Starrer Rahmen bis in den Steigbügel | Baumgewicht soll über Metall in den Boden, nicht über Knochen und Gelenke. |

## 2. Grundstruktur

```
           Schulterbügel ── Schulter (Seilscheibe, offenes Seilpaar) ── Oberarmschiene ── Ellbogen (Seilscheibe, Bowden-Paar) ── Unterarmschiene
                 │                                                                                                              └─ Schlaufenkopf am Handgelenk (rechts 2, links 1)
  Rückenrahmen (Kraxe-Prinzip, Alu) ── Antriebsbox: 6 Gelenkmotoren, Akku, Steuerung; 3 Schlaufenwinden am Unterarm
                 │
           Hüftgurt + Hüftbügel
                 │
  Abspreizgelenk (frei) ── Hüftgelenk (Lager + Umlenkscheibe) ── Drehlager ── Oberschenkelschiene ── Kniegelenk (Lager + Kurvenscheibe) ── Unterschenkelschiene
                                                                                                    │
                                                                      Sprunggelenk (frei, gummigedämpft) ── Steigbügel mit Spikes
```

- **Bauweise:** komplett **geschraubt**, keine Drehteile und keine tragenden Schweißnähte. Lager in gekauften
  Flanschlagergehäusen, Achsen als Passschrauben M10/M12, Seilrollen fertig mit Lager gekauft.
- **Rückenrahmen:** Alu-Vierkantrohr 25 × 25 × 2 mm, Bauform wie eine Lastenkraxe, Eckverbindungen mit Doppelplatten.
  Als Start kann ein gekauftes Kraxengestell mit Hüftgurt dienen.
- **Einstellbar:** Länge von Ober- und Unterschenkel, Ober- und Unterarm über Lochraster mit Steckbolzen, Hüftbreite über
  Langlöcher.
- **Hüfte:** breiter Hüftgurt mit Alu-Bügel, seitlich je ein Hüftgelenk mit Kugellagern, Achse auf Höhe des eigenen
  Hüftgelenks. **Drei Bewegungen:** vor/zurück (angetrieben), seitlich abspreizen über ein freies Scharnier mit Achse nach
  vorne (Gummianschläge ±20°) und Drehen der Fußspitze über ein Drehlager in der Oberschenkelschiene (±15°). In Entwurf 3
  war das Bein noch in einer Ebene gesperrt; jetzt kannst du die Füße seitlich stellen, auch im Gelände.
- **Beine:** Schienen aus Alu-Rohr (Außen 25 × 25, innen 20 × 20 teleskopierbar), Kniegelenk einachsig mit Anschlag.
- **Schultern:** Bügel vom Rahmen über die Schultern, Schultergelenk außen. Ein freies Gelenk erlaubt das seitliche
  Abspreizen, angetrieben wird Heben und Senken nach vorne.
- **Arme:** Schienen an Ober- und Unterarm, Ellbogen als Scharnier mit Seilscheibe.
- **Manschetten:** gedruckte PETG-Schalen mit Polster und Klettgurt, jede auf einer **Wägezelle** gelagert (siehe Abschnitt 5).

## 3. Füße: Steigbügel für Schlamm und Gelände

Die langen Fußplatten aus Entwurf 2 bleiben in Schlamm stecken, saugen sich fest, haken an Wurzeln und machen
unebenes Gelände gefährlich. **Neu: Steigbügel nach dem Prinzip eines Steigeisens.**

- **Sohlenrahmen** nur so groß wie der Schuh: Alu oder Edelstahl 4 mm, **offene Rahmenform** (große Aussparungen),
  damit Schlamm durchfällt und sich nichts festsaugt.
- **Spikes:** 8 bis 10 kurze, verschraubte Hartmetall-Spikes oder Schuhketten-Zacken, tauschbar. Greifen in Schnee,
  Eis und Schlamm, auf Asphalt abschraubbar.
- **Bindung:** wie bei einem Steigeisen: Zehenkorb aus Kunststoff vorne, Fersenbügel hinten, ein Ratschenriemen.
  Passt zu jedem festen Arbeitsschuh, an- und ausziehen in Sekunden.
- **Sprunggelenk:** frei nach vorne/hinten (Gehen, Hocke), seitlich etwas nachgiebig über Gummipuffer, damit der Fuß
  sich an schräges Gelände anpasst, ohne dass die Schiene das Knie verdreht.
- **Lasteinleitung:** Die Unterschenkelschiene endet in einem U-Bügel seitlich am Schuh, der unter der Schuhmitte in
  den Sohlenrahmen geht. So landet die Last dort, wo du ohnehin stehst.
- **Gleichgewicht ohne lange Platten:** Beim Aufrichten schwerer Bäume ein Ausfallschritt, das ist ohnehin die
  natürliche Technik. Beim Tragen und Schleifen bleibt der Schwerpunkt über dem Fuß (siehe Kraftverteilung).

## 4. Antriebe

**Sechs Gelenkkanäle:**

| Kanal | wirkt auf | Richtung | Übertragung |
|---|---|---|---|
| Bein links / rechts | Hüfte **und** Knie über ein gemeinsames Seil | strecken | offenes Dyneema-Seil um Umlenkscheibe (Hüfte) und Kurvenscheibe (Knie) |
| Schulter links / rechts | Oberarm | **heben und senken** | offenes Seilpaar über Rollen auf Seilscheibe Ø 14 cm |
| Ellbogen links / rechts | Unterarm | **beugen und strecken** | Bowdenzug-Paar auf Seilscheibe Ø 10 cm |

- **Motoren:** bürstenlose Robotergelenk-Motoren mit Planetengetriebe (Quasi-Direct-Drive-Klasse, z. B. aus dem
  Laufroboter-Bereich), Encoder und CAN-Bus eingebaut, 24–48 V, etwa 150 bis 500 € je Stück. Sie lassen sich von
  Hand zurückdrehen: Ohne Strom laufen Arme und Beine frei mit.
- **Seiltrommel** Ø 3 cm auf jedem Motor.
- **Beinseil über Hüfte und Knie:** läuft hinten um eine Umlenkscheibe Ø 19 cm, die genau auf der Hüftachse sitzt, dann
  über zwei Rollen am Oberschenkel und vorne um eine Kurvenscheibe am Knie (fest am Unterschenkel, Radius 5 cm gestreckt bis
  8 cm in der Hocke). Zieht der Motor, strecken sich Hüfte und Knie gleichzeitig.
  - Weil die Scheiben auf den Gelenkachsen sitzen, bleibt der Hebel an der Hüfte in jeder Stellung gleich und das Seil
    kommt nie ans Gelenk (in Entwurf 3 lief es in tiefer Hocke durch das Kniegelenk).
  - Die Kurvenscheibe gibt dort den größten Hebel, wo es am schwersten ist: in der Hocke. Knie-Spitzenmoment je Seite
    steigt so von etwa 90 auf 120 Nm, ohne stärkeren Motor.
  - Nur drei kleine Rollen im ganzen Weg, alle mit Kugellager und mindestens 3 cm Durchmesser: wenig Reibung.
- **Schulter: offenes Seilpaar.** Die Seile laufen von der Box über zwei Rollen, die genau auf der Achse des seitlichen
  Schwenkgelenks sitzen (Schwenken ändert die Seillänge dann nicht), und über zwei Rollen hinter der Seilscheibe. Statt
  etwa 30 % Reibung im Bowdenzug nur etwa 12 %: rund ein Viertel mehr Kraft an der Schulter.
- **Bowdenzug-Paar für die Ellbogen:** Auf der Trommel sind zwei Innenzüge gegenläufig aufgewickelt. Dreht der Motor, wird
  einer aufgewickelt und der andere gleich viel abgewickelt. Am Gelenk umschlingen beide die Seilscheibe von zwei Seiten.
  Eine Spannschraube an der Hülle stellt die Vorspannung ein. Innenzug 2 mm Edelstahl 7 × 7 oder Dyneema in PTFE-Hülle,
  große Biegeradien (mindestens 15 cm), damit die Reibung klein bleibt.
- **Keine Gasfedern mehr:** Die Arm-Motoren arbeiten jetzt in beide Richtungen; das Gewicht von Arm und Exo-Schiene
  gleicht die Software aus. Weniger Teile, nichts drückt gegen das Absenken.

Was die Antriebe leisten und wie viel du noch spürst: [kraftverteilung.md](kraftverteilung.md).

## 5. Steuerung: Sensor-Manschetten

Kurz (Details in [steuerung.md](steuerung.md)):

- Jede blaue Manschette sitzt nicht fest auf der Schiene, sondern auf einer **Wägezelle** (Biegebalken aus Alu, wie in
  Küchenwaagen). Drückst du deinen Arm nach oben, misst die Zelle diese Kraft.
- Die Steuerung dreht das Gelenk in genau diese Richtung, mit einem einstellbaren Verstärkungsfaktor. Das Exo folgt dir,
  du führst.
- Die Baumlast geht über die Schlaufen direkt in die Schienen, **nicht** über die Manschetten. Die Sensoren messen also
  nur deine Absicht, nicht das Gewicht des Baums.
- 8 Sensor-Manschetten: Oberarm, Unterarm, Oberschenkel, Unterschenkel je Seite.
- Bedienung: Ein/Aus-Schalter und Not-Aus am Schultergurt, Daumentaster für die Schlaufen (rechts 2, links 1).

## 6. Greifer: Schlaufen am Handgelenk

**Prinzip Fangschlinge:** Eine Schlaufe aus Dyneema-Seil steckt in einem Gummischlauch mit Federstahl-Einlage. Dadurch
bleibt sie offen wie ein Ring und lässt sich über den Stamm schieben. Ein Ende ist fest, das andere läuft zu einer
kleinen Winde. Verkürzt die Winde das Seil, zieht sich die Schlaufe um den Stamm zu.

- **Schlaufenkopf:** kurzes Alu-Profil an der Unterseite des Handgelenks, über einen Halter fest mit der Unterarmschiene
  verschraubt, mit **Zahnleiste** aus Alu/TPU. Die Schlaufen kommen hier nach unten heraus. Der Stamm wird gegen die Zähne
  gezogen und hält formschlüssig, nicht nur über Reibung.
- **Der Stamm liegt längs als Verlängerung des Unterarms**, das Schnittende direkt am Handgelenk, wie der Stiel eines
  Werkzeugs. Die Hand kann zusätzlich zugreifen und führen.
- **Rechts zwei Schlaufen** (am Handgelenk und etwa 9 cm weiter auf Höhe der Hand), **links eine.** Jede Schlaufe hat eine
  eigene Winde am Unterarm und wird einzeln gesteuert; die Seile laufen innen an der Schiene zum Kopf.
  - Use Case 1: rechts beide Schlaufen um den Stumpf (zwei Schlaufen hintereinander halten den Stamm in Richtung des
    Unterarms). Zum Tragen zeigt der Unterarm steil nach oben wie bei einer Fackel, die Last drückt dann fast längs auf
    Unterarm und Ellbogen. Die linke Hand stützt weiter oben.
  - Use Case 2: bis zu 3 Stümpfe, rechts zwei, links einer. Arme hängen, du gehst vorwärts, die Bäume schleifen hinterher.
- **Winde:** selbsthemmender Schneckengetriebemotor 24 V (Bauform 5840, etwa 40 U/min, 15–25 €) mit Trommel Ø 12 mm:
  etwa 400 N Seilzug, schließt in 3 bis 6 Sekunden, **hält ohne Strom**.
- **Bedienung:** große Daumentaster an der Hand, rechts zwei, links einer (auch mit Handschuh): kurz drücken = zuziehen bis zur eingestellten
  Kraft (Motorstrom), lang drücken = öffnen.
- **Notöffnung:** Das feste Ende jeder Schlaufe hängt an einem Schnappschäkel mit Zuglasche. Ein Ruck an der Lasche
  öffnet die Schlaufe sofort, auch ohne Strom.
- Schlaufenschlauch und Winden sind Verschleißteile, steckbar und schnell tauschbar.

## 7. Energie

- **Akku: Stihl AP 500 S** (36 V, etwa 337 Wh), unten in der Antriebsbox in einem gedruckten Adapter.
- **Laufzeit, grob:** bei durchschnittlich 100 bis 150 W etwa 2 bis 3 Stunden. Schlaufenwinden brauchen fast nichts.
- **Kälte und Nässe:** Akku in der gedämmten Box, wasserdichte Rundstecker, Elektronik im dichten Gehäuse.

## 8. Gewicht

Motoren 6 × 0,5–1 kg, Schlaufenwinden 3 × 0,3 kg, Akku etwa 2 kg, Alu-Rahmen etwa 5 kg, Steigbügel etwa 1 kg,
Rest etwa 2 kg. **Ziel: 15 bis 18 kg.** Ohne lange Fußplatten und Gasfedern etwas leichter als Entwurf 2,
dafür kommen Winden und Wägezellen dazu.

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
| 6 Robotergelenk-Motoren | 900 bis 3 000 € |
| 3 Schlaufenwinden (Schneckengetriebemotor, Treiber) | 80 bis 130 € |
| Steuerung, CAN, 8 Wägezellen + Verstärker, Not-Aus, Kabel | 200 bis 320 € |
| Akku-Adapter für Stihl AP (Akku vorhanden) | 20 bis 80 € |
| Alu-Profile, Lager, Bolzen, Rollen, Seilscheiben, Dyneema, Bowdenzüge | 300 bis 450 € |
| Steigbügel, Spikes, Hüftgurt, Kraxengestell | 120 bis 250 € |
| Filament (PETG, TPU) | 50 bis 100 € |
| **Summe** | **etwa 1 700 bis 4 400 €** |

## 12. Bauen in Stufen

1. **Schlaufengreifer als Handwerkzeug** (an einer Unterarm-Manschette, ohne Exo) und Steigbügel testen.
2. **Rahmen, Beine, Hüfte passiv**, mit Steigbügeln: Lastpfad und Passform prüfen.
3. **Prüfstand:** ein Motor, ein Seilpaar, eine Sensor-Manschette auf einem Holzarm.
4. **Beinmotoren** mit Sensor-Manschetten an Ober- und Unterschenkel.
5. **Arme:** Schulter mit offenem Seilpaar, Ellbogen mit Bowdenzug-Paar, dann Schlaufen ans Exo.
6. **Feinschliff:** Verstärkung, Filter, Akkulaufzeit, eine Saison Praxis.

Jede Stufe ist für sich testbar und verteilt die Kosten über die Zeit.
