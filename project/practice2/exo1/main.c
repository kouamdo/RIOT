/*
 * Practice 2 - Exercise 1
 * Bluetooth retry example for ESP32.
 *
 * We use Bluetooth because the long range radio setup is difficult with the
 * current board. The idea is to retry after a random delay when something fails.
 */

#include <stdint.h>
#include <stdio.h>

#include "random.h"
#include "xtimer.h"

#define ADV_MIN_DELAY_MS        (500U)
#define ADV_MAX_DELAY_MS        (2000U)
#define SEND_MIN_DELAY_MS       (1000U)
#define SEND_MAX_DELAY_MS       (4000U)
#define MAX_TRIES               (5U)

static uint32_t random_delay(uint32_t min_ms, uint32_t max_ms)
{
    return random_uint32_range(min_ms, max_ms + 1U);
}

int main(void)
{
    uint32_t seed = xtimer_now_usec();
    random_init(seed);

    puts("Practice 2 - Exercise 1");
    puts("Bluetooth retry example on ESP32");
    printf("seed = %lu\n", (unsigned long)seed);

    puts("\n1) Advertising the board");

    for (unsigned i = 1; i <= MAX_TRIES; i++) {
        printf("advertising try %u/%u\n", i, MAX_TRIES);
        puts("device name: RIOT-ESP32-EXO1");

        if (i == 3) {
            puts("scanner found the board");
            break;
        }

        uint32_t wait = random_delay(ADV_MIN_DELAY_MS, ADV_MAX_DELAY_MS);
        printf("not found yet, wait %lu ms\n", (unsigned long)wait);
        xtimer_msleep(250);
    }

    puts("\n2) Sending a small Bluetooth value");

    for (unsigned i = 1; i <= MAX_TRIES; i++) {
        printf("send try %u/%u\n", i, MAX_TRIES);
        puts("value: hello from esp32");

        if (i == 2) {
            puts("ack received");
            puts("exercise finished");
            return 0;
        }

        uint32_t wait = random_delay(SEND_MIN_DELAY_MS, SEND_MAX_DELAY_MS);
        printf("no ack, wait %lu ms\n", (unsigned long)wait);
        xtimer_msleep(250);
    }

    puts("no ack after all tries");
    return 1;
}
