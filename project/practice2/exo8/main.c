/*
 * Practice 2, Exercise 8: LoRaWAN smart lock security analysis.
 *
 * ESP32-only mode: simulate packets and protocol checks locally. The point is
 * to show why a lock that opens on a fixed payload is vulnerable to replay and
 * why a monotonic counter or cryptographic authentication is needed.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "xtimer.h"

#define UNLOCK_COMMAND_LEN      (4U)
#define SECURE_PACKET_LEN       (UNLOCK_COMMAND_LEN + 2U)

typedef enum {
    LOCK_LOCKED,
    LOCK_UNLOCKED,
    LOCK_ALARM,
} lock_state_t;

typedef struct {
    uint8_t payload[16];
    uint8_t len;
    int8_t rssi;
    uint32_t received_ms;
} radio_packet_t;

typedef struct {
    lock_state_t state;
    uint16_t expected_counter;
    uint16_t replay_detections;
} smart_lock_t;

static const uint8_t unlock_command[UNLOCK_COMMAND_LEN] = { 'o', 'p', 'e', 'n' };

static smart_lock_t lock = {
    .state = LOCK_LOCKED,
    .expected_counter = 1U,
};

static const char *lock_state_name(lock_state_t state)
{
    switch (state) {
    case LOCK_LOCKED:
        return "locked";
    case LOCK_UNLOCKED:
        return "unlocked";
    case LOCK_ALARM:
        return "alarm";
    default:
        return "unknown";
    }
}

static bool payload_is_open_command(const radio_packet_t *packet)
{
    return packet->len >= UNLOCK_COMMAND_LEN &&
           memcmp(packet->payload, unlock_command, UNLOCK_COMMAND_LEN) == 0;
}

static void vulnerable_handler(const radio_packet_t *packet)
{
    printf("vulnerable handler: len=%u rssi=%d\n", packet->len, packet->rssi);

    if (packet->len == UNLOCK_COMMAND_LEN && payload_is_open_command(packet)) {
        lock.state = LOCK_UNLOCKED;
        puts("  accepted fixed payload 'open' -> lock opens");
    }
    else {
        puts("  rejected packet");
    }
}

static void secure_counter_handler(const radio_packet_t *packet)
{
    printf("counter handler: len=%u rssi=%d\n", packet->len, packet->rssi);

    if (packet->len != SECURE_PACKET_LEN || !payload_is_open_command(packet)) {
        puts("  rejected malformed command");
        return;
    }

    uint16_t counter = ((uint16_t)packet->payload[4] << 8) | packet->payload[5];
    printf("  counter=%u expected=%u\n", counter, lock.expected_counter);

    if (counter == lock.expected_counter) {
        lock.expected_counter++;
        lock.state = LOCK_UNLOCKED;
        puts("  accepted fresh command -> lock opens");
    }
    else if (counter < lock.expected_counter) {
        lock.replay_detections++;
        lock.state = LOCK_ALARM;
        puts("  rejected old counter -> replay detected");
    }
    else {
        puts("  rejected future counter -> sequence gap");
    }
}

static radio_packet_t fixed_open_packet(void)
{
    return (radio_packet_t) {
        .payload = { 'o', 'p', 'e', 'n' },
        .len = UNLOCK_COMMAND_LEN,
        .rssi = -92,
        .received_ms = xtimer_now_usec() / 1000U,
    };
}

static radio_packet_t counted_open_packet(uint16_t counter)
{
    return (radio_packet_t) {
        .payload = { 'o', 'p', 'e', 'n', (uint8_t)(counter >> 8), (uint8_t)counter },
        .len = SECURE_PACKET_LEN,
        .rssi = -88,
        .received_ms = xtimer_now_usec() / 1000U,
    };
}

static void demonstrate_replay(void)
{
    radio_packet_t packet = fixed_open_packet();

    puts("Replay demonstration with fixed payload:");
    vulnerable_handler(&packet);
    printf("  state after first packet: %s\n", lock_state_name(lock.state));

    lock.state = LOCK_LOCKED;
    vulnerable_handler(&packet);
    printf("  state after replayed packet: %s\n", lock_state_name(lock.state));
    puts("  conclusion: capture-and-replay works against a fixed command\n");
}

static void demonstrate_counter_mitigation(void)
{
    radio_packet_t fresh = counted_open_packet(1U);
    radio_packet_t replay = counted_open_packet(1U);

    lock.state = LOCK_LOCKED;
    lock.expected_counter = 1U;
    lock.replay_detections = 0U;

    puts("Counter mitigation demonstration:");
    secure_counter_handler(&fresh);
    printf("  state after fresh packet: %s\n", lock_state_name(lock.state));

    lock.state = LOCK_LOCKED;
    secure_counter_handler(&replay);
    printf("  state after replayed packet: %s\n", lock_state_name(lock.state));
    printf("  replay detections: %u\n\n", lock.replay_detections);
}

static void discuss_jamming(void)
{
    puts("Jamming analysis:");
    puts("  - LoRa modulation helps against noise but does not make jamming impossible");
    puts("  - a smart lock must fail safe when communication is unavailable");
    puts("  - local access should not depend only on the radio path");
    puts("  - repeated radio failures should be logged and reported later");
}

int main(void)
{
    puts("=== Practice 2, Exercise 8: LoRaWAN Smart Lock Security Simulation ===");
    puts("ESP32-only mode: local replay and mitigation model.\n");

    demonstrate_replay();
    demonstrate_counter_mitigation();
    discuss_jamming();

    puts("\nExercise complete");
    return 0;
}
