#include <stdint.h>
#include <stdio.h>

#include "board.h"
#include "periph/i2c.h"
#include "xtimer.h"
#include "lsm6ds3.h"

static lsm6ds3_t dev;
static int turn_counter = 0;
static int last_z = 0;

#define FLIP_THRESHOLD   (2000)

int main(void)
{
    puts("\n=== Exercise 12: Turn Counter ===");
    puts("Shake or flip the board to increment the counter.\n");

    /* Initialize the default I2C bus */
    i2c_init(I2C_DEV(0));

    /* Initialize the LSM6DS3 sensor */
    if (lsm6ds3_init(&dev) != 0) {
        puts("Error: LSM6DS3 accelerometer not found.");
        puts("Check wiring (SDA=GPIO21, SCL=GPIO22) and power supply.");
        return 1;
    }

    puts("LSM6DS3 sensor initialized.");

    while (1) {
        lsm6ds3_data_t acc;

        if (lsm6ds3_read_acc(&dev, &acc)) {
            int z = acc.acc_z;

            /* Count a flip only when the Z axis changes sign with enough amplitude. */
            if ((last_z > FLIP_THRESHOLD && z < -FLIP_THRESHOLD) ||
                (last_z < -FLIP_THRESHOLD && z > FLIP_THRESHOLD)) {
                turn_counter++;
                printf("Flip detected. Counter = %d\n", turn_counter);
            }

            last_z = z;
        }

        xtimer_msleep(100);
    }

    return 0;
}
