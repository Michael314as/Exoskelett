# Bedienung, Features und Fertigung (Entwurf 3.2)

Ziel: Das Exo soll sich anfühlen wie ein gutes Werkzeug von einem Profi-Hersteller: Man steigt in Sekunden ein,
nichts klappert, nichts drückt, es reagiert weich und leise, man weiß jederzeit, was es tut. Und es lässt sich trotzdem
**ohne eigene teure Maschinen** bauen: Bohrständer, Säge, Feilen, 3D-Drucker und bestellte Laserteile reichen.

3D: In [`visualisierung/exo-konstruktion.html`](../visualisierung/exo-konstruktion.html) sind die Features als eigene
Baugruppen (14 bis 18) eingezeichnet. Mit „Umgebung“ wechselst du zwischen Werkstatt und verschneiter Plantage in der Dämmerung.

---

## 1. Was „hochwertig“ beim Benutzen ausmacht

| Gefühl | Woher es kommt | Wie wir es erreichen |
|---|---|---|
| **Kein Spiel, kein Klappern** | Spiel in Gelenken, lose Seile | Passschrauben statt Gewinde in Lagern, vorgespannte Seilpaare, Spannschrauben, Gummipuffer an allen Anschlägen |
| **Weich und ruhig** | Ruckartiges Ein- und Ausschalten | Momente werden immer über 0,3 s ein- und ausgeblendet; Dämpfung in der Regelung; Motoren mit Planetengetriebe sind leise |
| **Leicht anzuziehen** | Viele Gurte, Fummelei | 3 Magnet-Schnellverschlüsse + Steigbügel-Ratsche, Parkstütze zum Hineinsteigen (siehe 2.1) |
| **Passt wie maßgeschneidert** | Manschetten drücken | Schalen nach Abguss deines Arms/Beins drucken, Abstandsgewirke-Polster, abnehmbar und waschbar |
| **Man weiß, was es tut** | Kein Feedback | LED-Ring am Schultergurt, Vibration, kurze Töne, Anzeige für Akku und Modus |
| **Sieht nach Produkt aus** | Offene Kabel, rohes Alu, Schraubenwirrwarr | Eloxierte bzw. pulverbeschichtete Teile (als Dienstleistung), Kabel in Gewebeschlauch innen an den Schienen, gedruckte Abdeckungen über Seilscheiben und Box |
| **Vertrauen** | Angst, dass etwas zuschnappt | Sanfter Start, sichere Grenzen, Not-Aus, das Exo „gibt nach“ statt zu drücken, Schlaufen von Hand lösbar |

---

## 2. Neue Features (im Budget)

### 2.1 Parkstütze: in Sekunden ein- und aussteigen
- Zwei klappbare Alu-Stützen hinten am Rahmen (wie beim Fahrrad-Ständer, Rastbolzen). Ausgeklappt steht das Exo allein,
  Beinschienen leicht vorgebeugt.
- **Einsteigen:** rückwärts hineinstellen, Schuhe in die Steigbügel (Ratsche zu), Hüftgurt (Magnetschnalle), zwei
  Schultergurte (Magnetschnallen), Oberschenkel- und Armmanschetten (Klett). Stützen hochklappen. Ziel: **unter 90 Sekunden**.
- **Aussteigen:** Stützen runter, Schnallen auf, rausgehen. Das Exo bleibt stehen, nichts liegt im Schnee.
- Kosten: etwa 40 €.

### 2.2 Licht und Anzeige
- **Arbeitslicht:** zwei LED-Strahler (je 10 W, 12 V) auf den Schulterbügeln, leuchten dorthin, wohin du schaust.
  Christbaum-Saison heißt: ab 16 Uhr dunkel. Doppeltipp auf den Ein/Aus-Taster schaltet das Licht. Etwa 30 €.
- **LED-Ring** (12 farbige LEDs, WS2812) am linken Schultergurt, im Blickfeld:
  grün = bereit, blau = unterstützt, gelb = Grenze nah, rot = Fehler, weiß pulsierend = Nullen läuft.
  Der Ring zeigt außerdem den Akkustand (Anzahl LEDs). Etwa 5 €.
