/*
 * Practice 2, Exercise 7: Smart Device Network Integration
 * 
 * Extend practice1 smart devices (traffic light, safe, sensors)
 * with wireless capabilities using LoRaWAN or 6LoWPAN over BLE.
 * Adds centralized control and monitoring via radio communication.
 * 
 * Objectives:
 * - Integrate smart devices with wireless network
 * - Implement meaningful IoT use cases
 * - Demonstrate remote control and monitoring
 * - Design practical, justified applications
 * - Optional: Add external hardware (keypad, card reader)
 * 
 * Example: Smart traffic light receiving schedule from remote control center
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <xtimer.h>

/* GPIO configuration for traffic light */
#define GPIO_RED_LED      15
#define GPIO_YELLOW_LED   14
#define GPIO_GREEN_LED    13

/**
 * Traffic Light State
 */
typedef enum {
    STATE_RED = 0,
    STATE_YELLOW = 1,
    STATE_GREEN = 2,
    STATE_MANUAL = 3
} traffic_state_t;

/**
 * Control Command from Network
 */
typedef struct {
    uint16_t command_id;
    uint8_t command_type;  /* 0=state_change, 1=schedule_update */
    uint8_t state;         /* For state_change commands */
    uint16_t duration_sec; /* For schedule_update commands */
} network_command_t;

/**
 * Traffic Light Device Context
 */
typedef struct {
    traffic_state_t current_state;
    traffic_state_t next_state;
    uint32_t state_start_time;
    uint32_t state_duration_ms;
    uint8_t wireless_enabled;
    uint8_t remote_control_active;
    uint32_t last_command_time;
} traffic_light_device_t;

static traffic_light_device_t traffic_light = {
    .current_state = STATE_RED,
    .wireless_enabled = 1,
    .remote_control_active = 0,
};

/**
 * Get state name
 */
static const char *state_name(traffic_state_t state)
{
    switch (state) {
    case STATE_RED:    return "RED";
    case STATE_YELLOW: return "YELLOW";
    case STATE_GREEN:  return "GREEN";
    case STATE_MANUAL: return "MANUAL";
    default:           return "UNKNOWN";
    }
}

/**
 * Update LED outputs
 */
static void update_leds(traffic_state_t state)
{
    /* In a real implementation, control GPIO pins */
    printf("[LEDs] ");
    
    switch (state) {
    case STATE_RED:
        printf("RED ON, YELLOW OFF, GREEN OFF\n");
        break;
    case STATE_YELLOW:
        printf("RED OFF, YELLOW ON, GREEN OFF\n");
        break;
    case STATE_GREEN:
        printf("RED OFF, YELLOW OFF, GREEN ON\n");
        break;
    case STATE_MANUAL:
        printf("All OFF (manual override)\n");
        break;
    }
}

/**
 * Process command received from control center
 */
static void process_network_command(const network_command_t *cmd)
{
    printf("[Network] Command received (ID: %u, Type: %u)\n",
           cmd->command_id, cmd->command_type);
    
    traffic_light.last_command_time = xtimer_now_usec() / 1000;
    
    switch (cmd->command_type) {
    case 0: /* State change */
        printf("[Network] State change command: %s\n",
               state_name((traffic_state_t)cmd->state));
        traffic_light.current_state = (traffic_state_t)cmd->state;
        traffic_light.remote_control_active = 1;
        update_leds(traffic_light.current_state);
        break;
        
    case 1: /* Schedule update */
        printf("[Network] Schedule update: %u seconds per cycle\n",
               cmd->duration_sec);
        traffic_light.state_duration_ms = cmd->duration_sec * 1000;
        break;
        
    default:
        printf("[Network] Unknown command type\n");
    }
}

/**
 * Report traffic light status to control center
 */
static void report_status_to_network(void)
{
    printf("[Report] Transmitting status via network:\n");
    printf("  Current State: %s\n", state_name(traffic_light.current_state));
    printf("  Time in State: %u ms\n",
           (uint32_t)(xtimer_now_usec() / 1000 - traffic_light.state_start_time));
    printf("  Remote Control: %s\n",
           traffic_light.remote_control_active ? "ACTIVE" : "INACTIVE");
    printf("  Network Status: Connected\n");
}

/**
 * Run traffic light normally (autonomous mode)
 */
