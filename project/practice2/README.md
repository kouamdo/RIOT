# Practice 2: Wireless Communication and Network Integration

This directory contains advanced exercises covering wireless communication protocols and networked embedded systems built with RIOT OS, primarily targeting **nRF52-DK** development boards.

## Overview

Practice 2 focuses on wireless technologies including:

- **LoRaWAN**: Long-range, low-power wide-area network communication
- **BLE (Bluetooth Low Energy)**: Short-range wireless communication with NimBLE stack
- **6LoWPAN over BLE**: IPv6 connectivity over Bluetooth
- **CoAP (Constrained Application Protocol)**: Lightweight IoT messaging
- **Network Security**: Encryption and authentication protocols

## Hardware Requirements

The exercises are designed for the following hardware:

- **Primary Board**: nRF52-DK (Nordic Semiconductor)
- **Supported Count**: 2-3 boards per exercise (for networked demonstrations)
- **Optional Accessories**:
  - HD44780 LCD display (16x2 or 20x4)
  - External LEDs and buttons (GPIO connections)
  - LoRaWAN gateway or simulator
  - Real ECG sensor data files

## Software Stack

### Build System
- **Build Command**: `make clean all`
- **Flash Command**: `make flash term`
- **Board Configuration**: `BOARD=nrf52dk` (can be overridden)

### RIOT Configuration
- **Target Board**: nRF52-DK by default
- **Default UART Baud**: 115200
- **Toolchain**: ARM GCC for nRF52

### Key MODULES and PACKAGES

#### LoRaWAN
- `MODULE_LORA`
- `MODULE_SEMTECH_LORAMAC` or equivalent LoRaWAN MAC implementation
- Frequency Plan: Check regional requirements (EU868, RU864, etc.)

#### BLE/NimBLE
- `MODULE_NIMBLE`
- `MODULE_NIMBLE_SCAN` (for scanning)
- `MODULE_NIMBLE_AUTOCONN` (for 6LoWPAN auto-connection)

#### 6LoWPAN
- `MODULE_GNRC_IPV6`
- `MODULE_GNRC_NETIF`
- `MODULE_GNRC_RPL` (optional, for mesh routing)

#### CoAP
- `MODULE_GCOAP`
- `MODULE_NANOCOAP` (optional, lightweight alternative)

#### Cryptography
- `MODULE_NACL` (NaCl cryptographic library)
- `MODULE_CRYPTO` (generic crypto support)

#### Utilities
- `MODULE_SHELL` (for interactive debugging)
- `MODULE_STDIO` (for console I/O)

## Exercise Structure

Each exercise is contained in its own subdirectory with the following structure:

```
exoN/
├── main.c           # Main application source code
├── Makefile         # Build configuration
└── README.md        # Exercise-specific documentation (optional)
```

## Environment Setup

### 1. Install nRF52 Toolchain

From the RIOT root directory:

```bash
./dist/tools/nrfutil/install.sh
```

Or manually:
```bash
sudo apt-get install -y gcc-arm-none-eabi binutils-arm-none-eabi
```

### 2. Install Flashing Tools

```bash
sudo apt-get install -y nrf5x-command-line-tools
pip install nrfutil
```

### 3. Set Board Environment (Optional)

```bash
export BOARD=nrf52dk
export BOARD_USB_SERIAL=/dev/ttyACM0  # Adjust to your port
```

### 4. Verify Setup

```bash
which arm-none-eabi-gcc
arm-none-eabi-gcc --version
nrfutil --version
```

## Common Build Commands

### Build only (no flashing)
```bash
cd exoN
make BOARD=nrf52dk
```

### Build and flash
```bash
cd exoN
make BOARD=nrf52dk flash
```

### Build, flash, and open serial terminal
```bash
cd exoN
make BOARD=nrf52dk flash term
```

### Specify custom serial port
```bash
cd exoN
make BOARD=nrf52dk PORT=/dev/ttyACM0 flash term
```

### Clean build
```bash
cd exoN
make clean
make BOARD=nrf52dk all
```

## Troubleshooting Common Issues

### Port Detection

List available serial ports:
```bash
ls /dev/ttyACM* /dev/ttyUSB* 2>/dev/null
```

