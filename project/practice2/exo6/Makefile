APPLICATION = practice2_exo6
BOARD ?= nrf52dk
RIOTBASE ?= $(CURDIR)/../../

# Core modules
USEMODULE += shell
USEMODULE += stdio
USEMODULE += xtimer

# 6LoWPAN and IPv6
USEMODULE += gnrc
USEMODULE += gnrc_ipv6
USEMODULE += gnrc_netif
USEMODULE += gnrc_netif_6lo

# BLE networking
USEMODULE += nimble
USEMODULE += nimble_phy_1mphy
USEMODULE += nimble_ll_sched_earliest
USEMODULE += nimble_autoconn  # For auto-connection for mesh

# CoAP
USEMODULE += gcoap

# Logging
USEMODULE += fmt

include $(RIOTBASE)/Makefile.include
