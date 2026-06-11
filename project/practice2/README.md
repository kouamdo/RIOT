# Practice 2: Wireless Networks On ESP32

This directory follows the second practical assignment set. The original topic is wireless networking with RIOT: LoRaWAN, BLE/NimBLE, 6LoWPAN over BLE, CoAP, and security analysis.

The original assignment expects radio-capable hardware such as nRF52-DK boards, LoRaWAN credentials, multiple boards, and sometimes extra devices such as LCDs or sensors. For now, this repository targets the same board as practice1:

```make
BOARD ?= esp32-wroom-32
```

Because only one ESP32 board is currently available, the solved exercises are implemented as simulations, packet builders, protocol models, and security analysis programs. They can be run on the ESP32 now. The real radio demonstrations can be added later when the required hardware is available.

## ESP32 Environment

From the RIOT root, load the ESP32 toolchain before building or flashing:

```bash
export IDF_TOOLS_PATH=/root/.espressif
. ./dist/tools/esptools/export.sh esp32
```

Check that the compiler is available:

```bash
which xtensa-esp32-elf-gcc
xtensa-esp32-elf-gcc --version
```

If `which xtensa-esp32-elf-gcc` does not print a path under `/root/.espressif`, the ESP32 toolchain is not loaded in the current terminal.

## Common Commands

Build one exercise:

```bash
make -C project/practice2/exo1 BOARD=esp32-wroom-32
```

Clean and rebuild one exercise:

```bash
make -C project/practice2/exo1 BOARD=esp32-wroom-32 clean all
```

Flash and open the terminal, assuming the ESP32 is on `/dev/ttyUSB0`:

```bash
make -C project/practice2/exo1 BOARD=esp32-wroom-32 PORT=/dev/ttyUSB0 flash term
```

Use another exercise by replacing `exo1` with `exo2`, `exo3`, etc.

## Solved Exercises

### `exo1`: LoRaWAN Random Retry Intervals

Status: solved as an ESP32 retry/backoff simulation.

What it demonstrates:

- random retry intervals for a simulated `JoinRequest`
- random retry intervals for a simulated confirmed uplink
- bounded retry windows using RIOT's random API
- serial output showing each attempt and selected delay

Commands:

```bash
make -C project/practice2/exo1 BOARD=esp32-wroom-32
make -C project/practice2/exo1 BOARD=esp32-wroom-32 PORT=/dev/ttyUSB0 flash term
```

### `exo1_lorawan`: ESP32 + UMDK RF 107 LoRaWAN Test

Status: added as a real-hardware variant for an ESP32 connected to a UMDK RF 107 LoRa module.

What it demonstrates:

- OTAA join using RIOT's Semtech LoRaMAC package
- random retry intervals after failed `JoinRequest`
- confirmed uplink retry with random backoff
- configurable SX127x wiring for the external LoRa module

Default wiring:

```text
ESP32 GPIO18 -> LoRa SCK
ESP32 GPIO19 -> LoRa MISO
ESP32 GPIO23 -> LoRa MOSI
ESP32 GPIO5  -> LoRa NSS/CS
ESP32 GPIO14 -> LoRa RESET
ESP32 GPIO26 -> LoRa DIO0
ESP32 GPIO33 -> LoRa DIO1
ESP32 GPIO32 -> LoRa DIO2
3V3          -> LoRa VCC
GND          -> LoRa GND
```

The UMDK RF 107 is treated as an SX1276-compatible LoRa radio. If your physical pinout differs from the table above, edit `exo1_lorawan/include/sx127x_params.h`.

Build:

```bash
make -C project/practice2/exo1_lorawan BOARD=esp32-wroom-32
```

Flash with OTAA credentials:

```bash
make -C project/practice2/exo1_lorawan BOARD=esp32-wroom-32 PORT=/dev/ttyUSB0 \
    DEVEUI=0011223344556677 \
    APPEUI=0102030405060708 \
    APPKEY=00112233445566778899AABBCCDDEEFF \
    flash term
```

### `exo2`: Packet For A Legacy RIOT Crash

Status: solved as an ESP32 packet-construction and parser-safety model.

The assignment references commit `fffe8bb7328a61ab9756b28089c4f987a7f46f53`, but that commit object is not present in this local clone. The current program models the likely class of bug safely: an old parser trusting a packet length field before copying into a fixed-size buffer.

What it demonstrates:

- a safe control packet
- a malicious oversized packet
- a vulnerable parser model that would overflow
- a fixed parser model that rejects the packet

