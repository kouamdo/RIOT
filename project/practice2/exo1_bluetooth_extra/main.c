/*
 * Practice 2 - Exercise 1, Bluetooth version.
 *
 * This file replaces the old radio test with a simple Bluetooth-style retry
 * demo that can run on the ESP32 without extra modules.
 */

#include <stdint.h>
#include <stdio.h>

#include "random.h"
#include "xtimer.h"

#define MAX_TRIES       (4U)
#define MIN_WAIT_MS     (800U)
#define MAX_WAIT_MS     (3000U)

int main(void)
{
    random_init(xtimer_now_usec());

    puts("Practice 2 - Exercise 1");
    puts("Bluetooth connection retry demo");

    for (unsigned i = 1; i <= MAX_TRIES; i++) {
        printf("connection try %u/%u\n", i, MAX_TRIES);
        puts("advertising as RIOT-BT-STUDENT");

        if (i == 3) {
            puts("connected");
            puts("sending: practice2 exo1");
            puts("ack received");
            return 0;
        }

        uint32_t wait = random_uint32_range(MIN_WAIT_MS, MAX_WAIT_MS + 1U);
        printf("connection failed, wait %lu ms\n", (unsigned long)wait);
        xtimer_msleep(250);
    }

    puts("connection failed after all tries");
    return 1;
}
