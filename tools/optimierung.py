#!/usr/bin/env python3
"""Optimierung Entwurf 5 -> Entwurf 6: Kraft, Gewicht, Herstellkosten in einzelnen Schritten.

Aufruf:  python3 tools/optimierung.py            (alle Schritte, Markdown)
         python3 tools/optimierung.py --schritt 1 (nur bis Schritt 1)

Ausgangswert ist Entwurf 5 (Stückliste der technischen Zeichnung, 47,2 kg mit einem Akku; Preise aus
docs/stueckliste.md). Jede Maßnahme ändert Teile in der Stückliste und Kostenzeilen; danach rechnet das
Skript Masse, Kosten und Kraft neu. Die Kraft wird an echten Arbeitsschritten gemessen: der schwerste Baum,
den man so lange halten bzw. aufrichten kann, begrenzt durch das schwächste Gelenk (Mensch + Exo - was das
Exo für sich selbst braucht). Herstellerdaten der Motoren: RobStride-Handbücher RS04/RS03 (Stillstand- und
Drehend-Tabellen, siehe docs/realismus-check.md). Alles Schätzungen mit dem Ziel, eher zu tief als zu hoch zu
liegen; was am Prüfstand gemessen werden muss, steht in docs/optimierung.md.
"""
import sys
from math import exp, log, sqrt

