# RIOT Project

This directory contains several exercises and small demos built with the RIOT
embedded operating system, now configured by default for a generic ESP32
development board (`esp32-wroom-32` in RIOT).

Each exercise generally includes:

- a `main.c` file with the main source code
- a `Makefile` for building with RIOT

## Structure

The directory currently contains the following subprojects:

- `exo1`: lamp with one button and one LED
- `exo2`: code lock with buttons
- `exo3`: traffic light with pedestrian button
- `exo4`: Morse code encoding and decoding with GPIO
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

- `BOARD ?= esp32-wroom-32`

The following command is used to flash and open a serial terminal on the board:

```bash
make BOARD=esp32-wroom-32 flash term
```

If you use another RIOT ESP32 board definition, you can still override it from
the command line, for example:

```bash
make BOARD=esp32s3-devkit flash term
```

## Commands Used To Make ESP32 Work

Below is the full command sequence that was used in practice to make the ESP32
toolchain and flashing work under WSL with a CH340 USB serial adapter.

### 1. Install prerequisites and the ESP32 toolchain

Run from the RIOT root:

```bash
cd /mnt/e/RIOT
sudo apt update
sudo apt install -y curl python3 python3-serial telnet
./dist/tools/esptools/install.sh esp32
```

If the toolchain was installed under `/root/.espressif`, export that path:

```bash
export IDF_TOOLS_PATH=/root/.espressif
. ./dist/tools/esptools/export.sh esp32
```

Optional checks:

```bash
which xtensa-esp32-elf-gcc
xtensa-esp32-elf-gcc --version
```

### 2. Attach the ESP32 USB device to WSL from Windows

In a Windows terminal:

```powershell
usbipd list
```

Then attach the correct BUSID to WSL if needed:

```powershell
usbipd bind --busid 1-1
usbipd attach --wsl --busid 1-1
```

### 3. Check that Linux sees the USB adapter

In WSL:

```bash
lsusb
```

For the board used here, the USB/serial converter was:

- `1a86:7523` (`CH340/CH341`)

### 4. Load the USB serial driver in WSL

If `/dev/ttyUSB0` does not appear automatically, load the kernel module:

```bash
dmesg | tail -50
modprobe ch341
dmesg | tail -50
ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null
```

If needed, you can also load the generic USB serial layer first:

```bash
modprobe usbserial
modprobe ch341
```

### 5. Build an exercise

Example with `exo2`:

```bash
cd /mnt/e/RIOT/project/exo2
make BOARD=esp32-wroom-32
```

### 6. Flash the board

If the port is `/dev/ttyUSB0`:

```bash
make BOARD=esp32-wroom-32 PORT=/dev/ttyUSB0 flash
```

To flash and open the serial terminal in one command:

```bash
make BOARD=esp32-wroom-32 PORT=/dev/ttyUSB0 flash term
```

### 7. Useful troubleshooting commands

```bash
ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null
dmesg | grep -iE 'ch34|ch341|ttyUSB|usbserial' | tail -20
uname -r
```

## Wiring Notes For GPIO Exercises

Some exercises use external LEDs or buttons. For the ESP32 versions, the pins
are now defined directly in the source files and can be changed easily if your
wiring differs.

- `exo1`: integrated button via `BTN0_PIN` and integrated LED on `GPIO2`
- `exo2`: default buttons on GPIO18, GPIO19, GPIO21, GPIO22 and status LED on
  GPIO2
- `exo3`: default traffic-light LEDs on GPIO16, GPIO17, GPIO18 and pedestrian
  button on GPIO19
- `exo4`: integrated button via `BTN0_PIN` and integrated LED on `GPIO2`

## Exercise 1

`exo1` implements a lamp/switch exercise for ESP32 with the integrated button
and LED:

- short press: enable or stop LED blinking
- long press: change the blinking frequency
- debounce: handled by disabling the GPIO interrupt, then validating the button
  state in the main loop
- periodic polling: the main loop checks button release, long-press duration,
  and LED blink timing

## Exercise 4

`exo4` implements Morse code with the GPIO elements directly available on the
ESP32 board:

- encoding: the integrated LED blinks the message `RIOT ESP32` in Morse code
- decoding: the integrated BOOT button is used to enter Morse code manually
- short press: dot
- long press: dash
- end of letter: detected after a short silence, then decoded and printed

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
