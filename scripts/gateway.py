#!/usr/bin/env python3
"""Cold Chain hub gateway that logs authenticated LoRa telemetry to CSV.

The gateway reads +RCV frames arriving on the instructor USB-to-TTL radio
and treats every payload as a lowercase hex XChaCha20-Poly1305 envelope
authenticated with the node identifier byte 0x07. Authenticated frames are
decrypted, parsed, logged, and may trigger an actuator AT+SEND reply when a
reported temperature breaches the freeze threshold. Any frame that fails to
authenticate is logged as UNAUTHENTICATED and its forged JSON is never
parsed.

The CSV log has the columns utc, sender, auth, length, payload, rssi_snr.
The auth column is OK for an authenticated frame and UNAUTHENTICATED for a
rejected frame. The payload column holds the decrypted JSON body for an
authenticated frame and an empty string for a rejected frame, so no forged
telemetry ever enters the log as truth.
"""

import argparse
import csv
import datetime
import json
import sys
import time

import field_crypto

try:
    import serial
except ImportError as exc:
    raise SystemExit(
        "pip install pyserial to run the COLD_CHAIN_MONITOR hub gateway"
    ) from exc

FREEZE_TENTHS = -50
REPLY_PAYLOAD = '{"cmd":"heat","v":1}'
CSV_HEADER = ("utc", "sender", "auth", "length", "payload", "rssi_snr")
HUB_ADDRESS = 1
RADIO_NETWORK_ID = 18


def _parse_args():
    """Parse hub gateway command-line arguments.

    Parameters
    ----------
    None

    Returns
    -------
    argparse.Namespace
        Parsed hub gateway arguments.
    """
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--port", required=True, help="Serial port device")
    parser.add_argument("--baud", type=int, default=115200,
                        help="Radio baud rate")
    parser.add_argument("--log", default="telemetry.csv",
                        help="CSV log destination")
    return parser.parse_args()


def _open_serial(port, baud):
    """Open the radio serial port with a short read timeout.

    Parameters
    ----------
    port : str
        Serial port device path.
    baud : int
        Radio baud rate.

    Returns
    -------
    serial.Serial
        Open radio serial connection.
    """
    return serial.Serial(port, baud, timeout=1.0)


def _provision_radio(ser):
    """Program the hub radio address and network so nodes can target it.

    Parameters
    ----------
    ser : serial.Serial
        Open radio serial connection.

    Returns
    -------
    None
    """
    for command in (f"AT+ADDRESS={HUB_ADDRESS}",
                    f"AT+NETWORKID={RADIO_NETWORK_ID}"):
        ser.write(f"{command}\r\n".encode("utf-8"))
        ser.flush()
        time.sleep(0.2)
        while True:
            raw = ser.readline()
            if not raw:
                break
            print(raw.decode("utf-8", errors="replace").strip(), flush=True)


def _read_line(ser):
    """Read one raw CRLF-terminated radio line.

    Parameters
    ----------
    ser : serial.Serial
        Open radio serial connection.

    Returns
    -------
    str or None
        Decoded line, or None when the read timed out.
    """
    raw = ser.readline()
    if not raw:
        return None
    return raw.decode("utf-8", errors="replace").strip()


def _to_int(text):
    """Parse a decimal length field.

    Parameters
    ----------
    text : str
        Candidate decimal text.

    Returns
    -------
    int or None
        Parsed integer, or None on failure.
    """
    try:
        return int(text)
    except ValueError:
        return None


def _split_payload(text, length):
    """Slice the payload field from the +RCV tail fields.

    Parameters
    ----------
    text : str
        Remaining +RCV fields.
    length : int
        Declared payload byte length.

    Returns
    -------
    tuple
        (payload, tail) pair, or (None, None) on malformed data.
    """
    if length >= len(text) or text[length] != ",":
        return None, None
    return text[:length], text[length + 1:]


def _rcv_parts(line):
    """Split a +RCV line into sender, payload text, and declared length.

    Parameters
    ----------
    line : str
        Raw radio line.

    Returns
    -------
    tuple or None
        (sender, text, length) parts, or None on malformed data.
    """
    fields = line[5:].split(",", 2) if line.startswith("+RCV=") else []
    if len(fields) != 3:
        return None
    length = _to_int(fields[1])
    if length is None:
        return None
    return fields[0], fields[2], length


def _parse_rcv(line):
    """Parse one raw +RCV wire line into a record tuple.

    Parameters
    ----------
    line : str
        Raw radio line.

    Returns
    -------
    tuple or None
        (sender, payload, tail) record, or None on malformed data.
    """
    parts = _rcv_parts(line)
    if parts is None:
        return None
    sender, text, length = parts
    payload, tail = _split_payload(text, length)
    if payload is None:
        return None
    return sender, payload, tail


