#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "board.h"
#include "periph/gpio.h"
#include "xtimer.h"

/* Generic ESP32 setup:
 * - integrated LED usually connected to GPIO2
 * - BOOT button exposed as BTN0_PIN */
#ifndef EXO4_LED_PIN
#define EXO4_LED_PIN GPIO2
#endif

#ifndef EXO4_BTN_PIN
#define EXO4_BTN_PIN BTN0_PIN
#endif

#define MORSE_UNIT_MS           (200U)
#define DOT_THRESHOLD_MS        (400U)
#define DEBOUNCE_DELAY_MS       (50U)
#define LETTER_TIMEOUT_MS       (800U)
#define MAX_MORSE_SYMBOL_LEN    (15U)

typedef struct {
    char character;
    const char *code;
} morse_entry_t;

static const morse_entry_t morse_table[] = {
    {'A', ".-"}, {'B', "-..."}, {'C', "-.-."}, {'D', "-.."},
    {'E', "."}, {'F', "..-."}, {'G', "--."}, {'H', "...."},
    {'I', ".."}, {'J', ".---"}, {'K', "-.-"}, {'L', ".-.."},
    {'M', "--"}, {'N', "-."}, {'O', "---"}, {'P', ".--."},
    {'Q', "--.-"}, {'R', ".-."}, {'S', "..."}, {'T', "-"},
    {'U', "..-"}, {'V', "...-"}, {'W', ".--"}, {'X', "-..-"},
    {'Y', "-.--"}, {'Z', "--.."},
    {'0', "-----"}, {'1', ".----"}, {'2', "..---"}, {'3', "...--"},
    {'4', "....-"}, {'5', "....."}, {'6', "-...."}, {'7', "--..."},
    {'8', "---.."}, {'9', "----."},
    {'.', ".-.-.-"}, {',', "--..--"}, {'?', "..--.."},
    {'\0', NULL}
};

static const char message_to_encode[] = "RIOT ESP32";

static volatile bool irq_enabled = true;
static volatile bool debounce_pending = false;
static volatile uint32_t debounce_deadline_ms = 0;

static bool button_pressed = false;
static uint32_t press_start_ms = 0;
static uint32_t last_symbol_end_ms = 0;

static char decoded_symbol[MAX_MORSE_SYMBOL_LEN + 1];
static uint8_t decoded_symbol_len = 0;

static uint32_t now_ms(void)
{
    return xtimer_now_usec() / 1000U;
}

static void led_on(void)
{
    gpio_set(EXO4_LED_PIN);
}

static void led_off(void)
{
    gpio_clear(EXO4_LED_PIN);
}

static const char *char_to_morse(char c)
{
    c = (char)toupper((unsigned char)c);

    for (int i = 0; morse_table[i].code != NULL; i++) {
        if (morse_table[i].character == c) {
            return morse_table[i].code;
        }
    }

    return NULL;
}

static char morse_to_char(const char *code)
{
    for (int i = 0; morse_table[i].code != NULL; i++) {
        if (strcmp(morse_table[i].code, code) == 0) {
            return morse_table[i].character;
        }
    }

    return '?';
}

static void blink_dot(void)
{
    led_on();
    xtimer_msleep(MORSE_UNIT_MS);
    led_off();
    xtimer_msleep(MORSE_UNIT_MS);
}

static void blink_dash(void)
{
    led_on();
    xtimer_msleep(3U * MORSE_UNIT_MS);
    led_off();
    xtimer_msleep(MORSE_UNIT_MS);
}

static void encode_message(const char *message)
{
    printf("Encoding message: \"%s\"\n", message);

    for (size_t i = 0; message[i] != '\0'; i++) {
        if (message[i] == ' ') {
            puts("word gap");
            xtimer_msleep(6U * MORSE_UNIT_MS);
            continue;
        }

        const char *code = char_to_morse(message[i]);

        if (code == NULL) {
            printf("%c -> unsupported\n", message[i]);
            continue;
        }

        printf("%c -> %s\n", message[i], code);

        for (size_t j = 0; code[j] != '\0'; j++) {
            if (code[j] == '.') {
                blink_dot();
            }
            else if (code[j] == '-') {
                blink_dash();
            }
        }

        xtimer_msleep(2U * MORSE_UNIT_MS);
    }

    puts("Encoding complete\n");
}

static void reset_decoded_symbol(void)
{
    decoded_symbol_len = 0;
    decoded_symbol[0] = '\0';
}

static void decode_and_print(void)
{
    if (decoded_symbol_len == 0) {
        return;
    }

    decoded_symbol[decoded_symbol_len] = '\0';
    printf("Received Morse: %s -> %c\n",
           decoded_symbol, morse_to_char(decoded_symbol));
    reset_decoded_symbol();
}

static void store_symbol_char(char symbol)
{
    if (decoded_symbol_len >= MAX_MORSE_SYMBOL_LEN) {
        puts("Input too long, resetting Morse buffer");
        reset_decoded_symbol();
        return;
    }

    decoded_symbol[decoded_symbol_len++] = symbol;
    decoded_symbol[decoded_symbol_len] = '\0';
    putchar(symbol);
    fflush(stdout);
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
    gpio_irq_disable(EXO4_BTN_PIN);
}

int main(void)
{
    puts("\n=== Exercise 4: Morse code ===");
    puts("Part 1: the integrated LED encodes a message in Morse");
    puts("Part 2: the integrated button decodes Morse input");
    puts("Short press = dot, long press = dash\n");

    gpio_init(EXO4_LED_PIN, GPIO_OUT);
    led_off();

    if (gpio_init_int(EXO4_BTN_PIN, BTN0_MODE, BTN0_INT_FLANK,
                      button_irq_handler, NULL) != 0) {
        puts("Error: failed to initialize the button");
        return 1;
    }

    printf("LED on GPIO%d, button on GPIO%d\n\n", EXO4_LED_PIN, EXO4_BTN_PIN);

    encode_message(message_to_encode);

    puts("Decode mode enabled.");
    puts("Enter Morse with the BOOT button.");
    puts("Pause briefly after each letter to trigger decoding.\n");

    while (1) {
        uint32_t current_ms = now_ms();

        if (debounce_pending && (current_ms >= debounce_deadline_ms)) {
            debounce_pending = false;

            if (gpio_read(EXO4_BTN_PIN) == 0) {
                button_pressed = true;
                press_start_ms = current_ms;
            }
            else {
                irq_enabled = true;
                gpio_irq_enable(EXO4_BTN_PIN);
            }
        }

        if (button_pressed && (gpio_read(EXO4_BTN_PIN) != 0)) {
            uint32_t duration_ms = current_ms - press_start_ms;

            button_pressed = false;
            if (duration_ms < DOT_THRESHOLD_MS) {
                store_symbol_char('.');
            }
            else {
                store_symbol_char('-');
            }

            last_symbol_end_ms = current_ms;
            irq_enabled = true;
            gpio_irq_enable(EXO4_BTN_PIN);
        }

        if ((decoded_symbol_len > 0U) &&
            ((current_ms - last_symbol_end_ms) >= LETTER_TIMEOUT_MS)) {
            putchar('\n');
            decode_and_print();
        }

        xtimer_msleep(10);
    }

    return 0;
}
