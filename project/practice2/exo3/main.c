#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "xtimer.h"

#define EDDYSTONE_URL_SCHEME_HTTPS     (0x03U)
#define EDDYSTONE_FRAME_TYPE_URL       (0x10U)
#define TX_POWER                       (0x00U)
#define ADVERTISED_URL                 "riot-os.org"
#define ADV_INTERVAL_MS                (1000U)

static size_t build_eddystone_url_payload(uint8_t *payload, size_t payload_len,
                                          const char *url)
{
    size_t url_len = strlen(url);

    if (payload_len < (3U + url_len)) {
        return 0;
    }

    payload[0] = EDDYSTONE_FRAME_TYPE_URL;
    payload[1] = TX_POWER;
    payload[2] = EDDYSTONE_URL_SCHEME_HTTPS;
    memcpy(&payload[3], url, url_len);

    return 3U + url_len;
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
    uint8_t payload[32];
    size_t payload_len = build_eddystone_url_payload(payload, sizeof(payload),
                                                     ADVERTISED_URL);

    puts("=== Practice 2, Exercise 3: BLE Beacon Payload Lab ===");
    puts("ESP32 mode: preparing the Eddystone URL payload only.");
    puts("Real Skald advertising still needs compatible BLE radio support.");

    printf("URL: https://%s", ADVERTISED_URL);
    puts("");
    printf("Payload length: %u bytes", (unsigned)payload_len);
    puts("");
    printf("Payload bytes: ");
    print_payload(payload, payload_len);

    while (1) {
        puts("Simulated beacon tick");
        xtimer_msleep(ADV_INTERVAL_MS);
    }

    return 0;
}
