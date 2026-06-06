#include <stdio.h>

#include "xtimer.h"

typedef struct {
    const char *method;
    const char *path;
    const char *payload;
} coap_message_t;

static void send_coap_message(const char *node, const coap_message_t *msg)
{
    printf("[%s] CoAP %s %s", node, msg->method, msg->path);
    if (msg->payload && msg->payload[0] != 0) {
        printf(" payload='%s'", msg->payload);
    }
    puts("");
}

int main(void)
{
    puts("=== Practice 2, Exercise 6: 6LoWPAN/CoAP Scenario Model ===");
    puts("ESP32 mode: modelling the CoAP exchange locally.");
    puts("Real 6LoWPAN over BLE requires two or three compatible BLE boards.");
    puts("");

    coap_message_t discover = { "GET", "/.well-known/core", "" };
    coap_message_t read_light = { "GET", "/traffic/state", "" };
    coap_message_t set_light = { "PUT", "/traffic/state", "red" };

    send_coap_message("client", &discover);
    puts("[server] 2.05 Content: </traffic/state>;rt=text");
    xtimer_msleep(100);

    send_coap_message("client", &read_light);
    puts("[server] 2.05 Content: green");
    xtimer_msleep(100);

    send_coap_message("client", &set_light);
    puts("[server] 2.04 Changed");

    return 0;
}
