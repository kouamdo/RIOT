#include <stdio.h>
#include "board.h"

/* Global variables */
int global_int = 42;
static int global_static_int = 100;
const int global_const_int = 200;
static const int global_static_const_int = 300;

/* A simple function */
void my_function(void) {
    printf("  -> In my_function()\n");
}

int main(void) {
    puts("=== Memory Exploration (Exercise 7) ===\n");
    
    
    /* Local variables */
    int local_int = 10;
    static int local_static_int = 20;
    const int local_const_int = 30;
    static const int local_static_const_int = 40;
    
    printf("=== GLOBAL Variables ===\n");
    printf("global_int (global)                    : %p\n", (void*)&global_int);
    printf("global_static_int (global static)      : %p\n", (void*)&global_static_int);
    printf("global_const_int (global const)        : %p\n", (void*)&global_const_int);
    printf("global_static_const_int (global static const): %p\n\n", (void*)&global_static_const_int);
    
    printf("=== LOCAL Variables (in main) ===\n");
    printf("local_int (local)                      : %p\n", (void*)&local_int);
    printf("local_static_int (local static)        : %p\n", (void*)&local_static_int);
    printf("local_const_int (local const)          : %p\n", (void*)&local_const_int);
    printf("local_static_const_int (local static const): %p\n\n", (void*)&local_static_const_int);
    
    printf("=== Functions ===\n");
    printf("Address of main()                      : %p\n", (void*)main);
    printf("Address of my_function()               : %p\n", (void*)my_function);
    
    puts("\nAnalysis:");
    puts("- Global and static variables are stored in static memory (data/bss)");
    puts("- Local variables are stored on the stack");
    puts("- Constants may be stored in flash memory (rodata)");
    
    return 0;
}
