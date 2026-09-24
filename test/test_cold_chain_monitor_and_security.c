/**
 * FILE: test_cold_chain_monitor_and_security.c
 *
 * DESCRIPTION:
 * Comprehensive test suite for the RP2350 Cold Chain Agricultural
 * Environmental Monitor: provisioning constants, packet artifact, CRC,
 * DHT11 decoding and waveform, display formatting, RYLR998 AT command
 * building, +RCV parsing, line pumping, and state-machine ticks.
 *
 * BRIEF:
 * Native unit test runner for cold-chain-monitor-c-rp2350.
 *
 * AUTHOR: Kevin Thomas
 * DATE: September 2026
 */

#include "harness.h"
#include "mock/pico/stdlib.h"
#include "mock/pico/time.h"
#include "mock/hardware/gpio.h"
#include "mock/hardware/i2c.h"
#include "mock/hardware/uart.h"
#include "cold_chain_monitor.h"
#include "crc.h"
#include "sensor.h"
#include "display.h"
#include "radio.h"
#include "monitor.h"
#include "packet_artifact.h"
#include <string.h>

#undef SENSOR_HOST_PULSE_US
#define SENSOR_HOST_PULSE_US 0u

#include "../src/sensor.c"
#include "../src/crc.c"
#include "../src/display.c"
#include "../src/radio.c"
#include "../src/monitor.c"

/**
 * @brief Simulated DHT11 high-pulse widths for the canonical reading.
 *
 * Bytes decoded: humidity 61, humidity-decimal 0, temperature 23,
 * temperature-decimal 0, checksum 0x54.
 */
static const uint16_t s_widths[SENSOR_BIT_COUNT] = {
    26u, 26u, 70u, 70u, 70u, 70u, 26u, 70u,
    26u, 26u, 26u, 26u, 26u, 26u, 26u, 26u,
    26u, 26u, 26u, 70u, 26u, 70u, 70u, 70u,
    26u, 26u, 26u, 26u, 26u, 26u, 26u, 26u,
    26u, 70u, 26u, 70u, 26u, 70u, 26u, 26u,
};

/**
 * @brief Reset every host mock peripheral.
 *
 * @param void No parameters.
 * @return void
 */
static void reset_all(void) {
    mock_timer_reset();
    mock_gpio_reset();
    mock_i2c_reset();
    mock_uart_reset();
}

/**
 * @brief Append one timeline point and advance the entry count.
 *
 * @param offsets Pointer to mutable offset array.
 * @param levels Pointer to mutable level array.
 * @param n Current entry count.
 * @param level Level to record.
 * @param edge Absolute timestamp in microseconds.
 * @return size_t Updated entry count.
 */
static size_t timeline_pair(uint32_t *offsets, int *levels, size_t n,
                            int level, uint32_t edge) {
    offsets[n] = edge;
    levels[n] = level;
    return n + 1u;
}

/**
 * @brief Write the four leading DHT11 handshake timeline points.
 *
 * @param offsets Pointer to mutable offset array.
 * @param levels Pointer to mutable level array.
 * @return size_t Number of timeline entries written.
 */
static size_t timeline_header(uint32_t *offsets, int *levels) {
    size_t n = 0u;
    n = timeline_pair(offsets, levels, n, 1, 0u);
    n = timeline_pair(offsets, levels, n, 0, 30u);
    n = timeline_pair(offsets, levels, n, 1, 110u);
    n = timeline_pair(offsets, levels, n, 0, 190u);
    return n;
}

/**
 * @brief Append the 40 data-bit timeline point pairs.
 *
 * @param offsets Pointer to mutable offset array.
 * @param levels Pointer to mutable level array.
 * @param n Current entry count.
 * @param widths Pointer to 40 high-pulse width values.
 * @return size_t Updated entry count.
 */
static size_t timeline_bits(uint32_t *offsets, int *levels, size_t n,
                            const uint16_t *widths) {
    uint32_t edge = 190u;
    uint8_t i;
    for (i = 0u; i < SENSOR_BIT_COUNT; ++i) {
        edge += 50u;
        n = timeline_pair(offsets, levels, n, 1, edge);
        edge += widths[i];
        n = timeline_pair(offsets, levels, n, 0, edge);
    }
    return n;
}

/**
 * @brief Build a DHT11 one-wire waveform timeline from bit widths.
 *
 * @param offsets Pointer to mutable offset array.
 * @param levels Pointer to mutable level array.
 * @param widths Pointer to 40 high-pulse width values.
 * @return size_t Number of timeline entries written.
 */
static size_t build_timeline(uint32_t *offsets, int *levels,
                             const uint16_t *widths) {
    size_t n = timeline_header(offsets, levels);
    return timeline_bits(offsets, levels, n, widths);
}

/**
 * @brief Combine the high nibbles of two I2C bytes into one display byte.
 *
 * @param hi First raw mock I2C byte.
 * @param lo Second raw mock I2C byte.
 * @return char Decoded display byte.
 */
static char decode_nibble(uint8_t hi, uint8_t lo) {
    uint8_t h = (uint8_t)((hi >> 4u) & 0x0Fu);
    uint8_t l = (uint8_t)((lo >> 4u) & 0x0Fu);
    return (char)((h << 4u) | l);
}

/**
 * @brief Decode 4-bit I2C LCD writes back into display bytes.
 *
 * @param buf Pointer to raw mock I2C byte log.
 * @param n Number of raw log bytes.
 * @param out Pointer to mutable decoded text buffer.
 * @param out_max Capacity of the decoded text buffer.
 * @return size_t Number of decoded display bytes.
 */
