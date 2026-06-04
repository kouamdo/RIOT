APPLICATION = practice2_exo7
BOARD ?= nrf52dk
RIOTBASE ?= $(CURDIR)/../../

# Core modules
USEMODULE += shell
USEMODULE += stdio
USEMODULE += xtimer

# GPIO and LED control
USEMODULE += gpio
USEMODULE += led

# Wireless modules (choose one or both)
USEMODULE += lora              # For LoRaWAN
USEMODULE += gnrc              # For 6LoWPAN/BLE
USEMODULE += nimble            # For BLE

# Logging and debugging
USEMODULE += fmt

include $(RIOTBASE)/Makefile.include