# ---------------------------------------------------------------- Stückliste Entwurf 5
# (ID, Benennung, Stück, kg je Stück) aus visualisierung/exo-technik.html, Reiter "Stückliste" (getragen, ein Akku)
BOM_E5 = [
    ('RA-01', 'Rahmenrohr senkrecht', 2, 0.360),
    ('RA-02', 'Querrohr', 1, 0.090),
    ('RA-03', 'Eck-Doppelplatte, Lochleiste', 12, 0.040),
    ('RA-04', 'Schutzbügel', 2, 0.350),
    ('RA-05', 'Gummipuffer', 4, 0.030),
    ('RA-08', 'Hauben', 1, 0.100),
    ('RA-09', 'Lendengelenk mit Sperre', 2, 0.450),
    ('RA-10', 'Gasfeder Lendengelenk', 2, 0.120),
    ('RA-11', 'Kurbelschutz', 4, 0.030),
    ('RA-12', 'Rumpfdrehung', 3, 0.320),
    ('RA-13', 'Rumpfantrieb: Kurbeln, Hebel, Stangen', 1, 0.450),
    ('GU-01', 'Hüftgurt', 1, 0.450),
    ('GU-02', 'Schultergurte und Polster', 3, 0.170),
    ('GU-03', 'Riemen und Schnallen', 3, 0.120),
    ('EN-01', 'Akku', 1, 2.000),
    ('EN-02', 'Akkuschacht', 3, 0.250),
    ('EL-01', 'Hauptplatine', 1, 0.080),
    ('EL-02', 'Spannungswandler', 1, 0.250),
    ('EL-03', 'Windentreiber', 3, 0.030),
    ('EL-04', 'Schütz, Hauptsicherung', 1, 0.200),
    ('EL-06', 'Ideal-Dioden-Modul', 1, 0.030),
    ('EL-07', 'Lagesensor', 1, 0.005),
    ('EL-08', 'Stromschiene mit Sicherungen', 1, 0.120),
    ('EL-09', 'Leistungsleitungen', 1, 0.200),
    ('EL-10', 'CAN-Bus', 1, 0.050),
    ('EL-11', 'Rundstecker', 7, 0.030),
    ('KB-01', 'Kabelbaum Beinkapseln', 2, 0.200),
    ('KB-02', 'Kabelbaum Bein', 2, 0.100),
    ('KB-03', 'Kabelbaum Arm', 2, 0.150),
    ('KB-04', 'Kabel Licht, Anzeige, Not-Aus', 1, 0.050),
    ('KB-05', 'Kabelbaum Armkapseln', 2, 0.150),
    ('KB-06', 'Kabel Rumpfkapseln', 1, 0.060),
    ('KB-07', 'Kabelkanäle', 2, 0.070),
    ('AN-01', 'Elektronikbox', 1, 0.450),
    ('AN-02', 'Kapselmantel', 8, 0.050),
    ('AN-03', 'Dichtung und Heizung', 8, 0.030),
    ('AN-04', 'Motorträger', 8, 0.070),
    ('AN-05', 'Stiftkühlkörper', 8, 0.280),
    ('AN-06', 'Kapselteile Rumpf', 1, 0.750),
    ('AN-D', 'Rumpfkapsel Drehen', 1, 0.880),
    ('AN-E', 'Ellbogenkapsel-Motor', 2, 0.880),
    ('AN-H', 'Hüftkapsel-Motor', 2, 1.420),
    ('AN-K', 'Kniekapsel-Motor', 2, 1.420),
    ('AN-L', 'Rumpfkapsel Beugen', 1, 1.420),
    ('AN-S', 'Schulterkapsel-Motor', 2, 1.420),
    ('HU-01', 'Abspreizgabel', 2, 0.250),
    ('HU-02', 'Achsbolzen', 2, 0.100),
    ('HU-03', 'Zentrierfeder', 2, 0.010),
    ('HU-04', 'Nabe', 2, 0.200),
    ('HU-05', 'Hüftschwinge', 2, 0.550),
    ('HU-06', 'Hüftlager', 2, 0.200),
    ('HU-07', 'Oberschenkelplatte, Abtriebsflansch', 2, 0.100),
    ('BA-01', 'Kurbel Knie', 2, 0.060),
    ('BA-02', 'Schubstange Knie', 2, 0.150),
    ('BA-03', 'Gelenkköpfe Knie', 4, 0.030),
    ('BA-04', 'Kniehebel', 2, 0.060),
    ('BA-05', 'Federpaket', 2, 0.040),
    ('BE-01', 'Oberschenkelschiene', 2, 0.450),
    ('BE-02', 'Steckbolzen Lochraster', 8, 0.020),
    ('BE-03', 'Knieplatten', 2, 0.060),
    ('BE-04', 'Kniegelenklager', 2, 0.120),
    ('BE-05', 'Knieanschlag', 2, 0.010),
    ('BE-06', 'Unterschenkelschiene', 2, 0.400),
    ('BE-08', 'Knöchellager', 2, 0.070),
    ('BE-09', 'Fersenbügel', 2, 0.150),
    ('BE-10', 'Kippgelenk', 2, 0.030),
    ('BE-11', 'Steigbügel', 2, 0.250),
    ('BE-12', 'Spikes', 12, 0.005),
    ('BE-13', 'Fersenschale, Ristriemen', 2, 0.100),
    ('BE-14', 'Drehteller Steigbügel', 2, 0.080),
    ('SE-01', 'Manschette M3', 8, 0.130),
    ('SE-02', 'Wägezelle Manschette', 8, 0.030),
    ('SE-03', 'Messwandler Manschetten', 3, 0.005),
    ('SE-04', 'Überlastpuffer, Klemmhalter', 8, 0.020),
    ('SE-06', 'Fußkontakt', 2, 0.150),
    ('SE-07', 'Wägezelle Schlaufenkopf', 3, 0.040),
    ('SE-08', 'Drehring Unterarm-Manschette', 2, 0.030),
    ('SE-09', 'Rumpf-Wägezellen', 4, 0.040),
    ('SE-10', 'BOA-Drehverschluss', 8, 0.030),
    ('SE-11', 'Längsschlitten Manschette', 8, 0.050),
    ('SE-12', 'Sensorstecker und -leitung', 8, 0.010),
    ('SE-13', 'Akkustrom und -spannung', 1, 0.010),
    ('SE-14', 'Außentemperatur', 1, 0.003),
    ('SC-01', 'Schulterbügel', 2, 0.300),
    ('SC-02', 'Schultergabel', 2, 0.120),
    ('SC-03', 'Achse Schulter', 2, 0.080),
    ('SC-06', 'Schulternabe', 2, 0.120),
    ('SC-07', 'Armfeder', 2, 0.030),
    ('SC-08', 'Schulterschwinge', 2, 0.350),
    ('SC-09', 'Schulterlager, Abtriebsflansch', 2, 0.140),
    ('SC-12', 'Gleitführung Schulterblatt', 2, 0.100),
    ('AR-01', 'Oberarmschiene', 2, 0.200),
    ('AR-02', 'Unterarmschiene', 2, 0.180),
    ('AR-03', 'Ellbogenlager, Anschlag', 2, 0.080),
    ('AR-04', 'Kurbel Ellbogen', 2, 0.040),
    ('AR-06', 'Schubstange Ellbogen', 2, 0.080),
    ('AR-07', 'Gelenkköpfe Ellbogen', 4, 0.020),
    ('AR-08', 'Ellbogenhebel', 2, 0.040),
    ('SG-01', 'Schlaufenträger', 2, 0.120),
    ('SG-02', 'Schlaufenkopf', 3, 0.080),
    ('SG-03', 'Schnappschäkel', 3, 0.030),
    ('SG-04', 'Spannband', 3, 0.060),
    ('SG-05', 'Bandkanal und Speicher', 3, 0.050),
    ('SG-06', 'Bandantrieb', 3, 0.250),
    ('SG-07', 'Bedienteil Hand', 2, 0.050),
    ('BD-01', 'Not-Aus mit LED-Ring', 1, 0.090),
    ('BD-02', 'Ein/Aus-Taster', 1, 0.020),
    ('BD-03', 'Anzeige Oberarm', 1, 0.060),
    ('BD-04', 'Arbeitslicht', 2, 0.120),
    ('AS-01', 'Abtriebsadapter', 10, 0.020),
    ('AS-02', 'Abtriebsschrauben', 10, 0.027),
    ('AS-03', 'Gehäuseschrauben', 10, 0.020),
    ('AS-04', 'Passstifte Abtrieb', 10, 0.002),
    ('AS-05', 'Stecker auf der Treiberplatine', 10, 0.000),
    ('AS-06', 'Motorleitung', 10, 0.015),
    ('AS-07', 'Trennstelle Plus/Minus', 10, 0.004),
    ('AS-08', 'Trennstelle CAN', 10, 0.001),
    ('AS-09', 'Anschlussdom', 10, 0.012),
    ('AS-10', 'Kabelverschraubung', 10, 0.006),
    ('VK-01', 'Schutzgitter Kühlkörper', 10, 0.035),
    ('VK-02', 'Abdeckung Knieantrieb', 2, 0.110),
    ('VK-03', 'Abdeckung Ellbogenantrieb', 2, 0.060),
    ('VK-04', 'Rückenschale', 1, 0.240),
    ('VK-05', 'Deckel Kabelkanal', 2, 0.010),
    ('VK-06', 'Gelenkkappe', 4, 0.040),
    ('VK-07', 'Regenklappe Akkuschacht', 2, 0.040),
    ('VK-08', 'Schutzmanschette Gelenkkopf', 8, 0.005),
    ('VK-09', 'Vierteldrehverschluss', 8, 0.004),
]

