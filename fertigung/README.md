# Fertigungsdateien

Erzeugt mit [`tools/fertigung.py`](../tools/fertigung.py) aus Parametern, nicht von Hand gezeichnet. Wer ein Maß
ändert, ändert es im Skript und lässt es neu laufen:

```
pip install ezdxf manifold3d numpy
python3 tools/fertigung.py
```

Das Skript prüft bei jedem Lauf:
- Jede Kontur ist geschlossen.
- Jede Bohrung liegt ganz im Teil, mit mindestens 4 mm Steg zum Rand und zu jeder anderen Bohrung.
- Keine Bohrung ist mit einer Aussparung verschmolzen.
- Jedes Druckteil ist ein geschlossener Körper und passt in den A1 mini (180 × 180 × 180 mm).

Ist etwas nicht in Ordnung, bricht es mit einer Fehlermeldung ab.

| Ordner | Inhalt |
|---|---|
| `laser/*.dxf` | Laserteile, nur Schnittkonturen (Ebene SCHNITT), 1:1 in mm. Direkt beim Laserdienst hochladen. |
| `laser/uebersicht.svg` | alle Laserteile auf einen Blick |
| `druck/*.stl` | Druckteile für den Bambu Lab A1 mini |
| `teileliste.md` | Material, Dicke, Stückzahl, Größe, kleinster Steg, Zweck je Teil |

## Bestellen: Laserteile

1. **Zwei Maße vorher prüfen**, sie hängen vom gekauften Teil ab und stehen oben im Skript:
   - `FLANSCH_LOCHABSTAND`: Lochabstand der Flanschlager (Datenblatt des gekauften Lagers)
   - `MOTOR_LOCHKREIS`, `MOTOR_ZENTRIER`: Befestigung der Motoren (Datenblatt des Motors)
   Erst den Prototyp-Satz bestellen, Teile anhalten, dann den Rest.
2. DXF-Dateien hochladen, Material und Dicke laut `teileliste.md` wählen, Stückzahl eintragen.
3. **Entgraten** bestellen oder selbst mit Entgrater und Feile; das macht viel vom hochwertigen Gefühl aus.
4. Gewinde (Spikes M5 im Edelstahl) selbst schneiden: Kernloch ist schon passend gelasert.

**Seilscheiben ohne Drehbank:** Sie bestehen aus Lagen, die mit M4 verschraubt werden: außen je eine Bordscheibe, innen
die kleinere Rillenlage. Bei zwei Seilen (Schulter, Ellbogen) kommt dazwischen eine Trennlage. Die Rille entsteht aus dem
Durchmesserunterschied von 8 mm. Kanten der Rille vor dem Zusammenbau leicht anfasen.

## Drucken

- PETG, 0,2 mm Schicht, 4 Wände, 30 % Gyroid.
- TPU 95A langsam drucken (etwa 25 mm/s).
- Manschetten liegend auf der Schnittkante drucken, die Schalenwand steht dann senkrecht und ist am stabilsten.
- Haube in zwei Hälften drucken und in der Mitte verschrauben.
- Bohrschablonen auf das Rohr schieben und mit Ständerbohrmaschine und Bohrbuchse bohren.
- Die **Manschetten** passen zunächst auf die Maße einer Person von 1,75 m. Für die eigentliche Passform den Radius
  `r` im Skript nach deinem Maß einstellen: Umfang an der Stelle messen, durch 6,28 teilen, 5 mm Polster dazugeben.
