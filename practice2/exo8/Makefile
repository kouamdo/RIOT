APPLICATION = practice2_exo8
BOARD ?= nrf52dk
RIOTBASE ?= $(CURDIR)/../../

# Core modules
USEMODULE += shell
USEMODULE += stdio
USEMODULE += xtimer

# LoRaWAN modules
USEMODULE += lora
USEMODULE += semtech_loramac

# Cryptography for secure implementation
USEMODULE += crypto
USEMODULE += nacl  # For public key cryptography

# Logging
USEMODULE += fmt

include $(RIOTBASE)/Makefile.include
