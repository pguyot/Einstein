# MessagePad 2x00 bring-up: test plan

For: board 820-0894-A, schematic "Q EVT2" (`Schematics.pdf`), scans
`MainBoardTop.jpg` (test-pad side) and `MainBoardBottom.jpg` (component side).

## 0. Read first: how far to trust the schematic

The schematic is an early prototype (EVT2). The production board differs:

- **CPU:** the schematic shows an ARM710 (U10). Production MP2x00 units have a
  StrongARM SA-110 in the same spot. Its pinout is different, and it needs a
  **separate core supply (about 2.0 V)** that the schematic does not show. Do
  not probe U10 using the schematic's pin numbers.
- **Extra parts:** the board has parts the schematic lists as "not used" or
  doesn't have at all: U11, U15, U22, Q26–Q29, R110–R133. One of them is
  probably the CPU core regulator.
- **C88** (supercap) is drawn as 0.1 F / 2.5 V. In production it is a
  0.1 F / 5.5 V part (marked "GC5.5V0.10F" or similar).
- The power section (sheets 9–11) and the reset and clock circuits around
  Milton (sheet 1) match the board's silkscreen well, so they are the
  reliable part. Before relying on any net, check it with a continuity test.

**Board orientation used below**
- *Component side* (`MainBoardBottom.jpg`): the tablet connector notch (J3) is
  at the **top right**.
- *Test-pad side* (`MainBoardTop.jpg`): the notch is at the **top left**. The
  column of seven large gold pads in the lower left half is Apple's "Special
  System Assembly Test Points" (sheet 12):

| Pad  | Signal             | Expected when running                         |
|------|--------------------|-----------------------------------------------|
| TP27 | dGnd               | ground: clip the scope ground here            |
| TP28 | dVcc               | 3.3 V, always on (also while asleep)          |
| TP26 | miltonNotResetIn   | high (≈3.3 V) = reset released                |
| TP2  | clk32Khz           | 32.768 kHz square wave (see note in test C3)  |
| TP1  | memSwVcc           | 3.3 V while the system is powered             |
| TP29 | pwrNotBattFault    | high = no battery fault                       |
| TP3  | cpuAttention       | high; pulses low while Reset (S1) is pressed  |

These pads are the main place to measure. Solder short wires to TP27, TP28,
TP26 and TP29 and leave them there for every test.

## 1. Summary of what the symptoms suggest

- **None of the units start, sometimes one starts after minutes:** the
  problem is not in the logic. It is something that changes slowly: a rail
  that is held down and slowly recovers (a leaky or shorted tantalum, a leaky
  C88 supercap), a marginal 32 kHz oscillator, or contamination that dries
  out as the board warms up. Several units failing the same way points to
  aging parts, or to the one thing they all share: **your power adapter**.
- **Reset and power switch do nothing:** both go through Milton's always-on
  logic (S1 → cpuAttention → Milton ATTN; power switch → Milton GPIO0).
  If dVcc, the reset release or the 32 kHz clock is missing, both buttons are
  dead. This is consistent with the first point.
- **LCD comes on briefly, then shuts off:** the CPU *was* running. Switching
  on the LCD adds load (Q18 lcdSwVcc plus the 19–30 V boost). Then either a
  supply dips and trips a detector (U2 reset at ~3.0 V on dVcc, U35 battery
  fault at ~3.7 V on psVin), or NewtonOS reads a bad battery voltage through
  the Keynes ADC and powers down on purpose. Test D below tells these apart.

Note on "dried out tantalums": solid tantalums don't dry out. They fail
**short** or **leaky**, sometimes intermittently and depending on
temperature. Aluminium electrolytics dry out (high ESR). Both kinds appear as
the large rectangular pads on the scans. A forum report of a dead MP2100
found shorted caps and an open fuse (see the sources at the end).

## 2. Setup

1. **Use a lab supply instead of the Apple adapter.** Feed it through the DC
   jack (J2). Start at 7.0 V with a **current limit of 300 mA**; raise the
   limit later. The display of the current is your most useful instrument.
2. **Current on the scope:** put a 1 Ω resistor in the negative lead (low
   side), and measure across it with the probe ground on the supply side.
   Then 1 mV equals 1 mA. This shows inrush, hiccups and the moment the LCD
   switches on.
