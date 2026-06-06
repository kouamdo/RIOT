# Practice 2: Wireless Networks

This directory follows the second practical assignment set. The original topic is
wireless networking with RIOT: LoRaWAN, BLE, NimBLE, 6LoWPAN over BLE, CoAP, and
security analysis.

The assignment was designed mainly around nRF52-DK boards and LoRaWAN hardware.
For this repository, the current target board is the same ESP32 used in
`project/practice1`:

```make
BOARD ?= esp32-wroom-32
```

For now, exercises that can be prepared or demonstrated with only the ESP32 are
implemented as local simulations, packet builders, protocol models, or security
analysis programs. Exercises that require real BLE/NimBLE, nRF52 radio, multiple
boards, LoRaWAN credentials, or extra displays/sensors are documented as hardware
blocked until the needed equipment is available.

## Requested Exercises

- `exo1` (1 pt): add random retry intervals for LoRaWAN transmissions or JoinRequest attempts.
- `exo2` (2 pt): construct a packet that breaks old RIOT versions, based on commit `fffe8bb7328a61ab9756b28089c4f987a7f46f53`.
- `exo3` (1 pt + 2 pt): broadcast useful BLE beacons with Skald on nRF52-DK; scan with NimBLE and estimate the nearest beacon by RSSI.
- `exo4` (2 pt): implement the BLE Cycling Speed and Cadence profile using NimBLE and GPIO pulse counting.
- `exo5` (2 pt): extend `examples/nimble_heart_rate_sensor` using real ECG data from `examples-miem/dsp/ecg.h`.
- `exo6` (2 pt): connect two or three nRF52-DK boards using 6LoWPAN over BLE and exchange CoAP messages.
- `exo7` (2+ pt): add radio networking to a device from practice 1 or 2, with a meaningful use case.
- `exo8` (3 pt): analyze whether a LoRaWAN smart lock using a fixed `open` payload is vulnerable to replay/jamming attacks, and demonstrate or explain the result.
- `exo9` (3 pt + 3 pt): LoRaWAN Class C voting pager with LCD and four buttons; optional He-Su secret voting protocol with NaCl.
- `exo10` (3 pt): smart home system with 6LoWPAN and CoAP, including lights, switches, and sensors.
- `exo11` (2 pt + 10 pt): add RU864 frequency-plan support to mbed LoRaWAN and optionally upstream it.

## Current ESP32 Scope

These folders are useful with only the ESP32 board available now:

- `exo1`: random retry/backoff simulator for LoRaWAN-style retries.
- `exo2`: packet-construction lab for the historical RIOT crash exercise; the exact commit is not present in this clone, so the code documents that limitation and builds a reproducible oversized packet model.
- `exo3`: Eddystone/Skald payload preparation lab; real BLE advertising still requires compatible BLE radio support.
- `exo4`: Cycling Speed and Cadence payload encoder with simulated GPIO pulses; real BLE GATT notifications require NimBLE-capable hardware/support.
- `exo5`: ECG/heart-rate processing simulator; real BLE heart-rate service can be added later.
- `exo6`: local CoAP-over-6LoWPAN scenario model; real network exchange requires multiple BLE-capable boards.
- `exo7`: smart traffic-light network-integration scenario, modelled locally.
- `exo8`: smart-lock replay and jamming security analysis, modelled locally.

Hardware-blocked for now:

- `exo9`: requires LoRaWAN Class C setup, LCD, and buttons.
- `exo10`: requires several networked devices and sensors/switches.
- `exo11`: concerns mbed LoRaWAN RU864 support, not the ESP32 RIOT target.

## Build Setup For ESP32

Load the ESP32 toolchain from the RIOT root before building:

```bash
export IDF_TOOLS_PATH=/root/.espressif
. ./dist/tools/esptools/export.sh esp32
```

Build one exercise from the RIOT root:

```bash
make -C project/practice2/exo1 clean all
```

Build from inside an exercise directory:

```bash
make clean all
```

Flash to an ESP32 connected as `/dev/ttyUSB0`:

```bash
make -C project/practice2/exo1 BOARD=esp32-wroom-32 PORT=/dev/ttyUSB0 flash term
```

## Exercise Status

| Exercise | Current state | ESP32-only status |
| --- | --- | --- |
| `exo1` | Implemented as retry/backoff simulation | Buildable now |
| `exo2` | Added packet-construction lab | Buildable now |
| `exo3` | Corrected from nRF52 Skald build to ESP32 payload lab | Buildable now, radio demo blocked |
| `exo4` | Corrected from NimBLE-only build to ESP32 CSC encoder | Buildable now, BLE GATT blocked |
| `exo5` | Added ECG/heart-rate simulator | Buildable now |
| `exo6` | Added local CoAP/6LoWPAN scenario model | Buildable now, multi-board demo blocked |
| `exo7` | Corrected Makefile for ESP32 simulation | Buildable now, real radio integration blocked |
| `exo8` | Corrected Makefile for ESP32 security simulation | Buildable now |
| `exo9` | Not started | Hardware blocked |
| `exo10` | Not started | Hardware blocked |
| `exo11` | Not started | Not relevant to ESP32 RIOT build |

## Notes

The current ESP32 programs are preparation and validation steps. They make the
protocol logic, packet formats, state machines, and security reasoning runnable
now, while keeping the real wireless work clearly separated for later hardware.
