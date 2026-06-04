/*
 * Practice 2, Exercise 1: LoRaWAN Random Retry Intervals
 * 
 * This exercise implements randomized retry intervals for LoRaWAN
 * JoinRequest and data transmission packets as required by the
 * LoRaWAN specification.
 * 
 * Objectives:
 * - Implement random backoff for transmission retries
 * - Comply with LoRaWAN specification
 * - Use RIOT's random number generation
 */

#include <stdio.h>
#include <stdint.h>
#include <random.h>
#include <xtimer.h>

/* Compile-time configuration */
#ifndef RETRY_MIN_DELAY_MS
#define RETRY_MIN_DELAY_MS      1000    /* Minimum retry delay in milliseconds */
#endif

#ifndef RETRY_MAX_DELAY_MS
#define RETRY_MAX_DELAY_MS      10000   /* Maximum retry delay in milliseconds */
#endif

#ifndef MAX_RETRIES
#define MAX_RETRIES             5       /* Maximum number of retry attempts */
#endif

/**
 * Calculate random backoff delay according to LoRaWAN specification
 * 
 * @return Random delay in milliseconds
 */
static uint32_t calculate_random_backoff(void)
{
    uint32_t min_ms = RETRY_MIN_DELAY_MS;
    uint32_t max_ms = RETRY_MAX_DELAY_MS;
    uint32_t random_value = random_uint32();
    
    /* Map random value to range [min_ms, max_ms] */
    uint32_t delay_ms = min_ms + (random_value % (max_ms - min_ms + 1));
    
    return delay_ms;
}

/**
 * Simulate transmission with random retry logic
 */
static void simulate_transmission_with_retries(void)
{
    printf("Starting transmission with random retry intervals\n");
    printf("Configuration: [%u, %u] ms, max retries: %u\n",
           RETRY_MIN_DELAY_MS, RETRY_MAX_DELAY_MS, MAX_RETRIES);
    
    for (uint8_t attempt = 0; attempt < MAX_RETRIES; attempt++) {
        printf("\n[Attempt %u] Sending packet...\n", attempt + 1);
        
        /* Simulate packet transmission */
        printf("[Attempt %u] Packet sent, waiting for acknowledgment\n", attempt + 1);
        
        /* In a real application, check for ACK here */
        /* If ACK received, break; otherwise continue */
        
        if (attempt < MAX_RETRIES - 1) {
            uint32_t backoff_ms = calculate_random_backoff();
            printf("[Attempt %u] No ACK received. Retrying in %lu ms\n",
                   attempt + 1, (unsigned long)backoff_ms);
            xtimer_msleep(backoff_ms);
        }
    }
    
    printf("\nTransmission sequence complete\n");
}

int main(void)
{
    printf("=== Practice 2, Exercise 1: LoRaWAN Random Retry Intervals ===\n\n");
    
    /* Run simulation multiple times to demonstrate randomization */
    for (int i = 0; i < 3; i++) {
        printf("\n--- Transmission Sequence %d ---\n", i + 1);
        simulate_transmission_with_retries();
        xtimer_msleep(2000);  /* Pause between sequences */
    }
    
    printf("\n=== Exercise Complete ===\n");
    return 0;
}
