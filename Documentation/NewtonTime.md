# Time on the Newton: Y2K10, Y2K25 and 2040

This note describes how NewtonOS 2.1 keeps time, why the Y2K10 fix in
Einstein stopped working in 2025, how Einstein fixes it now, and what is
needed to fix other ROMs and physical Newtons. All addresses are for the
MessagePad 2x00 US ROM (717006) unless noted otherwise.

## Summary

- NewtonScript integers have 30 bits. A time in *seconds* from a base date
  wraps after 2^29 seconds, about 17 years.
- The original ROM counts NewtonScript seconds from 1993-01-01. That wrapped
  on **2010-01-05** (the "Y2K10" bug).
- Einstein moved the base to 2008-01-01. That wrapped on **2025-01-04**.
  Since then, `SetSysAlarm()` alarms landed in 1993, fired immediately, and
  were set again every few milliseconds. The Newton could no longer go to
  sleep (see *The symptom* below).
- Einstein now uses 2024-01-01 as the base. That works until **2041-01-04**.
- Independently, the clock itself is an unsigned 32-bit count of seconds
  since 1904-01-01. It overflows on **2040-02-06 06:28:16**, which also
  breaks `Time()` (minutes). This is the real deadline for every ROM and every
  physical Newton. A plan for it is at the end.

## How NewtonOS keeps time

### The hardware clock is a plain counter

The real-time clock (RTC) is a 32-bit register that counts seconds. There are
no year, month or day registers.

| Register | Address | Content |
|---|---|---|
| Calendar | `0x0F181000` | seconds, read and written by NewtonOS |
| Alarm | `0x0F181400` | seconds; an interrupt (`0x00000004`) when the calendar reaches it |

`GetRealTimeClock()` (`0x0019C4CC`) reads the calendar register twice and
repeats until both reads agree, the usual way to read a free-running counter.
`SetRealTimeClockAlarm()` (`0x0019C4EC`) writes the alarm register. The epoch
(1904-01-01, local time) and all calendar math (leap years, weekdays) are
software only.

### The boot check resets the clock to 1996

`AdjustRealTimeClock` (`0x000452FC`) runs once per boot, right before
`OsBoot`. If the calendar register is before `0xAE070B30` (1996-07-08
19:00:00) or at or after `0xC7635C00` (2010-01-01 08:00:00), it writes
`0xAE070B30` into the register:

```
000452FC  mov   r0, #0x00081000
00045300  add   r0, r0, #0x0f100000     ; r0 = 0x0F181000, calendar register
00045304  ldr   r2, [r0]
00045308  ldr   r1, =0xAE070B30          ; 1996-07-08 19:00:00
0004530C  cmp   r2, r1
00045310  bcc   00045324                 ; too early: reset
00045314  adds  r12, r2, #0x38800000     ; r2 + 0x389C8000 + 0x2400 must
00045318  addlss r12, r12, #0x001c8000   ; not overflow, i.e.
0004531C  cmnls r12, #0x00002400         ; r2 < 0xC7635C00 (2010-01-01)
00045320  movls pc, lr                   ; in range: keep it
00045324  str   r1, [r0]                 ; out of range: reset to 1996
00045328  mov   pc, lr
```

So after every boot the RTC of a NewtonOS 2.1 device runs from July 1996.
Einstein does not patch this yet, so its RTC runs from 1996 after every boot
too. The time that NewtonOS *shows* is right anyway, because Einstein patches
`RealClockSeconds()` (below). A consequence: alarms are compared with the
1996-based RTC, so alarms set for today lie 30 years in the future and never
fire in Einstein.

### From the counter to NewtonScript

