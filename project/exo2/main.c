#include <stdio.h>
#include "board.h"
#include "periph/gpio.h"
#include "xtimer.h"

/* Cablage par defaut pour un ESP32 generique sous RIOT.
 * Modifie ces definitions si ton montage est different. */
#ifndef EXO2_BTN1_PIN
#define EXO2_BTN1_PIN GPIO18
#endif
#ifndef EXO2_BTN2_PIN
#define EXO2_BTN2_PIN GPIO19
#endif
#ifndef EXO2_BTN3_PIN
#define EXO2_BTN3_PIN GPIO21
#endif
#ifndef EXO2_BTN4_PIN
#define EXO2_BTN4_PIN GPIO22
#endif
#ifndef EXO2_LED_PIN
#define EXO2_LED_PIN  GPIO2
#endif

// Code secret (exemple avec 4 boutons)
const int CODE_SECRET[] = {4, 2, 4, 4};
const int CODE_LONGUEUR = 4;

static int code_saisi[4];
static int index_saisie = 0;
static int porte_ouverte = 0;

// Fonction pour lire l'état des boutons
int lire_bouton(void) {
    if (gpio_read(EXO2_BTN1_PIN) == 0) return 1;
    if (gpio_read(EXO2_BTN2_PIN) == 0) return 2;
    if (gpio_read(EXO2_BTN3_PIN) == 0) return 3;
    if (gpio_read(EXO2_BTN4_PIN) == 0) return 4;
    return 0;
}

int verifier_code(void) {
    for (int i = 0; i < CODE_LONGUEUR; i++) {
        if (code_saisi[i] != CODE_SECRET[i]) return 0;
    }
    return 1;
}

int main(void) {
    puts("Exercice 2 : Serrure a code sur ESP32");
    printf("Code secret: [%d %d %d %d]\n", 
           CODE_SECRET[0], CODE_SECRET[1], CODE_SECRET[2], CODE_SECRET[3]);
    printf("LED: GPIO%d | Boutons: GPIO%d GPIO%d GPIO%d GPIO%d\n",
           EXO2_LED_PIN, EXO2_BTN1_PIN, EXO2_BTN2_PIN,
           EXO2_BTN3_PIN, EXO2_BTN4_PIN);

    // Initialisation LED
    gpio_init(EXO2_LED_PIN, GPIO_OUT);
    gpio_clear(EXO2_LED_PIN);

    // Initialisation boutons
    gpio_init(EXO2_BTN1_PIN, GPIO_IN_PU);
    gpio_init(EXO2_BTN2_PIN, GPIO_IN_PU);
    gpio_init(EXO2_BTN3_PIN, GPIO_IN_PU);
    gpio_init(EXO2_BTN4_PIN, GPIO_IN_PU);

    puts("Entrez le code (appuyez sur les boutons 1-4):");

    while (1) {
        int bouton = lire_bouton();
        
        if (bouton != 0) {
            // Anti-rebond
            xtimer_usleep(50000);
            while (lire_bouton() != 0) {
                xtimer_usleep(10000);
            }
            
            code_saisi[index_saisie] = bouton;
            index_saisie++;
            printf("Chiffre %d saisi (%d/%d)\n", bouton, index_saisie, CODE_LONGUEUR);
            
            if (index_saisie == CODE_LONGUEUR) {
                if (verifier_code()) {
                    porte_ouverte = !porte_ouverte;
                    gpio_write(EXO2_LED_PIN, porte_ouverte);
                    puts(porte_ouverte ? "Porte ouverte !" : "Porte refermee");
                } else {
                    puts("Code incorrect ! Reessayez.");
                }
                index_saisie = 0;
            }
        }
    }
    return 0;
}