static size_t decode_lcd_bytes(const uint8_t *buf, size_t n, char *out,
                               size_t out_max) {
    size_t k = 0u;
    size_t i = 0u;
    while ((i + 4u <= n) && (k + 1u < out_max)) {
        out[k] = decode_nibble(buf[i], buf[i + 2u]);
        ++k;
        i += 4u;
    }
    out[k] = '\0';
    return k;
}

/**
 * @brief Build a 40-bit width array from five response bytes.
 *
 * @param bytes Pointer to five DHT11 response bytes.
 * @param out Pointer to mutable width array of SENSOR_BIT_COUNT entries.
 * @return void
 */
static void build_bits_from_bytes(const uint8_t bytes[SENSOR_BYTE_COUNT],
                                  uint16_t *out) {
    /**
     * @brief Declaration of b.
     */
    uint8_t b;
    /**
     * @brief Declaration of bit.
     */
    uint8_t bit;
    /**
     * @brief Declaration of k.
     */
    size_t k = 0u;
    for (b = 0u; b < SENSOR_BYTE_COUNT; ++b) {
        for (bit = 0u; bit < 8u; ++bit) {
            out[k] =
                ((bytes[b] >> (7u - bit)) & 1u) ? 70u : 26u;
            k += 1u;
        }
    }
}

/**
 * @brief File-scope GPIO timeline offset scratch buffer.
 */
static uint32_t s_offsets[256];

/**
 * @brief File-scope GPIO timeline level scratch buffer.
 */
static int s_levels[256];

/**
 * @brief File-scope UART transmit capture buffer.
 */
static char s_tx[2048];

/**
 * @brief File-scope raw I2C log scratch buffer.
 */
static uint8_t s_raw[512];

/**
 * @brief File-scope decoded LCD scratch buffer.
 */
static char s_decoded[DISPLAY_LINE_LEN * 4];

/**
 * @brief File-scope first LCD render line buffer.
 */
static char s_line1[DISPLAY_LINE_LEN];

/**
 * @brief File-scope second LCD render line buffer.
 */
static char s_line2[DISPLAY_LINE_LEN];

/**
 * @brief File-scope decoded DHT11 reading.
 */
static dht_reading_t s_reading;

/**
 * @brief File-scope decoded inbound radio report.
 */
static radio_rcv_t s_rcv;

/**
 * @brief Copy the canonical high-pulse widths into a bit array.
 *
 * @param bits Pointer to mutable 40-entry width array.
 * @return void
 */
static void fill_widths(uint16_t *bits) {
    uint8_t i;
    for (i = 0u; i < SENSOR_BIT_COUNT; ++i) {
        bits[i] = s_widths[i];
    }
}

/**
 * @brief Arm a full DHT11 waveform timeline at a base timestamp.
 *
 * @param widths Pointer to 40 high-pulse width values.
 * @param base_us Absolute base timestamp in microseconds.
 * @return void
 */
static void mock_dht_timeline(const uint16_t *widths, uint64_t base_us) {
    size_t count = build_timeline(s_offsets, s_levels, widths);
    mock_gpio_timeline_begin_at(base_us, s_offsets, s_levels, count,
                                COLD_CHAIN_MONITOR_DHT_PIN);
}

/**
 * @brief Load the canonical LCD render lines into the fixtures.
 *
 * @param void No parameters.
 * @return void
 */
static void load_display_lines(void) {
    strcpy(s_line1, "T:23.0C H:61.0%");
    strcpy(s_line2, "N:07 S:0042 OK");
}

/**
 * @brief Assert the decoded LCD frame buffer contents.
 *
 * @param line1 Expected first-line text.
 * @param line2 Expected second-line text.
 * @return void
 */
static void assert_lcd_frame(const char *line1, const char *line2) {
    size_t count;
    count = mock_i2c_get_log(s_raw, sizeof(s_raw));
    TEST_ASSERT_EQUAL_UINT(136u, (unsigned)count);
    decode_lcd_bytes(s_raw, count, s_decoded, sizeof(s_decoded));
    TEST_ASSERT_EQUAL_UINT8(0x80u, (uint8_t)s_decoded[0]);
    TEST_ASSERT_TRUE(strncmp(line1, &s_decoded[1], strlen(line1)) == 0);
    TEST_ASSERT_EQUAL_UINT8(0xC0u, (uint8_t)s_decoded[17]);
    TEST_ASSERT_TRUE(strncmp(line2, &s_decoded[18], strlen(line2)) == 0);
}

/**
 * @brief Load a canonical valid reading into the fixture.
 *
 * @param void No parameters.
 * @return void
 */
static void load_reading(void) {
    s_reading.temperature_tenths = 230;
    s_reading.humidity_tenths = 610;
    s_reading.valid = true;
}

/**
 * @brief Assert the GPIO pin and bus provisioning constants.
 *
 * @param void No parameters.
 * @return void
 */