| Function | Address | What it does |
|---|---|---|
| `RealClockSeconds()` | `0x00255578` | RTC as `TTime`, converted to seconds, plus `GMTOffset()` plus `DaylightSavingsOffset()`: local seconds since 1904, unsigned 32 bits |
| `RealClock()` | `0x002555F4` | `RealClockSeconds() / 60` (unsigned): local minutes since 1904 |
| `Time()` (`FTime`) | `0x00089B4C` | `RealClock() << 2`: minutes since 1904 as a NewtonScript integer |
| `TimeInSeconds()` (`FTimeInSeconds`) | `0x00089B64` | `(RealClockSeconds() - 1993 base) << 2`: seconds since 1993 as a NewtonScript integer |
| `SetTimeInSeconds()` (`FSetTimeInSeconds`) | `0x0008A20C` | NewtonScript seconds + 1993 base, then `SetRealClockSeconds()` |
| `DateFromSeconds()` (`FDateFromSeconds`) | `0x0008A868` | NewtonScript seconds + 1993 base, then `TDate::InitWithSeconds()` |
| `SetSysAlarm()` (`FSetSysAlarm`) | `0x0030EEEC` | builds a `TTime` from the 1993 base (`0x0030F088`) plus the NewtonScript seconds |

Dates stored in soups are usually `Time()` values, minutes since 1904. Those
are not affected by the seconds base.

### The 1993 base in the ROM

The 1993 base appears in two forms. A search of the whole 8 MB 717006 ROM
finds exactly these places:

- **In native code**, as three immediate additions in a row:
  - `TimeInSeconds()`: `add #0x0000c600; add #0x00960000; add #0x58000000`
    (adds `0x5896C600` = minus `0xA7693A00`, at `0x00089B74`)
  - `SetTimeInSeconds()`: `add #0x00003a00; sub #0x00970000; add #0xa8000000`
    (adds `0xA7693A00`, at `0x0008A230`)
  - `DateFromSeconds()`: the same three instructions (at `0x0008A89C`)
- **As data words**:
  - `0xA7693A00` (1993-01-01 in seconds since 1904, 2,808,691,200) at
    `0x0030F088`, the literal pool of `SetSysAlarm()`
  - `0x0B292600` (1993-01-01 in minutes since 1904, 46,811,520, as a
    NewtonScript integer, that is times 4) at `0x00420750`, `0x00420798` and
    `0x004DCA14`. These are in the ROM's NewtonScript code and data; I did
    not trace which functions they belong to.

## The limits

| Value | Format | Wraps on |
|---|---|---|
| `TimeInSeconds()`, 1993 base (original ROM) | 30-bit NewtonScript integer, seconds | 2010-01-05 |
| same, 2008 base (Einstein until 2026) | same | 2025-01-04 |
| same, 2024 base (Einstein now) | same | 2041-01-04 |
| RTC, `RealClockSeconds()`, kernel alarms | unsigned 32-bit seconds since 1904 | **2040-02-06 06:28:16** |
| `Time()`, `RealClock()` | minutes = seconds / 60 | follows the seconds: **2040-02-06** |
| `Time()` if the minutes did not come from 32-bit seconds | 30-bit NewtonScript integer, minutes | 2924-10-06 |

A NewtonScript seconds value is signed, so the 2024 base covers 2006-12-27
to 2041-01-04. Seconds values before 2006-12-27 (for example a
`TimeInSeconds()` value stored long ago) can no longer be represented.

## The symptom: the Newton does not go to sleep

Found on Windows in October 2026 (the 150-day offset described below made it
visible there first). Pressing the power button turned the screen off for a
moment and back on; quitting with Fast start timed out.

1. Something in the NewtonScript world calls `SetSysAlarm()` with a time in
   seconds. With the 2008 base, today's date no longer fits into 30 bits, and
   the alarm time arrives as early 1993.
2. `TRealTimeClock::SetAlarm()` (`0x0019C0DC`) converts the `TTime` to
   seconds. The alarm is before the RTC (1996), so `RealTimeAlarm::Fire()`
   (`0x0019C324`) sends the alarm message (`SendForInterrupt`) to the
   NewtonScript task's port `gNewtPort` (`0x0C1054A8`) at once.
3. The NewtonScript task handles the message and sets the alarm again, about
   every 3.4 ms in Einstein.
