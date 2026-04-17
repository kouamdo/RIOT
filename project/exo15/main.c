#include <stdio.h>
#include "board.h"
#include "periph/i2c.h"

#define I2C_DEV_FIRST I2C_DEV(0)

int main(void) {
    puts("=== Exercise 15: I2C address scan ===\n");

    i2c_init(I2C_DEV_FIRST);

    printf("Scanning the I2C bus on board %s\n", RIOT_BOARD);
    puts("Addresses (7-bit format):");

    int found = 0;
    uint8_t dummy = 0;

    i2c_acquire(I2C_DEV_FIRST);
    for (int addr = 1; addr < 128; addr++) {
        if (i2c_write_bytes(I2C_DEV_FIRST, addr, &dummy, 0, 0) == 0) {
            printf("  - 0x%02X (%d)\n", addr, addr);
            found++;
        }
    }
    i2c_release(I2C_DEV_FIRST);

    if (found == 0) {
        puts("  No device found.");
    } else {
        printf("\nTotal: %d device(s) found.\n", found);
    }
    
    return 0;
}
