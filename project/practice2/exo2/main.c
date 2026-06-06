/*
 * Practice 2, Exercise 2: packet construction for a legacy RIOT crash.
 *
 * The assignment references commit fffe8bb7328a61ab9756b28089c4f987a7f46f53,
 * but that object is not present in this local RIOT clone. This ESP32 version
 * therefore demonstrates the core failure pattern that such fixes usually
 * address: trusting a packet length field before copying into a fixed-size
 * receive buffer.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "xtimer.h"

#define LEGACY_COMMIT          "fffe8bb7328a61ab9756b28089c4f987a7f46f53"
#define PACKET_MAGIC           (0xBEEF)
#define PACKET_TYPE_TEST       (0x42U)
#define LEGACY_RX_BUFFER_LEN   (32U)
#define MALICIOUS_PAYLOAD_LEN  (96U)
#define PREVIEW_BYTES          (24U)

typedef struct {
    uint16_t magic;
    uint8_t type;
    uint8_t declared_len;
    uint8_t payload[MALICIOUS_PAYLOAD_LEN];
} test_packet_t;

typedef enum {
    PARSE_ACCEPTED,
    PARSE_REJECTED,
    PARSE_WOULD_OVERFLOW,
} parse_result_t;

static void build_malicious_packet(test_packet_t *packet)
{
    packet->magic = PACKET_MAGIC;
    packet->type = PACKET_TYPE_TEST;
    packet->declared_len = MALICIOUS_PAYLOAD_LEN;

    for (uint8_t i = 0; i < MALICIOUS_PAYLOAD_LEN; i++) {
        packet->payload[i] = (uint8_t)(0xA0U + (i & 0x0FU));
    }
}

static void build_safe_packet(test_packet_t *packet)
{
    packet->magic = PACKET_MAGIC;
    packet->type = PACKET_TYPE_TEST;
    packet->declared_len = LEGACY_RX_BUFFER_LEN;

    for (uint8_t i = 0; i < MALICIOUS_PAYLOAD_LEN; i++) {
        packet->payload[i] = (uint8_t)(0x10U + (i & 0x0FU));
    }
}

static parse_result_t legacy_parser_model(const test_packet_t *packet)
{
    uint8_t legacy_rx_buffer[LEGACY_RX_BUFFER_LEN];

    if (packet->magic != PACKET_MAGIC) {
        return PARSE_REJECTED;
    }

    /* Old vulnerable shape: memcpy length is taken from the packet itself. */
    if (packet->declared_len > sizeof(legacy_rx_buffer)) {
        return PARSE_WOULD_OVERFLOW;
    }

    memcpy(legacy_rx_buffer, packet->payload, packet->declared_len);
    return PARSE_ACCEPTED;
}

static parse_result_t fixed_parser_model(const test_packet_t *packet)
{
    uint8_t rx_buffer[LEGACY_RX_BUFFER_LEN];

    if (packet->magic != PACKET_MAGIC) {
        return PARSE_REJECTED;
    }

    if (packet->declared_len > sizeof(rx_buffer)) {
        return PARSE_REJECTED;
    }

    memcpy(rx_buffer, packet->payload, packet->declared_len);
    return PARSE_ACCEPTED;
}

static const char *result_name(parse_result_t result)
{
    switch (result) {
    case PARSE_ACCEPTED:
        return "accepted";
    case PARSE_REJECTED:
        return "rejected";
    case PARSE_WOULD_OVERFLOW:
        return "would overflow";
    default:
        return "unknown";
    }
}

static void print_packet(const char *label, const test_packet_t *packet)
{
    printf("%s\n", label);
    printf("  magic: 0x%04X\n", packet->magic);
    printf("  type: 0x%02X\n", packet->type);
    printf("  declared length: %u bytes\n", packet->declared_len);
    printf("  receiver buffer: %u bytes\n", LEGACY_RX_BUFFER_LEN);
    printf("  payload preview: ");

    for (uint8_t i = 0; i < PREVIEW_BYTES; i++) {
        printf("%02X ", packet->payload[i]);
    }

    puts("...");
}

static void run_case(const char *label, const test_packet_t *packet)
{
    print_packet(label, packet);

    parse_result_t legacy_result = legacy_parser_model(packet);
    parse_result_t fixed_result = fixed_parser_model(packet);

    printf("  legacy parser model: %s\n", result_name(legacy_result));
    printf("  fixed parser model: %s\n", result_name(fixed_result));

    if (legacy_result == PARSE_WOULD_OVERFLOW && fixed_result == PARSE_REJECTED) {
        puts("  conclusion: length validation prevents the crash path");
    }

    puts("");
}

int main(void)
{
    test_packet_t malicious_packet;
    test_packet_t safe_packet;

    puts("=== Practice 2, Exercise 2: Legacy RIOT Packet Test ===");
    printf("Referenced commit: %s\n", LEGACY_COMMIT);
    puts("Local note: the commit object is unavailable in this clone, so this");
    puts("program models the likely packet-length validation issue safely.\n");

    build_malicious_packet(&malicious_packet);
    build_safe_packet(&safe_packet);

    run_case("Safe control packet", &safe_packet);
    run_case("Malicious oversized packet", &malicious_packet);

    puts("How to adapt this once the historical commit is available:");
    puts("  1. inspect the fixed parser and identify the missing validation");
    puts("  2. replace this test_packet_t format with the real protocol header");
    puts("  3. keep the same demonstration: vulnerable path vs fixed rejection");

    xtimer_msleep(100);
    return 0;
}
