#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "xtimer.h"

#define BENCHMARK_ITERATIONS   (50000U)

static uint32_t benchmark_workload(uint32_t iterations)
{
    uint32_t value = 0x12345678U;

    for (uint32_t i = 0; i < iterations; i++) {
        value ^= (i * 2654435761U);
        value = (value << 5) | (value >> 27);
        value += (i ^ 0xA5A5A5A5U);
    }

    return value;
}

int main(void)
{
    uint32_t iterations = BENCHMARK_ITERATIONS;
    uint32_t start_us;
    uint32_t end_us;
    uint32_t elapsed_us;
    uint32_t checksum;

    puts("\n=== Exercise 6: Simple benchmark ===\n");

    puts("Running integer workload...");
    start_us = xtimer_now_usec();
    checksum = benchmark_workload(iterations);
    end_us = xtimer_now_usec();

    elapsed_us = end_us - start_us;

    printf("Iterations: %" PRIu32 "\n", iterations);
    printf("Execution time: %" PRIu32 " us\n", elapsed_us);
    printf("Execution time: %" PRIu32 " ms\n", elapsed_us / 1000U);
    printf("Checksum: 0x%08" PRIX32 "\n", checksum);
    puts("A lower execution time means better performance.\n");

    return 0;
}
