# Exo 16 - Modbus RTU

## What this exercise is

In this exercise, the ESP32 works like a small `Modbus RTU slave`.

I used:

- the normal USB serial port of the ESP32
- the integrated LED
- fake sensor values stored in registers

So there is no real external sensor here. The values are simulated in the code.

## What the code does

The board waits for Modbus RTU frames on the console UART.

UART configuration:

- `UART_DEV(0)`
- `115200 baud`
- `8N1`
- slave address = `1`

The code supports these Modbus functions:

- `0x03` read holding registers
- `0x06` write one register
- `0x10` write several registers

## Registers used

The program uses a small array of holding registers.

- register `0`: temperature
- register `1`: humidity
- register `2`: pressure
- register `3`: LED state
- registers `4` to `7`: free / general use

The values are stored with one decimal digit:

- `234` means `23.4`
- `452` means `45.2`
- `10132` means `1013.2`

The LED is controlled by register `3`:

- write `0` -> LED off
- write `1` -> LED on

## Small explanation of how it works

When bytes arrive on UART, the callback stores them in a buffer.

Then the main loop waits for a short silence on the line. When there is no new
byte for a few milliseconds, the program considers that the Modbus frame is
complete.

After that:

1. it checks the slave address
2. it checks the CRC
3. it executes the Modbus function
4. it sends the answer back on UART

The code also updates the fake sensor values in the loop, so if I read them
many times, I can see that they change a little.

## Why there is almost no `printf`

I used the same UART for:

- console UART
- Modbus RTU communication

So printing debug text on this UART would break the Modbus frames. Because of
that, the program answers only with binary Modbus data.

## How to build and flash

From the RIOT folder:

```bash
cd /mnt/e/RIOT/project/exo16
make BOARD=esp32-wroom-32 PORT=/dev/ttyUSB0 flash
```

If needed, replace `/dev/ttyUSB0` with your real serial port.

Example:

```bash
make BOARD=esp32-wroom-32 PORT=/dev/ttyACM0 flash
```

## How to test it

For the tests I need a program on the PC that can send raw bytes on the serial
port. The subject says that `Termite` will be used.

Serial settings:

- baud rate: `115200`
- data bits: `8`
- parity: `none`
- stop bits: `1`
- flow control: `none`

Important: Modbus RTU is binary, not text.

## First test: read the registers

Send this request:

```text
01 03 00 00 00 04 44 09
```

Meaning:

- slave id = `01`
- function = `03`
- start address = `0000`
- number of registers = `0004`

This should read:

- temperature
- humidity
- pressure
- LED state

The response should start like this:

```text
01 03 08 ...
```

`08` means there are 8 data bytes, so 4 registers.

The exact values may change a bit because the sensors are simulated.

## Second test: turn LED on

Send:

```text
01 06 00 03 00 01 B8 0A
```

This writes value `1` into register `3`.

Expected result:

- the integrated LED turns on
- the response should be the same as the request

## Third test: turn LED off

Send:

```text
01 06 00 03 00 00 79 CA
```

Expected result:

- the integrated LED turns off
- the response should be the same as the request

## Fourth test: read only the LED register

Send:

```text
01 03 00 03 00 01 74 0A
```

If LED is off, the answer should contain:

```text
00 00
```

If LED is on, the answer should contain:

```text
00 01
```

## Fifth test: write several registers

Send:

```text
01 10 00 03 00 02 04 00 01 04 D2 60 E7
```

This writes:

- register `3` = `1`
- register `4` = `1234`

Expected result:

- LED turns on
- the board returns a normal Modbus reply for function `0x10`

## What I should observe

If everything works:

- the board answers only to slave `1`
- wrong CRC should give no valid response
- reading register `0`, `1`, `2` gives changing fake sensor values
- writing register `3` changes the LED state

## Problems I can have

If there is no response:

- wrong COM port
- wrong baud rate
- wrong CRC
- wrong slave ID
- serial program sends text instead of raw hex bytes

If I see strange characters:

- this is normal if the terminal tries to display Modbus RTU as plain text

## Quick test summary

The shortest test sequence is:

1. flash the program
2. open the serial port in Termite at `115200 8N1`
3. send `01 03 00 00 00 04 44 09`
4. check that registers are returned
5. send `01 06 00 03 00 01 B8 0A`
6. check that LED turns on
7. send `01 06 00 03 00 00 79 CA`
8. check that LED turns off

If these 3 requests work, then the exercise works.
