# OPERATION COLD IRON - Nation-State Accuracy Review

**An adversarial, evidence-based audit of the entire project where every claim is
verified by a re-runnable command or explicitly labelled as a limitation.**

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

## 1. Scope and Method

This review treats the project as hostile-to-itself. Every module, constant, test
vector, document, and artifact is independently checked. The method is:

1. **Re-run every gate** (`audit_c_standard`, `audit_python_standard`, `run_tests`,
   `check_coverage`) and record the exact output and exit codes.
2. **Re-verify every constant** against the generated artifact header and the
   JSON source of truth, by regenerating the header and diffing it.
3. **Re-verify every cryptographic claim** against a published standard vector,
   and separate vectors that run in the native C suite from vectors that run only
   in the Python suite.
4. **Re-verify every artifact** by SHA-256 and by the in-repo build guardrail,
   because this project ships no CTF firmware artifact.
5. **Read the documents adversarially** for overclaims, stale numbers, and
   omissions, then correct them in this review.

## 2. Gate Results (all re-run for this review)

| gate | command | observed result |
|---|---|---|
| C standard | `python3 scripts/audit_c_standard.py` | exit 0, no output, **0 violations** |
| Python standard | `python3 scripts/audit_python_standard.py` | exit 0, no output, **0 violations** |
| Native tests | `python3 scripts/run_tests.py` | **264 checks, 0 failures**, 89 test cases |
| Coverage | `python3 scripts/check_coverage.py` | exit 0, **100.00% line coverage**, 1708 owned lines |
| Python field crypto | `python3 -m unittest test.test_field_crypto -v` | **7 tests, OK** |

The coverage gate passes on line coverage. It does not require 100% branch or
region coverage, and the raw report is not 100% there: regions 99.51% and
branches 94.12%. That gap is real and is stated in the module table below.

## 3. Module-by-Module Audit

Owned lines are the instrumented statement lines reported by `llvm-cov report`
through `check_coverage.py`. Raw `wc -l` over `src/*.c` is 4748 lines including
comments and blank lines; `main.c` is excluded from coverage by design. Every
owned module is at 100.00% line coverage.

| module | role | owned lines | line coverage | verification performed | honest limitation |
|---|---|---|---|---|---|
| `crc.c` | CRC-16/CCITT-FALSE diagnostic | 20 | 100.00% | `test_crc16_ccitt` check value `0x29B1` | not on the wire; a checksum is not authentication |
| `sensor.c` | DHT11 sampling and JSON frame formatter | 142 | 100.00% | `test_dht_parse_bits_*`, `test_sensor_read_dht_waveform`, all timeout shapes, negative temp, CRC error | mock GPIO replays a recorded waveform, not real silicon |
| `display.c` | HD44780 over PCF8574 | 73 | 100.00% | `test_display_format_lines`, `test_display_render_lines` via recorded I2C | mock I2C, not real HD44780 bus timing |
| `radio.c` | RYLR998 provisioning, `AT+SEND`, `+RCV` parser | 187 | 100.00% | build/parse/reject/pump plus `test_radio_spoofed_sender_attribution` | mock UART; the RF band is not simulated |
| `status_led.c` | red/yellow/green annunciator | 26 | 100.00% | `test_status_led_show`, `test_status_led_breach`, `test_status_led_state` | none |
| `button.c` | debounced acknowledge button | 34 | 100.00% | `test_button_pressed`, `test_button_consume`, `test_button_debounce*`, `test_button_reset` | mock clock, not mechanical bounce |
| `servo.c` | 50 Hz damper PWM | 27 | 100.00% | `test_servo_map`, `test_servo_init`, `test_servo_actuate` | mock PWM; no real servo or inrush load |
| `ir_remote.c` | VS1838B NEC decode | 116 | 100.00% | decode valid/reject/bad leader/mark/ambiguous/address/command, `test_ir_poll_*` | path is unauthenticated by design; no anti-replay |
| `chacha20.c` | ChaCha20 and HChaCha20 | 99 | 100.00% | RFC 8439 block and stream vectors, HChaCha20 draft vector | none |
| `poly1305.c` | Poly1305 one-time authenticator | 169 | 100.00% | RFC 8439 tag vector, aligned path | none |
| `crypto_aead.c` | XChaCha20-Poly1305 seal/open | 38 | 100.00% | round-trip, tamper tag/ct/ad, constant-time `tag_equal` | built from the in-repo primitives, not an audited library |
| `blake2b.c` | BLAKE2b and Argon2 H' | 161 | 100.00% | `test_blake2b_abc`, multiblock, H' 32 and 256 vectors | none |
| `argon2.c` | Argon2id core (BLAMKA, hybrid addressing) | 336 | 100.00% | `test_argon2_lanes`, `test_argon2_type_i`, `test_argon2_clamp` branch coverage | the RFC 9106 KAT runs in Python, not in this C suite |
| `crypto_kdf.c` | Argon2id field key derivation | 28 | 100.00% | reject, empty password, determinism, salt sensitivity | classroom profile `t=3 p=1 m=64`; committed passphrase and salt |
| `envelope.c` | hex nonce/ciphertext/tag codec | 91 | 100.00% | nonce, round-trip, seal/open rejects, uppercase, known vector | none |
| `monitor.c` | state machine | 161 | 100.00% | init, transmit, no-key, nominal, rcv blink, not-ready, LCD fail, sensor timeout, button, IR vent, IR seal | mocks are not the real silicon |
| `main.c` | entry point | n/a | excluded | build only | excluded from coverage by design |

