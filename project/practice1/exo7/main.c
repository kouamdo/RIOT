#include <stdint.h>
#include <stdio.h>

#include "atomic_utils.h"
#include "thread.h"
#include "timex.h"
#include "xtimer.h"

int global_int = 42;
static int global_static_int = 100;
const int global_const_int = 200;

static char worker_stack[THREAD_STACKSIZE_DEFAULT +
                         THREAD_EXTRA_STACKSIZE_PRINTF];

static volatile uint8_t worker_done;
static volatile uint8_t callback_done;
static volatile uint32_t callback_local_value;
static volatile uint32_t callback_static_value;
static volatile uintptr_t callback_local_address;
static volatile uintptr_t callback_static_address;

static void simple_function(void)
{
    puts("simple_function is running");
}

static void *second_thread(void *arg)
{
    (void)arg;

    int thread_local = 50;
    static int thread_static = 60;

    puts("\n=== Variables from second thread ===");
    printf("thread_local:  value=%d address=%p\n",
           thread_local, (void *)&thread_local);
    printf("thread_static: value=%d address=%p\n",
           thread_static, (void *)&thread_static);

    atomic_store_u8(&worker_done, 1U);
    return NULL;
}

/* xtimer callbacks run in interrupt context, so values are saved atomically.
 * The main thread prints them later instead of calling printf from the ISR. */
static void timer_callback(void *arg)
{
    (void)arg;

    int callback_local = 70;
    static int callback_static = 80;

    atomic_store_u32(&callback_local_value, (uint32_t)callback_local);
    atomic_store_u32(&callback_static_value, (uint32_t)callback_static);
    atomic_store_uintptr(&callback_local_address, (uintptr_t)&callback_local);
    atomic_store_uintptr(&callback_static_address, (uintptr_t)&callback_static);
    atomic_store_u8(&callback_done, 1U);
}

static xtimer_t timer = {
    .callback = timer_callback,
};

int main(void)
{
    int main_local = 10;
    static int main_static = 20;

    puts("=== Memory Exploration (Exercise 7) ===");

    puts("\n=== Global variables ===");
    printf("global_int:        value=%d address=%p\n",
           global_int, (void *)&global_int);
    printf("global_static_int: value=%d address=%p\n",
           global_static_int, (void *)&global_static_int);
    printf("global_const_int:  value=%d address=%p\n",
           global_const_int, (void *)&global_const_int);

    puts("\n=== Variables from main thread ===");
    printf("main_local:  value=%d address=%p\n",
           main_local, (void *)&main_local);
    printf("main_static: value=%d address=%p\n",
           main_static, (void *)&main_static);

    puts("\n=== Functions ===");
    printf("main address:            %p\n", (void *)(uintptr_t)main);
    printf("simple_function address: %p\n",
           (void *)(uintptr_t)simple_function);
    simple_function();

    thread_create(worker_stack, sizeof(worker_stack),
                  THREAD_PRIORITY_MAIN - 1, 0,
                  second_thread, NULL, "memory_worker");

    xtimer_set(&timer, 200U * US_PER_MS);

    while (!atomic_load_u8(&worker_done) ||
           !atomic_load_u8(&callback_done)) {
        xtimer_msleep(10);
    }

    puts("\n=== Variables captured in timer callback (ISR context) ===");
    printf("callback_local:  value=%lu address=%p\n",
           (unsigned long)atomic_load_u32(&callback_local_value),
           (void *)atomic_load_uintptr(&callback_local_address));
    printf("callback_static: value=%lu address=%p\n",
           (unsigned long)atomic_load_u32(&callback_static_value),
           (void *)atomic_load_uintptr(&callback_static_address));

    puts("\nNote: the local callback address is printed only as a captured value.");
    puts("It must not be dereferenced after the callback returns.");

    return 0;
}