# ---------------------------------------------------------------- Kosten Entwurf 5
# Mittelwerte der Spannen aus docs/stueckliste.md (Stand Nov 2026), Nachtrag AS/VK/KB-05/07 geschätzt.
# (Schlüssel, Benennung, Euro)
COST_E5 = [
    # 1 Rahmen, Hüfte, Beine
    ('rahmen', 'Rahmenrohre, Eckplatten', 46), ('gurte', 'Gurtsystem', 105), ('abspreiz', 'Abspreizgelenk, Federn', 75),
    ('huefte', 'Hüftschwinge, Hüftlager', 45), ('schienen_bein', 'Bein-Teleskopschienen', 25),
    ('lagerplatten', 'Lagerplatten Flanschlager', 35), ('knielager', 'Kniegelenk-Lager', 14), ('passschr', 'Passschrauben, Muttern', 25),
    ('steckbolzen', 'Steckbolzen', 15), ('fuss', 'Steigbügel, Spikes, Fersenschale, Knöchellager, Fersenbügel', 109),
    # 2 Antrieb
    ('rs04', 'RobStride RS04 × 7', 2075), ('rs03', 'RobStride RS03 × 3', 530), ('kuehl', 'Stiftkühlkörper × 8', 230),
    ('pads', 'Wärmeleitpads × 8', 25), ('wangen', 'Wangen × 12', 40), ('abtriebsflansch', 'Abtriebsflansch × 4', 20),
    ('maentel', 'Kapselmäntel × 8', 15), ('dicht', 'V-/O-Ringe, Verschraubungen × 8', 40), ('druckausgl', 'Druckausgleich × 8', 50),
    ('heiz', 'Heizbänder × 8', 50), ('kurbeln', 'Kurbeln, Hebel Knie/Ellbogen', 30), ('stangen', 'Schubstangen, Passschrauben', 35),
    ('gelenkkoepfe', 'Gelenkköpfe × 8', 70), ('federpaket', 'Federpaket Knie', 15), ('rumpfantrieb', 'Rumpfantrieb', 60),
    ('rumpfzellen', 'Rumpf-Wägezellen', 20), ('bremswid', 'Bremswiderstand', 30),
    # 3 Arme, Manschetten, Greifer
    ('armrohre', 'Schulterbügel, Armschienen', 27), ('armlager', 'Schulter-/Ellbogenlager', 28), ('schalen', 'Manschetten-Schalen × 8', 30),
    ('polster', 'Polster × 8', 40), ('boa', 'BOA-Verschlüsse × 8', 160), ('schlitten', 'Längsschlitten × 8', 50),
    ('zellen20', 'Wägezellen 20 kg × 8', 40), ('zellhalter', 'Wägezellen-Halter', 5), ('schlaufen', 'Schlaufenköpfe, Schlaufen, Schäkel', 70),
    ('winden', 'Schlaufenantrieb × 3', 75), ('griffe', 'Daumentaster-Griffe', 40),
    # 4 Elektronik
    ('teensy', 'Teensy 4.1 + CAN', 55), ('messplatine', 'Manschetten-Messplatine (3 × ADS131M04)', 60), ('esp', 'ESP32-C3', 8),
    ('imu', 'BNO085', 30), ('zellen50', 'Wägezellen Schlaufenkopf + HX711', 20), ('fusszellen', 'Fußkontakt-Wägezellen + HX711', 20),
    ('uhr', 'CR2032', 3), ('dcdc', 'DC/DC', 30), ('windentreiber', 'Pololu G2 × 3', 125), ('schuetz', 'Sicherung, Schütz 100 A, Not-Aus', 90),
    ('akkuadapter', 'Stihl-Akku-Adapter', 50), ('stecker', 'Stecker IP67, Kabel', 60), ('platine', 'Platine', 10), ('led', 'LED-Ring, Summer', 15),
    ('anzeige', 'Anzeige Oberarm', 20), ('licht', 'LED-Strahler × 2', 30),
    # 5 Komfort und Finish
    ('parkstuetze', 'Parkstütze', 40), ('schutzbuegel', 'Schutzbügel CFK', 45), ('schnallen', 'Magnetschnallen', 40),
    ('kugelsperr', 'Kugelsperrbolzen Transport', 30), ('haube', 'Haube Box', 10), ('cordura', 'Schutzhauben Cordura', 30),
    ('oberflaeche', 'Eloxieren / Pulverbeschichten', 105), ('lende', 'Lendengelenk mit Sperre', 75), ('rumpfdreh', 'Rumpfdrehung', 70),
    ('gasfedern', 'Gasfedern Lendengelenk', 30), ('kurbelschutz', 'Kurbelschutz', 5), ('drehteller', 'Drehteller Steigbügel', 25),
    ('drehring', 'Drehring Unterarm', 30), ('schulterschlitten', 'Schulterschlitten', 25),
    # Nachtrag 14. Nov 2026 (Schätzung): Motoranschluss je Motor, Kanäle, Verkleidung
    ('anschluss', 'Abtriebsadapter, Schrauben, Stifte, Stecker, Verschraubung × 10 Motoren', 175),
    ('kanaele', 'Kabelbaum Armkapseln, Kabelkanäle', 40), ('verkleidung', 'Verkleidung VK-01 … VK-09', 90),
]

# ---------------------------------------------------------------- Motoren (Herstellerdaten)
STALL = {   # (Moment Nm, Sekunden) im Stillstand, große Kühlplatte, 25 °C
    'RS04': [(120, 1), (110, 1), (100, 1.5), (90, 3), (80, 6), (70, 10), (60, 33), (50, 74), (40, 130)],
    'RS03': [(60, 1), (50, 4), (40, 9), (30, 25), (20, 190)],
}
ROTATE = {  # drehend
    'RS04': [(120, 3), (115, 9), (110, 10), (105, 13), (100, 15), (90, 24), (80, 38), (70, 61), (60, 108), (50, 324)],
    'RS03': [(60, 13), (40, 67), (30, 393)],
}
CONT = {'RS04': 20, 'RS03': 11}     # Dauer im Stillstand in der Kühlkörper-Kapsel (realismus-check.md)
LIMIT = {'S': 100, 'E': 56, 'H': 110, 'K': 110}   # Spitze in der Firmware


def _hold(table, cont, T):
    if T <= cont:
        return float('inf')
    if T >= table[0][0]:
        return table[0][1]
    for (T1, t1), (T2, t2) in zip(table, table[1:]):
        if T2 <= T <= T1:
            return exp(log(t1) + (T1 - T) / (T1 - T2) * (log(t2) - log(t1)))
    T2, t2 = table[-1]                                  # bis zum Kapsel-Dauermoment logarithmisch auf 20 min
    return exp(log(t2) + (T2 - T) / (T2 - cont) * (log(1200) - log(t2)))


