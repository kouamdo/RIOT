#include <stdio.h>
#include "thread.h"
#include "mutex.h"
#include "xtimer.h"

static mutex_t my_mutex = MUTEX_INIT;

static char low_stack[THREAD_STACKSIZE_MAIN];
static char medium_stack[THREAD_STACKSIZE_DEFAULT];
static char high_stack[THREAD_STACKSIZE_DEFAULT];

static int counter = 0;

void *low_thread(void *arg)
{
    (void)arg;
    while (1) {
        mutex_lock(&my_mutex);
        printf("[LOW] I took the mutex\n");
        counter = 10;
        while (counter > 0) {
            printf("[LOW] Work: %d\n", counter);
            counter--;
            xtimer_usleep(100000);  // 100ms
        }
        printf("[LOW] Releasing the mutex\n");
        mutex_unlock(&my_mutex);
        xtimer_usleep(500000);  // 500ms
    }
    return NULL;
}

void *medium_thread(void *arg)
{
    (void)arg;
    while (1) {
        xtimer_usleep(300000);  // 300ms
        printf("[MEDIUM] Running with medium priority\n");
        xtimer_usleep(200000);
    }
    return NULL;
}

void *high_thread(void *arg)
{
    (void)arg;
    while (1) {
        xtimer_usleep(500000);  // 500ms
        printf("[HIGH] I need the mutex!\n");
        mutex_lock(&my_mutex);
        printf("[HIGH] I got the mutex!\n");
        mutex_unlock(&my_mutex);
        printf("[HIGH] Mutex released\n");
    }
    return NULL;
}

int main(void)
{
    puts("=== Exercise 8: Priority inversion ===\n");
    
    // Thread creation (priority: low=1, medium=2, high=3)
    thread_create(low_stack, sizeof(low_stack),
                  THREAD_PRIORITY_MAIN - 2, 0,
                  low_thread, NULL, "low");
                  
    thread_create(medium_stack, sizeof(medium_stack),
                  THREAD_PRIORITY_MAIN - 1, 0,
                  medium_thread, NULL, "medium");
                  
    thread_create(high_stack, sizeof(high_stack),
                  THREAD_PRIORITY_MAIN + 1, 0,
                  high_thread, NULL, "high");
    
    puts("Threads created. Observing priority inversion...\n");
    
    // The main thread does nothing, the others keep running
    while (1) {
        xtimer_usleep(1000000);
    }
    
    return 0;
}