def _frame_temp(payload):
    """Decode the temperature field from a telemetry frame.

    Parameters
    ----------
    payload : str
        JSON telemetry payload.

    Returns
    -------
    int or None
        Temperature in tenths Celsius, or None when absent.
    """
    try:
        data = json.loads(payload)
    except ValueError:
        return None
    return data.get("t")


def _write_row(writer, handle, auth, record, payload):
    """Append one telemetry or rejection row to the CSV log.

    Parameters
    ----------
    writer : csv.writer
        Open CSV writer.
    handle : file object
        Flushable log handle.
    auth : str
        Authentication status, either OK or UNAUTHENTICATED.
    record : tuple
        Parsed (sender, envelope, tail) record.
    payload : str
        Decrypted JSON body, or an empty string when rejected.

    Returns
    -------
    None
    """
    sender, _, tail = record
    stamp = datetime.datetime.now(datetime.timezone.utc).isoformat()
    writer.writerow((stamp, sender, auth, len(payload), payload, tail))
    handle.flush()


def _authenticated(record):
    """Decrypt and authenticate the envelope of one record.

    Parameters
    ----------
    record : tuple
        Parsed (sender, envelope, tail) record.

    Returns
    -------
    str or None
        Decrypted JSON body, or None when authentication fails.
    """
    try:
        plaintext = field_crypto.open_field_frame(record[1])
        return plaintext.decode("utf-8")
    except (ValueError, UnicodeDecodeError):
        return None


def _send_at(ser, address, payload):
    """Submit one AT+SEND command to the transceiver.

    Parameters
    ----------
    ser : serial.Serial
        Open radio serial connection.
    address : str
        Hexadecimal target node address.
    payload : str
        ASCII payload text.

    Returns
    -------
    None
    """
    data = f"AT+SEND={address},{len(payload)},{payload}\r\n"
    ser.write(data.encode("utf-8"))


def _trigger_reply(ser, sender, payload):
    """Fire the actuator reply when an authenticated frame reports freezing.

    Parameters
    ----------
    ser : serial.Serial
        Open radio serial connection.
    sender : str
        Authenticated sender address.
    payload : str
        Decrypted JSON telemetry body.

    Returns
    -------
    None
    """
    tenths = _frame_temp(payload)
    if tenths is not None and tenths < FREEZE_TENTHS:
        _send_at(ser, sender, REPLY_PAYLOAD)


def _accept(ser, writer, handle, record, payload):
    """Log an authenticated frame and apply the freeze policy.

    Parameters
    ----------
    ser : serial.Serial
        Open radio serial connection.
    writer : csv.writer
        Open CSV writer.
    handle : file object
        Flushable log handle.
    record : tuple
        Parsed (sender, envelope, tail) record.
    payload : str
        Decrypted JSON telemetry body.

    Returns
    -------
    None
    """
    _write_row(writer, handle, "OK", record, payload)
    _trigger_reply(ser, record[0], payload)


def _reject(writer, handle, record):
    """Log an unauthenticated frame without parsing its forged body.

    Parameters
    ----------
    writer : csv.writer
        Open CSV writer.
    handle : file object
        Flushable log handle.
    record : tuple
        Parsed (sender, envelope, tail) record.

    Returns
    -------
    None
    """
    print(f"UNAUTHENTICATED sender={record[0]}", flush=True)
    _write_row(writer, handle, "UNAUTHENTICATED", record, "")


def _handle_line(ser, writer, handle, line):
    """Process one inbound radio line against the CSV log.

    Parameters
    ----------
    ser : serial.Serial
        Open radio serial connection.
    writer : csv.writer
        Open CSV writer.
    handle : file object
        Flushable log handle.
    line : str
        Raw radio line.

    Returns
    -------
    None
    """
    record = _parse_rcv(line)
    if record is None:
        return
    payload = _authenticated(record)
    if payload is None:
        _reject(writer, handle, record)
        return
    _accept(ser, writer, handle, record, payload)


def _handle_available(ser, writer, handle):
    """Service any radio line that is currently waiting.

    Parameters
    ----------
    ser : serial.Serial
        Open radio serial connection.
    writer : csv.writer
        Open CSV writer.
    handle : file object
        Flushable log handle.

    Returns
    -------
    None
    """
    line = _read_line(ser)
    if line is None:
        return
    print(line, flush=True)
    _handle_line(ser, writer, handle, line)


def main():
    """Run the hub gateway loop until interrupted.

    Parameters
    ----------
    None

    Returns
    -------
    int
        Zero on clean shutdown.
    """
    args = _parse_args()
    with _open_serial(args.port, args.baud) as ser:
        _provision_radio(ser)
        with open(args.log, "a", newline="", encoding="utf-8") as handle:
            writer = csv.writer(handle)
            if handle.tell() == 0:
                writer.writerow(CSV_HEADER)
            while True:
                try:
                    _handle_available(ser, writer, handle)
                except KeyboardInterrupt:
                    return 0


if __name__ == "__main__":
    sys.exit(main())