def motor_torque(typ, sec, rotating=False):
    """Moment, das der Motor sec Sekunden lang liefert (Stillstand bzw. drehend)."""
    table = (ROTATE if rotating else STALL)[typ]
    if sec == float('inf'):
        return CONT[typ]
    lo, hi = CONT[typ], table[0][0]
    for _ in range(50):
        m = (lo + hi) / 2
        lo, hi = (m, hi) if _hold(table, CONT[typ], m) >= sec else (lo, m)
    return lo


# ---------------------------------------------------------------- Mensch und Arbeitsschritte
HUMAN = {'S': 65, 'E': 65, 'H': 250, 'K': 250}      # Nm je Seite, trainierter Mann 75 kg (Harbo et al. 2012)
ROHMERT = {10: 0.80, 120: 0.40, float('inf'): 0.15}  # Anteil der Maximalkraft, der so lange gehalten wird
DUR_TXT = {10: '10 s', 120: '2 min', float('inf'): 'dauerhaft'}
JNAME = {'S': 'Schulter', 'E': 'Ellbogen', 'H': 'Hüfte', 'K': 'Knie', 'B': 'Gleichgewicht'}

# Moment je kg Baum (Nm/kg, je Seite) aus tools/kraftverteilung.py, Baum 3 m; body = Moment des eigenen Körpers;
# legs = welcher Teil des Exo-Eigengewichts (Hocke = 1) in dieser Haltung an Hüfte und Knie hängt
TASKS = {
    # moves: Gelenke, die sich in diesem Schritt unter Last drehen (dort hilft eine Sperre nicht, der Motor liefert
    # nach der Drehend-Tabelle); 'arms_if_free': die Arme drehen mit, wenn sie keine Sperre haben
    'Aufrichten': dict(perkg={'S': 2.639, 'E': 2.542, 'H': 2.587, 'K': 1.574}, body={'S': 6, 'E': 2, 'H': 45, 'K': 33},
                       legs=1.0, moves='HK', arms_if_free=True, durs=[10], balance=45),
    'Aufladen': dict(perkg={'S': 3.139, 'E': 2.015, 'H': 0.951, 'K': 0.571}, body={'S': 6, 'E': 2, 'H': 8, 'K': 5},
                     legs=0.3, moves='S', arms_if_free=False, durs=[10]),
    'Hochheben': dict(perkg={'S': 3.139, 'E': 2.015, 'H': 0.951, 'K': 0.571}, body={'S': 6, 'E': 2, 'H': 8, 'K': 5},
                      legs=0.3, moves='', arms_if_free=False, durs=[10, 120]),
    'Tragen': dict(perkg={'S': 2.453, 'E': 1.018, 'H': 0.693, 'K': 0.208}, body={'S': 6, 'E': 1.5, 'H': 5, 'K': 3},
                   legs=0.2, moves='', arms_if_free=False, durs=[10, 120, float('inf')]),
}
TASK_TXT = {'Aufrichten': 'Aufrichten (Stamm steht am Boden)', 'Aufladen': 'Aufladen (Arm hebt den Baum auf die Ladefläche)',
            'Hochheben': 'Hochheben und halten (Brusthöhe)', 'Tragen': 'Tragen wie eine Fackel'}
# Was das Exo in der Hocke für sich selbst braucht (Entwurf 5, realismus-check.md): Schulter 7 (Exo-Arm), Ellbogen 1,5,
# Hüfte 25 (Exo-Oberteil 29,5 kg), Knie 17 (≈ 18 kg je Seite über dem Knie). Im Entwurf 6 skaliert mit der Masse.
SELF_E5 = {'S': 7.0, 'E': 1.5, 'H': 25.0, 'K': 17.0}
LEG_IDS = ('BE-', 'BA-', 'AN-K', 'SE-06', 'KB-01', 'KB-02', 'HU-05', 'HU-06', 'HU-07', 'LK-03')
ARM_IDS = ('AR-', 'AN-E', 'SG-', 'LK-02')            # sitzen weit vorn am Arm (zählen voll)
SHOULDER_IDS = ('SC-', 'AN-S', 'LK-01')               # sitzen nah an der Schulterachse (zählen 40 %)


# ---------------------------------------------------------------- Entwurf (Zustand)
class Design:
    def __init__(self):
        self.bom = {i: [n, q, kg] for i, n, q, kg in BOM_E5}
        self.cost = {k: [n, e] for k, n, e in COST_E5}
        # je Gelenk: Motor, Sperre (Nm, nur im Stillstand), Feder (Nm, immer)
        self.joint = {'S': dict(motor='RS04', lock=0, spring=0), 'E': dict(motor='RS03', lock=0, spring=0),
                      'H': dict(motor='RS04', lock=0, spring=0), 'K': dict(motor='RS04', lock=0, spring=0)}
        self.struct = {'S': 120, 'E': 80, 'H': 120, 'K': 120}   # Nm, was das Gerüst mit Sicherheit 2 trägt (festigkeit.md)
        self.log = []

    def mass(self):
        return sum(q * kg for _, q, kg in self.bom.values())

    def mass_of(self, prefixes):
        return sum(q * kg for i, (_, q, kg) in self.bom.items() if i.startswith(prefixes))

    def euro(self):
        return sum(e for _, e in self.cost.values())

    def qty(self, i, q):
        self.bom[i][1] = q

    def kg(self, i, kg, name=None):
        self.bom[i][2] = kg
        if name:
            self.bom[i][0] = name

    def add(self, i, name, q, kg):
        self.bom[i] = [name, q, kg]

    def eur(self, k, e, name=None):
        if k in self.cost:
            self.cost[k][1] = e
            if name:
                self.cost[k][0] = name
        else:
            self.cost[k] = [name, e]

    def upper_mass(self):
        """Exo-Masse über der Hüfte (Rumpf, Arme, Akku, Box)."""
        return self.mass() - self.mass_of(LEG_IDS)

    def arm_moment(self):
        return self.mass_of(ARM_IDS) + 0.4 * self.mass_of(SHOULDER_IDS)

    def self_torque(self, ref):
        """Was das Exo an jedem Gelenk für sich selbst braucht (Hocke), skaliert von Entwurf 5."""
        up = self.upper_mass() / ref.upper_mass()
        thigh = ('HU-05', 'HU-06', 'HU-07', 'BE-01', 'BE-03', 'BE-04', 'AN-K', 'BA-', 'LK-03')
        knee = (self.upper_mass() + self.mass_of(thigh)) / (ref.upper_mass() + ref.mass_of(thigh))
        fore = (self.mass_of(('AR-02', 'SG-', 'SE-08')) + 0.5 * self.mass_of(('LK-02',))) / ref.mass_of(('AR-02', 'SG-', 'SE-08'))
        return {'S': SELF_E5['S'] * self.arm_moment() / ref.arm_moment(), 'E': SELF_E5['E'] * fore,
                'H': SELF_E5['H'] * up, 'K': SELF_E5['K'] * knee}


