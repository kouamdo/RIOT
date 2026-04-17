# RIOT Project

This directory contains several exercises and small demos built with the RIOT
embedded operating system, mainly targeting the `unwd-range-l1-r3` board.

Each exercise generally includes:

- a `main.c` file with the main source code
- a `Makefile` for building with RIOT

## Structure

The directory currently contains the following subprojects:

- `exo2`: code lock with buttons
- `exo3`: traffic light with pedestrian button
- `exo6`: simple benchmark based on an integer workload
- `exo7`: memory exploration and address display
- `exo8`: priority inversion demonstration
- `exo8_2`: deadlock demonstration
- `exo14`: RIOT shell with custom commands
- `exo15`: I2C address scan
- `exo22`: exercise in progress / to be completed

## Build

From an exercise directory:

```bash
make clean all
```

Example:

```bash
cd project/exo14
make clean all
```

## Flashing and target board

The `Makefile` files are configured by default for:

- `BOARD ?= unwd-range-l1-r3`

The following command is used to flash and open a serial terminal on the board:

```bash
make BOARD=unwd-range-l1-r3 PROGRAMMER=openocd flash term
```

## Purpose

The goal of this directory is to gather RIOT exercises around several topics:

- GPIO and interrupts
- timers and time measurement
- memory
- scheduling and synchronization
- embedded shell
- I2C bus

## Notes

- Some exercises are educational demonstrations rather than final applications.
- Some subdirectories may still be in the process of cleanup or build
  validation.