Commands:

```bash
make -C project/practice2/exo2 BOARD=esp32-wroom-32
make -C project/practice2/exo2 BOARD=esp32-wroom-32 PORT=/dev/ttyUSB0 flash term
```

### `exo3`: BLE Beacon Payload Lab

Status: solved as an ESP32 Eddystone payload preparation lab.

What it demonstrates:

- construction of an Eddystone URL payload
- payload byte dump on the serial console
- simulated beacon ticks

Hardware note: real Skald BLE advertising is blocked until a compatible BLE radio target is available, for example nRF52-DK.

Commands:

```bash
make -C project/practice2/exo3 BOARD=esp32-wroom-32
make -C project/practice2/exo3 BOARD=esp32-wroom-32 PORT=/dev/ttyUSB0 flash term
```

### `exo4`: BLE Cycling Speed And Cadence Preparation

Status: solved as an ESP32 pulse and payload encoder.

What it demonstrates:

- simulated GPIO pulse stream
- cadence calculation in RPM
- Cycling Speed and Cadence measurement encoding
- payload bytes that can later be sent through a BLE GATT characteristic

Hardware note: real BLE GATT notifications require NimBLE-capable radio support.

Commands:

```bash
make -C project/practice2/exo4 BOARD=esp32-wroom-32
make -C project/practice2/exo4 BOARD=esp32-wroom-32 PORT=/dev/ttyUSB0 flash term
```

### `exo5`: ECG Heart-Rate Processing

Status: solved as an ESP32 ECG processing simulation.

What it demonstrates:

- processing a built-in ECG-like sample array
- R-peak detection
- BPM estimation
- logic that can later feed a BLE Heart Rate Service characteristic

Hardware/data note: replace the built-in sample array with `examples-miem/dsp/ecg.h` data when that file is available.

Commands:

```bash
make -C project/practice2/exo5 BOARD=esp32-wroom-32
make -C project/practice2/exo5 BOARD=esp32-wroom-32 PORT=/dev/ttyUSB0 flash term
```

### `exo6`: 6LoWPAN/CoAP Scenario Model

Status: solved as a local CoAP exchange model.

What it demonstrates:

- a simulated CoAP discovery request
- a simulated state read
- a simulated state update
- the message flow expected later between 6LoWPAN nodes

Hardware note: real 6LoWPAN over BLE requires two or three compatible BLE boards.

Commands:

```bash
make -C project/practice2/exo6 BOARD=esp32-wroom-32
make -C project/practice2/exo6 BOARD=esp32-wroom-32 PORT=/dev/ttyUSB0 flash term
```

### `exo7`: Smart Traffic-Light Network Integration

Status: solved as an ESP32 network-integration model.

What it demonstrates:

- traffic-light state model
- simulated network commands from a traffic control center
- status report generation
- timing update commands
- design justification for LoRaWAN or 6LoWPAN integration

Hardware note: real radio integration is blocked until LoRaWAN or BLE networking hardware is available.

Commands:

```bash
make -C project/practice2/exo7 BOARD=esp32-wroom-32
make -C project/practice2/exo7 BOARD=esp32-wroom-32 PORT=/dev/ttyUSB0 flash term
```

### `exo8`: LoRaWAN Smart-Lock Security Analysis

Status: solved as an ESP32 security simulation.

What it demonstrates:

- a vulnerable fixed-payload `open` command
- replay attack behavior
- mitigation using a monotonic counter
- jamming analysis and fail-safe design notes

Commands:

```bash
make -C project/practice2/exo8 BOARD=esp32-wroom-32
make -C project/practice2/exo8 BOARD=esp32-wroom-32 PORT=/dev/ttyUSB0 flash term
```

## Build All Solved ESP32 Exercises

From the RIOT root:

```bash
export IDF_TOOLS_PATH=/root/.espressif
. ./dist/tools/esptools/export.sh esp32

for exo in exo1 exo2 exo3 exo4 exo5 exo6 exo7 exo8; do
    make -C project/practice2/$exo BOARD=esp32-wroom-32 || exit 1
done
```

## Exercises Blocked For Now

- `exo9`: requires LoRaWAN Class C support, LCD, and four buttons.
- `exo10`: requires several networked devices, sensors, and switches.
- `exo11`: concerns mbed LoRaWAN RU864 support, not the current RIOT ESP32 setup.

## Notes

The current ESP32 programs are not final radio demos. They are preparation and validation steps that make the protocol logic, packet formats, state machines, and security reasoning runnable now on the hardware currently available.