- **Kleines Display** (1,5", E-Paper, im Sonnenlicht und bei Kälte lesbar) auf der Box-Klappe: Akku in %, Restzeit,
  Bäume heute, Fehlertext. Etwa 15 €.
- **Vibration** am Schultergurt: kurz = Schlaufe zu, lang = Schlaufe auf, dreimal = Achtung. Hören musst du nichts. 2 €.

### 2.3 Baumzähler und Waage
- Die Schlaufenwinden und Sensoren wissen, wann ein Baum hängt. Das Exo zählt die Bäume und schätzt aus den Motorströmen
  das Gewicht (±10 %). Nützlich für Verkauf und Planung: „heute 63 Bäume, 1,9 t bewegt“.
- Kostet nur Software.

### 2.4 Handy-App (ohne App-Store)
- Ein kleiner ESP32-Funkchip (BLE) neben dem Teensy. Eine Webseite im Handy-Browser (Web Bluetooth) zeigt:
  Akku, Bäume, Betriebsstunden, und stellt die Verstärkung ein (3 Stufen: sanft, normal, kräftig).
- **Wartungshinweise** aus den Betriebsstunden: „Beinseil links nach 40 h prüfen“.
- Etwa 8 €.

### 2.5 Komfort im Winter
- **Heizgriffe** in den Daumentaster-Griffen (Heizfolien wie bei Motorrädern), drei Stufen. Etwa 25 €.
- **Abstandsgewirke** (3D-Mesh) als Polster: atmet, trocknet schnell, abnehmbar und waschbar. Etwa 25 €.
- **Schutzhauben** aus Cordura über Box und Seilscheiben gegen Schnee und Harz. Etwa 30 €.

### 2.6 Transport
- Das Exo zerfällt mit **3 Schnellkupplungen** (Steckbolzen mit Kugelsperre) in Rücken, linkes und rechtes Bein.
  Passt in jeden Kofferraum, Arme klappen an den Rücken. Etwa 30 €.

### 2.7 Sicherheit, die man spürt
- **Sanftes Aufwachen:** Nach dem Einschalten steigt die Unterstützung über 3 Sekunden an.
- **Sturzerkennung** (Lagesensor): Kippt der Rumpf plötzlich, werden alle Motoren weich und die Unterstützung geht aus.
- **Not-Aus** bleibt rein elektrisch-hart.

---

## 3. Bedienkonzept: so wenig wie möglich

| Was du tust | Was passiert |
|---|---|
| Ein/Aus kurz | Exo an; 2 s stehen bleiben (Ring pulsiert weiß), dann grün |
| Ein/Aus doppelt | Arbeitslicht an/aus |
| Ein/Aus lang | Exo aus (weich, Seile werden locker) |
| Daumentaster kurz | Schlaufe zu (Vibration kurz) |
| Daumentaster lang | Schlaufe auf (Vibration lang) |
| Beide Daumentaster rechts 2 s | Unterstützungsstufe wechseln (Ring zeigt 1–3 LEDs) |
| Bewegen | Exo hilft, über die Sensor-Manschetten |

Mehr Knöpfe gibt es nicht. Alles andere stellt man selten in der App ein.

---

## 4. Fertigung ohne teure Maschinen

| Teil | Wie | Werkzeug/Dienst |
|---|---|---|
| Alu-Rohre, Schienen | ablängen, bohren nach gedruckter **Bohrschablone** | Kapp- oder Handsäge, Bohrständer |
| Platten, Seilscheiben, Kurvenscheiben, Steigbügel, Laschen | **online lasern lassen** (DXF hochladen, Alu 4–10 mm oder Edelstahl), Stückpreis wenige Euro | Laser-Dienstleister |
| Gelenkachsen | Passschrauben M10/M12 kaufen, keine Drehteile | – |
| Lager, Rollen | fertig kaufen (Flanschlager, Seilrollen mit Kugellager) | – |
| Seiltrommeln | Alu-Rohr + gelaserte Bordscheiben verschraubt, oder Trommel vom Motorhersteller | Bohrständer |
| Manschetten, Gehäuse, Abdeckungen, Schlaufenkopf-Hülle, Bohrschablonen | 3D-Druck PETG/TPU | Bambu Lab A1 mini |
| Passgenaue Schalen | Abguss mit Gipsbinde → Handy-3D-Scan (Foto-App) → Schale in CAD anpassen → drucken | Handy, A1 mini |
| Oberfläche | **eloxieren** (Alu, 80–150 € als Sammelauftrag) oder **pulverbeschichten** (60–120 €) beim Betrieb um die Ecke | Dienstleister |
| Elektronik | fertige Module, Stecker crimpen, Platine als Lochraster oder bestellt (JLC-Klasse, wenige Euro) | Lötkolben, Crimpzange |
| Polster, Gurte | Abstandsgewirke zuschneiden, Gurtband + Magnetschnallen nähen lassen oder mit Nieten | Schere, Nietzange |

**Kein Schweißen, keine Drehbank, keine Fräse.** Was präzise sein muss (Bohrungen für Lager, Scheibenprofile), kommt
vom Laser oder wird über gedruckte Schablonen gebohrt.

---

## 5. Kosten der Features

| Feature | etwa |
|---|---|
| Parkstütze | 40 € |
| Arbeitslicht 2 × 10 W | 30 € |
| LED-Ring, Vibration, E-Paper-Display | 22 € |
| BLE-Funkchip (Handy-App) | 8 € |
| Heizgriffe | 25 € |
| Abstandsgewirke-Polster, Magnetschnallen | 60 € |
| Schutzhauben Cordura | 30 € |
| Schnellkupplungen für Transport | 30 € |
| Eloxieren oder Pulverbeschichten | 80–150 € |
| **Summe** | **etwa 330 bis 400 €** |

Gesamt mit Entwurf 3.1 (etwa 1.700 bis 4.400 €) also etwa **2.000 bis 4.800 €**. Wer sparen will, lässt Eloxieren,
Display und Heizgriffe weg (spart etwa 200 €), ohne an der Funktion zu verlieren.