3. **Probes:** use 10x probes with the short ground spring. Use TP27 as
   ground. Don't connect the probe ground to psGnd and dGnd at different
   points while the board is running.
4. **Without the case:** connect the LCD, tablet and power switch flex so the
   load is realistic. Remove PCMCIA cards and the coin cell, if your unit has
   one. Put the card lock lever in the locked position.
5. **High voltage:** the EL backlight inverter (T1, connector J13) produces
   around 100 V AC. The LCD bias reaches 30 V.
6. **Do not clean this board in an ultrasonic bath** while Y2 is fitted.
   32 kHz tuning-fork crystals can be destroyed by it. Use isopropyl alcohol
   and a brush.

## 3. Tests, in order

Work through one board fully before moving to the next. Write every reading
into a table with one column per unit. **Comparing units is your reference,**
because there are no published "good" readings.

### A. Without power

**A1. Visual inspection** under magnification, both sides. The scans already
show dark or brown residue in these places:
- around L4/L5, J1 and C5 at the top of the test-pad side;
- around D1 and X2 at its top right corner;
- around the 5 V flyback (C151, C154, D22, Q24, C143) and the LCD boost (L18,
  D13, C123, C119, C124) along its bottom edge;
- around the pads of C88 on the component side.

Look for green or white corrosion, cracked tantalums, lifted pads, and
corroded vias, especially under and around the polarized caps and C88. Clean
with isopropyl alcohol, dry, then look again.

**A2. Fuse and input path** (component side, bottom right): check F1 (2 A) for
continuity. Measure D14 (input Schottky, near C125) in diode mode: ~0.2–0.3 V
forward, open in reverse. Measure D2 (TVS on the input, top center near R14):
it must not be shorted. A TVS that took a spike often fails short or leaky.

**A3. Resistance of each rail to ground.** Measure in both polarities and in
diode mode. A reading below ~10 Ω is a short. A reading clearly lower than on
the other units is a lead.

| Rail     | Where to measure                               | Notes |
|----------|------------------------------------------------|-------|
| psVin    | C147 (component side, bottom, next to U40)      | after F1 |
| dVcc     | TP28, or C139 (component side, bottom left corner) | **C88 is on this rail:** the reading creeps up as the meter charges the supercap. If it keeps climbing, that's normal. If it stays low, suspect C88 or a tantalum. |
| d12VVcc  | C148/C149 (component side, left, lower third)   | flash Vpp and PCMCIA |
| d5Vvcc   | C150 (component side, bottom center) or C151/C154 (test-pad side) | |
| d21VVcc  | C119/C124 (test-pad side, bottom right)         | LCD bias |
| aVcc     | C61 (near Keynes, U6)                           | from dVcc through XW4 |
| battery  | C152 (component side, bottom center)            | |

To find a short: feed the shorted rail from the lab supply at about 1 V with
a 1–2 A limit. The shorted part heats up: find it with a finger, a drop of
isopropyl alcohol that evaporates first, or a thermal camera. Or measure the
millivolt drop along the trace toward the short.

**A4. C88 supercap:** desolder one leg (or lift R5, the 0 Ω link next to it,
sheet 1) and measure leakage. Charge it from the lab supply to 3.3 V with a
current limit; a good one draws only a few µA after a minute. A cap that keeps
drawing mA is bad. **The board does not need C88 to run,** so leave it out
for all further tests. That alone may be the fix. It is my first suspect for
"starts after several minutes" (see B2).

**A5. Adapter:** measure the Apple adapter with no load and with about a
10 Ω / 10 W load (check the rated voltage on its label, probably 7.5 V). Look
at it on the scope: ripple, and whether it drops out. If all your units were
tested with the same adapter, this is cheap to rule out.

### B. First power-on (lab supply, 300 mA limit)

**B1. Slow ramp:** turn the voltage up from 0 to 7 V slowly while watching the
current.
- High current already below ~1 V: there is a short on the input (D2, C6,
  C13) or on a rail.
- Current that jumps and falls repeatedly: a converter is hiccupping.
- Write down the steady current after 10 s, 1 min and 5 min.