def exo_torque(d, j, sec, moving):
    """Was das Exo an Gelenk j sec Sekunden liefert (vor Abzug des Eigengewichts)."""
    jt = d.joint[j]
    mot = min(LIMIT[j], motor_torque(jt['motor'], sec, rotating=moving)) if jt['motor'] else 0.0
    lock = 0.0 if moving else jt['lock']        # Sperre hilft nur, wenn das Gelenk steht
    return min(d.struct[j], mot + lock + jt['spring'])


def human_only():
    d = Design()
    for j in d.joint:
        d.joint[j] = dict(motor=None, lock=0, spring=0)
    return d


def max_tree(d, task, sec, ref, with_exo=True):
    """Schwerster Baum (kg) für diesen Arbeitsschritt und diese Dauer, und was ihn begrenzt."""
    t = TASKS[task]
    selfT = d.self_torque(ref) if with_exo else {j: 0 for j in 'SEHK'}
    out = {}
    for j in 'SEHK':
        moving = j in t['moves'] or (t['arms_if_free'] and j in 'SE' and not d.joint[j]['lock'])
        exo = exo_torque(d, j, sec, moving) if with_exo else 0
        burden = selfT[j] * (t['legs'] if j in 'HK' else 1)
        avail = HUMAN[j] * ROHMERT[sec] + exo - burden - t['body'][j]
        out[j] = max(0.0, avail / t['perkg'][j])
    if 'balance' in t:
        out['B'] = t['balance']             # Schwerpunkt > 20 cm vor dem Knöchel: man kippt (kraftverteilung.md)
    jmin = min(out, key=out.get)
    return out[jmin], jmin, out


def strength_cells(d, ref, with_exo=True):
    return [(task, sec) + max_tree(d, task, sec, ref, with_exo)[:2] for task, t in TASKS.items() for sec in t['durs']]


# ---------------------------------------------------------------- Maßnahmen
MOTOR_PARTS = ('AS-01', 'AS-02', 'AS-03', 'AS-04', 'AS-05', 'AS-06', 'AS-07', 'AS-08', 'AS-09', 'AS-10', 'VK-01')
CAPSULE = ('AN-02', 'AN-03', 'AN-04', 'AN-05')


def step(d, code, text, fn):
    m0, e0 = d.mass(), d.euro()
    fn()
    d.log.append((code, text, d.mass() - m0, d.euro() - e0))