static void run_autonomous_mode(void)
{
    printf("=== Autonomous Traffic Light Mode ===\n");
    printf("Standard cycle: RED(30s) -> GREEN(25s) -> YELLOW(5s) -> repeat\n\n");
    
    /* Simulate a few state changes */
    traffic_light_device_t cycle[] = {
        {STATE_RED, STATE_RED, 0, 30000, 1, 0, 0},
        {STATE_GREEN, STATE_GREEN, 0, 25000, 1, 0, 0},
        {STATE_YELLOW, STATE_YELLOW, 0, 5000, 1, 0, 0},
    };
    
    for (int i = 0; i < 3; i++) {
        traffic_light.current_state = cycle[i].current_state;
        printf("State: %s (Duration: %lu ms)\n",
               state_name(traffic_light.current_state),
               (unsigned long)cycle[i].state_duration_ms);
        update_leds(traffic_light.current_state);
    }
}

/**
 * Simulate remote control from network
 */
static void simulate_remote_control(void)
{
    printf("\n=== Network Remote Control Scenario ===\n");
    printf("Control Center sends emergency override...\n\n");
    
    network_command_t cmd = {
        .command_id = 101,
        .command_type = 0,  /* State change */
        .state = STATE_RED,
    };
    
    process_network_command(&cmd);
    
    printf("\nAfter 10 seconds, control center sends resume...\n\n");
    
    cmd.command_id = 102;
    cmd.command_type = 1;  /* Schedule update */
    cmd.duration_sec = 60;
    
    process_network_command(&cmd);
}

/**
 * Demonstrate practical use case
 */
static void demonstrate_use_case(void)
{
    printf("\n=== Practical Use Case ===\n");
    printf("Smart Traffic Light with Central Traffic Management System\n\n");
    
    printf("Features:\n");
    printf("1. Regular autonomous operation (predefined cycles)\n");
    printf("2. Emergency override from control center\n");
    printf("3. Adaptive timing based on traffic flow\n");
    printf("4. Continuous status reporting\n");
    printf("5. Fault detection and alerts\n\n");
    
    printf("Wireless Link:\n");
    printf("  - LoRaWAN Class A device\n");
    printf("  - Uplink: Status report every 60 seconds\n");
    printf("  - Downlink: Commands during RX windows\n");
    printf("  - Range: City-wide coverage\n\n");
    
    printf("Benefits:\n");
    printf("  - Improved traffic flow during peak hours\n");
    printf("  - Emergency vehicles get priority\n");
    printf("  - Reduced infrastructure costs (wireless vs hardwired)\n");
    printf("  - Easy deployment and reconfiguration\n");
}

/**
 * Optional: Extended features with external hardware
 */
static void describe_extended_features(void)
{
    printf("\n=== Optional Extensions ===\n");
    printf("\nWith smart card reader + keypad:\n");
    printf("  - Maintenance technician authentication\n");
    printf("  - Manual override with audit logging\n");
    printf("  - Configuration changes logged to network\n\n");
    
    printf("With pedestrian detection sensor:\n");
    printf("  - Adaptive signal timing\n");
    printf("  - Longer green for busy lanes\n");
    printf("  - Accessible walk time for elderly\n\n");
    
    printf("With vehicle detection (induction loops):\n");
    printf("  - Real-time traffic density reporting\n");
    printf("  - Congestion alerts to control center\n");
    printf("  - Intelligent cycle adjustment\n");
}

int main(void)
{
    printf("=== Practice 2, Exercise 7: Smart Device Network Integration ===\n\n");
    
    /* Demonstrate autonomous operation */
    run_autonomous_mode();
    
    /* Demonstrate remote control */
    simulate_remote_control();
    
    /* Send status report */
    printf("\n=== Status Report ===\n");
    report_status_to_network();
    
    /* Show practical justification */
    demonstrate_use_case();
    
    /* Optional hardware ideas */
    describe_extended_features();
    
    printf("\n=== Implementation Guidelines ===\n");
    printf("1. Start with basic state machine in autonomous mode\n");
    printf("2. Add wireless module for status reporting\n");
    printf("3. Implement command reception and processing\n");
    printf("4. Add error handling and network timeouts\n");
    printf("5. Log all state changes and commands\n");
    printf("6. Test with simulation before hardware\n");
    
    printf("\n=== Exercise Complete ===\n");
    return 0;
}