**Total owned lines at 100.00% line coverage: 1708.**

Branch coverage below 100% in the same report: `monitor.c` 82.00%, `display.c`
85.71%, `radio.c` 89.87%, `envelope.c` 92.86%, `sensor.c` 96.08%, `ir_remote.c`
96.00%, `argon2.c` 97.56%.

## 4. Cryptographic Claim Verification

The native suite asserts the following published vectors. Each name below appears
as a passing case in the `run_tests.py` output for this review.

| claim | standard | vector | observed |
|---|---|---|---|
| ChaCha20 block function | RFC 8439 section 2.3.2 | key 00..1f, nonce 000000090000004a00000000 | `test_chacha20_block` PASS |
| ChaCha20 stream cipher | RFC 8439 section 2.4.2 | "Ladies and Gentlemen..." 114-byte ciphertext | `test_chacha20_stream` PASS |
| HChaCha20 subkey | XChaCha20 draft (irtf-cfrg-xchacha) | published subkey vector | `test_hchacha20` PASS |
| Poly1305 tag | RFC 8439 section 2.5.2 | "Cryptographic Forum Research Group" tag `a8061dc1305136c6c22b8baf0c0127a9` | `test_poly1305`, `test_poly1305_aligned` PASS |
| BLAKE2b-512 | BLAKE2 reference | digest of "abc", multiblock, long-input | `test_blake2b_abc`, `test_blake2b_multiblock` PASS |
| Argon2 variable-length hash H' | RFC 9106 section 3.3 | H' of {1,2,3,4} at 32 and 256 bytes | `test_blake2b_long_short`, `test_blake2b_long` PASS |
| Argon2id known-answer | RFC 9106 section 5.3 | `0d640df58d78766c08c037a34a8b53c9d01ef0452d75b65eb52520e96b01e659` | `test.test_field_crypto.TestFieldCrypto.test_rfc9106_argon2id_vector` PASS (Python suite) |
| Envelope layout | project vector | nonce `00..17`, node id 7, fixed body | `test_envelope_known_vector` PASS |

The RFC 9106 Argon2id known-answer test is a Python `unittest` in
`test/test_field_crypto.py`; it is not part of the 264 native checks. Running it
directly confirms all 7 field-crypto tests pass, including the KAT and the
firmware-interop envelope vector. The paper describes it as being in the "native
suite," which is imprecise, because the native runner never executes it.

## 5. Artifact Verification

This project ships no CTF firmware artifact (`build/` holds only untracked local
test binaries). The companion CTF is external:
`https://github.com/mytechnotalent/CTF_cold-chain-monitor`, which ships the
compromised image and its verifier. What is verified in this repository is the
source tree and the provisioning artifact.

Source-tree aggregate SHA-256 over all 36 `.c` and `.h` files under `src/` and
`include/`, computed as `find src include ... | sort | xargs shasum -a 256 |
shasum -a 256`:

```
90a16025ea41afcc3047325adf3ca49c7352c2fbc71135822c663ef7b2f5c17d
```

Key artifacts by SHA-256:

```
paper.pdf                        4c7301df91efd24533cf13890fd9e16f743a5b15abd3b0b65cd95cd30f0e556e
cold-chain-monitor-c-rp2350.png  5934cceab593df519638b878f0d27781cab1672aa8718cb55655c843c97325da
scripts/packet_artifact.json     37ec896e0e3357d75c36111e268b902ec4ae8155de30d38ae1ac5a5bd661e352
include/packet_artifact.h        f24be6739bffdaaab7f015bd02c27c548896f303a00dcf783d200b3ca304f4c0
include/field_secrets.h          63cffbbeb9e4740c4031865d6bcf5bb8302ede090579eb4fbf0ef41764135d07
```