def schritt1_gewicht(d):
    """Gewicht: Motoren nur noch dort, wo der Mensch schwach ist (Schultern)."""
    def g1():   # Beine ohne Motoren: Exo-Gewicht und Last laufen über die Schienen in den Steigbügel
        for i in ('AN-H', 'AN-K', 'KB-01', 'KB-02', 'SE-06', 'BA-01', 'BA-02', 'BA-03', 'BA-04', 'BA-05', 'VK-02'):
            d.qty(i, 0)
        for i in CAPSULE:
            d.qty(i, 4)
        for i in MOTOR_PARTS:
            d.qty(i, 6)
        d.qty('VK-06', 2); d.qty('VK-08', 4); d.qty('RA-11', 2); d.qty('SE-02', 4); d.qty('SE-12', 4)
        d.kg('HU-05', 0.38, 'Hüftschwinge 35 × 35 × 2 (ohne Motormoment)')
        d.kg('HU-07', 0.06, 'Oberschenkelplatte (ohne Abtriebsflansch)')
        d.kg('EL-09', 0.12)
        d.joint['H'] = dict(motor=None, lock=0, spring=0)
        d.joint['K'] = dict(motor=None, lock=0, spring=0)
        d.eur('rs04', 3 * 296.4, 'RobStride RS04 × 3'); d.eur('kuehl', 4 * 28.75); d.eur('pads', 12.5); d.eur('wangen', 20)
        d.eur('abtriebsflansch', 10); d.eur('maentel', 7.5); d.eur('dicht', 20); d.eur('druckausgl', 25); d.eur('heiz', 25)
        d.eur('kurbeln', 15); d.eur('stangen', 15); d.eur('gelenkkoepfe', 35); d.eur('federpaket', 0)
        d.eur('zellen20', 20); d.eur('fusszellen', 0); d.eur('anschluss', 105); d.eur('verkleidung', 70); d.eur('kurbelschutz', 2.5)
    step(d, 'G1', 'Beine ohne Motoren: Hüfte und Knie laufen frei mit, Exo-Gewicht und Last gehen über die Schienen in den Steigbügel', g1)

    def g2():   # Rumpfmotoren weg (offene Entscheidung 3)
        for i in ('AN-L', 'AN-D', 'AN-06', 'RA-13', 'KB-06', 'SE-09'):
            d.qty(i, 0)
        for i in MOTOR_PARTS:
            d.qty(i, 4)
        d.eur('rs04', 2 * 296.4, 'RobStride RS04 × 2'); d.eur('rs03', 2 * 176.7, 'RobStride RS03 × 2')
        d.eur('rumpfantrieb', 0); d.eur('rumpfzellen', 0); d.eur('anschluss', 70)
    step(d, 'G2', 'Rumpfmotoren weg: Aufrichten tragen Gasfedern und Lendensperre, Drehen bleibt passiv (offene Entscheidung 3)', g2)

    def g3():   # Ellbogenmotoren weg
        for i in ('AN-E', 'AR-04', 'AR-06', 'AR-07', 'AR-08', 'VK-03', 'VK-06', 'VK-08', 'RA-11'):
            d.qty(i, 0)
        for i in CAPSULE:
            d.qty(i, 2)
        for i in MOTOR_PARTS:
            d.qty(i, 2)
        d.kg('KB-05', 0.08)
        d.joint['E'] = dict(motor=None, lock=0, spring=0)
        # Ohne Ellbogenmotor fehlt sein Encoder: Die Schulter-Vorsteuerung braucht den Ellbogenwinkel weiterhin.
        # Hall-Winkelsensor AS5600 (Analogausgang, 3 × 3 mm Magnet auf der Achse), je ≈ 4 € + Leitung.
        d.add('SE-16', 'Winkelsensor Ellbogen (AS5600, Analogausgang) mit Magnet und Leitung', 2, 0.015)
        d.eur('ellbogenwinkel', 12, 'Winkelsensor Ellbogen × 2 (AS5600, Magnet, Leitung)')
        d.eur('rs03', 0); d.eur('kuehl', 2 * 28.75); d.eur('pads', 6.25); d.eur('wangen', 10); d.eur('maentel', 3.75)
        d.eur('dicht', 10); d.eur('druckausgl', 12.5); d.eur('heiz', 12.5); d.eur('kurbeln', 0); d.eur('stangen', 0)
        d.eur('gelenkkoepfe', 0); d.eur('anschluss', 35); d.eur('verkleidung', 50); d.eur('kurbelschutz', 0)
    step(d, 'G3', 'Ellbogenmotoren weg: den Ellbogen bewegst du selbst, halten übernimmt in Schritt 2 eine Sperre', g3)

    def g4():   # Leichtbau ohne Funktionsverlust
        for i in ('AN-04', 'HU-01', 'SC-02', 'BE-03', 'RA-03', 'BE-11'):
            d.kg(i, d.bom[i][2] * 0.75)             # Taschen, Steg >= 2 × Blechdicke
        d.qty('EN-02', 1)                           # ein Akkuschacht, Ersatzakku bleibt im Fahrzeug
        d.kg('AN-01', 0.35, 'Elektronikbox (ein CAN-Strang, 2 Motoren)')
        d.kg('EL-04', 0.12, 'Schütz 50 A, Hauptsicherung 30 A')
        d.kg('EL-08', 0.06)
        d.qty('EL-11', 4)
    step(d, 'G4', 'Leichtbau ohne Funktionsverlust: Taschen in Laserteilen, ein Akkuschacht, kleinere Box und Schütz', g4)


def schritt2_kraft(d):
    """Kraft: Halten ohne Wärme (Sperren), Federn; Motoren nur noch zum Bewegen."""
    def k1():
        # Schulter: blockierbare Gasfeder, starr in Druckrichtung bis 10 kN (SUSPA HY4 / Bansbach Typ P, Rohr 28 mm),
        #   F1 400 N hebt den Arm mit (wie eine passive Armstütze), ausgelegt auf 3 kN × Hebel 40 mm = 120 Nm.
        # Ellbogen und Knie: blockierbare Gasfeder starr in Zugrichtung bis 3,5 kN (SUSPA HY1, Rohr 22 mm), so eingebaut,
        #   dass die Last sie auseinanderzieht; F1 80 N (schwächste Füllung), ausgelegt auf 2,5 kN:
        #   Ellbogen Hebel 32 mm = 80 Nm, Knie Hebel 40 mm = 100 Nm. F1 drückt dort leicht gegen (≈ 3 Nm).
        d.add('LK-01', 'Blockierbare Gasfeder Schulter (SUSPA HY4 o. ä., Rohr 28, starr auf Druck, F1 400 N)', 2, 0.33)
        d.add('LK-02', 'Blockierbare Gasfeder Ellbogen (SUSPA HY1 o. ä., Rohr 22, starr auf Zug, F1 80 N)', 2, 0.20)
        d.add('LK-03', 'Blockierbare Gasfeder Knie (SUSPA HY1 o. ä., Rohr 22, starr auf Zug, F1 80 N)', 2, 0.22)
        d.add('LK-04', 'Federhalter, Gabelköpfe M8 (Laser, Kauf)', 6, 0.04)
        d.add('LK-05', 'Auslöser: Bowdenzug + Hebel (Daumenhebel je Hand, Rasthebel am Hüftgurt) + Mikroschalter', 3, 0.08)
        d.joint['S'].update(lock=120, spring=12)    # F1 400 N am wirksamen Hebel ≈ 30 mm
        d.joint['E'].update(lock=80, spring=-2.5)
        d.joint['K'].update(lock=100, spring=-3)
        d.eur('sperren', 6 * 60 + 3 * 25 + 30 + 10, 'Blockierbare Gasfedern × 6, Auslöser × 3, Halter, Mikroschalter')
    step(d, 'K1', 'Sperren an Schulter, Ellbogen und Knie (blockierbare Gasfedern): halten ohne Wärme; beim Aufrichten Arme sperren und aus den Beinen heben', k1)

    def k2():
        # Schulter: Federhebel 60 mm statt 40 mm -> 3 kN Federkraft für 180 Nm (Feder starr bis 10 kN). Schulterbügel
        # 30 × 30 × 3 hat bei 180 Nm ≈ 68 MPa, Oberarmschiene 25 × 25 × 3 ≈ 103 MPa (Grenze mit Sicherheit 2: 125 MPa).
        # Die Federkraft läuft über Schultergabel und Hebel, nicht über das Motorlager.
        d.add('LK-06', 'Federhebel Schulter 60 mm, Gabelkopf M10 (Alu 10 mm, Laser)', 2, 0.06)
        d.struct['S'] = 180
        d.joint['S'].update(lock=180)
        d.eur('sperren_s', 20, 'Federhebel Schulter, Gabelköpfe M10')
    step(d, 'K2', 'Schulter auf 180 Nm gesperrtes Haltemoment (Federhebel 60 mm); Gerüst nachgerechnet', k2)


