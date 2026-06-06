/*
 * Practice 2, Exercise 4: Cycling Speed and Cadence profile preparation.
 *
 * ESP32-only mode: simulate GPIO pulses and encode the Bluetooth CSC
 * Measurement characteristic payload. A later NimBLE implementation can reuse
 * the counter, timestamp, cadence, and payload encoding logic.
 */

#include <stdint.h>
#include <stdio.h>

#include "xtimer.h"

#define CSC_FLAG_CRANK_REV_PRESENT     (1U << 1)
#define SIMULATED_PULSES               (12U)
#define PULSE_INTERVAL_MS              (350U)
#define EVENT_TIME_UNITS_PER_SEC       (1024U)

typedef struct {
    uint8_t flags;
    uint16_t crank_revolutions;
    uint16_t last_crank_event_time;
} csc_measurement_t;

static uint16_t crank_revolutions;
static uint16_t last_event_time_1024;
static uint32_t previous_pulse_ms;

static size_t encode_csc_measurement(uint8_t *buf, const csc_measurement_t *meas)
{
    buf[0] = meas->flags;
    buf[1] = (uint8_t)(meas->crank_revolutions & 0xffU);
    buf[2] = (uint8_t)(meas->crank_revolutions >> 8);
    buf[3] = (uint8_t)(meas->last_crank_event_time & 0xffU);
    buf[4] = (uint8_t)(meas->last_crank_event_time >> 8);

    return 5U;
}

static uint32_t cadence_rpm(uint32_t current_ms)
{
    if (previous_pulse_ms == 0U || current_ms <= previous_pulse_ms) {
        return 0U;
    }

    uint32_t delta_ms = current_ms - previous_pulse_ms;
    return 60000U / delta_ms;
}

static csc_measurement_t simulate_pulse(uint32_t elapsed_ms)
{
    uint32_t rpm = cadence_rpm(elapsed_ms);

    crank_revolutions++;
    last_event_time_1024 = (uint16_t)((elapsed_ms * EVENT_TIME_UNITS_PER_SEC) / 1000U);
    previous_pulse_ms = elapsed_ms;

    printf("Pulse at %lu ms -> cadence %lu RPM\n",
           (unsigned long)elapsed_ms, (unsigned long)rpm);

    return (csc_measurement_t) {
        .flags = CSC_FLAG_CRANK_REV_PRESENT,
        .crank_revolutions = crank_revolutions,
        .last_crank_event_time = last_event_time_1024,
    };
}

static void print_payload(const uint8_t *payload, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        printf("%02X ", payload[i]);
    }
    puts("");
}

int main(void)
{
    puts("=== Practice 2, Exercise 4: Cycling Speed and Cadence Encoder ===");
    puts("ESP32-only mode: GPIO pulse simulation + BLE CSC payload encoding.");
    puts("Real GATT notifications will need NimBLE-capable BLE support.\n");

    for (uint8_t i = 0; i < SIMULATED_PULSES; i++) {
        uint32_t elapsed_ms = (uint32_t)(i + 1U) * PULSE_INTERVAL_MS;
        csc_measurement_t measurement = simulate_pulse(elapsed_ms);
        uint8_t payload[5];
        size_t payload_len = encode_csc_measurement(payload, &measurement);

        printf("  revolutions=%u, event_time=%u, payload=",
               measurement.crank_revolutions,
               measurement.last_crank_event_time);
        print_payload(payload, payload_len);

        xtimer_msleep(50);
    }

    puts("Exercise complete");
    return 0;
}
