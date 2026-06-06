/*
 * Practice 2, Exercise 1: LoRaWAN-style random retry intervals.
 *
 * The real LoRaWAN MAC is not used here because the current target is a single
 * ESP32 board without LoRa radio hardware. This program demonstrates the part
 * requested by the exercise: each retry delay is chosen randomly inside a
 * bounded window, as would be done for JoinRequest or confirmed uplink retries.
 */

#include <stdint.h>
#include <stdio.h>

#include "random.h"
#include "xtimer.h"

#ifndef JOIN_RETRY_MIN_MS
#define JOIN_RETRY_MIN_MS       (1000U)
#endif

#ifndef JOIN_RETRY_MAX_MS
#define JOIN_RETRY_MAX_MS       (5000U)
#endif

#ifndef UPLINK_RETRY_MIN_MS
#define UPLINK_RETRY_MIN_MS     (2000U)
#endif

#ifndef UPLINK_RETRY_MAX_MS
#define UPLINK_RETRY_MAX_MS     (10000U)
#endif

#ifndef MAX_RETRIES
#define MAX_RETRIES             (5U)
#endif

/* Keep the demo responsive on the serial console while printing real delays. */
#define DEMO_SLEEP_MS           (250U)

typedef struct {
    const char *name;
    uint32_t min_delay_ms;
    uint32_t max_delay_ms;
    uint8_t success_attempt;
} retry_scenario_t;

static uint32_t random_delay_ms(uint32_t min_delay_ms, uint32_t max_delay_ms)
{
    if (max_delay_ms <= min_delay_ms) {
        return min_delay_ms;
    }

    return random_uint32_range(min_delay_ms, max_delay_ms + 1U);
}

static void print_scenario_header(const retry_scenario_t *scenario)
{
    puts("");
    printf("Scenario: %s\n", scenario->name);
    printf("Retry window: %lu..%lu ms\n",
           (unsigned long)scenario->min_delay_ms,
           (unsigned long)scenario->max_delay_ms);
    printf("Max retries: %u\n", MAX_RETRIES);
}

static void run_retry_scenario(const retry_scenario_t *scenario)
{
    print_scenario_header(scenario);

    for (uint8_t attempt = 1U; attempt <= MAX_RETRIES; attempt++) {
        printf("Attempt %u: transmit packet\n", attempt);

        if (attempt == scenario->success_attempt) {
            printf("Attempt %u: ACK received, retry loop stops\n", attempt);
            return;
        }

        if (attempt == MAX_RETRIES) {
            puts("No ACK received after the final attempt");
            break;
        }

        uint32_t delay_ms = random_delay_ms(scenario->min_delay_ms,
                                            scenario->max_delay_ms);
        printf("Attempt %u: no ACK, next retry after %lu ms\n",
               attempt, (unsigned long)delay_ms);

        xtimer_msleep(DEMO_SLEEP_MS);
    }

    puts("Result: retry budget exhausted");
}

int main(void)
{
    uint32_t seed = xtimer_now_usec();
    random_init(seed);

    retry_scenario_t scenarios[] = {
        {
            .name = "JoinRequest",
            .min_delay_ms = JOIN_RETRY_MIN_MS,
            .max_delay_ms = JOIN_RETRY_MAX_MS,
            .success_attempt = 3U,
        },
        {
            .name = "Confirmed uplink",
            .min_delay_ms = UPLINK_RETRY_MIN_MS,
            .max_delay_ms = UPLINK_RETRY_MAX_MS,
            .success_attempt = 0U,
        },
    };

    puts("=== Practice 2, Exercise 1: LoRaWAN Random Retry Intervals ===");
    puts("ESP32-only mode: this is a retry/backoff simulation, not a LoRa radio demo.");
    printf("PRNG seed: %lu\n", (unsigned long)seed);

    for (unsigned i = 0; i < (sizeof(scenarios) / sizeof(scenarios[0])); i++) {
        run_retry_scenario(&scenarios[i]);
    }

    puts("");
    puts("Exercise complete");
    return 0;
}
