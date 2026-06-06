#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "board.h"
#include "periph/gpio.h"
#include "xtimer.h"

/* Generic ESP32 setup:
 * - integrated LED usually connected to GPIO2
 * - BOOT button exposed as BTN0_PIN */
#ifndef EXO1_LED_PIN
#define EXO1_LED_PIN GPIO2
#endif

#ifndef EXO1_BTN_PIN
#define EXO1_BTN_PIN BTN0_PIN
#endif

#define DEBOUNCE_DELAY_MS   (50U)
#define LONG_PRESS_MS       (1000U)

static const uint16_t blink_frequencies_hz[] = {1U, 2U, 4U};

static volatile bool irq_enabled = true;
static volatile bool debounce_pending = false;
static volatile uint32_t debounce_deadline_ms = 0;

static bool blink_enabled = false;
static bool led_state = false;
static bool button_pressed = false;
static bool long_press_handled = false;
static uint8_t frequency_index = 0;
static uint32_t press_start_ms = 0;
static uint32_t last_toggle_ms = 0;

static uint32_t now_ms(void)
{
    return xtimer_now_usec() / 1000U;
}

static void set_led(bool on)
{
    led_state = on;
    gpio_write(EXO1_LED_PIN, on);
}

static void print_frequency(void)
{
    printf("Blink frequency: %u Hz\n", blink_frequencies_hz[frequency_index]);
}

static void handle_short_press(void)
{
    blink_enabled = !blink_enabled;
    last_toggle_ms = now_ms();

    if (!blink_enabled) {
        set_led(false);
        puts("Blinking stopped");
    }
    else {
        set_led(true);
        puts("Blinking enabled");
        print_frequency();
    }
}

static void handle_long_press(void)
{
    frequency_index = (frequency_index + 1U) %
                      (sizeof(blink_frequencies_hz) / sizeof(blink_frequencies_hz[0]));
    last_toggle_ms = now_ms();
    long_press_handled = true;

    puts("Long press detected");
    print_frequency();
}

static void button_irq_handler(void *arg)
{
    (void)arg;

    if (!irq_enabled) {
        return;
    }

    irq_enabled = false;
    debounce_pending = true;
    debounce_deadline_ms = now_ms() + DEBOUNCE_DELAY_MS;
    gpio_irq_disable(EXO1_BTN_PIN);
}

int main(void)
{
    puts("\n=== Exercise 1: Lamp with switch ===");
    puts("Short press : enable or stop LED blinking");
    puts("Long press  : change the blinking frequency");
    puts("Method      : interrupt handling + debounce + periodic polling\n");

    gpio_init(EXO1_LED_PIN, GPIO_OUT);
    set_led(false);

    if (gpio_init_int(EXO1_BTN_PIN, BTN0_MODE, BTN0_INT_FLANK,
                      button_irq_handler, NULL) != 0) {
        puts("Error: failed to initialize the button");
        return 1;
    }

    printf("LED on GPIO%d, button on GPIO%d\n\n", EXO1_LED_PIN, EXO1_BTN_PIN);

    while (1) {
        uint32_t current_ms = now_ms();

        if (debounce_pending && (current_ms >= debounce_deadline_ms)) {
            debounce_pending = false;

            if (gpio_read(EXO1_BTN_PIN) == 0) {
                button_pressed = true;
                long_press_handled = false;
                press_start_ms = current_ms;
            }
            else {
                irq_enabled = true;
                gpio_irq_enable(EXO1_BTN_PIN);
            }
        }

        if (button_pressed) {
            if ((gpio_read(EXO1_BTN_PIN) == 0) &&
                !long_press_handled &&
                ((current_ms - press_start_ms) >= LONG_PRESS_MS)) {
                handle_long_press();
            }

            if (gpio_read(EXO1_BTN_PIN) != 0) {
                button_pressed = false;

                if (!long_press_handled) {
                    handle_short_press();
                }

                irq_enabled = true;
                gpio_irq_enable(EXO1_BTN_PIN);
            }
        }

        if (blink_enabled) {
            uint32_t half_period_ms = 500U / blink_frequencies_hz[frequency_index];

            if ((current_ms - last_toggle_ms) >= half_period_ms) {
                set_led(!led_state);
                last_toggle_ms = current_ms;
            }
        }

        xtimer_msleep(10);
    }

    return 0;
}
