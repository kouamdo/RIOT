/*
 * Practice 2 - Exercise 7
 * Bluetooth control model for the traffic light from practice 1.
 *
 * The ESP32 receives simple Bluetooth commands and updates the traffic light
 * state. This is a local simulation, but it shows the message flow clearly.
 */

#include <stdint.h>
#include <stdio.h>

#include "xtimer.h"

typedef enum {
    RED,
    YELLOW,
    GREEN,
    MAINTENANCE,
} light_state_t;

typedef struct {
    const char *from;
    const char *text;
    light_state_t new_state;
    int change_state;
} bt_command_t;

static light_state_t current_state = GREEN;
static unsigned commands_done = 0;

static const char *state_name(light_state_t state)
{
    if (state == RED) {
        return "red";
    }
    if (state == YELLOW) {
        return "yellow";
    }
    if (state == GREEN) {
        return "green";
    }
    return "maintenance";
}

static void print_status(void)
{
    printf("traffic light state: %s, commands: %u\n",
           state_name(current_state), commands_done);
}

int main(void)
{
    bt_command_t commands[] = {
        { "phone", "status", GREEN, 0 },
        { "phone", "set red", RED, 1 },
        { "tablet", "set green", GREEN, 1 },
        { "phone", "maintenance", MAINTENANCE, 1 },
        { "phone", "status", GREEN, 0 },
    };

    puts("Practice 2 - Exercise 7");
    puts("Bluetooth traffic light control model\n");

    print_status();
    puts("");

    for (unsigned i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
        printf("Bluetooth command from %s: %s\n",
               commands[i].from, commands[i].text);

        commands_done++;

        if (commands[i].change_state) {
            current_state = commands[i].new_state;
            printf("new state: %s\n", state_name(current_state));
        }
        else {
            print_status();
        }

        puts("send Bluetooth ACK\n");
        xtimer_msleep(100);
    }

    puts("Design idea:");
    puts("- phone or gateway sends small Bluetooth commands");
    puts("- traffic light sends ACK and status");
    puts("- if Bluetooth is not connected, the light keeps local timing");

    return 0;
}
