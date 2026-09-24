# OPERATION COLD IRON - Hardware Parts and Products

---
**LEGAL DISCLAIMER:**
The information, tools, and code provided in this repository and course are strictly for educational, research, and defensive purposes only.

You are explicitly prohibited from using any materials contained herein to access, test, modify, or exploit any device, network, or system that you do not own 100% or for which you do not have explicit, documented, and legally binding authorization to interact with.

By using this repository and course, you acknowledge and agree that:
1. Any illegal, unauthorized, or malicious use of this information is solely your responsibility.
2. The author(s) and contributor(s) of this repository and course shall not be held liable for any damages, legal repercussions, criminal charges, or unauthorized actions resulting from the use, misuse, or abuse of the contents herein.
3. You will comply with all applicable local, state, national, and international laws regarding cybersecurity and computer fraud.

**IF YOU DO NOT AGREE WITH THESE TERMS, DO NOT USE THIS REPOSITORY AND COURSE.**
---

The hardware below builds the OPERATION COLD IRON depot node and its classroom
labs. Unlike the original monitor, every peripheral in the Embedded Hacking kit
is **required**: the LEDs annunciate cold-chain state, the button acknowledges an
alarm, the servo drives the cold-store damper, and the infrared eye receives the
maintenance remote. The RYLR998 radio carries the telemetry that the
authentication labs protect.

## Radio count at a glance

| Goal | Radios needed | Parts |
| ---- | ------------- | ----- |
| Legitimate authenticated telemetry loop | **2** | 1x node RYLR998 (on the Pico) + 1x hub RYLR998 (USB-to-TTL) |
| Live spoof lab (watch a forged frame land, then die at the tag) | **3** | the 2 above + 1x attacker RYLR998 (USB-to-TTL) |
| Spoof lab without a 3rd radio | 2 or 0 | use the offline parser demo or the `test_radio_spoofed_sender_attribution` unit test |

A radio never receives its own transmission, and the hub radio is busy
listening as `gateway.py`, so the live attack needs a separate attacker radio.

## Required parts

### Microcontroller and debug

- [1x Raspberry Pi Pico 2 with pre-soldered header](https://www.amazon.com/s?k=raspberry+pi+pico+2+with+pre-soldered+header)
- [1x Raspberry Pi Pico Debug Probe](https://www.amazon.com/s?k=raspberry+pi+pico+2+debug+probe)
- [2x USB A-male to micro-USB cable (1 for the Pico 2, 1 for the Debug Probe)](https://www.amazon.com/s?k=micro+usb+cable)

### Breadboard and wiring

- [1x Full-size breadboard (long)](https://www.amazon.com/s?k=full+size+breadboard)
- [1x Assorted jumper wires (male-to-male, male-to-female, female-to-female)](https://www.amazon.com/s?k=breadboard+jumper+wires+assortment)

### Human interface and actuators

- [1x 1602 LCD with PCF8574 I2C backpack](https://www.amazon.com/s?k=1602+lcd+i2c+module)
- [1x DHT11 temperature and humidity sensor](https://www.amazon.com/s?k=dht11+temperature+and+humidity+sensor)
- [1x 10K resistor (DHT11 pull-up, only if your module has none)](https://www.amazon.com/s?k=10k+resistor+assortment)
- [3x 5mm LEDs (1 red, 1 green, 1 yellow)](https://www.amazon.com/s?k=5mm+led+kit)
- [3x 100, 220, or 330 Ohm resistors (for the LEDs)](https://www.amazon.com/s?k=resistor+assortment+kit)
- [1x Push button (tactile switch)](https://www.amazon.com/s?k=tactile+push+button+assortment)

### Actuator and infrared control surface

- [1x SG90 servo motor](https://www.amazon.com/s?k=sg90+micro+servo+motor)
- [1x 1000uF 25V capacitor (servo power stabilization)](https://www.amazon.com/s?k=1000uf+25v+capacitor)
- [1x Infrared (IR) receiver (VS1838B)](https://www.amazon.com/s?k=vs1838b+ir+receiver+module)
- [1x Infrared (IR) remote controller (NEC-compatible)](https://www.amazon.com/s?k=arduino+ir+remote+control)

### LoRa radios and serial adapters

- [3x RYLR998 LoRa module with antenna (node, hub, and attacker)](https://www.amazon.com/s?k=rylr998+lora+module)
  - Use **2** for the legitimate telemetry loop and **3** for the live spoof lab.
  - Use the **same band variant** on every module (for example 915 MHz or 868 MHz).
- [2x USB-to-TTL serial adapter, 3.3V logic (FTDI FT232, CP2102, or CH340)](https://www.amazon.com/s?k=usb+to+ttl+serial+adapter+3.3v)
  - One adapter is the **hub**. The second adapter is the **attacker** for the live spoof lab.
  - Choose a 3.3V-logic adapter; the RYLR998 is **not** 5V tolerant.
- [2x USB A-male to mini/micro-USB cable for the serial adapters (match your adapter)](https://www.amazon.com/s?k=usb+to+ttl+cable)

## Pin map

| Peripheral | GPIO | Notes |
| ---------- | ---- | ----- |
| DHT11 data | GP4 | 10K pull-up required |
| 1602 LCD SDA | GP2 | I2C1 |
| 1602 LCD SCL | GP3 | I2C1, address 0x27 |
| RYLR998 TX (Pico RX) | GP9 | UART1 |
| RYLR998 RX (Pico TX) | GP8 | UART1 |
| Infrared receiver | GP5 | VS1838B, active low |
| Servo signal | GP14 | PWM, 50 Hz, 1000uF bulk on the 5V rail |
| Red breach LED | GP16 | 220-330 Ohm to ground |
| Yellow warning LED | GP17 | 220-330 Ohm to ground |
| Green nominal LED | GP18 | 220-330 Ohm to ground |
| Acknowledge button | GP15 | Active low, internal pull-up |
| Onboard heartbeat LED | GP25 | Heartbeat and transmit blink |

## Cryptography

The authentication layer is implemented entirely in-repo and has no third-party
dependencies:

- **Argon2id** (RFC 9106) derives the 256-bit session key from a provisioned
  passphrase and salt. The classroom profile is `t=3, p=1, m=64` blocks so it
  fits the RP2350 SRAM budget; raise it on a host gateway.
- **XChaCha20-Poly1305** seals every telemetry frame with a 192-bit nonce and a
  128-bit Poly1305 tag, so a forged or replayed frame fails authentication and
  never yields plaintext.

The RP2350 has no hardware AES engine (it accelerates SHA-256 only), so ChaCha20
is both the modern and the faster choice on this silicon.
