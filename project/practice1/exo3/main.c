#include <stdbool.h>
#include <stdio.h>

#include "periph/gpio.h"
#include "xtimer.h"

/* Generic ESP32 wiring for the traffic light exercise. */
#ifndef EXO3_RED_LED_PIN
#define EXO3_RED_LED_PIN        GPIO16
#endif

#ifndef EXO3_YELLOW_LED_PIN
#define EXO3_YELLOW_LED_PIN     GPIO17
#endif

#ifndef EXO3_GREEN_LED_PIN
#define EXO3_GREEN_LED_PIN      GPIO18
#endif

#ifndef EXO3_BUTTON_PIN
#define EXO3_BUTTON_PIN         GPIO19
#endif

#define DEBOUNCE_DELAY_MS       (50U)
#define GREEN_TIME_MS           (5000U)
#define YELLOW_TIME_MS          (2000U)
#define RED_TIME_MS             (5000U)
#define PEDESTRIAN_TIME_MS      (4000U)

static void set_traffic_light(bool red, bool yellow, bool green)
{
    gpio_write(EXO3_RED_LED_PIN, red);
    gpio_write(EXO3_YELLOW_LED_PIN, yellow);
    gpio_write(EXO3_GREEN_LED_PIN, green);
}

static bool pedestrian_button_pressed(void)
{
    if (gpio_read(EXO3_BUTTON_PIN) != 0) {
        return false;
    }

    xtimer_msleep(DEBOUNCE_DELAY_MS);
    return gpio_read(EXO3_BUTTON_PIN) == 0;
}

static bool wait_and_track_request(uint32_t duration_ms, bool request_pending)
{
    uint32_t elapsed_ms = 0;

    while (elapsed_ms < duration_ms) {
        if (pedestrian_button_pressed()) {
            request_pending = true;
            puts("Pedestrian request registered");

            while (gpio_read(EXO3_BUTTON_PIN) == 0) {
                xtimer_msleep(10);
            }
        }

        xtimer_msleep(10);
        elapsed_ms += 10;
    }

    return request_pending;
}

int main(void)
{
    bool pedestrian_request = false;

    puts("=== Exercise 3: Traffic light with pedestrian button ===");
    printf("Red: GPIO%d | Yellow: GPIO%d | Green: GPIO%d | Button: GPIO%d\n",
           EXO3_RED_LED_PIN, EXO3_YELLOW_LED_PIN,
           EXO3_GREEN_LED_PIN, EXO3_BUTTON_PIN);

    gpio_init(EXO3_RED_LED_PIN, GPIO_OUT);
    gpio_init(EXO3_YELLOW_LED_PIN, GPIO_OUT);
    gpio_init(EXO3_GREEN_LED_PIN, GPIO_OUT);
    gpio_init(EXO3_BUTTON_PIN, GPIO_IN_PU);

    while (1) {
        puts("Cars: green");
        set_traffic_light(false, false, true);
        pedestrian_request = wait_and_track_request(GREEN_TIME_MS, pedestrian_request);

        puts("Cars: yellow");
        set_traffic_light(false, true, false);
        pedestrian_request = wait_and_track_request(YELLOW_TIME_MS, pedestrian_request);

        puts("Cars: red");
        set_traffic_light(true, false, false);

        if (pedestrian_request) {
            puts("Pedestrians may cross");
            xtimer_msleep(PEDESTRIAN_TIME_MS);
            pedestrian_request = false;
        }
        else {
            xtimer_msleep(RED_TIME_MS);
        }
    }

    return 0;
}
