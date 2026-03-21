#include <stdio.h>
#include "board.h"
#include "periph/gpio.h"
#include "xtimer.h"

// À définir selon votre carte (exemple pour Nucleo)
#define BTN_PIN BTN0_PIN
#define LED_PIN LED0_PIN

// Variables globales
static int etat_led = 0;
static int frequence = 1;  // Hz

void bouton_handler(void *arg) {
    // Désactiver les interruptions temporairement (anti-rebond)
    gpio_irq_disable(BTN_PIN);
    
    // Gestion de l'appui (à compléter)
    
    // Réactiver après délai
    xtimer_usleep(50000);  // 50ms
    gpio_irq_enable(BTN_PIN);
}

int main(void) {
    puts("Exercice 1 : Lampe avec interrupteur");
    
    // Configuration du bouton en entrée avec interruption
    gpio_init_int(BTN_PIN, GPIO_IN_PU, GPIO_FALLING, bouton_handler, NULL);
    
    // Configuration de la LED en sortie
    gpio_init(LED_PIN, GPIO_OUT);
    
    while (1) {
        // À compléter : faire clignoter la LED selon frequence
        // Utiliser xtimer_usleep() pour les délais
    }
    
    return 0;
}
