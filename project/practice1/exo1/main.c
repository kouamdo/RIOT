#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "atomic_utils.h"
#include "board.h"
#include "periph/gpio.h"
#include "timex.h"
#include "xtimer.h"

#ifndef EXO1_LED_PIN
#define EXO1_LED_PIN GPIO2
#endif

#ifndef EXO1_BTN_PIN
#define EXO1_BTN_PIN BTN0_PIN
#endif

#define DEBOUNCE_DELAY_US   (50U * US_PER_MS)
#define LONG_PRESS_US       (1U * US_PER_SEC)

static const uint16_t frequencies[] = { 1U, 2U, 4U };

static volatile uint8_t debounce_ready;
static volatile uint8_t button_irq_enabled = 1U;

static bool blinking;
static bool led_on;
static bool button_pressed;
static bool long_press_done;
static uint8_t frequency_index;
static uint32_t press_start_us;
static uint32_t last_toggle_us;

static void debounce_callback(void *arg)
{
    (void)arg;
    atomic_store_u8(&debounce_ready, 1U);
}

static xtimer_t debounce_timer = {
    .callback = debounce_callback,
};

static void set_led(bool on)
{
    led_on = on;
    gpio_write(EXO1_LED_PIN, on);
}

static void enable_button_irq(void)
{
    atomic_store_u8(&button_irq_enabled, 1U);
    gpio_irq_enable(EXO1_BTN_PIN);
}

static void button_irq(void *arg)
{
    (void)arg;

    if (!atomic_load_u8(&button_irq_enabled)) {
        return;
    }

    atomic_store_u8(&button_irq_enabled, 0U);
    gpio_irq_disable(EXO1_BTN_PIN);
    xtimer_set(&debounce_timer, DEBOUNCE_DELAY_US);
}

int main(void)
{
    puts("\n=== Exercise 1: Lamp with switch ===");
    puts("Short press: start or stop blinking");
    puts("Long press: change frequency\n");

    gpio_init(EXO1_LED_PIN, GPIO_OUT);
    set_led(false);

    if (gpio_init_int(EXO1_BTN_PIN, BTN0_MODE, BTN0_INT_FLANK,
                      button_irq, NULL) != 0) {
        puts("Button initialization failed");
        return 1;
    }

    while (1) {
        uint32_t now_us = xtimer_now_usec();

        if (atomic_load_u8(&debounce_ready)) {
            atomic_store_u8(&debounce_ready, 0U);

            if (gpio_read(EXO1_BTN_PIN) == 0) {
                button_pressed = true;
                long_press_done = false;
                press_start_us = now_us;
            }
            else {
                enable_button_irq();
            }
        }

        if (button_pressed && gpio_read(EXO1_BTN_PIN) == 0 &&
            !long_press_done &&
            (uint32_t)(now_us - press_start_us) >= LONG_PRESS_US) {
            frequency_index = (frequency_index + 1U) % 3U;
            last_toggle_us = now_us;
            long_press_done = true;
            printf("Long press: frequency = %u Hz\n",
                   frequencies[frequency_index]);
        }

        if (button_pressed && gpio_read(EXO1_BTN_PIN) != 0) {
            button_pressed = false;

            if (!long_press_done) {
                blinking = !blinking;
                last_toggle_us = now_us;
                set_led(blinking);
                printf("Blinking %s\n", blinking ? "started" : "stopped");
            }

            enable_button_irq();
        }

        if (blinking) {
            uint32_t half_period_us = 500000U / frequencies[frequency_index];

            if ((uint32_t)(now_us - last_toggle_us) >= half_period_us) {
                set_led(!led_on);
                last_toggle_us = now_us;
            }
        }

        xtimer_msleep(10);
    }
}
