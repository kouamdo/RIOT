/*
 * Practice 2, Exercise 4: BLE Cycling Speed and Cadence Profile
 * 
 * Implement a Bluetooth Low Energy GATT service for Cycling Speed and Cadence.
 * Counts pulses on a GPIO input and transmits cadence data via BLE.
 * 
 * Objectives:
 * - Implement GATT Cycling Speed and Cadence (CSC) service
 * - Count pulses on GPIO input
 * - Use NimBLE library for BLE profile implementation
 * - Transmit sensor data via BLE notifications
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <host/ble_gap.h>
#include <host/ble_gatt.h>
#include <host/ble_hs.h>

/* GPIO configuration */
#ifndef GPIO_SENSOR_PIN
#define GPIO_SENSOR_PIN     13      /* GPIO pin for pulse sensor */
#endif

/* GATT Service and Characteristic UUIDs */
#define CSC_SERVICE_UUID            0x1816  /* Cycling Speed and Cadence */
#define CSC_MEASUREMENT_CHAR_UUID   0x2A5B  /* CSC Measurement */
#define CSC_FEATURE_CHAR_UUID       0x2A5C  /* CSC Feature */

/**
 * Cycling Speed and Cadence measurement data
 */
typedef struct {
    uint8_t flags;              /* Measurement flags */
    uint16_t crank_revolutions; /* Cumulative crank revolutions */
    uint16_t crank_event_time;  /* Last crank event time */
    uint16_t wheel_revolutions; /* Cumulative wheel revolutions (optional) */
    uint16_t wheel_event_time;  /* Last wheel event time (optional) */
} csc_measurement_t;

/**
 * CSC Feature flags
 */
#define CSC_FEATURE_WHEEL_REV_SUPPORTED (1 << 0)
#define CSC_FEATURE_CRANK_REV_SUPPORTED (1 << 1)

/**
 * Global pulse counter
 */
static volatile uint32_t pulse_count = 0;
static uint32_t last_pulse_time = 0;

/**
 * GPIO interrupt handler (pulse sensor)
 */
static void pulse_sensor_callback(void *arg)
{
    (void)arg;
    pulse_count++;
    last_pulse_time = 0;  /* Mark for timestamp update */
    printf("[Pulse] Count: %lu\n", (unsigned long)pulse_count);
}

/**
 * Prepare CSC measurement data
 */
static void prepare_csc_measurement(csc_measurement_t *meas)
{
    meas->flags = CSC_FEATURE_CRANK_REV_SUPPORTED;
    meas->crank_revolutions = (uint16_t)(pulse_count & 0xFFFF);
    meas->crank_event_time = (uint16_t)(last_pulse_time & 0xFFFF);
}

/**
 * Encode CSC measurement for BLE transmission
 */
static int encode_csc_measurement(uint8_t *buf, const csc_measurement_t *meas)
{
    int pos = 0;
    
    /* Flags byte */
    buf[pos++] = meas->flags;
    
    /* Crank revolutions (little-endian) */
    buf[pos++] = (meas->crank_revolutions >> 0) & 0xFF;
    buf[pos++] = (meas->crank_revolutions >> 8) & 0xFF;
    
    /* Crank event time (little-endian) */
    buf[pos++] = (meas->crank_event_time >> 0) & 0xFF;
    buf[pos++] = (meas->crank_event_time >> 8) & 0xFF;
    
    /* Wheel data (optional, if flag is set) */
    if (meas->flags & CSC_FEATURE_WHEEL_REV_SUPPORTED) {
        buf[pos++] = (meas->wheel_revolutions >> 0) & 0xFF;
        buf[pos++] = (meas->wheel_revolutions >> 8) & 0xFF;
        buf[pos++] = (meas->wheel_event_time >> 0) & 0xFF;
        buf[pos++] = (meas->wheel_event_time >> 8) & 0xFF;
    }
    
    return pos;  /* Return number of bytes encoded */
}

/**
 * Simulate pulse events (for demonstration without hardware)
 */
static void simulate_pedal_pulses(void)
{
    printf("=== Simulating Pedal Pulses ===\n");
    
    /* Simulate 10 pulses (one pedal revolution = 2 pulses per leg) */
    for (int i = 0; i < 10; i++) {
        pulse_count++;
        printf("Pulse #%d detected (Cadence: ~%u RPM)\n", i + 1,
               (pulse_count * 60) / 2);  /* Assuming 1 rev = 2 pulses */
    }
}

/**
 * Transmit CSC measurement via BLE
 */
static void send_csc_notification(void)
{
    csc_measurement_t measurement;
    uint8_t encoded[11];  /* Max size for CSC measurement */
    
    printf("=== Sending CSC Measurement ===\n");
    
    prepare_csc_measurement(&measurement);
    int encoded_len = encode_csc_measurement(encoded, &measurement);
    
    printf("Encoded CSC data (%d bytes):\n  ", encoded_len);
    for (int i = 0; i < encoded_len; i++) {
        printf("%02X ", encoded[i]);
    }
    printf("\n");
    
    printf("Measurement data:\n");
    printf("  Flags: 0x%02X\n", measurement.flags);
    printf("  Crank Revolutions: %u\n", measurement.crank_revolutions);
    printf("  Crank Event Time: %u\n", measurement.crank_event_time);
    
    /* In a real implementation, send via ble_gatt_chr_notify() */
    printf("[Simulated] BLE notification sent\n");
}

int main(void)
{
    printf("=== Practice 2, Exercise 4: BLE Cycling Speed & Cadence ===\n\n");
    
    printf("Configuration:\n");
    printf("  Sensor GPIO Pin: %d\n", GPIO_SENSOR_PIN);
    printf("  GATT Service: Cycling Speed and Cadence (0x%04X)\n",
           CSC_SERVICE_UUID);
    printf("  Measurement Characteristic: 0x%04X\n",
           CSC_MEASUREMENT_CHAR_UUID);
    printf("\n");
    
    /* Simulate pedal activity */
    simulate_pedal_pulses();
    printf("\n");
    
    /* Send measurement via BLE */
    send_csc_notification();
    
    printf("\n=== Exercise Implementation Notes ===\n");
    printf("1. GPIO interrupt handler triggers on rising edge\n");
    printf("2. Each pulse increments the crank revolution counter\n");
    printf("3. Timestamp captures the event time for cadence calculation\n");
    printf("4. BLE notifications sent at regular intervals or on change\n");
    printf("5. Mobile app can calculate cadence from timestamp deltas\n");
    
    printf("\n=== Exercise Complete ===\n");
    return 0;
}
