/*
 * Practice 2, Exercise 7: network integration model for a practice1 device.
 *
 * ESP32-only mode: model a traffic light that can receive network commands and
 * report state. The LoRaWAN or 6LoWPAN transport is represented by local test
 * messages until radio hardware is available.
 */

#include <stdint.h>
#include <stdio.h>

#include "xtimer.h"

typedef enum {
    LIGHT_RED,
    LIGHT_YELLOW,
    LIGHT_GREEN,
    LIGHT_MAINTENANCE,
} traffic_state_t;

typedef enum {
    CMD_SET_STATE,
    CMD_SET_TIMING,
    CMD_STATUS_REQUEST,
} command_type_t;

typedef struct {
    uint16_t id;
    command_type_t type;
    traffic_state_t state;
    uint16_t green_time_s;
    uint16_t red_time_s;
} network_command_t;

typedef struct {
    traffic_state_t state;
    uint16_t green_time_s;
    uint16_t red_time_s;
    uint32_t last_command_ms;
    uint16_t commands_processed;
} traffic_light_t;

static traffic_light_t traffic_light = {
    .state = LIGHT_GREEN,
    .green_time_s = 25U,
    .red_time_s = 30U,
};

static const char *state_name(traffic_state_t state)
{
    switch (state) {
    case LIGHT_RED:
        return "red";
    case LIGHT_YELLOW:
        return "yellow";
    case LIGHT_GREEN:
        return "green";
    case LIGHT_MAINTENANCE:
        return "maintenance";
    default:
        return "unknown";
    }
}

static void print_status_report(void)
{
    printf("status: state=%s, green=%us, red=%us, processed=%u\n",
           state_name(traffic_light.state), traffic_light.green_time_s,
           traffic_light.red_time_s, traffic_light.commands_processed);
}

static void apply_network_command(const network_command_t *cmd)
{
    printf("received command #%u\n", cmd->id);
    traffic_light.last_command_ms = xtimer_now_usec() / 1000U;
    traffic_light.commands_processed++;

    switch (cmd->type) {
    case CMD_SET_STATE:
        traffic_light.state = cmd->state;
        printf("  action: set state to %s\n", state_name(cmd->state));
        break;
    case CMD_SET_TIMING:
        traffic_light.green_time_s = cmd->green_time_s;
        traffic_light.red_time_s = cmd->red_time_s;
        printf("  action: set timing green=%us red=%us\n",
               cmd->green_time_s, cmd->red_time_s);
        break;
    case CMD_STATUS_REQUEST:
        puts("  action: status request");
        print_status_report();
        break;
    default:
        puts("  action: unknown command ignored");
        break;
    }
}

static void run_demo_script(void)
{
    const network_command_t script[] = {
        { .id = 101U, .type = CMD_STATUS_REQUEST },
        { .id = 102U, .type = CMD_SET_STATE, .state = LIGHT_RED },
        { .id = 103U, .type = CMD_SET_TIMING, .green_time_s = 40U, .red_time_s = 20U },
        { .id = 104U, .type = CMD_SET_STATE, .state = LIGHT_MAINTENANCE },
        { .id = 105U, .type = CMD_STATUS_REQUEST },
    };

    for (unsigned i = 0; i < (sizeof(script) / sizeof(script[0])); i++) {
        apply_network_command(&script[i]);
        xtimer_msleep(100);
    }
}

int main(void)
{
    puts("=== Practice 2, Exercise 7: Smart Traffic Light Network Model ===");
    puts("ESP32-only mode: local command script instead of real LoRaWAN/6LoWPAN.");
    puts("Use case: traffic center can override state and update timings.\n");

    puts("Initial device status:");
    print_status_report();
    puts("");

    run_demo_script();

    puts("\nDesign justification:");
    puts("  - uplink: periodic state, timing, and fault reports");
    puts("  - downlink: emergency red, maintenance mode, timing updates");
    puts("  - LoRaWAN is reasonable for city-scale low-rate commands");
    puts("  - local autonomous mode should continue if radio is unavailable");

    return 0;
}
