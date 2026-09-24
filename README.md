![cold-chain-monitor-c-rp2350](https://raw.githubusercontent.com/mytechnotalent/cold-chain-monitor-c-rp2350/main/cold-chain-monitor-c-rp2350.png)

<br>

## FREE Reverse Engineering Self-Study Course [HERE](https://github.com/mytechnotalent/reverse-engineering)
## FREE Embedded Hacking Course [HERE](https://github.com/mytechnotalent/Embedded-Hacking)

<br>

# OPERATION COLD IRON

### The Cold Chain & Agricultural Environmental Monitor
#### Act I of OPERATION COLD IRON

<br>

***
**LEGAL DISCLAIMER:**
The information, tools, and code provided in this repository and course are strictly for educational, research, and defensive purposes only. 

You are explicitly prohibited from using any materials contained herein to access, test, modify, or exploit any device, network, or system that you do not own 100% or for which you do not have explicit, documented, and legally binding authorization to interact with.

By using this repository and course, you acknowledge and agree that:

1. Any illegal, unauthorized, or malicious use of this information is solely your responsibility.
2. The author(s) and contributor(s) of this repository and course shall not be held liable for any damages, legal repercussions, criminal charges, or unauthorized actions resulting from the use, misuse, or abuse of the contents herein.
3. You will comply with all applicable local, state, national, and international laws regarding cybersecurity and computer fraud.

**IF YOU DO NOT AGREE WITH THESE TERMS, DO NOT USE THIS REPOSITORY AND COURSE.**
***

<br>
<br>

> Hello, friend.
>
> I want to tell you about the quietest crime in the world.
>
> Nobody gets shot. Nothing explodes. A refrigerator in a depot you will never
> visit gets a firmware update it never asked for. Three weeks later, two hundred
> children in a town with one road get a vaccine that stopped being a vaccine
> eleven hours before it reached them. The wall thermometer read five degrees
> above zero. The monitor read minus eighteen.
>
> The monitor was lying. That is the part nobody believes.
>
> The lie is the product.

<br>

## THE SYSTEM

NorthPharma owns the cold chain. Not all of it. Enough of it. The monitors, the
gateways, the insurance, the audits. When a shipment spoils, they pay a claim,
raise the premium, and sell a newer monitor. Failure is not a bug in their
business. Failure is the business.

Their security arm is called FROSTLINE. You will not find it on an org chart. It
exists so that when a million doses die, no one can point at a person. It does
not break into buildings. It makes the buildings believe they are safe.

<br>

## THE STAKES

**1.4 billion doses** are in motion right now. Forty countries. Nine thousand
clinics. Rural depots and coastal labs and the last mile on the back of a
motorcycle. Every one of them trusts a five dollar chip to tell the truth about
temperature. Every one of those chips was signed by the same company that gets
paid when the chip is wrong.

That is not a supply chain. That is a loaded weapon pointed at a nursery, and the
safety is a firmware flag.

<br>

## NIGHTINGALE

She was a firmware engineer. Thirteen years. She wrote the bootloader they all
trust. Then she found a function in the shipping build that was not in the
repository, and she understood exactly what she was looking at.

She copied the image to a dead drop and sent one message to a crew that does not
exist on paper. Us. **WHITEOUT**.

Then the messages stopped.

<br>

## THE MACHINE

The firmware in this repository is her copy. On a breadboard it is a toy: a Pico
2, a temperature sensor, an LCD, three LEDs, a button, a servo, an infrared eye,
and a radio.

Two doors are open. **The radio** takes an order from anyone on the band.
**The infrared eye** takes an order from anyone with a universal remote. Neither
one asks who is talking. And the servo will drive the cold-store damper on
command, because a machine that cannot tell friend from enemy will obey the last
voice it heard.

Then there is the face of the thing. The LEDs. The LCD. The calm heartbeat. They
do not watch the vaccine. They watch your belief that someone is watching the
vaccine. When the firmware lies, they lie beautifully, in green.

<br>

## THE JOB

You do not have to be a hero. You just have to be honest. The machine is lying.
Make it tell the truth.

1. **Read the dead.** Take her image apart. Map the sensor, the display, the
   radio, the LEDs, the button, the servo, and the infrared eye.
2. **Find the doors.** Replay the remote. Forge the telemetry. Watch the hub
   believe a ghost.
3. **Lock them.** Seal every frame with **XChaCha20-Poly1305**, keyed through
   **Argon2id**, so a forged reading dies on the authentication tag and never
   reaches a clinic.
4. **Fix the face.** Make the lights and the screen report the truth, even when
   the truth is that the store is dying.

This document is the manual for the job. Work it on a breadboard. When the green
light lies to you, remember what it is: not a warning. A witness.

Goodbye, friend.

<br>

## A NOTE ON THE ROADMAP

This capstone is the last defense of the cold chain. In the same way the course's
FINAL projects became full capture-the-flag investigations, **OPERATION COLD
IRON** now ships its own CTF: a stolen firmware image, two open doors, and a
forged frame that must be caught before the doses ship. The investigation lives
in the companion repository:

- [OPERATION COLD IRON CTF](https://github.com/mytechnotalent/CTF_cold-chain-monitor)

This repository is the defended device. The CTF repository is the compromised
one.

---


<br>

## WHERE THIS FITS: OPERATION COLD IRON

This repository is **Act I (COLD IRON)** of the ten-act OPERATION COLD IRON saga.
Acts I and II are the vulnerability-only foundation; the malware track begins at
Act III. The full spine is in [SAGA.md](SAGA.md).

- This act: Act I, COLD IRON, the cold chain
- Next act: Act II, IRON GATE, [access-gate](https://github.com/mytechnotalent/access-gate)
- Companion CTF: [CTF_cold-chain-monitor](https://github.com/mytechnotalent/CTF_cold-chain-monitor)


<br>

## THE MINISTRY

The Ministry runs the state: the surveillance, the cold chain, the gates, the
pipelines. NorthPharma is one of its deniable industrial fronts, and FROSTLINE is
the contractor that does the work no Ministry letterhead will admit to. Against
them is WHITEOUT, and the engineer who copied this image, NIGHTINGALE. This act is
one node of the Ministry's industrial edge. TELESCREEN, the surveillance backbone
that watches it, comes after the ten.

An adversarial, evidence-based audit of this act, including its honest limitations, is in [NATION-STATE-REVIEW.md](NATION-STATE-REVIEW.md).


<br>

## How This Project Fits the Embedded Hacking Course

This repository is the capstone integration for the
[Embedded Hacking](https://github.com/mytechnotalent/Embedded-Hacking) course.
Each earlier module teaches one peripheral or language concept in isolation;
this project wires several of them into a single, tested product and adds the
wireless and security layers the course builds toward.

| Embedded Hacking module | Concept you learn | Where it lives here |
| ----------------------- | ----------------- | ------------------- |
| Week 1: Introduction, Ethics, Scoping | Authorized lab work | The spoofing lab is self-contained and authorized by design |
| Week 3: RP2350 Architecture and Firmware Analysis | Bare-metal targets, ELF/UF2, SWD | Pico SDK build, `build/*.uf2`, Debug Probe flash via OpenOCD |
| Weeks 4-6: Variables, Integers/Floats, Static | Data types, GPIO | `src/monitor.c` state, LED on GP25 |
| Week 7: Constants with 1602 LCD I2C | I2C bus, HD44780 commands | `src/display.c` |
| Week 9: Operators with DHT11 | Bit operations, edge timing | `src/sensor.c` |
| Week 11: Structures and Functions | Modular design | `include/*.h` and `src/*.c` module boundaries |
| This project adds | UART AT driver, LoRa link, hub gateway, sender spoofing, artifact guardrail, strict testing | `src/radio.c`, `scripts/gateway.py`, `scripts/spoof.py`, `scripts/gen_packet.py`, `test/` |

If you have not worked through Weeks 7 and 9 yet, do those first: this project
assumes you are comfortable with I2C wiring and one-wire edge timing.

<br>

## Learning Objectives

By the end of this chapter and its labs you will be able to:

- Explain why sub-GHz LoRa is used for cold-chain telemetry and what its
  unauthenticated AT interface implies for security.
- Wire and drive a 1602 LCD through a PCF8574 I2C backpack.
- Read a DHT11 one-wire sensor by timing high pulses, validating the checksum,
  and enforcing edge timeouts.
- Design a fixed-size wire frame and parse it safely when the payload contains
  the delimiter character.
- Build an RP2350 firmware image with the Pico SDK and flash it over SWD or
  BOOTSEL.
- Run a hub gateway that logs telemetry and reacts to a threshold breach.
- Drive a tri-color status annunciator, an acknowledge button, a PWM damper
  servo, and an infrared maintenance remote.
- Demonstrate two unauthenticated control surfaces: a LoRa sender spoof and an
  NEC infrared replay.
- Derive a key with Argon2id and seal every frame with XChaCha20-Poly1305 so a
  forged reading fails authentication.
- Read and run a native host test suite with hardware mocks, and interpret line
  coverage.

<br>

## Prerequisites

- The [Embedded Hacking](https://github.com/mytechnotalent/Embedded-Hacking)
  breadboard (`EHP2_bb.png`) and parts list.
- Comfort with C, the Linux/macOS shell, and basic electronics.
- A Pico 2, a Debug Probe (recommended), a 1602 LCD with PCF8574 backpack, a
  DHT11, the full Embedded Hacking kit (3 LEDs, 3 resistors, a push button, an
  SG90 servo, a 1000uF capacitor, and a VS1838B infrared receiver plus NEC
  remote), two RYLR998 modules, and one USB-to-TTL serial adapter.
- Toolchain: Pico SDK 2.2.0+, `arm-none-eabi-gcc`, CMake, Ninja, Python 3, and
  (optionally) `typst` to rebuild the paper.

<br>

## Table of Contents

1. [Background](#background)
2. [System Architecture](#system-architecture)
3. [The Wire Protocol](#the-wire-protocol)
4. [The Cryptographic Envelope](#the-cryptographic-envelope)
5. [Hardware You Need](#hardware-you-need)
6. [Wiring the Node](#wiring-the-node)
7. [Build and Flash](#build-and-flash)
8. [Lab 1: Bring-Up and Verify](#lab-1-bring-up-and-verify)
9. [Lab 2: Inspect the Wire Protocol](#lab-2-inspect-the-wire-protocol)
10. [Lab 3: Spoofed Sender Injection](#lab-3-spoofed-sender-injection)
11. [Lab 4: Infrared Replay and the Damper](#lab-4-infrared-replay-and-the-damper)
12. [Lab 5: Lock the Doors](#lab-5-lock-the-doors)
13. [Troubleshooting](#troubleshooting)
14. [Testing Philosophy and Coverage](#testing-philosophy-and-coverage)
15. [Generating Packet Artifacts](#generating-packet-artifacts)
16. [Code Standards](#code-standards)
17. [Project Layout](#project-layout)
18. [Glossary](#glossary)
19. [Further Reading](#further-reading)
20. [License](#license)

<br>

## Background

### Cold chain and agricultural monitoring

Perishable goods (vaccines, produce, dairy, seafood) must stay within a
temperature band from production to consumption. A "cold chain" is the
refrigerated logistics path that keeps that promise. On farms and in small
distribution operations the monitoring endpoints must be inexpensive, run for a
long time on small power budgets, and cover distances far larger than Wi-Fi can
reach. That combination is exactly what sub-GHz LoRa radios are built for.

### Why the radio is the interesting part

The RYLR998 is a low-cost LoRa module with a serial AT interface:

- `AT+SEND=<address>,<len>,<payload>` transmits.
- `+RCV=<address>,<len>,<payload>,<rssi>,<snr>` reports a received frame.

The module ships with **no per-frame authentication**. Any radio on the same
band and network identifier can set `AT+ADDRESS=<victim>` and inject frames the
receiver cannot distinguish from the real node. This project makes that attack,
and its fix, observable in a lab.

### The two on-wire problems this project solves

1. **Payloads that contain commas.** The telemetry body is JSON, e.g.
   `{"n":7,"s":12,"t":235,"h":610}`, and JSON uses commas. A naive receiver
   that splits the line on commas corrupts the payload. The correct discipline
   is the **declared-length** rule: slice exactly `L` characters after the
   second comma and require the next character to be a comma.
2. **Telling a node from an impostor.** The hub records the sender address
   exactly as the radio reports it. That is the gap the spoofing lab exploits.

### Inter-Integrated Circuit (I2C)

I2C is a two-wire bus: **SDA** (data) and **SCL** (clock), each pulled up to the
supply rail. A controller (the Pico) addresses a target by its 7-bit address and
writes or reads bytes. The 1602 LCD backpack carries a **PCF8574** I/O expander
at address `0x27`; the firmware bit-bangs the HD44780 nibble protocol over that
expander. Pull-ups are mandatory: the firmware enables the internal ones and the
backpack usually adds its own.

### The DHT11 one-wire protocol

The DHT11 is a low-cost digital temperature and humidity sensor. It speaks a
custom single-wire protocol:

1. The host pulls the line low for at least 18 ms (the **start pulse**), then
   releases it and enables its pull-up.
2. The sensor answers with an 80 us low, then an 80 us high handshake.
3. The sensor sends **40 bits**. Each bit begins with a 50 us low, then a high
   pulse whose width encodes the value: about 26-28 us for a `0`, about 70 us
   for a `1`.
4. Five bytes follow: humidity integer, humidity decimal, temperature integer,
   temperature decimal, and a checksum equal to the low byte of their sum.

Reading it means timing edges on the order of tens of microseconds, so the
firmware uses an 18 ms host pulse, a 50 us bit-classification threshold, and a
240 us per-edge timeout so a dead or unplugged sensor fails fast instead of
hanging the loop.

### Universal Asynchronous Receiver/Transmitter (UART) and AT commands

The RYLR998 is driven over a UART at 115200 baud using CRLF-terminated ASCII
commands. The firmware writes `AT+SEND=...` and drains inbound `+RCV=...` lines.
Because the radio is a separate processor, its configuration (address, network
identifier, band) persists until changed; the firmware and the hub each
provision their own radio at start-up so they agree before any telemetry flows.

### Cyclic Redundancy Check (CRC)

`src/crc.c` implements CRC-16/CCITT-FALSE (`poly = 0x1021`, `init = 0xFFFF`,
check value `0x29B1` for `"123456789"`). It is provided as a reusable
integrity diagnostic and exercised by the test suite. It is **not** part of the
LoRa frame in this project; the lesson is the *absence* of authentication, not
the absence of a checksum.

<br>

## System Architecture

There are four roles:

| Role | Runs on | Job |
| ---- | ------- | --- |
| **Node** | Pico 2 firmware | Samples the DHT11, renders the LCD, transmits telemetry every 5 s |
| **Hub** | laptop + USB-TTL radio | Logs every `+RCV` frame to `telemetry.csv`, fires the freeze reply |
| **Edge simulator** | laptop + USB-TTL radio | Pretends to be a second node (`scripts/sim_edge.py`) |
| **Spoofing client** | laptop + USB-TTL radio | Impersonates a victim node and injects forged data (`scripts/spoof.py`) |

### Data flow

```text
+----------------------+                              +----------------------+
|   Pico 2 node        |        LoRa (sub-GHz)        |   Instructor hub     |
|   DHT11  -> GP4      |  AT+SEND=0001,<len>,<json>   |  USB-TTL radio       |
|   LCD    -> GP2/GP3  |----------------------------->|  scripts/gateway.py  |
|   Radio  -> GP8/GP9  |<-----------------------------|  telemetry.csv       |
+----------------------+  AT+SEND=<sender>,20,{...}   +----------------------+

+----------------------+                              +----------------------+
|   Attacker laptop    |  forged AT+SEND=<victim>,..  |   (same hub)         |
|   scripts/spoof.py   |----------------------------->|   logs it as victim  |
+----------------------+                              +----------------------+
```

### Firmware module map

| File | Responsibility |
| ---- | -------------- |
| `src/main.c` | Entry point: `stdio_init_all`, `monitor_init`, tick loop |
| `src/monitor.c` | State machine: I2C bus scan, init gating, transmit tick, receive tick |
| `src/sensor.c` | DHT11 one-wire sampling and JSON frame formatting |
| `src/display.c` | HD44780 driver over the PCF8574 backpack and line rendering |
| `src/radio.c` | RYLR998 provisioning, `AT+SEND` builder, `+RCV` parser, line pump |
| `src/status_led.c` | Red/yellow/green cold-chain annunciator and temperature mapping |
| `src/button.c` | Debounced acknowledge button around the internal pull-up |
| `src/servo.c` | 50 Hz PWM damper actuator for the cold-store vent |
| `src/ir_remote.c` | VS1838B edge timing and NEC remote decode |
| `src/chacha20.c` | ChaCha20 stream cipher and HChaCha20 subkey derivation |
| `src/poly1305.c` | Poly1305 one-time message authenticator |
| `src/crypto_aead.c` | XChaCha20-Poly1305 seal/open envelope |
| `src/blake2b.c` | BLAKE2b and the Argon2 variable-length hash H' |
| `src/argon2.c` | Argon2id core (BLAMKA, hybrid addressing) |
| `src/crypto_kdf.c` | Argon2id passphrase key derivation |
| `src/crc.c` | CRC-16/CCITT-FALSE diagnostic |
| `include/cold_chain_monitor.h` | Pin map, bus, and provisioning constants |

<br>

## The Wire Protocol

### Telemetry frame

The node sends one fixed-shape JSON body:

```json
{"n":7,"s":12,"t":235,"h":610}
```

- `n` is the node id (7)
- `s` is the monotonic sequence number
- `t` is temperature in tenths of a degree Celsius (235 = 23.5 C)
- `h` is relative humidity in tenths of a percent (610 = 61.0 %)

Integer tenths avoid floating point on the RP2350 and keep frames compact. The
body is padded into a fixed 48-byte artifact buffer, but only the actual body
length is transmitted:

```text
AT+SEND=0001,30,{"n":7,"s":12,"t":235,"h":610}
```

A sequence number below 10 produces a 29-byte body; a two-digit sequence
produces 30 bytes. The declared length always matches the bytes between the
second comma and the RSSI field.

### Declared-length slicing invariant

Given the substring `T` after the second comma:

```text
C = T[0 : L]   and   T[L] == ","
```

The receiver checks `T[L] == ","`, so a mismatch between the declared length and
the actual payload is a parse error rather than silent corruption. This is what
makes comma-bearing JSON payloads safe to carry.

### LCD layout

```text
T:23.0C H:61.0%
N:07 S:0042 OK
```

Line 1 is the reading; line 2 is the node id, sequence number, and `OK`/`!!`
for the last sensor read. The onboard LED (GP25) blinks **three times** on a
successful transmit and **once** on any inbound `+RCV` report.

### Radio provisioning

For the link to work, both radios must share the same **network identifier**
and each must have the address the other targets:

- Firmware sets its own radio: `AT+ADDRESS=7`, `AT+NETWORKID=18`.
- `gateway.py` sets the hub radio: `AT+ADDRESS=1`, `AT+NETWORKID=18`.

Both radios must also be the **same band variant** (for example 915 MHz or
868 MHz); band and RF parameters are left at factory defaults, so use matching
modules.

### Timing

| Quantity | Value |
| -------- | ----- |
| Telemetry interval | 5000 ms |
| DHT11 host start pulse | 18000 us |
| DHT11 bit threshold | 50 us |
| DHT11 per-edge timeout | 240 us |
| LCD I2C clock | 100000 Hz |
| Radio UART baud | 115200 |
| Freeze threshold | -50 tenths (-5.0 C) |
| Actuator reply payload | `{"cmd":"heat","v":1}` (20 bytes) |

<br>

## The Cryptographic Envelope

The radio is the first open door. The fix is authenticated encryption: every
telemetry frame is sealed so a forged reading dies at the authentication tag
instead of reaching a clinic. The full implementation lives in `src/chacha20.c`,
`src/poly1305.c`, and `src/crypto_aead.c`, and every primitive is checked against
its published test vectors in the native suite.

### Why XChaCha20-Poly1305

- **256-bit key, 192-bit nonce.** The extended nonce means nonces can be drawn at
  random forever, so the node never needs a shared counter that a reboot could
  reuse.
- **AEAD in one pass.** Confidentiality and integrity come from one operation;
  the associated data (node id and sequence) is authenticated even though it is
  not encrypted.
- **Constant-time software.** ChaCha20 has no data-dependent table lookups, so it
  has no cache-timing surface. The RP2350 has no hardware AES engine (it
  accelerates SHA-256 only), which makes software AES both slower and riskier on
  this silicon.
- **128-bit Poly1305 tag.** Guessing a valid tag succeeds with probability
  2^-128.

### Why Argon2id

A passphrase is not a key. Argon2id (RFC 9106) is the memory-hard password hash:
it mixes the passphrase with a salt across memory and time so an attacker cannot
cheaply recover the field passphrase from a captured image. The classroom profile
is `t=3`, `p=1`, `m=64` blocks to fit the RP2350 SRAM budget; raise it on the
gateway.

### Envelope layout

The sealed frame is carried as hex inside the `AT+SEND` payload:

```text
nonce[24] || ciphertext[L] || tag[16]
```

The receiver recomputes the Poly1305 tag over the associated data and ciphertext,
compares it in constant time, and only then decrypts. This envelope is wired end
to end: `src/monitor.c` seals every frame with `src/envelope.c`, `scripts/gateway.py`
authenticates before it parses or acts, and `scripts/spoof.py` can no longer
inject a believable frame. Authenticated telemetry carries the node id as
associated data, so a replayed or forged sender is rejected before the JSON is
ever parsed.

<br>

## Hardware You Need

Full parts list with links: [PARTS.md](PARTS.md).

| Qty | Part | Notes |
| --- | ---- | ----- |
| 1 | Raspberry Pi Pico 2 (RP2350) with headers | The node |
| 1 | Raspberry Pi Debug Probe | SWD flashing and UART0 console (recommended) |
| 1 | Full-size breadboard | |
| 1 | Assorted jumper wires | |
| 1 | 1602 LCD with PCF8574 I2C backpack | Address `0x27` |
| 1 | DHT11 temperature/humidity sensor | 3-pin module or bare sensor |
| 1 | 10K resistor | Only if your DHT11 has no onboard pull-up |
| 3 | 5mm LEDs (red, yellow, green) | Cold-chain annunciator |
| 3 | 100, 220, or 330 Ohm resistors | One per LED |
| 1 | Push button (tactile switch) | Acknowledge and re-seal, active low |
| 1 | SG90 servo motor | Cold-store damper actuator |
| 1 | 1000uF 25V capacitor | Bulk decoupling on the servo 5V rail |
| 1 | VS1838B infrared receiver | Maintenance override input |
| 1 | NEC-compatible infrared remote | Maintenance override trigger |
| 3 | RYLR998 LoRa modules with antennas | 2 for the telemetry loop, 3 for the live spoof lab |
| 2 | USB-to-TTL serial adapters (FTDI FT232, CP2102, or CH340), 3.3V logic | 1 for the hub, 1 for the attacker in the spoof lab |
| 4 | USB cables | Pico 2, Debug Probe, and serial adapter(s) |

### How many radios do you actually need?

| Goal | Radios | What is connected |
| ---- | ------ | ----------------- |
| Legitimate telemetry loop (Labs 1-2) | **2** | 1x RYLR998 on the Pico (UART1) + 1x RYLR998 on a USB-to-TTL adapter (the hub) |
| Live spoof lab (Lab 3, watch it land at the hub) | **3** | the 2 above + 1x RYLR998 on a second USB-to-TTL adapter (the attacker) |
| Spoof concept with no extra hardware | 2 or 0 | read-and-run the offline parser demo in Lab 3 |

A radio never receives its own transmission, and the hub radio is busy listening
as `gateway.py`, so the live attack needs a separate attacker radio. The 2-radio
kit runs the whole legitimate system; only the live spoof observation needs the
third.

> Serial adapter warning: the RYLR998 is **not** 5V tolerant. Use a
> **3.3V-logic** USB-to-TTL adapter (or set its jumper to 3.3V).

<br>

### How each part works

Every part in the bill of materials does one physical job and one job in this act:

| Part | How it works | Role in this act |
| ---- | ------------ | ---------------- |
| 1x Full-size breadboard (long) | The two columns of spring-clip tie points sit on a 0.1 inch grid, so every hole in a row is bridged by a metal clip, while the two outer power rails run the full length and distribute power and ground to the whole build. | It is the substrate that holds the Pico 2, LCD, sensor, servo, and radio headers and distributes 3.3V and 5V across the node. |
| 1x Assorted jumper wires (male-to-male, male-to-female, female-to-female) | Male pins push into breadboard tie points or female headers, female sockets slide over the Pico 2, LCD, and servo header pins, and male-to-female leads bridge a breadboard row to a module header. | They carry power and the I2C, one-wire, UART, PWM, IR, and GPIO signals between the boards. |
| 1x Raspberry Pi Pico 2 with header | The RP2350 pairs two Arm Cortex-M33 cores with 3.3V logic and a GPIO block that exposes ADC, I2C, UART, and PWM, plus the onboard GP25 LED. | It is the cold-chain node controller that reads the DHT11, decodes the IR override, drives the annunciator, damper servo, and LCD, and runs the sealed protocol over the LoRa link. |
| 1x Raspberry Pi Pico Debug Probe | It drives the two-wire SWD port (SWCLK and SWDIO) to flash and single-step the target, and it also presents a USB UART bridge for the serial console. | It flashes and debugs the cold-chain node and shows the 115200 console during the labs. |
| 2x USB A-male to USB micro-B cables | USB carries 5V power and a data channel on the same cable, so one cable powers the Pico 2 and presents its USB CDC console while the other powers the Debug Probe and carries its SWD and UART traffic. | They power the two boards and carry the console and debug links. |
| 3x 5mm LEDs (1 red, 1 green, 1 yellow) | An LED is a diode with a forward voltage drop of roughly 2V, so current flows only from the anode to the cathode, and a GPIO pin set high sources that current and lights the lamp. | They are the red breach, yellow warning, and green nominal cold-chain annunciator, and exactly one is lit per state. |
| 3x 100, 220, or 330 Ohm resistors | A resistor in series with each LED sets the current by Ohm's law, I equals (supply minus forward voltage) divided by resistance, which protects the LED and keeps the GPIO within its current limit. | One resistor per lamp limits the LED current on each of the three annunciator pins. |
| 1x Push button (tactile switch) | The switch shorts its input pin to ground when pressed, and the RP2350 internal pull-up holds that pin high at rest so the press reads as active low. | It is the operator acknowledge and re-seal button that clears a breach and drives the damper back to sealed. |
| 1x 1602 LCD with PCF8574 I2C backpack | The HD44780 controller takes a 4-bit nibble protocol with register-select and enable strobes, and the PCF8574 I2C expander latches those eight control lines so the whole display is driven over two I2C wires at address 0x27. | It renders the two-line telemetry readout, temperature, humidity, and link status. |
| 1x DHT11 temperature and humidity sensor | The host pulls the single data line low for a start pulse, then the sensor answers with 40 bits of humidity, temperature, and checksum timed by pulse widths, and the checksum must match. | It is the cold-store temperature and humidity source that decides the nominal, warning, and breach bands. |
| 1x SG90 servo motor | The servo expects a 50 Hz PWM signal whose high pulse of 1 to 2 ms selects the shaft angle, and a Pico PWM slice generates that pulse train. | It is the cold-store damper actuator that vents on a breach and seals otherwise. |
| 1x 1000uF 25V capacitor | Wired across the servo 5V rail and ground, the capacitor is a bulk reservoir that supplies the motor inrush current and smooths the rail while the servo starts. | It keeps the damper move from browning out the RP2350 and resetting the node. |
| 1x Infrared (IR) receiver (VS1838B) | The receiver pairs a photodiode with a 38 kHz bandpass demodulator that ignores ambient light and outputs an active-low logic pulse for each IR burst. | It is the maintenance override input that captures the NEC frame on GP5. |
| 1x Infrared (IR) remote controller (NEC-compatible) | The remote emits its bursts modulated at 38 kHz in the NEC frame, a 9 ms leader followed by 32 bits where the address and command are each sent with their bitwise complements for validation. | It triggers the vent 0x47 and seal 0x45 maintenance override commands. |
| 1x RYLR998 LoRa radio module | The module is configured and driven over UART with AT commands and carries sub-GHz LoRa packets, and its logic pins are 3.3V only so the radio must never see 5V. | It is the UART1 telemetry and command link that carries the sealed frames between the node and the hub, and it is not 5V tolerant. |

<br>

## Wiring the Node

### Pin map

This is the authoritative map; it is defined in
`include/cold_chain_monitor.h` and enforced by the test suite.

| Peripheral | Signal | Pico 2 GPIO |
| ---------- | ------ | ----------- |
| DHT11 | DATA (one-wire) | **GP4** |
| 1602 LCD (PCF8574) | SDA (I2C1) | **GP2** |
| 1602 LCD (PCF8574) | SCL (I2C1) | **GP3** |
| RYLR998 | RX <- Pico TX (UART1) | **GP8** |
| RYLR998 | TX -> Pico RX (UART1) | **GP9** |
| Infrared receiver | OUT (VS1838B) | **GP5** |
| Servo | PWM signal | **GP14** |
| Red breach LED | anode | **GP16** |
| Yellow warning LED | anode | **GP17** |
| Green nominal LED | anode | **GP18** |
| Acknowledge button | to ground | **GP15** |
| Onboard LED | heartbeat | GP25 |
| Debug Probe / UART0 console | TX | GP0 |
| Debug Probe / UART0 console | RX | GP1 |

> Note: GPIO 2/3 are the classic I2C1 pins used throughout the Embedded Hacking
> breadboard; this project's map matches that board.

### 1602 LCD with I2C backpack

| LCD backpack | Pico 2 |
| ------------ | ------ |
| VCC | 3.3V |
| GND | GND |
| SDA | GP2 |
| SCL | GP3 |

### DHT11

| DHT11 | Pico 2 |
| ----- | ------ |
| VCC | 3.3V |
| DATA | GP4 |
| GND | GND |

If your DHT11 has no onboard pull-up, add a **10K resistor between DATA and
3.3V**. The firmware also enables the internal pull-up, but the external
resistor makes reads far more reliable over jumper wires.

### Status LEDs

| LED | Pico 2 | Series resistor |
| --- | ------ | --------------- |
| Red (breach) | GP16 (anode) | 220-330 Ohm to GND |
| Yellow (warning) | GP17 (anode) | 220-330 Ohm to GND |
| Green (nominal) | GP18 (anode) | 220-330 Ohm to GND |

Exactly one lamp is lit at a time. Green is nominal below **-5.0 C**, yellow is
the warning band from **-5.0 C** up to (but not including) **0.0 C**, and red is a
confirmed breach at **0.0 C** or above.

**LED behavior**

| State | Lamp | Indication | Meaning |
| ----- | ---- | ---------- | ------- |
| `STATUS_LED_OFF` | none | All dark | Returned for an invalid reading by `status_led_state_for_temperature`; the live failed-read path shows WARNING instead. |
| `STATUS_LED_NOMINAL` | Green | Solid | Temperature is below **-5.0 C** (`COLD_CHAIN_MONITOR_TEMP_WARN_TENTHS`). |
| `STATUS_LED_WARNING` | Yellow | Solid | Temperature is **-5.0 C** up to but not including **0.0 C**, or a sensor read failed. |
| `STATUS_LED_BREACH` | Red | Solid | Temperature is at or above **0.0 C** (`COLD_CHAIN_MONITOR_TEMP_BREACH_TENTHS`). |

The three annunciator lamps are always solid; `status_led.c` never blinks them.
Exactly one lamp is lit at a time, and `STATUS_LED_OFF` leaves all three dark.
The onboard GP25 LED is separate: it blinks 3 times (`MONITOR_LED_BLINKS`) after
each successful telemetry transmit and once on each valid inbound `+RCV` radio
line. There is no free-running heartbeat.

### Acknowledge button

| Button | Pico 2 |
| ------ | ------ |
| Leg 1 | GP15 |
| Leg 2 | GND |

The firmware enables the internal pull-up, so **do not** connect 3.3V to the
button. Pressing it seals the damper again and prints `ACK`.

### SG90 damper servo

| Servo | Pico 2 |
| ----- | ------ |
| Signal (orange) | GP14 |
| VCC (red) | 5V (VBUS) |
| GND (brown) | GND |

Solder the **1000uF capacitor** across the servo 5V and GND rails to absorb the
inrush current; without it the RP2350 can brown out when the servo moves.

### Infrared receiver

| VS1838B | Pico 2 |
| ------- | ------ |
| OUT | GP5 |
| VCC | 3.3V |
| GND | GND |

Point any NEC-compatible remote at the receiver. The firmware decodes the NEC
frame and acts on the maintenance override commands. There is no challenge and no
secret, which is the point of Lab 4.

**Using the remote**

Point the NEC remote at the VS1838B receiver on **GP5** and press a mapped button;
the receiver idles high and pulls low on a mark. Every valid frame prints `IR 0xNN`
on the console, and the two maintenance commands drive the damper servo.

| NEC command | Name | Action |
| ----------- | ---- | ------ |
| `0x47` | `MONITOR_IR_VENT_COMMAND` | Drives the cold-store damper servo to the vent position (`servo_vent`). |
| `0x45` | `MONITOR_IR_SEAL_COMMAND` | Drives the cold-store damper servo back to the sealed position (`servo_seal`). |

Any other decoded command is printed and ignored.

### RYLR998 LoRa radio

> The RYLR998 must be powered. Forgetting **VDD** is the single most common
> reason the link appears dead: the firmware prints telemetry while the radio
> sits silent.

| RYLR998 | Pico 2 |
| ------- | ------ |
| VDD | 3.3V |
| GND | GND |
| RXD | GP8 (Pico UART1 TX) |
| TXD | GP9 (Pico UART1 RX) |

Attach the antenna before transmitting. TX and RX are **crossed**: the radio's
RXD is the Pico's TX and vice versa.

### Debug Probe (recommended)

| Debug Probe | Pico 2 |
| ----------- | ------ |
| SWCLK | SWCLK (3-pin debug header) |
| SWDIO | SWDIO |
| GND | GND |
| UART TX | GP1 (Pico RX) |
| UART RX | GP0 (Pico TX) |
| GND | GND |

The firmware enables stdio on **both** UART0 (`115200`) and USB, so you can
watch boot output on the probe's console or on the Pico's own USB serial port.

### Peripherals used

Every part in the Act I bill of materials is exercised by the firmware:

| Peripheral | Role | Where it is used |
| ---------- | ---- | ---------------- |
| Red, yellow, green LEDs | Tri-color cold-chain annunciator | `status_led.c` drives exactly one lamp per state |
| Push button (GP15) | Operator acknowledge and re-seal | `button.c` consumes one debounced press in `monitor_handle_button` |
| 1602 I2C LCD | Two-line telemetry readout | `display.c` renders the formatted lines over I2C1 |
| DHT11 (GP4) | Temperature and humidity source | `sensor.c` reads the one-wire frame every telemetry interval |
| SG90 servo (GP14) | Cold-store damper actuator | `servo.c` vents on a breach and seals otherwise |
| 1000uF capacitor | Bulk decoupling on the servo 5V rail | Required to keep the RP2350 from browning out on servo moves |
| VS1838B IR receiver (GP5) | NEC maintenance input | `ir_remote.c` captures and decodes the frame |
| NEC IR remote | Maintenance command trigger | Sends the vent `0x47` and seal `0x45` commands |
| RYLR998 (UART1) | LoRa telemetry and command link | `radio.c` sends the sealed frame and pumps `+RCV` lines |
| Onboard GP25 LED | Transmit and receive indicator | `monitor.c` blinks it 3 times per transmit and once per `+RCV` |
| Debug Probe | SWD flashing and UART0 console | `stdio` is enabled on both UART0 `115200` and USB |

No Act I peripheral is unused.

<br>

### How the functionality works

Every input feeds the state machine in `monitor_step`, every output is driven
once per tick, and the interactive console mirrors both over UART0 and USB.

**What each input does**

| Input | Where | What the firmware does |
| ----- | ----- | ---------------------- |
| Infrared remote (VS1838B) | GP5 | Decodes the NEC frame and prints the mapped name and command byte, `VENT (0x47)` or `SEAL (0x45)`. VENT opens the cold-store damper; SEAL closes it. |
| Acknowledge button | GP15 to GND | One debounced press re-seals the damper and prints `BUTTON acknowledge -> damper sealed`. |
| DHT11 cold-chain sensor | GP4 | Reads temperature and humidity every tick and prints one live status line; a failed read prints `DHT read failed -> WARNING` and treats the reading as a breach. |
| RYLR998 LoRa link | UART1 GP8/GP9 | Pumps inbound `+RCV` lines and prints `RX from 0xNNNN, N bytes` for every report before the sealed telemetry is verified. |

**What each output does**

| Output | Where | What it shows |
| ------ | ----- | ------------- |
| Red BREACH LED | GP16 | Solid when the cold-store is at or above `0.0 C` or a read failed. |
| Yellow WARNING LED | GP17 | Solid at or below `-5.0 C`. |
| Green NOMINAL LED | GP18 | Solid while the cold-store is in band. Exactly one lamp is lit at a time and the annunciator never blinks. |
| 1602 I2C LCD | GP2/GP3 at `0x27` | Line 1 shows the reading (`T:2.3C H:45.0%`); line 2 shows the node id, sequence, and `OK`/`!!` (`N:01 S:0007 OK`). |
| Cold-store damper servo | GP14 | Sealed at 0 degrees, vented at 90 degrees on a breach or a remote VENT. |
| Onboard GP25 LED | GP25 | Pulsed once per monitor tick as a heartbeat. |

## Build and Flash

### 1. Install toolchain prerequisites

- Pico SDK 2.2.0+
- ARM GNU toolchain (`arm-none-eabi`)
- CMake and Ninja
- Python 3.x

**Linux:**

```bash
export PICO_SDK_PATH="$HOME/.pico-sdk/sdk/2.2.0"
```

**macOS:**

```bash
brew install cmake ninja arm-none-eabi-gcc python
export PICO_SDK_PATH="$HOME/.pico-sdk/sdk/2.2.0"
```

**Windows:** install PowerShell, Visual Studio Build Tools, CMake, Ninja,
Python 3, and the ARM embedded toolchain.

### 2. Build the firmware

```bash
mkdir -p build && cmake -S . -B build -G Ninja -DPICO_BOARD=pico2 -DPICO_PLATFORM=rp2350-arm-s && cmake --build build
```

Build-time artifact guardrail:

- The build regenerates `packet_artifact.h` from
  `scripts/packet_artifact.json` before compiling.
- The build fails if the committed `include/packet_artifact.h` is stale
  relative to the JSON artifact.

Generated outputs:

- `build/cold_chain_monitor_c_rp2350.elf` (primary firmware binary)
- `build/cold_chain_monitor_c_rp2350.uf2` (UF2 for BOOTSEL/picotool)
- `build/cold_chain_monitor_app.elf` / `.uf2` (backward-compatible copies)

### 3. Flash the RP2350

**BOOTSEL (drag-and-drop):** hold BOOTSEL while plugging in USB, then:

```bash
cp build/cold_chain_monitor_c_rp2350.uf2 /Volumes/RP2350/
```

**picotool:**

```bash
picotool load build/cold_chain_monitor_c_rp2350.uf2 -fx
```

*(If `picotool` is not on your PATH, invoke it from
`$HOME/.pico-sdk/picotool/*/picotool/picotool`.)*

**Debug Probe (SWD):** with `openocd` installed you can flash and reset
without touching BOOTSEL:

```bash
openocd -f interface/cmsis-dap.cfg -f target/rp2350.cfg \
  -c "program build/cold_chain_monitor_c_rp2350.elf verify reset exit"
```

### 4. Watch the console

Open the UART0 console (Debug Probe) or the Pico's USB serial port at
`115200`. On reset you should see:

```text
BOOT
I2C scan:
  found 0x27
```

`found 0x27` confirms the LCD backpack answered on the I2C bus. If a
peripheral fails, the firmware prints `INIT FAIL` and stops.

<br>

## Lab 1: Bring-Up and Verify

**Goal:** prove the node reads the sensor, drives the LCD, and reaches the hub.

1. Wire the node per the pin map and attach the antenna.
2. Build and flash the firmware.
3. Connect the hub radio to the laptop and find its port (`/dev/cu.usbserial-*`
   on macOS, `/dev/ttyUSB*` on Linux).
4. Start the hub gateway:

   ```bash
   python3 scripts/gateway.py --port /dev/cu.usbserial-XXXX --baud 115200
   ```

5. Watch the hub. It first provisions its radio (two `+OK` lines), then prints
   one frame every five seconds:

   ```text
   +OK
   +OK
   +RCV=7,29,{"n":7,"s":6,"t":245,"h":480},-11,10
   +RCV=7,29,{"n":7,"s":7,"t":245,"h":480},-11,11
   ```

**Checkpoint:** the LCD shows `T:24.5C H:48.0%`, the onboard LED blinks three
times every five seconds, and `telemetry.csv` gains one row per frame:

```text
utc,sender,length,payload,rssi_snr
2026-09-19T18:30:05+00:00,7,29,"{""n"":7,""s"":0,""t"":246,""h"":480}","-11,11"
```

**Theory check:** why does a successful frame prove the LCD initialized? Because
`monitor_init()` only returns true when every peripheral, including the LCD, is
ready; otherwise `main` prints `INIT FAIL` and never enters the loop.

<br>

## Lab 2: Inspect the Wire Protocol

**Goal:** see the declared-length rule in action.

1. Note the sequence numbers in the hub output. Each increments by one.
2. Copy a full `+RCV` line and confirm the declared length equals the number of
   characters between the second comma and the RSSI field.
3. Locate the payload, its declared length, and the two tail fields in
   `scripts/gateway.py` (`_split_payload`), and explain why finding the *first*
   comma would be a bug.
4. Challenge: the payload contains commas itself. Write down the exact index at
   which the parser must test for `","`.

**Checkpoint:** you can explain why `+RCV=0007,29,{"n":7,...}` must be sliced by
the number 29, not by delimiter counting.

<br>

## Lab 3: Spoofed Sender Injection

**Goal:** understand and demonstrate why the hub cannot tell a real node from an
impostor, then reason about the fix.

### The idea in plain language

Every `+RCV` frame carries a **sender address** that the radio reports. The hub
uses that address as the truth: it logs it, and it replies to it. Nothing on the
wire proves the sender really owns that address. So an attacker who can transmit
on the same band and network identifier simply tells their own radio "your
address is now `7`" (`AT+ADDRESS=7`) and sends a reading to the hub (address
`0001`). The hub records it as node 7 and reacts to it exactly like a genuine
frame.

The payload is forged to look alarming:

```text
AT+SEND=0001,29,{"n":7,"s":1,"t":-80,"h":500}
```

That claims node 7 reports **-8.0 C**, below the freeze threshold (-50 tenths, or
-5.0 C). The hub replies with a heat-actuator command to the claimed sender:

```text
AT+SEND=<claimed sender>,20,{"cmd":"heat","v":1}
```

The legitimate node keeps streaming its own sequence, none the wiser.

### Method A: live, with a third radio (recommended for the full effect)

Use three radios: the Pico node, the hub, and the attacker.

1. Hub: start the gateway on USB-to-TTL adapter #1.

   ```bash
   python3 scripts/gateway.py --port /dev/cu.usbserial-HUB --baud 115200
   ```

2. Attacker: on USB-to-TTL adapter #2, claim the victim address and inject.

   ```bash
   python3 scripts/spoof.py --port /dev/cu.usbserial-ATTACK --victim 7
   ```

3. Watch the hub. It logs a row whose `sender` is `7` and whose payload claims
   `-8.0 C`, then transmits the heat reply. `telemetry.csv` has no way to tell
   that row from a real one.

Why the third radio is required: a radio does not receive its own transmission,
and the hub radio is busy listening as `gateway.py`. The attacker must be a
separate radio transmitting **to** the hub while the hub listens.

### Method B: no extra radio, show the trust gap offline

The vulnerability is in the hub's parser and logging, which run on your laptop,
so you can see the trust gap with no radio at all. This feeds a forged `+RCV`
line straight to the gateway's parser:

```bash
python3 - <<'PY'
import sys
sys.path.insert(0, "scripts")
import gateway
line = '+RCV=7,29,{"n":7,"s":1,"t":-80,"h":500},-60,5'
print(gateway._parse_rcv(line))
PY
```

It prints a record whose sender is `7` and whose payload is the forged frozen
reading, proving the hub accepts the claimed identity. The same behavior is
asserted by the unit test `test_radio_spoofed_sender_attribution`.

With exactly two radios you can also run the loop in two passes: run
`gateway.py` and capture legitimate rows, then stop it and run `spoof.py` on the
same adapter to see exactly what the attacker would transmit. You just cannot
watch the forged frame arrive at the hub in the same moment.

### The important variables

- node address authentication (none on the wire today),
- the declared length field used to slice the payload from the tail fields,
- physical access to a radio that can impersonate a node address.

### Hardening checklist

- Append a keyed authenticator (HMAC or AEAD tag) to each frame and verify it
  before logging or acting on the payload.
- Maintain per-node sequence windows at the hub and reject replays or
  out-of-order frames.
- Reject frames whose declared length contradicts the payload invariant.
- Use `AT+ADDRESS`, band, and key provisioning controls to limit which radios
  may join, and remember that provisioning is cleartext on the wire.
- Rate-limit and quarantine senders that violate the sequence ledger.

<br>

## Troubleshooting

| Symptom | Likely cause | Fix |
| ------- | ------------ | --- |
| No `BOOT` on the console | Wrong console pins / not reset | Check UART0 GP0/GP1 or USB; press RESET |
| `INIT FAIL` with no `0x27` in the scan | LCD not answering | Check LCD VCC=3.3V, SDA=GP2, SCL=GP3, contrast pot |
| LCD shows blocks / nothing | Contrast or address | Turn the backpack contrast pot; confirm address `0x27` vs `0x3F` |
| LCD reads but values are `!!` | DHT11 not reading | Check DATA=GP4; add 10K pull-up to 3.3V; wait 1-2 s after power-up |
| `AT+SEND` sent but hub sees nothing | Radio unpowered / wrong band | **Power VDD**, attach antenna, use matching band modules |
| Hub sees nothing but `+OK` | Address/network mismatch | Confirm hub radio provisioned to `AT+ADDRESS=1`, `AT+NETWORKID=18` |
| `telemetry.csv` stays empty while `+RCV` prints | Gateway parser regression | Ensure `_split_payload` checks the comma at the declared length |

<br>

## Testing Philosophy and Coverage

Hardware bugs are expensive to find on the bench, so the firmware is written so
that almost all of it can be tested on the host. The suite compiles the real
`src/*.c` files against mock Pico SDK headers (`test/mock/`), replacing GPIO,
I2C, UART, and time with deterministic fakes.

- The mock GPIO can replay a recorded DHT11 waveform as an absolute time/level
  timeline, so the exact edge-timing decoder is exercised without a sensor.
- The mock I2C records every LCD byte, so rendered text can be decoded and
  asserted.
- The mock UART records outbound `AT+SEND` bytes and injects inbound `+RCV`
  lines.

Run the native test suite:

```bash
python3 scripts/run_tests.py
```

Or configure via CMake and CTest:

```bash
cmake -S test -B build-test -G Ninja && cmake --build build-test && ctest --test-dir build-test --output-on-failure
```

The suite has **89 cases (264 checks)** covering the full DHT11 waveform and every timeout
shape, the declared-length parser with comma-bearing payloads, and monitor ticks
for transmit, inbound-blink, sensor-timeout, and LCD-fail paths.

Verify **100% line coverage** of owned firmware modules:

```bash
python3 scripts/check_coverage.py
```

The harness itself is a small in-repo framework (`test/harness/`) so the repo
vendors no third-party code and every owned file obeys the coding standard.

<br>

## Generating Packet Artifacts

`scripts/gen_packet.py` writes the build-time generated header from the JSON
artifact:

- `scripts/packet_artifact.json` is the source of truth.
- `include/packet_artifact.h` is the generated header, committed for the build
  guardrail.

Why these constants are compiled into firmware:

- The RP2350 firmware has no runtime JSON parser or filesystem on this path.
- `include/packet_artifact.h` is generated from the JSON so the fixed frame
  size, node id, hub address, timeouts, and provisioning constants are embedded
  in flash.
- This is provisioned data; regenerate whenever you rotate node identity or hub
  addressing.

To sync the committed header from the JSON artifact:

```bash
python3 scripts/gen_packet.py --from-json scripts/packet_artifact.json --header-out include/packet_artifact.h
```

The `check_packet_artifact_header` CMake target fails the build when the
committed header is stale.

<br>

## Code Standards

This repository enforces unusually strict standards because the point is to
teach disciplined embedded and tooling practice, not just working code.

### C standard

- Every function body has **no blank lines**.
- Every function body is **at most eight lines** (Doxygen comment blocks and
  lone braces excluded).
- Every file, function, macro, type, and struct member carries Doxygen
  `@brief` documentation.
- Naming: `snake_case` files/functions, `UPPER_SNAKE` macros, `snake_case_t`
  types.

Run the C audit:

```bash
python3 scripts/audit_c_standard.py
```

### Python standard

- Strict PEP8, four-space indents, `snake_case`, 79-character lines.
- Every function has a NumPy-style docstring.
- Every function executable body is **at most eight lines**, with no
  exceptions.
- No blank lines inside function bodies.

Run the Python audit:

```bash
python3 scripts/audit_python_standard.py
```

Both audits must report nothing.

<br>

## Project Layout

- `src/main.c`: firmware entry point
- `src/monitor.c`: state machine tying sensor, LCD, and radio together
- `src/sensor.c`: DHT11 one-wire sampling and telemetry frame formatter
- `src/display.c`: 1602 LCD rendering over the PCF8574 I2C backpack
- `src/radio.c`: RYLR998 provisioning, AT-command interface, and `+RCV` parser
- `src/crc.c`: CRC-16/CCITT-FALSE helper
- `include/cold_chain_monitor.h`: board-level pin and provisioning configuration
- `include/packet_artifact.h`: generated packet artifact header
- `test/test_cold_chain_monitor_and_security.c`: comprehensive test suite
- `test/test_cold_chain_monitor.py`: VS Code test explorer Python adapter
- `test/mock/`: Pico SDK hardware mocks (GPIO, I2C, UART, timer)
- `test/harness/`: minimal in-repo test harness (strictly C-standard compliant)
- `scripts/gateway.py`: classroom hub with radio provisioning, CSV logging, freeze reply
- `scripts/spoof.py`: spoofed-sender injection client
- `scripts/sim_edge.py`: laptop edge-node simulator
- `scripts/gen_packet.py` / `scripts/packet_artifact.json`: packet artifact generator and source
- `scripts/run_tests.py`, `scripts/check_coverage.py`: test runner and coverage report
- `scripts/audit_c_standard.py`, `scripts/audit_python_standard.py`: code-standard auditors
- `paper.typ` / `paper.pdf`: classroom paper describing the protocol and exercise
- `.github/workflows/release.yml`: tag-driven UF2 release workflow

<br>

## Glossary

- **AT command**: a short ASCII command (`AT+...`) understood by the radio.
- **Cold chain**: the refrigerated logistics path that keeps perishables within
  a temperature band.
- **CRC**: cyclic redundancy check, a checksum for detecting corruption.
- **DHT11**: a low-cost temperature/humidity sensor using a custom one-wire
  protocol.
- **Declared length**: the byte count the sender claims for a payload; the
  receiver slices exactly that many characters.
- **HD44780**: the character-LCD controller inside a 1602 module.
- **I2C**: a two-wire bus (SDA/SCL) used here for the LCD backpack.
- **LoRa**: a long-range, low-power sub-GHz radio modulation.
- **PCF8574**: an I2C I/O expander that drives the LCD's parallel interface.
- **RSSI / SNR**: received signal strength and signal-to-noise ratio reported
  with each `+RCV` frame.
- **Spoofing**: sending frames that claim a sender address the attacker does not
  own.
- **UART**: a serial port used to talk to the radio.

<br>

## Further Reading

- Embedded Hacking course and breadboard:
  https://github.com/mytechnotalent/Embedded-Hacking
- Reverse Engineering self-study course:
  https://github.com/mytechnotalent/Reverse-Engineering
- `paper.typ` / `paper.pdf`: the classroom paper for this project.
- DHT11 datasheet and RYLR998 AT command reference (module vendors).

<br>

# Next
[OPERATION COLD IRON CTF](https://github.com/mytechnotalent/CTF_cold-chain-monitor)

<br>

# License
[MIT License](https://github.com/mytechnotalent/cold-chain-monitor/blob/main/LICENSE)
