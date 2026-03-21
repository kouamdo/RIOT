#include <stdio.h>
#include "board.h"

// Variables globales
int global_int = 42;
static int global_static_int = 100;
const int global_const_int = 200;
static const int global_static_const_int = 300;

// Une fonction simple
void ma_fonction(void) {
    printf("  -> Dans ma_fonction()\n");
}

int main(void) {
    puts("=== Exploration mémoire (Exercice 7) ===\n");
    
    // Variables locales
    int local_int = 10;
    static int local_static_int = 20;
    const int local_const_int = 30;
    static const int local_static_const_int = 40;
    
    printf("=== Variables GLOBALES ===\n");
    printf("global_int (global)                    : %p\n", (void*)&global_int);
    printf("global_static_int (global static)      : %p\n", (void*)&global_static_int);
    printf("global_const_int (global const)        : %p\n", (void*)&global_const_int);
    printf("global_static_const_int (global static const): %p\n\n", (void*)&global_static_const_int);
    
    printf("=== Variables LOCALES (dans main) ===\n");
    printf("local_int (local)                      : %p\n", (void*)&local_int);
    printf("local_static_int (local static)        : %p\n", (void*)&local_static_int);
    printf("local_const_int (local const)          : %p\n", (void*)&local_const_int);
    printf("local_static_const_int (local static const): %p\n\n", (void*)&local_static_const_int);
    
    printf("=== Fonctions ===\n");
    printf("Adresse de main()                       : %p\n", (void*)main);
    printf("Adresse de ma_fonction()                 : %p\n", (void*)ma_fonction);
    
    puts("\nAnalyse :");
    puts("- Les variables globales et static sont dans la mémoire statique (data/bss)");
    puts("- Les variables locales sont sur la pile (stack)");
    puts("- Les constantes peuvent être en flash (rodata)");
    
    return 0;
}