static void assert_pin_constants(void) {
    TEST_ASSERT_EQUAL_UINT(25u, COLD_CHAIN_MONITOR_LED_PIN);
    TEST_ASSERT_EQUAL_UINT(4u, COLD_CHAIN_MONITOR_DHT_PIN);
    TEST_ASSERT_EQUAL_UINT(2u, COLD_CHAIN_MONITOR_I2C_SDA);
    TEST_ASSERT_EQUAL_UINT(3u, COLD_CHAIN_MONITOR_I2C_SCL);
    TEST_ASSERT_EQUAL_UINT(100000u, COLD_CHAIN_MONITOR_I2C_BAUD);
    TEST_ASSERT_EQUAL_UINT(8u, COLD_CHAIN_MONITOR_UART_TX);
    TEST_ASSERT_EQUAL_UINT(9u, COLD_CHAIN_MONITOR_UART_RX);
}

/**
 * @brief Assert the UART and frame provisioning constants.
 *
 * @param void No parameters.
 * @return void
 */
static void assert_frame_constants(void) {
    TEST_ASSERT_EQUAL_UINT(115200u, COLD_CHAIN_MONITOR_UART_BAUD);
    TEST_ASSERT_EQUAL_UINT(48u, COLD_CHAIN_MONITOR_FRAME_SIZE);
    TEST_ASSERT_EQUAL_UINT(5000u, COLD_CHAIN_MONITOR_TX_INTERVAL_MS);
    TEST_ASSERT_EQUAL_UINT(PACKET_LCD_I2C_ADDRESS, COLD_CHAIN_MONITOR_LCD_ADDR);
}

/**
 * @brief Assert the packet artifact identity constants.
 *
 * @param void No parameters.
 * @return void
 */
static void assert_artifact_ids(void) {
    TEST_ASSERT_EQUAL_STRING("cold-chain-monitor-packets-demo-v1", PACKET_ARTIFACT_FORMAT);
    TEST_ASSERT_EQUAL_UINT(1u, PACKET_FRAME_VERSION);
    TEST_ASSERT_EQUAL_UINT(7u, PACKET_NODE_ID);
    TEST_ASSERT_EQUAL_HEX16(0x0001u, PACKET_HUB_ADDRESS);
    TEST_ASSERT_EQUAL_UINT(48u, PACKET_FRAME_SIZE);
    TEST_ASSERT_EQUAL_UINT(5000u, PACKET_TX_INTERVAL_MS);
    TEST_ASSERT_EQUAL_UINT(240u, PACKET_DHT_TIMEOUT_US);
}

/**
 * @brief Assert the packet artifact frame constants.
 *
 * @param void No parameters.
 * @return void
 */
static void assert_artifact_frame(void) {
    TEST_ASSERT_EQUAL_UINT(0x27u, PACKET_LCD_I2C_ADDRESS);
    TEST_ASSERT_EQUAL_UINT(256u, PACKET_MAX_RCV_LEN);
    TEST_ASSERT_EQUAL_UINT(48u, (unsigned)sizeof(PACKET_EXAMPLE_FRAME));
    TEST_ASSERT_EQUAL_UINT8(0x7Bu, PACKET_EXAMPLE_FRAME[0]);
    TEST_ASSERT_EQUAL_UINT8(0x37u, PACKET_EXAMPLE_FRAME[5]);
    TEST_ASSERT_EQUAL_UINT8(0x7Du, PACKET_EXAMPLE_FRAME[28]);
    TEST_ASSERT_EQUAL_UINT8(0x00u, PACKET_EXAMPLE_FRAME[29]);
    TEST_ASSERT_EQUAL_UINT8(0x00u, PACKET_EXAMPLE_FRAME[47]);
}

/**
 * @brief Assert the formatted frame text and zero padding.
 *
 * @param frame Pointer to the formatted frame buffer.
 * @param n Length of the JSON body.
 * @param cap Capacity of the frame buffer.
 * @return void
 */
static void assert_frame_padding(const char *frame, size_t n, size_t cap) {
    static const char zeros[COLD_CHAIN_MONITOR_FRAME_SIZE] = {0};
    TEST_ASSERT_EQUAL_UINT(29u, (unsigned)n);
    TEST_ASSERT_EQUAL_STRING("{\"n\":7,\"s\":0,\"t\":230,\"h\":610}", frame);
    TEST_ASSERT_EQUAL_MEMORY(zeros, &frame[n], cap - n);
}

/**
 * @brief Assert frame-builder rejection of null arguments.
 *
 * @param frame Pointer to a frame buffer.
 * @param cap Capacity of the frame buffer.
 * @return void
 */
static void assert_frame_rejects(char *frame, size_t cap) {
    TEST_ASSERT_EQUAL_UINT(0u,
                           (unsigned)sensor_build_frame(&s_reading, 0u, NULL, 0u));
    TEST_ASSERT_EQUAL_UINT(0u,
                           (unsigned)sensor_build_frame(NULL, 0u, frame, cap));
}

/**
 * @brief Assert command-builder rejection paths.
 *
 * @param cmd Pointer to a command buffer.
 * @param cap Capacity of the command buffer.
 * @return void
 */
static void assert_build_rejects(char *cmd, size_t cap) {
    TEST_ASSERT_EQUAL(RADIO_RESULT_OVERSIZE,
                      radio_build_send_cmd(0x0001u, (const uint8_t *)"abc",
                                           257u, cmd, cap));
    TEST_ASSERT_EQUAL(RADIO_RESULT_PARSE_ERROR,
                      radio_build_send_cmd(0x0001u, NULL, 3u, cmd, cap));
}

/**
 * @brief Assert +RCV parsing of a comma-laden JSON payload.
 *
 * @param void No parameters.
 * @return void
 */
/**
 * @brief Parse the canonical comma-laden JSON +RCV line.
 *
 * @param void No parameters.
 * @return radio_result_t Parsed result code.
 */
