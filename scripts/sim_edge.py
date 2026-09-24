#!/usr/bin/env python3
"""Simulate a real Cold Chain edge node from a laptop without a Pico.

sim_edge.py drives a USB-to-TTL radio with the same fixed-field JSON
telemetry and AT+SEND cadence as the RP2350 firmware, sealing each frame
with the OPERATION COLD IRON field key so the authenticated hub gateway
accepts it. Students can validate the gateway and then practice spoofing
against a live target before touching embedded hardware.
"""

import argparse
import json
import sys
import time

import field_crypto

try:
    import serial
except ImportError as exc:
    raise SystemExit(
        "pip install pyserial to run the COLD_CHAIN_MONITOR edge simulator"
    ) from exc

HUB_ADDRESS = "0001"


def _parse_args():
    """Parse edge simulator command-line arguments.

    Parameters
    ----------
    None

    Returns
    -------
    argparse.Namespace
        Parsed edge simulator arguments.
    """
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--port", required=True, help="Serial port device")
    parser.add_argument("--baud", type=int, default=115200,
                        help="Radio baud rate")
    parser.add_argument("--node", type=int, default=7,
                        help="Node identifier")
    parser.add_argument("--interval", type=float, default=5.0,
                        help="Transmit interval in seconds")
    parser.add_argument("--temp", type=int, default=235,
                        help="Temperature in tenths Celsius")
    parser.add_argument("--hum", type=int, default=610,
                        help="Humidity in tenths percent")
    return parser.parse_args()


def _payload(node, seq, temp, hum):
    """Build the fixed-field JSON telemetry body.

    Parameters
    ----------
    node : int
        Node identifier.
    seq : int
        Transmit sequence number.
    temp : int
        Temperature in tenths Celsius.
    hum : int
        Humidity in tenths percent.

    Returns
    -------
    str
        Compact JSON telemetry payload.
    """
    return json.dumps({"n": node, "s": seq, "t": temp, "h": hum},
                      separators=(",", ":"))


def _drain(ser):
    """Read and print any inbound radio lines.

    Parameters
    ----------
    ser : serial.Serial
        Open radio serial connection.

    Returns
    -------
    None
    """
    while True:
        line = ser.readline()
        if not line:
            return
        print(line.decode("utf-8", errors="replace").strip(), flush=True)


def _tick(ser, args, seq):
    """Transmit one simulated telemetry frame.

    Parameters
    ----------
    ser : serial.Serial
        Open radio serial connection.
    args : argparse.Namespace
        Parsed edge simulator arguments.
    seq : int
        Current sequence number.

    Returns
    -------
    None
    """
    body = _payload(args.node, seq, args.temp, args.hum)
    envelope = field_crypto.seal_field_frame(body.encode("utf-8"))
    cmd = f"AT+SEND={HUB_ADDRESS},{len(envelope)},{envelope}\r\n"
    ser.write(cmd.encode("utf-8"))
    ser.flush()
    print(f"[node {args.node}] sealed {body}", flush=True)


def main():
    """Run the edge simulator loop until interrupted.

    Parameters
    ----------
    None

    Returns
    -------
    int
        Zero on clean shutdown.
    """
    args = _parse_args()
    seq = 0
    with serial.Serial(args.port, args.baud, timeout=0.1) as ser:
        while True:
            try:
                _tick(ser, args, seq)
                seq += 1
                time.sleep(args.interval)
                _drain(ser)
            except KeyboardInterrupt:
                return 0


if __name__ == "__main__":
    sys.exit(main())