**B2. dVcc over time** (TP28, DMM plus scope at 1 s/div):
- It should be 3.3 V within milliseconds and stay there.
- **U2 (RN5VL30A) holds Milton in reset below ~3.0 V,** so there is only
  0.3 V of margin. If dVcc sits at 2.8–3.1 V and creeps up over minutes
  until the unit suddenly starts, you've found the "starts after minutes"
  mechanism. Then look for what loads dVcc: C88, a leaky tantalum, or the
  3.3 V buck itself (U40 MAX1651, Q20, L21, D21, R102 0.22 Ω current-sense,
  C139, C147; component side, bottom left).
- Ripple: the MAX1651 skips pulses, so expect irregular bursts of a few tens
  of mV. Several hundred mV means C139 or C147 is bad (high ESR).

**B3. Reset:** TP26 must be high when dVcc is OK. If dVcc is OK but TP26 is
low, suspect U2 (component side, left edge below U9) or R11.

**B4. 32 kHz clock.** Milton makes all clocks from Y2 with its internal PLL,
so without 32 kHz nothing runs.
- First look at TP2. It may only toggle once the software enables it (on the
  schematic it is Milton's LCD_DC_CLK pin). If it shows 32.768 kHz, the
  oscillator is fine.
- If TP2 is silent, probe Y2 directly (the flat cylindrical crystal right of
  C88, between C85 and Q15) with a 10x probe. On one pin expect a sine of
  roughly 0.5–1 V p-p at 32.768 kHz. The probe can detune it, so check both
  pins.
- **Start-up time:** a healthy crystal starts in under 1 s. Taking several
  seconds or longer means it is marginal: contamination around Y2, C84 or C85
  (C88 sits right next to it and may have leaked), or a tired crystal.
  Replacing Y2 with a new 12.5 pF 32.768 kHz crystal is cheap.

**B5. memSwVcc** (TP1) and **battery fault** (TP29) should both be high.
- TP29 low while the lab supply is at 7 V: check U35 (RN5VL37A, near R97,
  component side, bottom left), Q8 and U28.

### C. Power-on capture (the most informative single test)

Set up four channels and trigger single-shot on CH1 rising, at 20–50 ms/div,
then plug in the supply:

- CH1: dVcc (TP28)
- CH2: TP26 (reset in)
- CH3: TP29 (battery fault)
- CH4: supply current (across the shunt)

Expected order: dVcc rises → TP26 goes high → current rises in steps as
Milton and the CPU start (data bus activity begins here). Then press the
power switch and repeat with a longer time base to catch the wake-up.

**C2. Buttons.** Check the hardware separately from the firmware:
- TP3 must go low while Reset (S1, component side, center right near L11)
  is pressed.
- The power switch connector J7 (component side, right edge, near R43/R45):
  pin 2 (gpPowerSwIn) must go low while the switch is pressed.

If both signals arrive but nothing happens, the problem is Milton or the CPU,
not the switches.

**C3. Is the CPU running?** Find signals at QFP corners, where pins are easy
to count. On U18 (Bradley, lower right), pin 1 is RESET_L (miltonNotResetOut)
and pin 100, right next to it, is PSLEEP_L (pwrPowerEnable). Check this with
continuity to Keynes (U6) pin 32 and pin 31 first, because the schematic is
old.
- miltonNotResetOut high: Milton released the CPU.
- pwrPowerEnable high: NewtonOS is awake.
- pwrPowerEnable low with a running dVcc: the Newton is asleep or off.
- For bus activity, the ROM chip select on J11 (ROM SIMM connector) is a good
  indicator.

**C4. CPU core supply:** find the SA-110's core VDD pins (see the SA-110
datasheet in the sources) and measure them. Expect about 2.0 V. Then trace
back to the regulator; it is not in the schematic.

### D. The "LCD on, then off" event

When a unit gets as far as switching the LCD on, capture it. Trigger on
d21VVcc rising (C124 on the test-pad side, or TP4/TP5; one of those is
lcdSwVcc and the other d21VVcc), at 5–20 ms/div:

- CH1: dVcc (TP28)
- CH2: psVin (C147)
- CH3: TP26 (reset)
- CH4: TP29 (battery fault)

Then use **whichever signal moves first:**