static radio_result_t parse_json_rcv(void) {
    return radio_parse_rcv("+RCV=0007,29,{\"n\":7,\"s\":0,\"t\":230,\"h\":610}"
                           ",-78,5", &s_rcv);
}

/**
 * @brief Assert +RCV parsing of a comma-laden JSON payload.
 *
 * @param void No parameters.
 * @return void
 */
static void assert_rcv_json(void) {
    TEST_ASSERT_EQUAL(RADIO_RESULT_OK, parse_json_rcv());
    TEST_ASSERT_EQUAL_HEX16(0x0007u, s_rcv.sender);
    TEST_ASSERT_EQUAL_UINT(29u, (unsigned)s_rcv.len);
    TEST_ASSERT_EQUAL_STRING("{\"n\":7,\"s\":0,\"t\":230,\"h\":610}",
                             s_rcv.payload);
    TEST_ASSERT_EQUAL_INT(-78, s_rcv.rssi);
    TEST_ASSERT_EQUAL_INT(5, s_rcv.snr);
}

/**
 * @brief Assert +RCV parsing of a short comma-bearing payload.
 *
 * @param void No parameters.
 * @return void
 */
static void assert_rcv_comma(void) {
    TEST_ASSERT_EQUAL(RADIO_RESULT_OK,
                      radio_parse_rcv("+RCV=0008,9,{\"a\",\"b\"},-60,3", &s_rcv));
    TEST_ASSERT_EQUAL_HEX16(0x0008u, s_rcv.sender);
    TEST_ASSERT_EQUAL_STRING("{\"a\",\"b\"}", s_rcv.payload);
}

/**
 * @brief Assert +RCV parsing of a frame with no RSSI/SNR tail.
 *
 * @param void No parameters.
 * @return void
 */
static void assert_rcv_no_tail(void) {
    TEST_ASSERT_EQUAL(RADIO_RESULT_OK,
                      radio_parse_rcv("+RCV=0007,2,ok", &s_rcv));
    TEST_ASSERT_EQUAL_INT(0, s_rcv.rssi);
    TEST_ASSERT_EQUAL_INT(0, s_rcv.snr);
}

/**
 * @brief Assert +RCV rejection of null arguments.
 *
 * @param void No parameters.
 * @return void
 */
static void assert_rcv_null(void) {
    TEST_ASSERT_EQUAL(RADIO_RESULT_PARSE_ERROR, radio_parse_rcv(NULL, &s_rcv));
    TEST_ASSERT_EQUAL(RADIO_RESULT_PARSE_ERROR,
                      radio_parse_rcv("+RCV=0001,1,a", NULL));
}

/**
 * @brief Assert +RCV rejection of malformed and oversized lines.
 *
 * @param void No parameters.
 * @return void
 */
static void assert_rcv_rejects(void) {
    TEST_ASSERT_EQUAL(RADIO_RESULT_PARSE_ERROR,
                      radio_parse_rcv("AT+SEND=0001,3,abc", &s_rcv));
    TEST_ASSERT_EQUAL(RADIO_RESULT_OVERSIZE,
                      radio_parse_rcv("+RCV=0001,300,abcdef", &s_rcv));
    TEST_ASSERT_EQUAL(RADIO_RESULT_PARSE_ERROR,
                      radio_parse_rcv("+RCV=0001,5,abc", &s_rcv));
    assert_rcv_null();
}

/**
 * @brief Assert the inbound line pump consumes two CRLF-terminated lines.
 *
 * @param line Pointer to line buffer.
 * @param len Pointer to accumulated length.
 * @return void
 */
static void assert_pump_lines(char *line, size_t *len) {
    TEST_ASSERT_TRUE(radio_line_pump(uart0, line, len));
    TEST_ASSERT_EQUAL_STRING("ab", line);
    TEST_ASSERT_TRUE(radio_line_pump(uart0, line, len));
    TEST_ASSERT_EQUAL_STRING("cd", line);
    TEST_ASSERT_FALSE(radio_line_pump(uart0, line, len));
    TEST_ASSERT_EQUAL_UINT(0u, (unsigned)*len);
}

/**
 * @brief Assert the positive display formatting path.
 *
 * @param void No parameters.
 * @return void
 */
static void assert_format_ok(void) {
    s_reading.temperature_tenths = 230;
    s_reading.humidity_tenths = 610;
    s_reading.valid = true;
    display_format_lines(&s_reading, 42u, true, s_line1, s_line2);
    TEST_ASSERT_EQUAL_STRING("T:23.0C H:61.0%", s_line1);
    TEST_ASSERT_EQUAL_STRING("N:07 S:0042 OK", s_line2);
}

/**
 * @brief Assert the negative display formatting path.
 *
 * @param void No parameters.
 * @return void
 */
static void assert_format_fail(void) {
    s_reading.temperature_tenths = -53;
    display_format_lines(&s_reading, 0u, false, s_line1, s_line2);
    TEST_ASSERT_EQUAL_STRING("T:-5.3C H:61.0%", s_line1);
    TEST_ASSERT_EQUAL_STRING("N:07 S:0000 !!", s_line2);
}

/**
 * @brief Reset mocks and arm a due transmit tick with a valid waveform.
 *
 * @param void No parameters.
 * @return void
 */
static void arm_transmit_tick(void) {
    reset_all();
    TEST_ASSERT_TRUE(monitor_init());
    mock_i2c_reset();
    mock_dht_timeline(s_widths, 6000000u);
    mock_timer_set_us(6000000u);
}

