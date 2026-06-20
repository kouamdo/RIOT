# RIOT Practice Projects

This directory groups the local RIOT practical assignments implemented in this repository. The current hardware target is a generic ESP32 development board using RIOT's `esp32-wroom-32` board definition.

## Directory Overview

- `practice1`: first assignment set, focused on GPIO, timers, interrupts, memory, scheduling, shell, I2C, and UART/Modbus. These exercises avoid radio.
- `practice2`: second assignment set, originally focused on wireless networks. Because only one ESP32 board is currently available, the solved work is mostly simulation and protocol preparation for Bluetooth, BLE, 6LoWPAN, CoAP, and security topics.

## ESP32 Setup

From the RIOT root, load the ESP32 toolchain before compiling:

```bash
export IDF_TOOLS_PATH=/root/.espressif
. ./dist/tools/esptools/export.sh esp32
```

Check the compiler:

```bash
which xtensa-esp32-elf-gcc
```

Build an exercise:

```bash
make -C project/practice1/exo3 BOARD=esp32-wroom-32
make -C project/practice2/exo1 BOARD=esp32-wroom-32
```

Flash an exercise, assuming the ESP32 is on `/dev/ttyUSB0`:

```bash
make -C project/practice1/exo3 BOARD=esp32-wroom-32 PORT=/dev/ttyUSB0 flash term
make -C project/practice2/exo1 BOARD=esp32-wroom-32 PORT=/dev/ttyUSB0 flash term
```

## Practice 1 Summary

`practice1` is documented in `project/practice1/README.md`.

Implemented or prepared exercises include:

- `exo1`: lamp with switch, debounce, blinking, and long-press frequency change.
- `exo2`: code lock with buttons and status LED.
- `exo3`: ESP32 traffic light with pedestrian button.
- `exo4`: Morse encoding and decoding with GPIO.
- `exo6`: non-standard workload; official Dhrystone/CoreMark still pending.
- `exo7`: memory/address exploration across main, worker thread, and timer callback.
- `exo8`: priority inversion demonstration.
- `exo8_2`: deadlock demonstration.
- `exo12`: accelerometer-based rotation counter.
- `exo14`: RIOT shell with custom commands.
- `exo15`: I2C address scanner.
- `exo16`: Modbus RTU slave over console UART; hardware validation pending.
- `exo22`: exercise in progress.

## Practice 2 Summary

`practice2` is documented in `project/practice2/README.md`.

Solved ESP32-ready exercises include:

- `exo1`: Bluetooth-style random retry/backoff simulation.
- `exo2`: packet-construction and legacy parser-safety model.
- `exo3`: Eddystone/Skald payload preparation lab.
- `exo4`: Cycling Speed and Cadence payload encoder.
- `exo5`: ECG heart-rate processing simulation.
- `exo6`: local CoAP/6LoWPAN message-flow model.
- `exo7`: smart traffic-light network-integration model.
- `exo8`: Bluetooth smart-lock replay and counter security analysis.

Radio-dependent parts of `practice2` are intentionally marked as blocked until compatible hardware is available.

## Useful Build Loops

Build all currently solved `practice2` ESP32 exercises:

```bash
for exo in exo1 exo2 exo3 exo4 exo5 exo6 exo7 exo8; do
    make -C project/practice2/$exo BOARD=esp32-wroom-32 || exit 1
done
```

Build selected `practice1` exercises:

```bash
for exo in exo1 exo2 exo3 exo4 exo6 exo7 exo8 exo8_2 exo14 exo15 exo16; do
    make -C project/practice1/$exo BOARD=esp32-wroom-32 || exit 1
done
```

## Notes

Most exercises print their behavior on the RIOT serial terminal. For ESP32 under WSL, make sure the board is attached and visible as `/dev/ttyUSB0` or another serial device before using `flash term`.
