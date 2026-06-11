/*
 * Practice 2 - Exercise 1
 * ESP32 + UMDK RF 107 LoRaWAN test.
 *
 * The goal is simple: try to join a LoRaWAN network, wait a random time
 * between failed tries, then send one confirmed message.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "fmt.h"
#include "net/loramac.h"
#include "random.h"
#include "semtech_loramac.h"
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

#ifndef MAX_JOIN_ATTEMPTS
#define MAX_JOIN_ATTEMPTS       (5U)
#endif

#ifndef MAX_UPLINK_ATTEMPTS
#define MAX_UPLINK_ATTEMPTS     (5U)
#endif

extern semtech_loramac_t loramac;

static uint8_t deveui[LORAMAC_DEVEUI_LEN];
static uint8_t appeui[LORAMAC_APPEUI_LEN];
static uint8_t appkey[LORAMAC_APPKEY_LEN];

int main(void)
{
    uint32_t seed = xtimer_now_usec();
    random_init(seed);

    puts("Practice 2 - Exercise 1: LoRaWAN retry test");
    puts("Board: ESP32 with UMDK RF 107 module");
    printf("Random seed: %lu\n", (unsigned long)seed);

    /* Convert the keys given in the Makefile command line. */
    fmt_hex_bytes(deveui, CONFIG_LORAMAC_DEV_EUI_DEFAULT);
    fmt_hex_bytes(appeui, CONFIG_LORAMAC_APP_EUI_DEFAULT);
    fmt_hex_bytes(appkey, CONFIG_LORAMAC_APP_KEY_DEFAULT);

    semtech_loramac_set_deveui(&loramac, deveui);
    semtech_loramac_set_appeui(&loramac, appeui);
    semtech_loramac_set_appkey(&loramac, appkey);

    semtech_loramac_set_class(&loramac, LORAMAC_CLASS_A);
    semtech_loramac_set_dr(&loramac, LORAMAC_DR_5);
    semtech_loramac_set_tx_port(&loramac, 2);
    semtech_loramac_set_tx_mode(&loramac, LORAMAC_TX_CNF);

    puts("Trying to join the LoRaWAN network with OTAA...");

    int joined = 0;
    for (unsigned attempt = 1; attempt <= MAX_JOIN_ATTEMPTS; attempt++) {
        printf("Join try %u/%u\n", attempt, MAX_JOIN_ATTEMPTS);

        uint8_t ret = semtech_loramac_join(&loramac, LORAMAC_JOIN_OTAA);
        if ((ret == SEMTECH_LORAMAC_JOIN_SUCCEEDED) ||
            (ret == SEMTECH_LORAMAC_ALREADY_JOINED)) {
            puts("Join OK");
            joined = 1;
            break;
        }

        printf("Join failed, code = %u\n", ret);

        if (attempt < MAX_JOIN_ATTEMPTS) {
            uint32_t delay = random_uint32_range(JOIN_RETRY_MIN_MS,
                                                 JOIN_RETRY_MAX_MS + 1U);
            printf("Waiting %lu ms before next join try\n",
                   (unsigned long)delay);
            xtimer_msleep(delay);
        }
    }

    if (!joined) {
        puts("Could not join the network");
        return 1;
    }

    char message[] = "practice2 exo1";
    puts("Sending one confirmed uplink...");

    for (unsigned attempt = 1; attempt <= MAX_UPLINK_ATTEMPTS; attempt++) {
        printf("Send try %u/%u\n", attempt, MAX_UPLINK_ATTEMPTS);

        uint8_t ret = semtech_loramac_send(&loramac,
                                           (uint8_t *)message,
                                           strlen(message));
        if (ret == SEMTECH_LORAMAC_TX_DONE) {
            puts("Message sent");
            puts("Exercise finished");
            return 0;
        }

        printf("Send failed, code = %u\n", ret);

        if (attempt < MAX_UPLINK_ATTEMPTS) {
            uint32_t delay = random_uint32_range(UPLINK_RETRY_MIN_MS,
                                                 UPLINK_RETRY_MAX_MS + 1U);
            printf("Waiting %lu ms before next send try\n",
                   (unsigned long)delay);
            xtimer_msleep(delay);
        }
    }

    puts("Message was not sent after all tries");
    return 1;
}
