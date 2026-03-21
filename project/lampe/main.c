#include <stdio.h>
#include "board.h"
#include "periph/gpio.h"
#include "xtimer.h"

// Définitions des boutons (à vérifier selon le brochage réel)
#define BTN1_PIN UNWD_GPIO_1   // Bouton 1 (celui que tu utilisais)
#define BTN2_PIN UNWD_GPIO_4   // Bouton 2
#define BTN3_PIN UNWD_GPIO_7   // Bouton 3
#define BTN4_PIN UNWD_GPIO_6   // Bouton 4

// LED (PB0)
#define LED_PIN GPIO_PIN(PORT_B, 0)

// Code secret (exemple avec 4 boutons)
const int CODE_SECRET[] = {4, 2, 4, 4};
const int CODE_LONGUEUR = 4;

static int code_saisi[4];
static int index_saisie = 0;
static int porte_ouverte = 0;

// Fonction pour lire l'état des boutons
int lire_bouton(void) {
    if (gpio_read(BTN1_PIN) == 0) return 1;
    if (gpio_read(BTN2_PIN) == 0) return 2;
    if (gpio_read(BTN3_PIN) == 0) return 3;
    if (gpio_read(BTN4_PIN) == 0) return 4;
    return 0;
}

int verifier_code(void) {
    for (int i = 0; i < CODE_LONGUEUR; i++) {
        if (code_saisi[i] != CODE_SECRET[i]) return 0;
    }
    return 1;
}

int main(void) {
    puts("Exercice 2 : Serrure à code avec UMDK-4BTN");
    printf("Code secret: [%d %d %d %d]\n", 
           CODE_SECRET[0], CODE_SECRET[1], CODE_SECRET[2], CODE_SECRET[3]);

    // Initialisation LED
    gpio_init(LED_PIN, GPIO_OUT);
    gpio_clear(LED_PIN);

    // Initialisation boutons
    gpio_init(BTN1_PIN, GPIO_IN_PU);
    gpio_init(BTN2_PIN, GPIO_IN_PU);
    gpio_init(BTN3_PIN, GPIO_IN_PU);
    gpio_init(BTN4_PIN, GPIO_IN_PU);

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
                    gpio_write(LED_PIN, porte_ouverte);
                    puts(porte_ouverte ? "✅ Porte ouverte !" : "🔒 Porte refermée");
                } else {
                    puts("❌ Code incorrect ! Réessayez.");
                }
                index_saisie = 0;
            }
        }
    }
    return 0;
}
