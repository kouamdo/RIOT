/*
 * Practice 2 - Exercise 8
 * Bluetooth smart lock security simulation.
 *
 * A fixed Bluetooth command like "open" is easy to replay. A small counter is
 * added to show a simple protection idea.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CMD_LEN             (4U)
#define SECURE_CMD_LEN      (6U)

typedef enum {
    LOCKED,
    UNLOCKED,
    ALARM,
} lock_state_t;

typedef struct {
    uint8_t data[8];
    uint8_t len;
    int8_t rssi;
} bt_packet_t;

static lock_state_t state = LOCKED;
static uint16_t next_counter = 1;

static const char *state_name(void)
{
    if (state == LOCKED) {
        return "locked";
    }
    if (state == UNLOCKED) {
        return "unlocked";
    }
    return "alarm";
}

static int starts_with_open(const bt_packet_t *pkt)
{
    return (pkt->len >= CMD_LEN) && (memcmp(pkt->data, "open", CMD_LEN) == 0);
}

static void weak_lock_code(const bt_packet_t *pkt)
{
    printf("weak check, rssi=%d\n", pkt->rssi);

    if ((pkt->len == CMD_LEN) && starts_with_open(pkt)) {
        state = UNLOCKED;
        puts("accepted 'open' -> door opens");
    }
    else {
        puts("command rejected");
    }
}

static void better_lock_code(const bt_packet_t *pkt)
{
    printf("counter check, rssi=%d\n", pkt->rssi);

    if ((pkt->len != SECURE_CMD_LEN) || !starts_with_open(pkt)) {
        puts("bad command");
        return;
    }

    uint16_t counter = ((uint16_t)pkt->data[4] << 8) | pkt->data[5];
    printf("counter=%u expected=%u\n", counter, next_counter);

    if (counter == next_counter) {
        next_counter++;
        state = UNLOCKED;
        puts("fresh command accepted");
    }
    else {
        state = ALARM;
        puts("old command detected: possible replay");
    }
}

int main(void)
{
    bt_packet_t open_packet = {
        .data = { 'o', 'p', 'e', 'n' },
        .len = CMD_LEN,
        .rssi = -45,
    };

    bt_packet_t secure_packet = {
        .data = { 'o', 'p', 'e', 'n', 0, 1 },
        .len = SECURE_CMD_LEN,
        .rssi = -43,
    };

    puts("Practice 2 - Exercise 8");
    puts("Bluetooth smart lock security\n");

    puts("1) Weak version with fixed command");
    weak_lock_code(&open_packet);
    printf("state: %s\n", state_name());

    state = LOCKED;
    puts("replay the same packet");
    weak_lock_code(&open_packet);
    printf("state: %s\n\n", state_name());

    puts("2) Better version with a counter");
    state = LOCKED;
    next_counter = 1;
    better_lock_code(&secure_packet);
    printf("state: %s\n", state_name());

    state = LOCKED;
    puts("replay the same secure packet");
    better_lock_code(&secure_packet);
    printf("state: %s\n\n", state_name());

    puts("Conclusion:");
    puts("- Bluetooth commands also need freshness checks");
    puts("- a fixed payload is not enough for a smart lock");
    puts("- the lock should stay safe if radio commands look suspicious");

    return 0;
}
