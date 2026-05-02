#include <stdio.h>
#include <string.h>
#include "shell.h"
#include "thread.h"
#include "periph/pm.h"
#include "xtimer.h"

// Global variable for the example
static int counter = 0;

// "hello" command: says hello
int cmd_hello(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("Hello from RIOT!\n");
    return 0;
}

// "counter" command: displays or increments a counter
int cmd_counter(int argc, char **argv)
{
    if (argc > 1) {
        if (argv[1][0] == 'i') {
            counter++;
            printf("Counter incremented -> %d\n", counter);
        } else if (argv[1][0] == 'r') {
            counter = 0;
            printf("Counter reset\n");
        } else {
            printf("Usage: counter [i|r]\n");
            printf("  i: increment\n");
            printf("  r: reset\n");
        }
    } else {
        printf("Counter = %d\n", counter);
    }
    return 0;
}

// "info" command: displays information about the board
int cmd_info(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("Board: %s\n", RIOT_BOARD);
    puts("Microcontroller: depends on the selected RIOT board");
    printf("Uptime: about %lu seconds\n", xtimer_now_usec() / 1000000);
    return 0;
}

// "led" command: controls an LED (example)
int cmd_led(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: led on|off\n");
        return 1;
    }
    
    if (strcmp(argv[1], "on") == 0) {
        printf("LED turned on\n");
        // gpio_set(LED0_PIN);  // Uncomment if you have an LED
    } else if (strcmp(argv[1], "off") == 0) {
        printf("LED turned off\n");
        // gpio_clear(LED0_PIN); // Uncomment if you have an LED
    } else {
        printf("Unknown command. Use 'on' or 'off'\n");
        return 1;
    }
    return 0;
}

// "help" command: lists all commands
int cmd_help(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    puts("\nAvailable commands:");
    puts("  hello       - Display a welcome message");
    puts("  counter     - Display or modify a counter");
    puts("  info        - Display information about the board");
    puts("  led on/off  - Control the LED");
    puts("  help        - Display this help");
    puts("  reboot      - Reboot the board\n");
    return 0;
}

// "reboot" command
int cmd_reboot(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    puts("Rebooting the board...");
    xtimer_usleep(100000);
    pm_reboot();
    return 0;
}

// Shell command list
static const shell_command_t shell_commands[] = {
    { "hello", "Display a welcome message", cmd_hello },
    { "counter", "Display/modify a counter [i|r]", cmd_counter },
    { "info", "Display board information", cmd_info },
    { "led", "Control the LED (on/off)", cmd_led },
    { "help", "Display the command list", cmd_help },
    { "reboot", "Reboot the board", cmd_reboot },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("\n=== Exercise 14: RIOT Shell ===\n");
    puts("Type 'help' to see the available commands.");
    puts("Use Ctrl+C to leave the shell if supported by your terminal.\n");
    
    // Start the shell
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    
    return 0;
}