/**
 * @brief Locate the hex payload after the AT+SEND address and length fields.
 *
 * @param void No parameters.
 * @return char* Pointer to the hex payload inside the captured frame.
 */
static char *tx_hex_start(void) {
    char *first = strchr(s_tx, ',');
    char *second = strchr(first + 1, ',');
    return second + 1;
}

/**
 * @brief Trim the trailing CRLF from the captured hex payload.
 *
 * @param hex Pointer to the mutable hex payload.
 * @return size_t Number of remaining hex characters.
 */
static size_t tx_hex_len(char *hex) {
    size_t n = strlen(hex);
    while (n > 0u && (hex[n - 1u] == '\r' || hex[n - 1u] == '\n')) {
        n -= 1u;
    }
    hex[n] = '\0';
    return n;
}

/**
 * @brief Report whether every character is a lowercase hex digit.
 *
 * @param hex Pointer to the NUL-terminated candidate text.
 * @return bool true when the text is entirely lowercase hexadecimal.
 */
static bool tx_all_hex(const char *hex) {
    size_t i;
    for (i = 0u; hex[i] != '\0'; ++i) {
        if (hex_digit(hex[i]) < 0 || (hex[i] >= 'A' && hex[i] <= 'F')) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Assert the transmitted payload is an even-length lowercase hex envelope.
 *
 * @param void No parameters.
 * @return void
 */
static void assert_hex_payload(void) {
    char *hex = tx_hex_start();
    size_t hex_len = tx_hex_len(hex);
    TEST_ASSERT_TRUE(hex_len > 48u);
    TEST_ASSERT_TRUE((hex_len % 2u) == 0u);
    TEST_ASSERT_TRUE(tx_all_hex(hex));
}

/**
 * @brief Assert the transmitted AT+SEND frame and LED blinks.
 *
 * @param void No parameters.
 * @return void
 */
static void assert_transmit_frame(void) {
    size_t tx_len = mock_uart_get_tx(s_tx, sizeof(s_tx) - 1u);
    s_tx[tx_len] = '\0';
    TEST_ASSERT_TRUE(strstr(s_tx, "AT+SEND=0001,") != NULL);
    TEST_ASSERT_TRUE(strstr(s_tx, "\r\n") != NULL);
    assert_hex_payload();
    TEST_ASSERT_EQUAL_UINT(6u, mock_gpio_toggles(COLD_CHAIN_MONITOR_LED_PIN));
}

/**
 * @brief Reset mocks and arm a due tick whose sensor read times out.
 *
 * @param void No parameters.
 * @return void
 */
static void arm_timeout_tick(void) {
    static const uint32_t off[4] = {0u};
    static const int lvl[4] = {1};
    TEST_ASSERT_TRUE(monitor_init());
    mock_uart_reset();
    mock_i2c_reset();
    mock_gpio_timeline_begin_at(6000000u, off, lvl, 1u,
                                COLD_CHAIN_MONITOR_DHT_PIN);
    mock_timer_set_us(6000000u);
}

/**
 * @brief Reset mocks and arm a due nominal-temperature tick.
 *
 * @param bits Pointer to the 40-entry high-pulse width array.
 * @return void
 */
static void arm_nominal_tick(const uint16_t *bits) {
    reset_all();
    TEST_ASSERT_TRUE(monitor_init());
    mock_i2c_reset();
    mock_dht_timeline(bits, 6000000u);
    mock_timer_set_us(6000000u);
}

/**
 * @brief Verify a nominal reading renders the seal actuator path.
 *
 * @param void No parameters.
 * @return void
 */
static void test_monitor_step_nominal(void) {
    /**
     * @brief Declaration of bytes.
     */
    const uint8_t bytes[SENSOR_BYTE_COUNT] = {50u, 0u, 0x8Au, 0u, 0xBCu};
    /**
     * @brief Declaration of bits.
     */
    uint16_t bits[SENSOR_BIT_COUNT];
    build_bits_from_bytes(bytes, bits);
    arm_nominal_tick(bits);
    monitor_step();
    assert_lcd_frame("T:-10.0C H:50.0%", "N:07 S:0000 OK");
}

void test_config_constants(void) {
    assert_pin_constants();
    assert_frame_constants();
}

void test_packet_artifact_constants(void) {
    assert_artifact_ids();
    assert_artifact_frame();
}

void test_crc16_ccitt(void) {
    TEST_ASSERT_EQUAL_HEX16(0xFFFFu,
                            crc16_ccitt((const uint8_t *)"", 0u));
    TEST_ASSERT_EQUAL_HEX16(0x29B1u,
                            crc16_ccitt((const uint8_t *)"123456789", 9u));
}

void test_dht_parse_bits_valid(void) {
    uint16_t bits[SENSOR_BIT_COUNT];
    fill_widths(bits);
    TEST_ASSERT_TRUE(dht_parse_bits(bits, &s_reading));
    TEST_ASSERT_TRUE(s_reading.valid);
    TEST_ASSERT_EQUAL_INT(230, s_reading.temperature_tenths);
    TEST_ASSERT_EQUAL_UINT(610u, s_reading.humidity_tenths);
}

void test_dht_parse_bits_checksum_fail(void) {
    /**
     * @brief Declaration of bits.
     */
    uint16_t bits[SENSOR_BIT_COUNT];
    /**
     * @brief Declaration of r.
     */
    dht_reading_t r;
    /**
     * @brief Declaration of i.
     */
    uint8_t i;
    for (i = 0u; i < SENSOR_BIT_COUNT; ++i) {
        bits[i] = s_widths[i];
    }
    bits[39] = 70u;
    TEST_ASSERT_FALSE(dht_parse_bits(bits, &r));
}

void test_dht_parse_bits_null(void) {
    /**
     * @brief Declaration of bits.
     */
    uint16_t bits[SENSOR_BIT_COUNT];
    /**
     * @brief Declaration of r.
     */
    dht_reading_t r;
    TEST_ASSERT_FALSE(dht_parse_bits(NULL, &r));
    TEST_ASSERT_FALSE(dht_parse_bits(bits, NULL));
}

void test_sensor_build_frame(void) {
    char frame[COLD_CHAIN_MONITOR_FRAME_SIZE];
    size_t n;
    load_reading();
    n = sensor_build_frame(&s_reading, 0u, frame, sizeof(frame));
    assert_frame_padding(frame, n, sizeof(frame));
    assert_frame_rejects(frame, sizeof(frame));
}

void test_sensor_read_dht_waveform(void) {
    sensor_init();
    mock_dht_timeline(s_widths, 0u);
    TEST_ASSERT_EQUAL(SENSOR_RESULT_OK, sensor_read(&s_reading));
    TEST_ASSERT_TRUE(s_reading.valid);
    TEST_ASSERT_EQUAL_INT(230, s_reading.temperature_tenths);
    TEST_ASSERT_EQUAL_UINT(610u, s_reading.humidity_tenths);
}

void test_sensor_read_timeout(void) {
    /**
     * @brief Declaration of r.
     */
    dht_reading_t r;
    sensor_init();
    TEST_ASSERT_EQUAL(SENSOR_RESULT_TIMEOUT, sensor_read(&r));
}

void test_radio_build_send_cmd(void) {
    char cmd[64];
    radio_result_t rc;
    rc = radio_build_send_cmd(0x0001u, (const uint8_t *)"abc", 3u, cmd,
                              sizeof(cmd));
    TEST_ASSERT_EQUAL(RADIO_RESULT_OK, rc);
    TEST_ASSERT_EQUAL_STRING("AT+SEND=0001,3,abc\r\n", cmd);
    assert_build_rejects(cmd, sizeof(cmd));
}

void test_radio_parse_rcv(void) {
    assert_rcv_json();
    assert_rcv_comma();
    assert_rcv_no_tail();
}

void test_radio_parse_rcv_rejects(void) {
    assert_rcv_rejects();
}

void test_radio_line_pump(void) {
    char line[32];
    size_t len = 0u;
    mock_uart_reset();
    mock_uart_set_rx("ab\r\ncd\r\n", 8u);
    assert_pump_lines(line, &len);
}

void test_radio_spoofed_sender_attribution(void) {
    /**
     * @brief Declaration of rcv.
     */
    radio_rcv_t rcv;
    /**
     * @brief Declaration of rc.
     */
    radio_result_t rc;
    rc = radio_parse_rcv("+RCV=0007,7,{\"a\",1},-90,3", &rcv);
    TEST_ASSERT_EQUAL(RADIO_RESULT_OK, rc);
    TEST_ASSERT_TRUE(radio_frame_is_from(&rcv, 0x0007u));
    TEST_ASSERT_FALSE(radio_frame_is_from(&rcv, 0x0008u));
}

void test_display_format_lines(void) {
    assert_format_ok();
    assert_format_fail();
}

void test_display_render_lines(void) {
    load_display_lines();
    mock_i2c_reset();
    display_render_lines(i2c1, COLD_CHAIN_MONITOR_LCD_ADDR, s_line1, s_line2);
    assert_lcd_frame("T:23.0C H:61.0%", "N:07 S:0042 OK");
}

void test_monitor_init(void) {
    reset_all();
    TEST_ASSERT_TRUE(monitor_init());
    TEST_ASSERT_EQUAL_UINT(115200u, s_mock_uart_baud);
    TEST_ASSERT_TRUE(s_mock_gpio_dirs[COLD_CHAIN_MONITOR_LED_PIN]);
    TEST_ASSERT(mock_i2c_log_count() > 0u);
}

void test_monitor_step_transmits(void) {
    arm_transmit_tick();
    monitor_step();
    assert_transmit_frame();
    assert_lcd_frame("T:23.0C H:61.0%", "N:07 S:0000 OK");
}

void test_monitor_transmit_no_key(void) {
    g_key_ready = false;
    mock_uart_reset();
    monitor_transmit(&s_reading);
    TEST_ASSERT_EQUAL_UINT(0u, (unsigned)mock_uart_get_tx(s_tx, sizeof(s_tx) - 1u));
    g_key_ready = true;
}

/**
 * @brief Initialize the monitor and queue one inbound +RCV line.
 *
 * @param void No parameters.
 * @return void
 */
static void arm_rcv_blink(void) {
    TEST_ASSERT_TRUE(monitor_init());
    mock_uart_reset();
    mock_uart_set_rx("+RCV=0002,10,{\"cmd\":91},-58,4\r\n",
                     sizeof("+RCV=0002,10,{\"cmd\":91},-58,4\r\n") - 1u);
}

void test_monitor_step_rcv_blink(void) {
    size_t tx_len;
    arm_rcv_blink();
    monitor_step();
    tx_len = mock_uart_get_tx(s_tx, sizeof(s_tx) - 1u);
    TEST_ASSERT_EQUAL_UINT(0u, (unsigned)tx_len);
    TEST_ASSERT_EQUAL_UINT(2u, mock_gpio_toggles(COLD_CHAIN_MONITOR_LED_PIN));
}

void test_sensor_policy_not_ready(void) {
    /**
     * @brief Declaration of r.
     */
    dht_reading_t r;
    sensor_deinit();
    TEST_ASSERT_EQUAL(SENSOR_RESULT_POLICY_ERROR, sensor_read(&r));
}

void test_sensor_policy_null_out(void) {
    sensor_init();
    TEST_ASSERT_EQUAL(SENSOR_RESULT_POLICY_ERROR, sensor_read(NULL));
}

void test_sensor_dht_negative_temp(void) {
    /**
     * @brief Declaration of bytes.
     */
    const uint8_t bytes[SENSOR_BYTE_COUNT] = {0u, 0u, 0x82u, 3u, 0x85u};
    /**
     * @brief Declaration of bits.
     */
    uint16_t bits[SENSOR_BIT_COUNT];
    /**
     * @brief Declaration of r.
     */
    dht_reading_t r;
    build_bits_from_bytes(bytes, bits);
    TEST_ASSERT_TRUE(dht_parse_bits(bits, &r));
    TEST_ASSERT_TRUE(r.valid);
    TEST_ASSERT_EQUAL_INT(-17, r.temperature_tenths);
    TEST_ASSERT_EQUAL_UINT(0u, r.humidity_tenths);
}

void test_sensor_read_timeout_response_low(void) {
    /**
     * @brief Declaration of offsets.
     */
    uint32_t offsets[4] = {0u, 30u};
    /**
     * @brief Declaration of levels.
     */
    int levels[4] = {1, 0};
    /**
     * @brief Declaration of r.
     */
    dht_reading_t r;
    sensor_init();
    mock_gpio_timeline_begin_at(0u, offsets, levels, 2u, COLD_CHAIN_MONITOR_DHT_PIN);
    TEST_ASSERT_EQUAL(SENSOR_RESULT_TIMEOUT, sensor_read(&r));
}

void test_sensor_read_timeout_response_high(void) {
    /**
     * @brief Declaration of offsets.
     */
    uint32_t offsets[4] = {0u, 30u, 110u};
    /**
     * @brief Declaration of levels.
     */
    int levels[4] = {1, 0, 1};
    /**
     * @brief Declaration of r.
     */
    dht_reading_t r;
    sensor_init();
    mock_gpio_timeline_begin_at(0u, offsets, levels, 3u, COLD_CHAIN_MONITOR_DHT_PIN);
    TEST_ASSERT_EQUAL(SENSOR_RESULT_TIMEOUT, sensor_read(&r));
}

void test_sensor_read_timeout_bit_low(void) {
    /**
     * @brief Declaration of offsets.
     */
    uint32_t offsets[4] = {0u, 30u, 110u, 190u};
    /**
     * @brief Declaration of levels.
     */
    int levels[4] = {1, 0, 1, 0};
    /**
     * @brief Declaration of r.
     */
    dht_reading_t r;
    sensor_init();
    mock_gpio_timeline_begin_at(0u, offsets, levels, 4u, COLD_CHAIN_MONITOR_DHT_PIN);
    TEST_ASSERT_EQUAL(SENSOR_RESULT_TIMEOUT, sensor_read(&r));
}

void test_sensor_read_measure_timeout(void) {
    /**
     * @brief Declaration of offsets.
     */
    uint32_t offsets[8] = {0u, 30u, 110u, 190u, 240u};
    /**
     * @brief Declaration of levels.
     */
    int levels[8] = {1, 0, 1, 0, 1};
    /**
     * @brief Declaration of r.
     */
    dht_reading_t r;
    sensor_init();
    mock_gpio_timeline_begin_at(0u, offsets, levels, 5u, COLD_CHAIN_MONITOR_DHT_PIN);
    TEST_ASSERT_EQUAL(SENSOR_RESULT_TIMEOUT, sensor_read(&r));
}

void test_sensor_read_dht_crc_error(void) {
    uint16_t bad[SENSOR_BIT_COUNT];
    fill_widths(bad);
    bad[39] = 70u;
    sensor_init();
    mock_dht_timeline(bad, 0u);
    TEST_ASSERT_EQUAL(SENSOR_RESULT_CRC_ERROR, sensor_read(&s_reading));
}

void test_radio_hex_digits(void) {
    /**
     * @brief Declaration of rcv.
     */
    radio_rcv_t rcv;
    TEST_ASSERT_EQUAL(RADIO_RESULT_OK,
                      radio_parse_rcv("+RCV=00a7,2,ok,-3,2", &rcv));
    TEST_ASSERT_EQUAL_HEX16(0x00A7u, rcv.sender);
    TEST_ASSERT_EQUAL(RADIO_RESULT_OK,
                      radio_parse_rcv("+RCV=00FE,2,ok,-3,2", &rcv));
    TEST_ASSERT_EQUAL_HEX16(0x00FEu, rcv.sender);
}

void test_radio_build_send_cmd_oversize_cmd(void) {
    /**
     * @brief Declaration of payload.
     */
    const uint8_t payload[30] = "{\"n\":7,\"s\":0,\"t\":230,\"h\":610}";
    /**
     * @brief Declaration of tiny.
     */
    char tiny[16];
    TEST_ASSERT_EQUAL(RADIO_RESULT_OVERSIZE,
                      radio_build_send_cmd(0x0001u, payload, 29u, tiny,
                                           sizeof(tiny)));
}

void test_radio_send_frame_oversize(void) {
    /**
     * @brief Declaration of payload.
     */
    const uint8_t payload[30] = "{\"n\":7,\"s\":0,\"t\":230,\"h\":610}";
    TEST_ASSERT_EQUAL(RADIO_RESULT_OVERSIZE,
                      radio_send_frame(uart0, payload, 257u));
}

void test_radio_parse_missing_commas(void) {
    /**
     * @brief Declaration of rcv.
     */
    radio_rcv_t rcv;
    TEST_ASSERT_EQUAL(RADIO_RESULT_PARSE_ERROR,
                      radio_parse_rcv("+RCV=007ZX,3,hi,-1,1", &rcv));
    TEST_ASSERT_EQUAL(RADIO_RESULT_PARSE_ERROR,
                      radio_parse_rcv("+RCV=0007,9Z,hi,-1,1", &rcv));
}

void test_monitor_not_ready(void) {
    monitor_deinit();
    TEST_ASSERT_FALSE(monitor_step());
}

void test_monitor_init_lcd_fail(void) {
    mock_i2c_set_write_fail(true);
    TEST_ASSERT_FALSE(monitor_init());
}

void test_monitor_step_sensor_timeout(void) {
    size_t tx_len;
    arm_timeout_tick();
    monitor_step();
    tx_len = mock_uart_get_tx(s_tx, sizeof(s_tx) - 1u);
    TEST_ASSERT_EQUAL_UINT(0u, (unsigned)tx_len);
    TEST_ASSERT_EQUAL_UINT(0u, mock_gpio_toggles(COLD_CHAIN_MONITOR_LED_PIN));
    assert_lcd_frame("T:0.0C H:0.0%", "N:07 S:0000 !!");
}

void setUp(void) {
    reset_all();
}

void tearDown(void) {
}

/**
 * @brief Run the provisioning, artifact, and display tests.
 *
 * @param void No parameters.
 * @return void
 */
static void run_basic_tests(void) {
    RUN_TEST(test_config_constants);
    RUN_TEST(test_packet_artifact_constants);
    RUN_TEST(test_crc16_ccitt);
    RUN_TEST(test_display_format_lines);
    RUN_TEST(test_display_render_lines);
}

/**
 * @brief Run the sensor sampling and frame tests.
 *
 * @param void No parameters.
 * @return void
 */
static void run_sensor_tests(void) {
    RUN_TEST(test_dht_parse_bits_valid);
    RUN_TEST(test_dht_parse_bits_checksum_fail);
    RUN_TEST(test_dht_parse_bits_null);
    RUN_TEST(test_sensor_build_frame);
    RUN_TEST(test_sensor_read_dht_waveform);
    RUN_TEST(test_sensor_read_timeout);
}

/**
 * @brief Run the radio protocol tests.
 *
 * @param void No parameters.
 * @return void
 */
static void run_radio_tests(void) {
    RUN_TEST(test_radio_build_send_cmd);
    RUN_TEST(test_radio_parse_rcv);
    RUN_TEST(test_radio_parse_rcv_rejects);
    RUN_TEST(test_radio_line_pump);
    RUN_TEST(test_radio_spoofed_sender_attribution);
}

/**
 * @brief Run the monitor state-machine tests.
 *
 * @param void No parameters.
 * @return void
 */
static void run_monitor_tests(void) {
    RUN_TEST(test_monitor_init);
    RUN_TEST(test_monitor_step_transmits);
    RUN_TEST(test_monitor_transmit_no_key);
    RUN_TEST(test_monitor_step_nominal);
    RUN_TEST(test_monitor_step_rcv_blink);
    RUN_TEST(test_monitor_not_ready);
    RUN_TEST(test_monitor_init_lcd_fail);
    RUN_TEST(test_monitor_step_sensor_timeout);
}

/**
 * @brief Run the sensor policy and timeout tests.
 *
 * @param void No parameters.
 * @return void
 */
static void run_policy_tests(void) {
    RUN_TEST(test_sensor_policy_not_ready);
    RUN_TEST(test_sensor_policy_null_out);
    RUN_TEST(test_sensor_dht_negative_temp);
    RUN_TEST(test_sensor_read_timeout_response_low);
    RUN_TEST(test_sensor_read_timeout_response_high);
    RUN_TEST(test_sensor_read_timeout_bit_low);
    RUN_TEST(test_sensor_read_measure_timeout);
    RUN_TEST(test_sensor_read_dht_crc_error);
}

/**
 * @brief Run the spoof-surface and builder edge-case tests.
 *
 * @param void No parameters.
 * @return void
 */
static void run_security_tests(void) {
    RUN_TEST(test_radio_hex_digits);
    RUN_TEST(test_radio_build_send_cmd_oversize_cmd);
    RUN_TEST(test_radio_send_frame_oversize);
    RUN_TEST(test_radio_parse_missing_commas);
}

/**
 * @brief Run the peripheral and security module test groups.
 *
 * @param void No parameters.
 * @return void
 */
extern void run_peripheral_and_crypto_tests(void);

int main(void) {
    TEST_BEGIN();
    run_basic_tests();
    run_sensor_tests();
    run_radio_tests();
    run_monitor_tests();
    run_policy_tests();
    run_security_tests(); run_peripheral_and_crypto_tests();
    return TEST_END();
}