For WSL (Windows Subsystem for Linux):
```bash
# In PowerShell
usbipd list
usbipd bind --busid <BUSID>
usbipd attach --wsl --busid <BUSID>

# Then in WSL
modprobe usbserial
```

### Firmware Flashing Issues

If flashing fails, try:
```bash
# Erase entire device
nrfutil device program --chip nrf52832 --erase
nrfutil device program --chip nrf52832 --program build/nrf52dk/bin/app.hex
```

### Serial Connection

Test serial connection:
```bash
miniterm.py /dev/ttyACM0 115200
# or
screen /dev/ttyACM0 115200
```

### Compiler Errors

Ensure all required modules are enabled in Makefile. Check RIOT documentation for specific module dependencies.

## Code Standards

- **Language**: C (C99 standard)
- **Coding Style**: Follow RIOT conventions from `CODING_CONVENTIONS.md`
- **Comments**: English only, descriptive and concise
- **Variable Naming**: `lowercase_with_underscores`
- **Function Naming**: `module_function_name()`

## Exercise Summary

### exo1: LoRaWAN Random Retry Intervals
**Topics**: LoRaWAN, Random Number Generation, Timing
- Implement randomized retry intervals for JoinRequest and data transmission
- Comply with LoRaWAN specification requirements
- Use RIOT's random number generation utilities

### exo2: Legacy RIOT Version Compatibility Issue
**Topics**: Packet Construction, Protocol Compliance, Debugging
- Create test packet that breaks old RIOT versions
- Analyze historical commit for root cause
- Demonstrate vulnerability and mitigation

### exo3: BLE Beacon Broadcasting (Skald)
**Topics**: BLE Advertising, Multi-board Communication
- Broadcast useful information via BLE beacons from nRF52-DK
- Measure signal strength (RSSI) on receiving devices
- Determine closest beacon based on signal quality

### exo4: BLE Cycling Speed and Cadence Profile
**Topics**: BLE GATT, NimBLE Library, GPIO Input
- Implement GATT Cycling Speed and Cadence service
- Count pulses on GPIO input
- Use NimBLE library for BLE profile implementation

### exo5: BLE Heart Rate with ECG Data
**Topics**: BLE Heart Rate Service, ECG Signal Processing, Data Simulation
- Extend nimble_heart_rate_sensor example
- Parse real ECG data from `examples-miem/dsp/ecg.h`
- Simulate physiological measurements from recorded data

### exo6: 6LoWPAN over BLE with CoAP
**Topics**: 6LoWPAN, BLE, CoAP, IPv6 Networking
- Connect 2-3 nRF52-DK boards in 6LoWPAN mesh over BLE
- Implement CoAP server on one board
- Implement CoAP client on another board
- Demonstrate message exchange over the network

### exo7: Smart Device Network Integration
**Topics**: LoRaWAN/6LoWPAN Integration, IoT Applications, Network Design
- Add wireless capabilities to practice1 smart devices (traffic light, safe, sensors)
- Connect devices to central control via LoRaWAN or 6LoWPAN
- Implement meaningful use cases with justification
- Optional external hardware (keypad, card reader, etc.)
- Points awarded for originality and practical design

### exo8: LoRaWAN Smart Lock Security Analysis
**Topics**: Security, Cryptography, Attack Analysis, Protocol Design
- Design LoRaWAN-based smart lock triggered by payload
- Analyze vulnerability to replay and jamming attacks
- Demonstrate attack if vulnerable, explain protections if not
- Implement counter-measures if needed

### exo9: Distant Voting Pager System
**Topics**: LoRaWAN, Class C Devices, HD44780 Display, User Interface
- Implement Class C LoRaWAN device for voting
- Receive questions via LoRaWAN
- Display on HD44780 LCD screen
- Collect user input via 4 buttons (4 candidates max)
- Send choice back to voting server via LoRaWAN

### exo10: Secure Voting Protocol (He-Su/Moscow E-Voting)
**Topics**: Cryptography, NaCl Library, Voting Protocols, Security
- Implement He-Su secret voting protocol
- Use NaCl cryptographic library
- Integrate with voting pager system
- Ensure voter privacy and election integrity