def schritt3_kosten(d):
    """Herstellkosten: gleiche Funktion, günstiger bezogen und gefertigt."""
    def c1():
        d.eur('rs04', 2 * 220, 'RobStride RS04 × 2 (Hersteller-Shop: ¥ 1.199 + Versand, EUSt 19 %, Zoll 2,7 %)')
    step(d, 'C1', 'Motoren direkt im Hersteller-Shop statt beim Händler', c1)

    def c2():
        d.eur('messplatine', 35, 'Messplatine mit 1 × ADS131M04 (4 Armmanschetten)')
        d.qty('SE-03', 1)
        d.eur('schuetz', 70, 'Sicherung, Schütz 50 A, Not-Aus'); d.eur('stecker', 45); d.eur('bremswid', 25)
    step(d, 'C2', 'Elektronik auf 2 Motoren zugeschnitten: ein Messwandler, kleineres Schütz, weniger Stecker', c2)

    def c3():
        d.eur('boa', 80, 'BOA × 4 (Arme)'); d.eur('schlitten', 25, 'Längsschlitten × 4 (Arme)')
        d.qty('SE-10', 4); d.qty('SE-11', 4)
        d.add('SE-15', 'Klettgurt 40 mm mit Leiterschnalle (Beinmanschetten)', 4, 0.03)
        d.eur('klett', 20, 'Klettgurte Beine × 4')
    step(d, 'C3', 'Beinmanschetten messen nichts mehr: Klettgurt statt BOA, kein Längsschlitten', c3)

    def c4():
        d.eur('oberflaeche', 60, 'Pulverbeschichten, Sammelauftrag'); d.eur('cordura', 0)
        d.eur('akkuadapter', 20, 'Stihl-Akku-Adapter gedruckt'); d.eur('lagerplatten', 25)
    step(d, 'C4', 'Fertigung: Pulver statt Eloxal, Akku-Adapter gedruckt, weniger Lagerplatten, keine Cordura-Hauben', c4)


def schritt4_optionen(d):
    """Was noch fehlt bis Faktor 2 bei Gewicht und Kosten: jede Option ändert etwas, das du merkst."""
    def o1():
        d.qty('RA-12', 0); d.eur('rumpfdreh', 0)
    step(d, 'O1', 'Rumpfdrehung weglassen: Rahmen dreht nur noch im Gurt mit (≈ ±5°)', o1)

    def o2():
        d.qty('SG-06', 0); d.qty('EL-03', 0)
        d.add('SG-08', 'Ratschenspanner 25 mm (Hand), Edelstahl', 3, 0.10)
        d.eur('winden', 24, 'Ratschenspanner × 3'); d.eur('windentreiber', 0)
    step(d, 'O2', 'Schlaufen mit Handratsche statt Motor spannen', o2)

    def o3():
        d.qty('BD-04', 0); d.eur('licht', 0)
    step(d, 'O3', 'Arbeitslicht weg (Stirnlampe)', o3)

    def o4():
        rails, frame = ('BE-01', 'BE-06', 'AR-01', 'AR-02'), ('RA-01', 'SC-01', 'SC-08', 'HU-05')
        for i in rails:
            d.kg(i, d.bom[i][2] * (1 - 0.7 * 0.4))  # Außenrohre CFK, Innenrohre (Teleskop) bleiben Alu
        for i in frame:
            d.kg(i, d.bom[i][2] * 0.6)
        d.eur('cfk', 240, 'CFK-Vierkantrohre statt Alu (≈ 6 m, Alu-Endstücke eingeklebt)')
    step(d, 'O4', 'Rahmen, Schulter und Außenrohre der Schienen aus CFK (Teleskop bleibt)', o4)


def variante_passiv(d):
    """Vergleich: auch die Schultermotoren weg, keine Elektronik mehr (nur Sperren und Federn)."""
    def p1():
        for i in ('AN-S', 'AN-02', 'AN-03', 'AN-04', 'AN-05', 'KB-05', 'KB-03', 'KB-04', 'SC-09', 'VK-01', 'EN-01', 'EN-02', 'AN-01',
                  'SE-02', 'SE-03', 'SE-04', 'SE-07', 'SE-12', 'SE-13', 'SE-14', 'SE-16', 'BD-01', 'BD-02', 'BD-03', 'RA-04', 'RA-08', 'VK-07') + \
                 tuple(k for k in d.bom if k.startswith(('EL-', 'AS-'))):
            d.qty(i, 0)
        d.joint['S'].update(motor=None)
        for k in ('rs04', 'kuehl', 'pads', 'wangen', 'abtriebsflansch', 'maentel', 'dicht', 'druckausgl', 'heiz', 'bremswid', 'teensy',
                  'messplatine', 'esp', 'imu', 'zellen50', 'uhr', 'dcdc', 'schuetz', 'akkuadapter', 'stecker', 'platine', 'led', 'anzeige',
                  'zellen20', 'zellhalter', 'ellbogenwinkel', 'anschluss', 'kanaele', 'schutzbuegel', 'haube', 'licht', 'winden', 'windentreiber'):
            d.eur(k, 0)
        d.qty('SG-06', 0)
        d.add('SG-08', 'Ratschenspanner 25 mm (Hand), Edelstahl', 3, 0.10)
        d.eur('ratschen', 24, 'Ratschenspanner × 3')
    step(d, 'P1', 'Variante ganz passiv: auch Schultermotoren, Akku und Elektronik weg', p1)


STEPS = [('Schritt 1: Gewicht', schritt1_gewicht), ('Schritt 2: Kraft', schritt2_kraft), ('Schritt 3: Kosten', schritt3_kosten),
         ('Schritt 4: Optionen', schritt4_optionen)]


