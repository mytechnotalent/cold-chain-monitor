"""Unit test adapter for VS Code Test Explorer."""
import subprocess
import sys
import unittest

_CACHED_OUTPUT = ""


def _get_harness_output() -> str:
    """
    Execute native tests and return stdout.

    Parameters
    ----------
    None

    Returns
    -------
    str
        Standard output from native test suite.
    """
    global _CACHED_OUTPUT
    if not _CACHED_OUTPUT:
        cmd = [sys.executable, "scripts/run_tests.py"]
        res = subprocess.run(cmd, capture_output=True, text=True)
        _CACHED_OUTPUT = res.stdout
    return _CACHED_OUTPUT


def _assert_harness_pass(test_name: str) -> None:
    """
    Assert that a named harness test passed.

    Parameters
    ----------
    test_name : str
        Name of harness test function.

    Returns
    -------
    None
    """
    output = _get_harness_output()
    expected = f":{test_name}:PASS"
    assert expected in output, f"{test_name} did not pass in harness output"


class TestColdChainMonitorFirmware(unittest.TestCase):
    """Test cases for RP2350 Cold Chain monitor firmware."""

    def test_01_config_constants(self) -> None:
        """
        Verify provisioning constants.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_config_constants")

    def test_02_packet_artifact_constants(self) -> None:
        """
        Verify packet artifact header constants.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_packet_artifact_constants")

    def test_03_crc16_ccitt(self) -> None:
        """
        Verify CRC-16/CCITT-FALSE known-answer values.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_crc16_ccitt")

    def test_04_dht_parse_bits_valid(self) -> None:
        """
        Verify raw 40-bit decoding.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_dht_parse_bits_valid")

    def test_05_dht_parse_bits_checksum_fail(self) -> None:
        """
        Verify checksum rejection.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_dht_parse_bits_checksum_fail")

    def test_06_dht_parse_bits_null(self) -> None:
        """
        Verify null-argument rejection.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_dht_parse_bits_null")

    def test_07_sensor_build_frame(self) -> None:
        """
        Verify telemetry frame formatter.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_sensor_build_frame")

    def test_08_sensor_read_dht_waveform(self) -> None:
        """
        Verify full DHT11 waveform transaction.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_sensor_read_dht_waveform")

    def test_09_sensor_read_timeout(self) -> None:
        """
        Verify sensor timeout path.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_sensor_read_timeout")

    def test_10_radio_build_send_cmd(self) -> None:
        """
        Verify AT+SEND command builder.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_radio_build_send_cmd")

    def test_11_radio_parse_rcv(self) -> None:
        """
        Verify +RCV parsing.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_radio_parse_rcv")

    def test_12_radio_parse_rcv_rejects(self) -> None:
        """
        Verify +RCV rejection paths.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_radio_parse_rcv_rejects")

    def test_13_radio_line_pump(self) -> None:
        """
        Verify inbound line state machine.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_radio_line_pump")

    def test_14_radio_spoofed_sender_attribution(self) -> None:
        """
        Verify hub trusts the +RCV sender field.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_radio_spoofed_sender_attribution")

    def test_15_display_format_lines(self) -> None:
        """
        Verify rendered display line text.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_display_format_lines")

    def test_16_display_render_lines(self) -> None:
        """
        Verify LCD frame buffer emission.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_display_render_lines")

    def test_17_monitor_init(self) -> None:
        """
        Verify monitor initialization.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_monitor_init")

    def test_18_monitor_step_transmits(self) -> None:
        """
        Verify a monitor tick transmits one frame.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_monitor_step_transmits")

    def test_19_monitor_step_rcv_blink(self) -> None:
        """
        Verify inbound +RCV LED blink.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_monitor_step_rcv_blink")

    def test_20_sensor_policy_not_ready(self) -> None:
        """
        Verify policy rejection while the sensor is uninitialized.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_sensor_policy_not_ready")

    def test_21_sensor_policy_null_out(self) -> None:
        """
        Verify policy rejection of a null reading pointer.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_sensor_policy_null_out")

    def test_22_sensor_dht_negative_temp(self) -> None:
        """
        Verify signed temperature decoding with the sign bit set.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_sensor_dht_negative_temp")

    def test_23_sensor_read_timeout_response_low(self) -> None:
        """
        Verify the second response edge wait reports a timeout.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_sensor_read_timeout_response_low")

    def test_24_sensor_read_timeout_response_high(self) -> None:
        """
        Verify the third response edge wait reports a timeout.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_sensor_read_timeout_response_high")

    def test_25_sensor_read_timeout_bit_low(self) -> None:
        """
        Verify a data-bit low that never ends reports a timeout.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_sensor_read_timeout_bit_low")

    def test_26_sensor_read_measure_timeout(self) -> None:
        """
        Verify a high pulse that never falls reports a timeout.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_sensor_read_measure_timeout")

    def test_27_sensor_read_dht_crc_error(self) -> None:
        """
        Verify the full transaction reports a checksum failure.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_sensor_read_dht_crc_error")

    def test_28_radio_hex_digits(self) -> None:
        """
        Verify lowercase and uppercase hexadecimal sender fields.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_radio_hex_digits")

    def test_29_radio_build_send_cmd_oversize_cmd(self) -> None:
        """
        Verify command-builder rejection on a truncated buffer.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_radio_build_send_cmd_oversize_cmd")

    def test_30_radio_send_frame_oversize(self) -> None:
        """
        Verify the wire transmitter forwards a command-building failure.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_radio_send_frame_oversize")

    def test_31_radio_parse_missing_commas(self) -> None:
        """
        Verify +RCV parsing rejects missing field separators.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_radio_parse_missing_commas")

    def test_32_monitor_not_ready(self) -> None:
        """
        Verify the monitor returns false while not initialized.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_monitor_not_ready")

    def test_33_monitor_init_lcd_fail(self) -> None:
        """
        Verify monitor initialization fails when the LCD does not ACK.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_monitor_init_lcd_fail")

    def test_34_monitor_step_sensor_timeout(self) -> None:
        """
        Verify a sensor-timeout monitor tick renders the fail line.

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        _assert_harness_pass("test_monitor_step_sensor_timeout")


if __name__ == "__main__":
    unittest.main()