4. To power off, the power manager sends `'newt' 'idle' 'powr'` to the
   NewtonScript task, which then runs `CyclePower()` (`0x00192764`) itself.
   After `BatteryShutDown`, `LCDPowerOff` and `TabShutDown`, `CyclePower()`
   checks `gNewtPort` for a waiting message (`TUPort::Receive` with a
   message filter of 1 and no wait, at `0x0019281C`). If one is waiting, it
   does not call `IOPowerOffAll()` and `PowerOffSystem()`, but powers
   everything back on. The alarm message was always waiting.

A physical Newton with a Y2K10 fix based on 2008 should show the same: no
sleep since 2025-01-04 whenever such an alarm is set.

## What Einstein patches

In `Emulator/JIT/Generic/TJITGenericROMPatch.cpp`, for the US ROM only (the
other three columns are `kROMPatchVoid`):

| Address | Original | Patch | Name |
|---|---|---|---|
| `0x00255578` | `RealClockSeconds()` | replaced: returns host local time in seconds since 1904 (`TInterruptManager::GetHostLocalTime()`) | `RealClockSeconds` |
| `0x00089B80` | `mov r0, r0, lsl #2` | `r0 = (r0 - 978220800) << 2` | `FTimeInSeconds` |
| `0x0008A8A8` | `mov r0, r13` | `r1 += 978220800; r0 = r13` | `FDateFromSeconds` |
| `0x0008A20C` | `mov r12, r13` | `mov pc, lr`: `SetTimeInSeconds()` does nothing, the time comes from the host | `Ignore setting time` |
| `0x00420750` | `0x0B292600` | `0x0F0C3F00` (252460800) | `Time base (1/4)` |
| `0x00420798` | `0x0B292600` | `0x0F0C3F00` | `Time base (2/4)` |
| `0x004DCA14` | `0x0B292600` | `0x0F0C3F00` | `Time base (3/4)` |
| `0x0030F088` | `0xA7693A00` | `0xE1B7B100` (3786912000) | `Time base (4/4)` |

The values for the 2024 base:

| Value | Decimal | Hex | How |
|---|---|---|---|
| 1993-01-01 to 2024-01-01, seconds | 978,220,800 | `0x3A4E7700` | 11,322 days |
| 2024-01-01 in minutes since 1904, as NS integer | 252,460,800 | `0x0F0C3F00` | 43,830 days × 1440 × 4 |
| 2024-01-01 in seconds since 1904 | 3,786,912,000 | `0xE1B7B100` | 43,830 days × 86,400 |

The same formulas give the old 2008 values exactly (473,299,200;
218,799,360; 3,281,990,400), which is a good check when choosing another
base.

