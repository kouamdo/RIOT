# Practice 1: RIOT Exercises Without Radio

This directory contains RIOT OS exercises for the first practical assignment set.
The assignment is about local embedded features only: GPIO, timers, interrupts,
threads, sensors, I2C, UART, shell commands, and small drivers. Radio networking
is not part of this practice set.

The local versions in this repository are adapted for a generic ESP32 development
board, using RIOT's `esp32-wroom-32` board by default.

## Assignment Summary

The original assignment asks for individual RIOT applications that can be shown
working on hardware. Points after each exercise number indicate the assignment
value. Not every exercise is required; points are cumulative across the practical
assignment sets.

Useful RIOT documentation:

- Core and thread management: https://doc.riot-os.org/group__core.html
- GPIO peripheral API: https://doc.riot-os.org/group__drivers__periph__gpio.html
- `xtimer`: https://doc.riot-os.org/group__sys__xtimer.html
- Atomic utilities: https://doc.riot-os.org/group__sys__atomic__utils.html

Important implementation note: GPIO interrupt handlers and timer callbacks run
in interrupt context. If the main thread and an interrupt handler access the same
variables, protect the shared state or use RIOT atomic helpers where needed.

Common modules for the first GPIO/timer exercises:

```make
USEMODULE += periph_gpio      # GPIO input/output
USEMODULE += periph_gpio_irq  # GPIO interrupts
USEMODULE += xtimer           # timers and delays
USEMODULE += atomic_utils     # atomic memory access helpers
```

## Requested Exercises

- `exo1` (1 pt): lamp with switch. Implement button polling, then GPIO interrupt handling with debounce, then blinking at selectable frequency using long press.
- `exo2` (1 pt): code lock with several buttons. Correct button sequence toggles a LED; wrong attempts and state changes are printed on the console.
- `exo3` (1 pt): traffic light with a pedestrian button. Switch multiple LEDs according to a schedule; button presses may reschedule the next green phase when it is too far away.
- `exo3` LCD extension (1 pt): add a character LCD and display the remaining time before the next switch.
- `exo4` (1 pt + 2 pt): Morse code encoding and decoding using GPIO.
- `exo5` (2 pt): electronic gear simulation with an encoder and stepper motor driver, including rotation direction.
- `exo6` (2 pt): benchmark the MCU with CoreMark or Dhrystone and compare the result with a PC.
- `exo7` (1 pt): print addresses of globals, locals, static variables, constants, interrupt data, functions, and thread variables to study memory layout.
- `exo8` (1 pt): demonstrate priority inversion and deadlock; enable mutex priority inheritance and discuss its limits.
- `exo9` (1 pt): connect BME280 and DHT11 sensors and compare their readings.
- `exo10` (2 pt): weather station with LCD using BME280 or DHT11 readings.
- `exo11` (2 pt): shock or movement detector using an accelerometer.
- `exo12` (3 pt): rotation counter using an accelerometer by detecting direction-vector flips.
- `exo13` (2 pt): proximity-sensor timer that activates an actuator for a fixed time.
- `exo14` (2 pt): enable RIOT shell and add custom commands, for example sensor commands.
- `exo15` (2 pt): scan and print all 7-bit I2C device addresses.
- `exo16` (2 pt): Modbus RTU over the standard console UART; Termite is used for testing.
- `exo17` (1 pt): connect a 1-Wire device using RIOT's `drivers/onewire`.
- `exo18` (2 pt): night light using PWM brightness control based on a light sensor.
- `exo19` (2 pt): stepper-motor indicator controlled by an ADC signal level.
- `exo20` (2 pt): code lock with contactless card reader and per-card passwords.
- `exo21` (2-4 pt): add a driver for an external device that RIOT does not already support.
- `exo22` (3 pt + 2 pt): bit-banged single-wire bus between devices; optional `netif` interface.
- `exo23` (2 pt + 10 pt): improve STM32L1 low-power sleep handling and optionally upstream the change.

## Implemented In This Repository

The directory currently contains these subprojects:

- `exo1`: lamp with one button and one LED.
- `exo2`: code lock with four buttons and one status LED.
- `exo3`: ESP32 traffic light with pedestrian button.
- `exo4`: Morse code encoding and decoding with GPIO.
- `exo6`: simple benchmark based on an integer workload.
- `exo7`: memory exploration and address display.
- `exo8`: priority inversion demonstration.
- `exo8_2`: deadlock demonstration.
- `exo12`: accelerometer-based rotation counter.
- `exo14`: RIOT shell with custom commands.
- `exo15`: I2C address scan.
- `exo16`: Modbus RTU slave on the console UART.
- `exo22`: exercise in progress / to be completed.

## ESP32 Target

The `Makefile` files in this practice directory are intended to use:

```make
BOARD ?= esp32-wroom-32
```

You can override the board from the command line if your ESP32 variant has a
different RIOT board definition:

```bash
make BOARD=esp32s3-devkit flash term
```

## Build

From the RIOT root, build one exercise with:

```bash
make -C project/practice1/exo3 clean all
```

From inside an exercise directory, use:

```bash
make clean all
```

If the ESP32 toolchain is installed under `/root/.espressif`, load it before
building:

```bash
export IDF_TOOLS_PATH=/root/.espressif
. ./dist/tools/esptools/export.sh esp32
```

Useful checks:

```bash
which xtensa-esp32-elf-gcc
xtensa-esp32-elf-gcc --version
```

## Flashing

If the ESP32 appears as `/dev/ttyUSB0`, flash with:

```bash
make -C project/practice1/exo3 BOARD=esp32-wroom-32 PORT=/dev/ttyUSB0 flash
```

To flash and open the RIOT serial terminal:

```bash
make -C project/practice1/exo3 BOARD=esp32-wroom-32 PORT=/dev/ttyUSB0 flash term
```

## WSL And USB Setup

The following commands were used to make an ESP32 with a CH340 USB serial
adapter work under WSL.

Install prerequisites and the ESP32 toolchain from the RIOT root:

```bash
sudo apt update
sudo apt install -y curl python3 python3-serial telnet
./dist/tools/esptools/install.sh esp32
```

Attach the USB device to WSL from a Windows terminal:

```powershell
usbipd list
usbipd bind --busid 1-1
usbipd attach --wsl --busid 1-1
```

Check the device from WSL:

```bash
lsusb
ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null
```

For the board used here, the USB/serial converter was `1a86:7523` (`CH340/CH341`).
If `/dev/ttyUSB0` does not appear automatically, load the serial driver:

```bash
sudo modprobe usbserial
sudo modprobe ch341
ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null
```

Useful troubleshooting commands:

```bash
dmesg | grep -iE 'ch34|ch341|ttyUSB|usbserial' | tail -20
uname -r
```

## Wiring Notes

The ESP32 versions define pins directly in each source file so they can be
changed easily if your wiring differs.

- `exo1`: integrated button via `BTN0_PIN`; integrated LED on `GPIO2`.
- `exo2`: buttons on `GPIO18`, `GPIO19`, `GPIO21`, `GPIO22`; status LED on `GPIO2`.
- `exo3`: red LED on `GPIO16`, yellow LED on `GPIO17`, green LED on `GPIO18`, pedestrian button on `GPIO19`.
- `exo4`: integrated button via `BTN0_PIN`; integrated LED on `GPIO2`.
- `exo16`: integrated LED on `GPIO2`; console UART used for Modbus RTU.

## Exercise Notes

`exo1` implements the lamp/switch task with interrupt handling, debounce,
periodic polling in the main loop, short press to toggle blinking, and long
press to change blink frequency.

`exo3` implements the requested traffic-light exercise for ESP32. The current
version keeps a pedestrian request if the button is pressed during the green or
yellow phase, then handles it during the red phase.

`exo4` implements Morse code with GPIO. The integrated LED blinks `RIOT ESP32`,
and the integrated BOOT button can be used to enter Morse manually.

`exo16` implements a simple Modbus RTU slave over the standard console UART at
`115200` baud, with slave ID `1` and function codes `0x03`, `0x06`, and `0x10`.
It is intended to be tested from a serial Modbus RTU master such as Termite.

## Notes

Some exercises are educational demonstrations rather than final applications.
Some subdirectories may still be in cleanup or build validation.
