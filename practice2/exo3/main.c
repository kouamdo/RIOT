// exo3_annonceur/main.c
#include <stdio.h>
#include "board.h"
#include "xtimer.h"
#include "skald.h"
#include "skald_eddystone.h"

#define SKALD_INTERVAL_US   (1000U * 1000U) // Annonce toutes les 1s

static skald_ctx_t skald_ctx;
static skald_eddystone_uri_t eddystone_data = {
    .prefix = SKALD_EDDYSTONE_URL_PREFIX_HTTPS,
    .url = "riot-os.org" // ou "RIOT BLE Beacon"
};

static uint8_t tx_buf[SKALD_EDDYSTONE_CTX_DATA_LEN_MAX];

int main(void)
{
    puts("=== Exercice 3: BLE Beacon avec Skald (Annonceur) ===");

    // 1. Initialisation radio
    skald_init();

    // 2. Préparation du contexte Skald
    skald_ctx_init(&skald_ctx);
    skald_eddystone_uri_prep(&eddystone_data, tx_buf);
    skald_ctx_set_advdata(&skald_ctx, tx_buf, SKALD_EDDYSTONE_CTX_DATA_LEN_MAX);
    // Active le "Tx Power" pour le calcul de distance
    skald_eddystone_set_tx_power(&skald_ctx, 0);

    // 3. Démarrage de l'annonce
    skald_adv_start(&skald_ctx);
    puts("✅ Annonce BLE démarrée, vérifiez avec un autre appareil !");

    while (1) {
        xtimer_usleep(1000000);
    }

    return 0;
}
