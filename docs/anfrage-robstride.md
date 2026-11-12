# Anfrage an RobStride: wasserdichte RS03 / RS04

**Was das ist:** eine E-Mail an den Hersteller RobStride Dynamics (Peking). Wir fragen, ob es die starken Motoren RS03 und
RS04 auch wasserdicht gibt (wie das kleine RS02 als IP67), was sie kosten und wann sie lieferbar sind. Dazu die genaue
Drehzahl-Kennlinie, die im Handbuch nur als Bild steht. Senden musst du sie selbst.

**Wohin:** über das Kontaktformular auf [robstride.com](https://www.robstride.com) oder an die Vertriebsadresse, die dort steht.
Eine Kopie an einen EU-Händler (z. B. openelab) schadet nicht: Die kennen die Lieferwege nach Europa.

---

**Subject:** Inquiry: IP67 versions of RS03 / RS04 for an outdoor exoskeleton (prototype, small series)

Dear RobStride team,

I am building an active exoskeleton for outdoor work (loading and dragging Christmas trees in snow, rain and mud,
down to about −15 °C). Your RS04 (hip, knee, shoulder) and RS03 (elbow) fit our torque requirements very well.

Because the joints are exposed to snow and water, I would like to ask:

1. Do you offer the **RS03 and RS04 with IP67** (or IP65), similar to the RS02-IP67? If not, is a sealed version planned,
   or can you supply them with a shaft seal / sealed housing on request?
2. **Price and lead time** for: 1 × RS04 now (test bench), later 5 × RS04 + 2 × RS03, shipping to Germany.
3. The **T-N curve (torque vs. speed) of RS04 and RS03 at 36 V** (we use a 36 V tool battery), or as a table at 48 V.
4. The **continuous torque without the reference heat sink** (motor mounted in a small closed plastic housing,
   ambient −15 … +10 °C), or the thermal resistance / thermal time constant.
5. Is the **operating temperature range** (−20 … 50 °C) valid for the encoder and driver at −20 °C after a cold start?
6. Can the motors **limit torque internally** (parameter) as a second safety layer independent of the host controller?
7. What **radial load and tilting moment** may the output flange carry? At the knee a push rod is attached to a crank on
   the output (up to 3.2 kN radial, 25 mm in front of the flange); at hip and shoulder the limb is mounted directly.
8. **Phase resistance** and **maximum DC bus current** of RS04 and RS03 at peak torque (to size fuses and battery).
9. Is the reported position after power-up **absolute within one output revolution** (0–2π), also when the output was
   turned by hand while powered off?
10. Does the parameter **0x7028 (canTimeout)** stop the motor in operation-control mode, and must it be written after
    every power-up?

Thank you very much. I am happy to share test results from our sealed housing with you.

Best regards,
[Name]
[E-Mail / Telefon]

---

**Auf Deutsch (zum Nachlesen):** Wir fragen nach IP67-Versionen von RS03/RS04, nach Preis und Lieferzeit (erst 1 RS04,
später 5 RS04 + 2 RS03 nach Deutschland), nach der Drehmoment-Drehzahl-Kurve bei 36 V, nach dem Dauermoment ohne die große
Kühlplatte des Herstellers (unser Motor sitzt in einer kleinen Kunststoffkapsel), ob −20 °C beim Kaltstart gilt und ob man
im Motor selbst eine Momentgrenze als zweite Sicherung einstellen kann. Neu (Durchsicht Nov 2026): zulässige Querkraft am Abtrieb (Schubstange am Knie), Strangwiderstand und Busstrom bei Spitze (Sicherungen), ob der Winkel nach dem Einschalten absolut ist, und ob der Bus-Wächter `canTimeout` im Betriebsmodus wirkt.