The build guardrail `check_packet_artifact_header` regenerates
`include/packet_artifact.h` from `scripts/packet_artifact.json` and fails if the
committed header is stale. Re-run for this review:

```
$ python3 scripts/gen_packet.py --from-json scripts/packet_artifact.json \
      --check-header-path include/packet_artifact.h
Wrote generated firmware header: include/packet_artifact.h
Verified header matches: include/packet_artifact.h
exit=0
```

Constants re-read from `include/packet_artifact.h` and matched to the README and
the pin map: `PACKET_NODE_ID` 7, `PACKET_HUB_ADDRESS` 0x0001, `PACKET_FRAME_SIZE`
48, `PACKET_TX_INTERVAL_MS` 5000, `PACKET_DHT_TIMEOUT_US` 240,
`PACKET_LCD_I2C_ADDRESS` 0x27, `PACKET_MAX_RCV_LEN` 256, plus DHT GP4, LCD SDA
GP2/SCL GP3, radio GP8/GP9, IR GP5, servo GP14, LEDs GP16/17/18, button GP15.

## 6. Adversarial Document Review

| document claim | audit verdict |
|---|---|
| README "Two doors are open" and the fix seals the radio | **accurate**; the IR path is labelled open by design in the README and paper |
| README "every primitive is checked against its published test vectors in the native suite" | **imprecise**; the RFC 9106 Argon2id KAT lives in the Python suite, not the native C runner |
| README "The suite has **34 cases**" | **stale**; the native suite actually runs 89 test cases and 264 checks |
| README "so an attacker cannot cheaply recover the field passphrase from a captured image" | **overclaim**; the passphrase is committed in `include/field_secrets.h`, so a captured image yields the key directly, not via cracking |
| README does not mention the debug port or SRAM | **omission**; the README never states that a Debug Probe can read the field key from SRAM, and the paper's Threat Model is the only place that admits it |
| README does not imply the device is unhackable | **accurate**; no such claim appears |
| paper Threat Model states OTP, shared key, unauthenticated IR, and gateway replay | **accurate and unusually candid** |

Corrections: the README's case count should read 89, its vector claim should say
"native and Python suites," its Argon2id sentence should say the passphrase is
committed for the lab, and the README should carry the same debug-port and SRAM
warning that the paper already carries.

## 7. Honest Limitations

- **Physical access wins.** A Debug Probe over SWD can read the field key from
  SRAM and read or write the running image. The authenticated telemetry does not
  stop this; only OTP debug disable would.
- **Key extraction from flash.** `include/field_secrets.h` commits the passphrase
  and salt. Anyone holding the image holds the key. This is a lab convenience,
  not a deployment.
- **One shared field key.** Every node uses the same key, so one compromised node
  can seal frames that authenticate as any node identity allowed by the
  associated data.
- **No gateway sequence window.** The sequence is authenticated inside the
  envelope but the gateway does not enforce monotonicity, so a captured valid
  frame can be replayed until a ledger is added.
- **Classroom crypto profile.** Argon2id runs at `t=3 p=1 m=64` to fit SRAM, and
  the tag nonce is drawn from the hardware random source; both are teaching
  parameters, not hardening parameters.
- **The IR path is unauthenticated.** Any NEC remote can replay a damper command.
- **Supply chain and sensor trust are out of scope.** The DHT11 is checksummed,
  not authenticated, and the firmware is only as trustworthy as the toolchain and
  the parts.
- **Availability is not protected.** An attacker on the band can jam or flood the
  receiver.
- **Coverage is line coverage.** Branch coverage is not 100%, and the harness
  mocks are not the real silicon.

## 8. Conclusion

The project is internally consistent and honest at the code level: 16 owned
modules, 1708 instrumented lines, 100.00% line coverage, 264 native checks
passing with 0 failures, 89 native cases, and 7 Python field-crypto tests
passing, every cryptographic primitive anchored to a published vector. The four
gates all pass with exit 0. The documentation is candid in its paper but has four
concrete defects in the README: a stale case count, an imprecise vector claim, an
Argon2id sentence that ignores the committed key, and the absence of the
debug-port and SRAM warning. The one structural truth is that the wire is sealed
while the key and the verdict live in readable memory; the project says so in the
paper and should say so in the README.

---

*This review is reproducible: run the five commands in section 2, the header
check in section 5, and the Python field-crypto suite in section 4.*

This is Act I of the ten-act OPERATION COLD IRON saga. See SAGA.md.
