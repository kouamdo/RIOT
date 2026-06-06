APPLICATION = practice2_exo4
BOARD ?= nrf52dk
RIOTBASE ?= $(CURDIR)/../../

# Core modules
USEMODULE += shell
USEMODULE += stdio

# Bluetooth Low Energy modules
USEMODULE += nimble
USEMODULE += nimble_gap
USEMODULE += nimble_gatt

# GPIO and timer for pulse counting
USEMODULE += gpio
USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include