Also fixed in October 2026: on Windows, the emulated RTC used an epoch 150
days and one hour off (`kEpochInNewtonBase`, "The MSWindows time is 150 days
off"). The RTC and `RealClockSeconds()` now share
`TInterruptManager::GetHostLocalTime()`.

## Patching the other ROMs

Einstein applies the time patches only to the US ROM. The German MP2100 ROM,
the eMate 300 ROM and Watson run without any Y2K10 fix. To add them:

1. Search the ROM for the data words `0xA7693A00` and `0x0B292600`. In the US
   ROM there is one `0xA7693A00` (in `SetSysAlarm`) and three `0x0B292600`.
   Check each hit: a value in a literal pool or in NewtonScript data, not an
   instruction that happens to have that encoding.
2. Search for the instruction pairs that build the base in native code:
   `add rX, rY, #0x0000c600` followed by `add #0x00960000` and
   `add #0x58000000` (`TimeInSeconds`), and `add #0x00003a00`,
   `sub #0x00970000`, `add #0xa8000000` (`SetTimeInSeconds`,
   `DateFromSeconds`). In the US ROM there are three such sequences.
3. Find `RealClockSeconds()` (it calls `TURealTimeAlarm::Time()`,
   `TTime::ConvertTo(0x384000)`, `GMTOffset()` and `DaylightSavingsOffset()`)
   and `AdjustRealTimeClock` (it loads `0xAE070B30` and accesses
   `0x0F181000`; called right before `OsBoot`).
4. Add the addresses in the second, third or fourth column of the
   `T_ROM_PATCH` and `TJITGenericPatch` lines: MP2100 D, eMate 300, Watson.
   `CalculateROMREXCheckSums` already has all four, as an example.

The functions are probably at different addresses, but the code should be
the same, because all ROMs are built from the same NewtonOS 2.1 sources.

## Patching physical Newtons

On a real device, a patch package can change the same places, but some
things work differently than in Einstein:

- The patch is installed after boot, so `AdjustRealTimeClock` has already
  reset the RTC to 1996 by then. Einstein has no such limit, because its JIT
  patches are in place from the first instruction, but a package cannot fix
  the boot check. The user (or the patch) has to set the time after every
  reset.
- `SetTimeInSeconds()` must store the time with the new base, so instead of
  ignoring the call as Einstein does, add 3,786,912,000 (2024 base in
  seconds since 1904) instead of 2,808,691,200.
- `RealClockSeconds()` on a real device reads the hardware RTC; nothing to
  change there.
- A fix based on 2008 needs to be re-released with the 2024 values above, or
  `SetSysAlarm()` alarms keep the Newton awake.

## Plan for 2040

On 2040-02-06 the 32-bit seconds since 1904 overflow, in the hardware RTC,
in `RealClockSeconds()` and in `RealClock()`, so `Time()` jumps back to 1904.
Moving the NewtonScript base does not help with that.

Because the RTC is a plain counter, the clock can be moved forward in
software. The idea that needs the least changes:

- **Run the clock 28 years behind.** Between 1901 and 2099 the calendar
  repeats every 28 years: the same leap years and the same weekdays. If the
  counter holds the date minus 28 years (2040 is stored as 2012), all leap
  year and weekday math in NewtonOS stays correct. Only the year has to be
  corrected where a date is shown or entered. The 32-bit counter then lasts
  until 2068.
- Shifts by other multiples of 4 years keep the leap years right, but the
  weekdays move, so the weekday code would need patching too.
- Crossing 2100 (not a leap year) needs extra care. Not yet checked: whether
  NewtonOS applies the 100 and 400 year rules at all.

What would have to change: every place where seconds or minutes become a
date and back (`DateFromSeconds`, `Date`, `TimeStr`, `SetTime`,
`TimeInSeconds`, and probably more), and stored `Time()` values in soups,
which would be 28 years off unless they are converted. In Einstein, `Time()`
alone could also be extended to 2924 by computing the minutes in `RealClock()`
from the 64-bit host time, but kernel seconds (RTC, alarms) would still wrap
in 2040.

## Useful addresses (717006)

| Address | Symbol |
|---|---|
| `0x000452FC` | `AdjustRealTimeClock` |
| `0x00089B4C` | `FTime` |
| `0x00089B64` | `FTimeInSeconds` |
| `0x0008A20C` | `FSetTimeInSeconds` |
| `0x0008A868` | `FDateFromSeconds` |
| `0x00192764` | `CyclePower()` |
| `0x0019BEF0` | `TRealTimeClock::Alarm()` (fires due alarms) |
| `0x0019C0DC` | `TRealTimeClock::SetAlarm(…, TTime, …)` |
| `0x0019C2C4` | `TRealTimeClock::PrimRawSetAlarm()` |
| `0x0019C324` | `RealTimeAlarm::Fire()` |
| `0x0019C4CC` | `GetRealTimeClock()` |
| `0x0019C4EC` | `SetRealTimeClockAlarm()` |
| `0x00255578` | `RealClockSeconds()` |
| `0x002555F4` | `RealClock()` |
| `0x0030EEEC` | `FSetSysAlarm` |
| `0x0C1054A8` | `gNewtPort` |
