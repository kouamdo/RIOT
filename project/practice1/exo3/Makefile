APPLICATION = exo3_annonceur
BOARD ?= esp32-wroom-32
RIOTBASE ?= $(CURDIR)/../..

USEMODULE += skald
USEMODULE += skald_eddystone
USEMODULE += xtimer

DISABLE_MODULE += nimble_netif
DISABLE_MODULE += nimble_ll

CFLAGS += -DSKALD_INTERVAL_US=1000000UL

include $(RIOTBASE)/Makefile.include
