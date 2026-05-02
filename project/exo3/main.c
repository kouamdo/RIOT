#include <stdio.h>
#include "board.h"
#include "periph/gpio.h"
#include "xtimer.h"

/* Cablage par defaut pour un ESP32 generique sous RIOT.
 * Modifie ces definitions si ton montage est different. */
#ifndef EXO3_LED_ROUGE
#define EXO3_LED_ROUGE  GPIO16
#endif
#ifndef EXO3_LED_ORANGE
#define EXO3_LED_ORANGE GPIO17
#endif
#ifndef EXO3_LED_VERTE
#define EXO3_LED_VERTE  GPIO18
#endif
#ifndef EXO3_BTN_PIETON
#define EXO3_BTN_PIETON GPIO19
#endif

// États du feu
typedef enum {
    ROUGE,
    ORANGE_VERS_VERT,  // Orange avant de passer au vert
    VERT,
    ORANGE_VERS_ROUGE  // Orange avant de passer au rouge
} etat_feu_t;

static etat_feu_t etat = ROUGE;
static uint32_t temps_debut_etat = 0;
static int demande_pieton = 0;

// Durées en ms
#define DUREE_ROUGE         5000
#define DUREE_ORANGE        1000
#define DUREE_VERT          5000

void changer_etat(etat_feu_t nouvel_etat) {
    // Éteindre toutes les LEDs d'abord
    gpio_clear(EXO3_LED_ROUGE);
    gpio_clear(EXO3_LED_ORANGE);
    gpio_clear(EXO3_LED_VERTE);
    
    // Allumer la bonne LED selon l'état
    switch (nouvel_etat) {
        case ROUGE:
            gpio_set(EXO3_LED_ROUGE);
            printf("Feu ROUGE\n");
            break;
        case ORANGE_VERS_VERT:
        case ORANGE_VERS_ROUGE:
            gpio_set(EXO3_LED_ORANGE);
            printf("Feu ORANGE\n");
            break;
        case VERT:
            gpio_set(EXO3_LED_VERTE);
            printf("Feu VERT\n");
            break;
    }
    
    etat = nouvel_etat;
    temps_debut_etat = xtimer_now_usec() / 1000;  // en ms
}

void bouton_handler(void *arg) {
    (void)arg;
    // Désactiver les interruptions temporairement
    gpio_irq_disable(EXO3_BTN_PIETON);
    
    // Anti-rebond
    xtimer_usleep(50000);
    
    if (gpio_read(EXO3_BTN_PIETON) == 0) {  // Bouton toujours appuyé
        demande_pieton = 1;
        puts("Demande pieton enregistree");
    }
    
    // Réactiver les interruptions
    gpio_irq_enable(EXO3_BTN_PIETON);
}

int main(void) {
    puts("Exercice 3 : Feu de signalisation avec bouton pieton");
    printf("Rouge=GPIO%d Orange=GPIO%d Vert=GPIO%d Bouton=GPIO%d\n",
           EXO3_LED_ROUGE, EXO3_LED_ORANGE, EXO3_LED_VERTE, EXO3_BTN_PIETON);
    
    // Initialiser les LEDs
    gpio_init(EXO3_LED_ROUGE, GPIO_OUT);
    gpio_init(EXO3_LED_ORANGE, GPIO_OUT);
    gpio_init(EXO3_LED_VERTE, GPIO_OUT);
    
    // Initialiser le bouton avec interruption
    gpio_init_int(EXO3_BTN_PIETON, GPIO_IN_PU, GPIO_FALLING, bouton_handler, NULL);
    
    // Démarrer avec le feu rouge
    changer_etat(ROUGE);
    
    while (1) {
        uint32_t maintenant = xtimer_now_usec() / 1000;
        uint32_t temps_ecoule = maintenant - temps_debut_etat;
        
        // Gestion du cycle automatique
        switch (etat) {
            case ROUGE:
                if (temps_ecoule >= DUREE_ROUGE) {
                    changer_etat(ORANGE_VERS_VERT);
                }
                break;
                
            case ORANGE_VERS_VERT:
                if (temps_ecoule >= DUREE_ORANGE) {
                    changer_etat(VERT);
                }
                break;
                
            case VERT:
                if (temps_ecoule >= DUREE_VERT) {
                    changer_etat(ORANGE_VERS_ROUGE);
                }
                break;
                
            case ORANGE_VERS_ROUGE:
                if (temps_ecoule >= DUREE_ORANGE) {
                    changer_etat(ROUGE);
                }
                break;
        }
        
        
        if (demande_pieton && etat == ROUGE) {
            // Si on est au rouge, on peut passer au orange si c'est bientôt le vert ?
            // Logique à implémenter selon la consigne
            demande_pieton = 0;
            puts("Demande pieton traitee");
        }
        
        xtimer_usleep(10000);  // 10ms pour éviter une boucle trop rapide
    }
    
    return 0;
}
