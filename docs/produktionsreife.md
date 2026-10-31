# Produktionsreife: Stand und Weg dorthin

„Produktionsreif“ heißt hier: Ein zweites und drittes Exo lassen sich allein aus diesem Repository bauen, flashen,
kalibrieren und abnehmen, und das erste hat eine Saison im Einsatz bestanden. Für einen **Verkauf** an andere kommt
noch die CE-Konformität dazu (siehe unten).

## Was fertig und geprüft ist

| Bereich | Stand | Nachweis |
|---|---|---|
| Konzept, Kräfte, Steuerungskonzept | fertig | [konzept.md](konzept.md), [kraftverteilung.md](kraftverteilung.md), [steuerung.md](steuerung.md) |
| Steuerung (Regler, Sicherheit, Bedienung) | fertig | Physik-Simulation, 50+ automatische Prüfungen (`make -C firmware test`) |
| Firmware Teensy 4.1 | baut | ARM-GCC, `exo.hex` (86 KB), keine Warnung im eigenen Code (`firmware/tools/build_teensy.sh`) |
| Firmware ESP32-C3 (Display, Handy) | geschrieben | Statuszeilen-Auswertung getestet; der ESP32-Build selbst ist noch nicht gelaufen |
| Laserteile | 18 DXF | automatische Prüfung: geschlossene Konturen, Stege ≥ 4 mm je Bohrung ([fertigung/](../fertigung/)) |
| Druckteile | 12 STL | wasserdicht, passen in den A1 mini |
| Stückliste, Verdrahtung | fertig | [stueckliste.md](stueckliste.md), [verdrahtung.md](verdrahtung.md) (Pinplan = Firmware) |
| Montage, Kalibrierung | fertig | [montage.md](montage.md), Werkstatt-Konsole in der Firmware (`null`, `kal`, `speichern`) |
| Abnahme | Protokoll fertig | [pruefung.md](pruefung.md): 35 Punkte mit messbaren Kriterien |
| Prüfstand-Modus, Messdaten-Log, Parameterbestimmung | fertig | `tools/auswertung.py` findet bekannte Werte aus einem Simulations-Log auf 20 % genau (`make -C firmware ident`) |
| Risikobeurteilung | fertig | [risikobeurteilung.md](risikobeurteilung.md) |
| Wartung | fertig | [wartung.md](wartung.md), Hinweise aus den Betriebsstunden in der Firmware |

Bei der Arbeit an diesen Punkten gefundene und behobene Fehler: zu viel Dämpfung, zu großes Totband,
Stufenwechsel hätte Schlaufen geöffnet, fehlendes Include für den ARM-Build, doppelt belegte Pins, Motoren-Fehler
beim Einschalten, blockierendes E-Paper im Regeltakt, zu dünne Stege an fünf Laserteilen.

## Was nur mit Hardware geht (Reihenfolge)

| Schritt | Ergebnis | Dauer |
|---|---|---|
| 1. Kaufteile bestellen; **Lochbild Flanschlager und Motor mit Datenblatt abgleichen**, Parameter in `tools/fertigung.py` anpassen | richtige DXF | 1 Woche |
| 2. Prototyp-Satz Laserteile bestellen, drucken, montieren ([montage.md](montage.md)) | Exo Nr. 1 | 4–8 Wochen |
| 3. Abnahme P1–P20 am Prüfstand | gemessene Werte | 1–2 Wochen |
| 4. Prüfstand-Log aufnehmen (`pruefstand`, `log 1`), mit `tools/auswertung.py` Trägheit, Reibung, Schwerkraft bestimmen, in Simulation und Firmware eintragen, Verstärkung neu einstellen | Firmware v1.0 | 1 Woche |
| 5. Abnahme P21–P30 am Körper | Exo freigegeben | 1–2 Wochen |
| 6. Eine Saison Einsatz, Mängelliste führen | Änderungsliste | 1 Saison |
| 7. Änderungen einarbeiten, Zeichnungen einfrieren (Git-Tag „v1.0“), Exo Nr. 2 nur nach Repository bauen | **produktionsreif** | 4–8 Wochen |

Erst wenn Exo Nr. 2 ohne Rückfragen nach diesem Repository gebaut und abgenommen ist, ist die Dokumentation vollständig.
Das ist der eigentliche Test der Produktionsreife.

## Falls verkauft werden soll

- Konformitätsbewertung nach Maschinenverordnung (EU) 2023/1230, gilt ab 20. Januar 2027; bis dahin Maschinenrichtlinie 2006/42/EG.
- Normen: DIN EN ISO 12100 (Risikobeurteilung), DIN EN ISO 13482 (Assistenzroboter, Exoskelette),
  DIN EN ISO 13849-1 (Sicherheitsfunktionen: Not-Aus, Watchdog brauchen ein Performance Level), EMV (EN 61000-6-x).
- Technische Dokumentation, Betriebsanleitung, CE-Kennzeichnung, Produkthaftpflicht.
- Erfahrungsgemäß lohnt sich dafür eine Prüfstelle (TÜV, DGUV Test) schon in der Entwurfsphase.