# ---------------------------------------------------------------- Ausgabe
def de(x, n=1):
    return f'{x:,.{n}f}'.replace(',', ' ').replace('.', ',').replace(' ', '.')


def score(cells, ref_cells):
    r = [c[2] / rc[2] for c, rc in zip(cells, ref_cells)]
    return exp(sum(log(x) for x in r) / len(r)), r


def run(upto=4):
    ref = Design()
    states = [('Entwurf 5', ref.mass(), ref.euro(), strength_cells(ref, ref), [])]
    d = Design()
    for k, (title, fn) in enumerate(STEPS[:upto]):
        n0 = len(d.log)
        fn(d)
        states.append((title, d.mass(), d.euro(), strength_cells(d, ref), d.log[n0:]))
    return ref, d, states


def report(upto=4):
    ref, d, states = run(upto)
    ref_cells = states[0][3]
    hum = strength_cells(human_only(), ref, with_exo=False)
    print('## Ausgangswerte Entwurf 5\n')
    print(f'- Masse getragen (ein Akku): **{de(ref.mass())} kg**')
    print(f'- Material ein Exo: **{de(ref.euro(), 0)} €** (Mittelwerte aus stueckliste.md)')
    print('- Kraft: schwerster Baum je Arbeitsschritt (Tabelle unten)\n')
    for title, m, e, cells, log_ in states[1:]:
        sc, _ = score(cells, ref_cells)
        print(f'## {title}\n')
        print('| Maßnahme | Masse | Kosten |\n|---|---|---|')
        for code, txt, dm, de_ in log_:
            print(f'| {code} {txt} | {de(dm, 2)} kg | {de(de_, 0)} € |')
        print(f'\nStand danach: **{de(m)} kg** (Faktor {de(ref.mass() / m, 2)} leichter), '
              f'**{de(e, 0)} €** (Faktor {de(ref.euro() / e, 2)} günstiger), Kraft Faktor **{de(sc, 2)}**.\n')
    print('## Kraft: schwerster Baum, den man so halten bzw. aufrichten kann (kg)\n')
    print('| Arbeitsschritt | Dauer | ohne Exo | ' + ' | '.join(s[0] for s in states) + ' |')
    print('|' + '---|' * (3 + len(states)))
    for i, (task, sec, _, _) in enumerate(ref_cells):
        row = [TASK_TXT[task], DUR_TXT[sec], de(hum[i][2], 0)] + [f'{de(s[3][i][2], 0)} ({JNAME[s[3][i][3]]})' for s in states]
        print('| ' + ' | '.join(row) + ' |')
    sc, r = score(states[-1][3], ref_cells)
    print('\nIn Klammern das Gelenk, das zuerst an seine Grenze kommt.\n')
    print(f'Endstand: **{de(d.mass())} kg**, **{de(d.euro(), 0)} €**, Kraft-Faktor {de(sc, 2)} '
          f'(je Zeile {", ".join(de(x, 1) for x in r)}).\n')
    v = Design()
    for _, fn in STEPS[:3]:
        fn(v)
    variante_passiv(v)
    vc = strength_cells(v, ref)
    vs, vr = score(vc, ref_cells)
    print(f'Zum Vergleich, Variante ganz passiv (Schritte 1–3, dann auch ohne Schultermotoren, Akku, Elektronik): '
          f'{de(v.mass())} kg, {de(v.euro(), 0)} €, Kraft-Faktor {de(vs, 2)} (je Zeile {", ".join(de(x, 1) for x in vr)}).\n')
    st = d.self_torque(ref)
    print('Eigengewicht des Exos an den Gelenken (Nm, Hocke), Entwurf 5 → jetzt: ' +
          ', '.join(f'{JNAME[j]} {de(SELF_E5[j], 1)} → {de(st[j], 1)}' for j in 'SEHK') + '\n')
    return ref, d


def bom_table(d):
    print('| ID | Teil | Stück | kg |\n|---|---|---|---|')
    for i, (n, q, kg) in d.bom.items():
        if q:
            print(f'| {i} | {n} | {q} | {de(q * kg, 2)} |')


def as_json():
    """Zahlen für visualisierung/exo-entwurf6.html."""
    import json
    ref, d, states = run(4)
    hum = strength_cells(human_only(), ref, with_exo=False)
    v = Design()
    for _, fn in STEPS[:3]:
        fn(v)
    variante_passiv(v)
    out = dict(steps=[], tasks=[[TASK_TXT[t], DUR_TXT[s]] for t, s, _, _ in states[0][3]], human=[round(c[2]) for c in hum])
    for title, m, e, cells, log_ in states:
        out['steps'].append(dict(title=title, kg=round(m, 1), eur=round(e), score=round(score(cells, states[0][3])[0], 2),
                                 trees=[[round(c[2]), JNAME[c[3]]] for c in cells],
                                 measures=[[c, t, round(dm, 2), round(de_)] for c, t, dm, de_ in log_]))
    vc = strength_cells(v, ref)
    out['passiv'] = dict(kg=round(v.mass(), 1), eur=round(v.euro()), score=round(score(vc, states[0][3])[0], 2),
                         trees=[[round(c[2]), JNAME[c[3]]] for c in vc])
    return json.dumps(out, ensure_ascii=False)


if __name__ == '__main__':
    if '--json' in sys.argv:
        print(as_json())
        sys.exit()
    if '--stueckliste-json' in sys.argv:   # Stückliste Entwurf 6 (Schritte 1–3) für visualisierung/exo-technik.html (E6BOM)
        import json
        v = Design()
        for _, fn in STEPS[:3]:
            fn(v)
        print(json.dumps({i: [n, q, kg] for i, (n, q, kg) in v.bom.items() if q}, ensure_ascii=False, separators=(',', ':')))
        sys.exit()
    upto = int(sys.argv[sys.argv.index('--schritt') + 1]) if '--schritt' in sys.argv else 4
    ref, d = report(upto)
    if '--stueckliste' in sys.argv:
        bom_table(d)
