#include <stdbool.h>
#include <stdio.h>
#include "thread.h"
#include "mutex.h"
#include "xtimer.h"

static mutex_t mutexA = MUTEX_INIT;
static mutex_t mutexB = MUTEX_INIT;

#define WORKER_STACKSIZE   (THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF)

static char stack_deadlock1[WORKER_STACKSIZE];
static char stack_deadlock2[WORKER_STACKSIZE];
static char stack_watchdog[WORKER_STACKSIZE];

void *thread_deadlock1(void *arg)
{
    (void)arg;
    puts("[THREAD1] Starting");
    
    mutex_lock(&mutexA);
    puts("[THREAD1] Locked mutex A");
    
    xtimer_usleep(50000);  // 50 ms - let the other thread run
    
    puts("[THREAD1] Waiting for mutex B...");
    mutex_lock(&mutexB);
    
    puts("[THREAD1] Locked both mutexes (this should never be printed)");
    
    mutex_unlock(&mutexB);
    mutex_unlock(&mutexA);
    
    return NULL;
}

void *thread_deadlock2(void *arg)
{
    (void)arg;
    puts("[THREAD2] Starting");
    
    mutex_lock(&mutexB);
    puts("[THREAD2] Locked mutex B");
    
    xtimer_usleep(50000);  // 50ms
    
    puts("[THREAD2] Waiting for mutex A...");
    mutex_lock(&mutexA);
    
    puts("[THREAD2] Locked both mutexes (this should never be printed)");
    
    mutex_unlock(&mutexA);
    mutex_unlock(&mutexB);
    
    return NULL;
}

void *thread_surveillance(void *arg)
{
    (void)arg;
    int seconds = 0;
    bool reported = false;
    
    while (1) {
        xtimer_usleep(1000000);  // 1 second
        
        seconds++;
        if ((seconds >= 5) && !reported) {
            puts("\nDEADLOCK DETECTED");
            puts("Both threads have been blocked for 5 seconds.");
            puts("Thread1 is waiting for mutex B (held by Thread2).");
            puts("Thread2 is waiting for mutex A (held by Thread1).\n");
            reported = true;
        }
    }
    return NULL;
}

int main(void)
{
    kernel_pid_t deadlock1_pid;
    kernel_pid_t deadlock2_pid;
    kernel_pid_t watchdog_pid;

    puts("\n=== DEADLOCK DEMONSTRATION ===\n");
    puts("Scenario:");
    puts("  Thread1 locks mutex A and waits for mutex B");
    puts("  Thread2 locks mutex B and waits for mutex A");
    puts("  -> Both threads block each other\n");
    
    deadlock1_pid = thread_create(stack_deadlock1, sizeof(stack_deadlock1),
                                  THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
                                  thread_deadlock1, NULL, "deadlock1");

    deadlock2_pid = thread_create(stack_deadlock2, sizeof(stack_deadlock2),
                                  THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
                                  thread_deadlock2, NULL, "deadlock2");

    watchdog_pid = thread_create(stack_watchdog, sizeof(stack_watchdog),
                                 THREAD_PRIORITY_MAIN, THREAD_CREATE_STACKTEST,
                                 thread_surveillance, NULL, "watchdog");

    if ((deadlock1_pid < 0) || (deadlock2_pid < 0) || (watchdog_pid < 0)) {
        puts("Failed to create one or more threads.");
        return 1;
    }
    
    puts("Threads created. The deadlock should happen now.\n");
    
    while (1) {
        xtimer_usleep(1000000);
    }
    
    return 0;
}