### exo11: Smart Home System with 6LoWPAN/CoAP
**Topics**: Home Automation, IoT Architecture, Sensor Networks, Control Logic
- Build distributed smart home network
- Support devices:
  - Lighting control (LEDs on development boards)
  - Button switches
  - Temperature sensors
  - Light sensors
- Central controller (microcontroller or PC)
- Implement meaningful automation scenarios
- Use CoAP for communication

### exo12: LoRaWAN RU864 Frequency Plan Support
**Topics**: LoRaWAN Specification, Embedded Networking, Contributing to Open Source
- Add RU864 frequency plan support to mbed LoRaWAN module
- Research regional regulations and frequencies
- Implement frequency tables and parameters
- Submit changes upstream (optional bonus)

## Building and Testing Workflow

```bash
# 1. Navigate to exercise
cd practice2/exoN

# 2. Review code
cat main.c

# 3. Build
make clean all BOARD=nrf52dk

# 4. Flash to board(s)
make BOARD=nrf52dk PORT=/dev/ttyACM0 flash

# 5. Monitor output
make BOARD=nrf52dk PORT=/dev/ttyACM0 term

# 6. Test functionality
# (As described in exercise-specific README)

# 7. Debug if needed
# Check build/nrf52dk/bin/app.elf for symbols
arm-none-eabi-gdb build/nrf52dk/bin/app.elf
```

## Documentation References

- [RIOT Documentation](https://doc.riot-os.org/)
- [LoRaWAN Specification](https://lora-alliance.org/lorawan-open-standards/)
- [Bluetooth SIG - BLE Specification](https://www.bluetooth.com/specifications/specs/)
- [NimBLE Documentation](https://github.com/apache/mynewt-nimble)
- [CoAP RFC 7252](https://tools.ietf.org/html/rfc7252)
- [6LoWPAN RFC 4944](https://tools.ietf.org/html/rfc4944)
- [NaCl Cryptography](https://nacl.cr.yp.to/)

## Tips and Best Practices

1. **Start Simple**: Begin with BLE before moving to LoRaWAN or 6LoWPAN
2. **Test Early**: Flash and test frequently, don't wait for perfect code
3. **Use Shell**: Enable `MODULE_SHELL` for interactive debugging
4. **Read Examples**: Study RIOT examples in `examples/` directory
5. **Monitor Power**: Consider battery life in wireless designs
6. **Backup State**: Save important configurations before major changes
7. **Use Git**: Commit working versions before attempting major modifications

## Common RIOT Makefile Variables

| Variable | Purpose | Example |
|----------|---------|---------|
| `BOARD` | Target board | `nrf52dk` |
| `PORT` | Serial port | `/dev/ttyACM0` |
| `BAUD` | Serial baudrate | `115200` |
| `PROGRAMMER` | Flash tool | `jlink` |
| `QUIET` | Suppress output | `1` |
| `DEBUG` | Enable debugging | `1` |
| `CFLAGS` | C compiler flags | `-Wall -O2` |

## Makefile Template

Each exercise Makefile should contain:

```makefile
APPLICATION = practice2_exoN
BOARD ?= nrf52dk
RIOTBASE ?= $(CURDIR)/../../

# Required modules
USEMODULE += shell
USEMODULE += stdio

# Optional modules (exercise-specific)
# USEMODULE += nimble
# USEMODULE += gcoap
# USEMODULE += lora

# Configure as needed
# CFLAGS += -DSOME_CONFIG=1

include $(RIOTBASE)/Makefile.include
```

## Notes

- Some exercises are educational demonstrations and may prioritize clarity over production efficiency
- Exercises build upon each other; complete earlier ones before advanced topics
- Group exercises with the same difficulty level can be done in parallel
- External hardware integration is optional but encouraged for originality points

## Getting Help

1. Check RIOT documentation at https://doc.riot-os.org/
2. Review RIOT examples in `examples/` directory
3. Search RIOT GitHub issues: https://github.com/RIOT-OS/RIOT/issues
4. Consult protocol specifications (links above)
5. Ask instructors during office hours or lab sessions

---

**Last Updated**: 2026-06-04
**RIOT Version Target**: Latest development branch
**Board**: nRF52-DK