| What you see first                 | Cause                                   | Look at |
|------------------------------------|-----------------------------------------|---------|
| psVin dips (below ~3.7 V)          | input can't deliver the inrush           | adapter, F1, D14, L4/L5, psVin caps (C147, C153, C123) |
| dVcc dips below 3.0 V → TP26 low   | 3.3 V buck can't deliver                  | U40, Q20, L21, R102, C139 (ESR) |
| TP29 goes low first                | battery fault detector                   | U35, Q8, U28, R97 |
| d21V never reaches 19 V, current spikes | LCD boost shorted                  | Q21, Q22, D13, L18, C119/C124, Q19 |
| nothing dips; rails switch off cleanly | **NewtonOS shut down on purpose** | battery and adapter readings via the Keynes ADC: dividers R33/R12 (psAdptIn), R21 (mainBatteryVPos), R24/R26 (vMeasured), thermistors RT1 and J16; calibration pads TP35–TP38 (component side, top center) |

The last case is likely when running from the adapter without batteries, or
with NiMH batteries (J15 senses the NiCd/NiMH pack). Try all three:
- fresh alkaline AA cells only;
- adapter only;
- both together.

Note which combination gets furthest.

### E. Localize temperature or time dependent faults

Once you know which signal is late (dVcc, 32 kHz, reset), cool and warm small
areas while the unit is in the failing state:
- freeze spray or a can held upside down;
- a hot-air station at about 60–80 °C, from a distance.

Areas to try:
- Y2 with C84 and C85;
- C88;
- U2;
- the U40 buck area;
- each tantalum on the affected rail.

If the fault appears or disappears within seconds of spraying one part,
you've found it.

### F. Repair order (after diagnosis, or if you want a shotgun fix)

1. Remove C88 (leave it out, or fit a new 0.1 F / 5.5 V supercap).
2. Clean the board thoroughly, especially around Y2, C88 and the power
   section.
3. Replace the tantalums on the rails that showed a problem. Use polymer
   tantalum or MLCC of equal or higher voltage rating and the same
   capacitance. For the 3.3 V buck (C139, C147), keep the output capacitor's
   ESR in a range the MAX1651 is stable with: polymer tantalum is safer than
   pure MLCC here.
4. Replace Y2 if its start-up was slow.
5. Once one unit runs, swap its ROM board, LCD and flex cables with the other
   units to separate board faults from peripheral faults.

## Rails at a glance (from sheets 9–11)

| Rail      | Made by                         | Value     | When on |
|-----------|---------------------------------|-----------|---------|
| psVin     | DC jack → L2 → D14 → F1, or battery via U31 | 4.5–7.5 V | always |
| dVcc      | U40 MAX1651 buck (Q20, L21)     | 3.3 V     | always |
| aVcc      | dVcc via XW4                    | 3.3 V     | always |
| memSwVcc  | Q15 from dVcc (sysPowerEnable)  | 3.3 V     | system on |
| lcdSwVcc  | Q18 from dVcc (lcdVccEn)        | 3.3 V     | LCD on |
| d21VVcc   | U32 MAX1771 boost (Q21, L18, D13) | 19–30 V | LCD on (lcdVeeEn) |
| d12VVcc   | U38 MAX1771 boost (Q23, L20, D20) | 12 V    | flash writes, PCMCIA (gp12VEnable) |
| d5Vvcc    | U34 MAX1771 flyback (T2, Q24/Q25, D22) | 5 V | serial, PCMCIA (gp5VEnable) |
| CPU core  | not in schematic                | ~2.0 V    | always? |

Detectors:
- U2 RN5VL30A on dVcc → reset (3.0 V);
- U5 RN5VL30A on psVin → sysPowerEnable (3.0 V);
- U35 RN5VL37A on psVin → battery fault (3.7 V);
- U27 RN5VL30A → adapter present.

## Sources

- [Apple Newton MP2100 Repair (electronics-lab forum)](https://www.electronics-lab.com/forums/threads/apple-newton-mp2100-repair.237564/):
  shorted capacitors and an open fuse on a dead MP2100.
- [Messagepad Power Problem Persist (iFixit)](https://es.ifixit.com/Respuestas/Ver/733946/Messagepad+Power+Problem+Persist):
  the GC5.5V0.10F supercap.
- [Burnt smell, flashing backlight, non functional (68kmla)](https://68kmla.org/bb/threads/burnt-smell-flashing-backlight-non-functional.26184)
- [StrongARM SA-110 datasheet (chipdb)](https://datasheets.chipdb.org/Intel/STRONG/TECHDOCS/27823001.PDF):
  pinout and core voltage (1.65 V / 2.0 V core, 3.3 V I/O).
