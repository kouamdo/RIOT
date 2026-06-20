#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "atomic_utils.h"
#include "board.h"
#include "periph/gpio.h"
#include "timex.h"
#include "xtimer.h"

#ifndef EXO4_LED_PIN
#define EXO4_LED_PIN GPIO2
#endif

#ifndef EXO4_BTN_PIN
#define EXO4_BTN_PIN BTN0_PIN
#endif

#define MORSE_UNIT_MS       (200U)
#define DOT_LIMIT_US        (400U * US_PER_MS)
#define LETTER_GAP_US       (800U * US_PER_MS)
#define DEBOUNCE_US         (50U * US_PER_MS)

/* Bits 7..5 store the length. Bits 4..0 store dots (0) and dashes (1). */
#define MORSE(length, dashes) ((uint8_t)(((length) << 5) | (dashes)))

static const uint8_t letter_codes[26] = {
    MORSE(2, 0x02), /* A .-   */ MORSE(4, 0x01), /* B -... */
    MORSE(4, 0x05), /* C -.-. */ MORSE(3, 0x01), /* D -..  */
    MORSE(1, 0x00), /* E .    */ MORSE(4, 0x04), /* F ..-. */
    MORSE(3, 0x03), /* G --.  */ MORSE(4, 0x00), /* H .... */
    MORSE(2, 0x00), /* I ..   */ MORSE(4, 0x0e), /* J .--- */
    MORSE(3, 0x05), /* K -.-  */ MORSE(4, 0x02), /* L .-.. */
    MORSE(2, 0x03), /* M --   */ MORSE(2, 0x01), /* N -.   */
    MORSE(3, 0x07), /* O ---  */ MORSE(4, 0x06), /* P .--. */
    MORSE(4, 0x0b), /* Q --.- */ MORSE(3, 0x02), /* R .-.  */
    MORSE(3, 0x00), /* S ...  */ MORSE(1, 0x01), /* T -    */
    MORSE(3, 0x04), /* U ..-  */ MORSE(4, 0x08), /* V ...- */
    MORSE(3, 0x06), /* W .--  */ MORSE(4, 0x09), /* X -..- */
    MORSE(4, 0x0d), /* Y -.-- */ MORSE(4, 0x03), /* Z --.. */
};

static const char message[] = "RIOT ESP32";

static volatile uint8_t debounce_ready;
static volatile uint8_t button_irq_enabled = 1U;

static bool button_pressed;
static uint32_t press_start_us;
static uint32_t last_symbol_us;
static uint8_t received_length;
static uint8_t received_dashes;

static void debounce_callback(void *arg)
{
    (void)arg;
    atomic_store_u8(&debounce_ready, 1U);
}

static xtimer_t debounce_timer = {
    .callback = debounce_callback,
};

static void button_irq(void *arg)
{
    (void)arg;

    if (!atomic_load_u8(&button_irq_enabled)) {
        return;
    }

    atomic_store_u8(&button_irq_enabled, 0U);
    gpio_irq_disable(EXO4_BTN_PIN);
    xtimer_set(&debounce_timer, DEBOUNCE_US);
}

static void enable_button_irq(void)
{
    atomic_store_u8(&button_irq_enabled, 1U);
    gpio_irq_enable(EXO4_BTN_PIN);
}

static uint8_t digit_code(char digit)
{
    static const uint8_t dash_bits[10] = {
        0x1f, 0x1e, 0x1c, 0x18, 0x10, 0x00, 0x01, 0x03, 0x07, 0x0f
    };

    return MORSE(5, dash_bits[digit - '0']);
}

static uint8_t char_code(char c)
{
    c = (char)toupper((unsigned char)c);

    if (c >= 'A' && c <= 'Z') {
        return letter_codes[c - 'A'];
    }
    if (c >= '0' && c <= '9') {
        return digit_code(c);
    }
    return 0U;
}

static void blink_symbol(bool dash)
{
    gpio_set(EXO4_LED_PIN);
    xtimer_msleep(dash ? (3U * MORSE_UNIT_MS) : MORSE_UNIT_MS);
    gpio_clear(EXO4_LED_PIN);
    xtimer_msleep(MORSE_UNIT_MS);
}

static void print_code(uint8_t code)
{
    uint8_t length = code >> 5;
    uint8_t dashes = code & 0x1fU;

    for (uint8_t i = 0; i < length; i++) {
        putchar((dashes & (1U << i)) ? '-' : '.');
    }
}

static void encode_message(void)
{
    printf("Message: %s\n", message);

    for (unsigned i = 0; message[i] != '\0'; i++) {
        if (message[i] == ' ') {
            xtimer_msleep(6U * MORSE_UNIT_MS);
            continue;
        }

        uint8_t code = char_code(message[i]);
        uint8_t length = code >> 5;
        uint8_t dashes = code & 0x1fU;

        printf("%c -> ", message[i]);
        print_code(code);
        putchar('\n');

        for (uint8_t bit = 0; bit < length; bit++) {
            blink_symbol((dashes & (1U << bit)) != 0U);
        }

        xtimer_msleep(2U * MORSE_UNIT_MS);
    }
}

static char decode_letter(uint8_t length, uint8_t dashes)
{
    uint8_t code = MORSE(length, dashes);

    for (unsigned i = 0; i < 26U; i++) {
        if (letter_codes[i] == code) {
            return (char)('A' + i);
        }
    }

    if (length == 5U) {
        for (char digit = '0'; digit <= '9'; digit++) {
            if (digit_code(digit) == code) {
                return digit;
            }
        }
    }

    return '?';
}

static void save_symbol(bool dash)
{
    if (received_length >= 5U) {
        puts("Code too long, reset");
        received_length = 0U;
        received_dashes = 0U;
        return;
    }

    if (dash) {
        received_dashes |= (uint8_t)(1U << received_length);
    }

    received_length++;
    putchar(dash ? '-' : '.');
    fflush(stdout);
}

int main(void)
{
    puts("\n=== Exercise 4: Compact Morse code ===");

    gpio_init(EXO4_LED_PIN, GPIO_OUT);
    gpio_clear(EXO4_LED_PIN);

    if (gpio_init_int(EXO4_BTN_PIN, BTN0_MODE, BTN0_INT_FLANK,
                      button_irq, NULL) != 0) {
        puts("Button initialization failed");
        return 1;
    }

    encode_message();
    puts("\nDecode mode: short press = dot, long press = dash");

    while (1) {
        uint32_t now_us = xtimer_now_usec();

        if (atomic_load_u8(&debounce_ready)) {
            atomic_store_u8(&debounce_ready, 0U);

            if (gpio_read(EXO4_BTN_PIN) == 0) {
                button_pressed = true;
                press_start_us = now_us;
            }
            else {
                enable_button_irq();
            }
        }

        if (button_pressed && gpio_read(EXO4_BTN_PIN) != 0) {
            uint32_t duration = now_us - press_start_us;

            button_pressed = false;
            save_symbol(duration >= DOT_LIMIT_US);
            last_symbol_us = now_us;
            enable_button_irq();
        }

        if (received_length > 0U &&
            (uint32_t)(now_us - last_symbol_us) >= LETTER_GAP_US) {
            char decoded = decode_letter(received_length, received_dashes);
            printf(" -> %c\n", decoded);
            received_length = 0U;
            received_dashes = 0U;
        }

        xtimer_msleep(10);
    }
}
