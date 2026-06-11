/*
 * SX127x wiring for an external LoRa module connected to an ESP32-WROOM-32.
 *
 * Default SPI pins come from the RIOT esp32-wroom-32 board:
 * SCK=GPIO18, MISO=GPIO19, MOSI=GPIO23. Wire NSS/CS to GPIO5.
 *
 * Adjust the DIO/reset pins below to match your UMDK RF 107 wiring.
 */

#ifndef SX127X_PARAMS_H
#define SX127X_PARAMS_H

#include "board.h"
#include "sx127x.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SX127X_PARAM_SPI
#define SX127X_PARAM_SPI        (SPI_DEV(0))
#endif

#ifndef SX127X_PARAM_SPI_NSS
#define SX127X_PARAM_SPI_NSS    GPIO5
#endif

#ifndef SX127X_PARAM_RESET
#define SX127X_PARAM_RESET      GPIO14
#endif

#ifndef SX127X_PARAM_DIO0
#define SX127X_PARAM_DIO0       GPIO26
#endif

#ifndef SX127X_PARAM_DIO1
#define SX127X_PARAM_DIO1       GPIO33
#endif

#ifndef SX127X_PARAM_DIO2
#define SX127X_PARAM_DIO2       GPIO32
#endif

#ifndef SX127X_PARAM_DIO3
#define SX127X_PARAM_DIO3       GPIO_UNDEF
#endif

#ifndef SX127X_PARAM_PASELECT
#define SX127X_PARAM_PASELECT   (SX127X_PA_BOOST)
#endif

static const sx127x_params_t sx127x_params[] =
{
    {
        .spi       = SX127X_PARAM_SPI,
        .nss_pin   = SX127X_PARAM_SPI_NSS,
        .reset_pin = SX127X_PARAM_RESET,
        .dio0_pin  = SX127X_PARAM_DIO0,
        .dio1_pin  = SX127X_PARAM_DIO1,
        .dio2_pin  = SX127X_PARAM_DIO2,
        .dio3_pin  = SX127X_PARAM_DIO3,
        .paselect  = SX127X_PARAM_PASELECT,
    }
};

#ifdef __cplusplus
}
#endif

#endif /* SX127X_PARAMS_H */
