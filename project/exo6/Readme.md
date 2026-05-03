## 📊 Benchmark Results: PC vs ESP32

# Command to run :

```bash
# Navigate to the benchmark test
cd ~/RIOT/tests/bench_xtimer

# Clean previous builds
make BOARD=esp32-wroom-32 clean
make BOARD=native clean

# Compile and flash for ESP32
make BOARD=esp32-wroom-32 all
make BOARD=esp32-wroom-32 flash

# Open serial terminal to run the test on ESP32
make BOARD=esp32-wroom-32 term
# Then press 's' to start the benchmark

# Compile and run natively on PC
make BOARD=native all
./bin/native/tests_bench_xtimer.elf
# Then press 's' to start the benchmark
```

## What we have getting :

inside esp32 :

```bash
# make BOARD=esp32-wroom-32 term 
/mnt/e/RIOT/dist/tools/pyterm/pyterm -p "/dev/ttyUSB0" -b "115200"  
Twisted not available, please install it if you want to use pyterm's JSON capabilities
2026-05-03 15:33:11,086 # Connect to serial port /dev/ttyUSB0
Welcome to pyterm!
Type '/exit' to exit.
2026-05-03 15:33:21,996 # ets Jul 29 2019 12:21:46
2026-05-03 15:33:21,996 # 
2026-05-03 15:33:22,000 # rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
2026-05-03 15:33:22,003 # configsip: 0, SPIWP:0xee
2026-05-03 15:33:22,009 # clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
2026-05-03 15:33:22,012 # mode:DOUT, clock div:2
2026-05-03 15:33:22,014 # load:0x3fff0030,len:1380
2026-05-03 15:33:22,017 # load:0x40078000,len:12788
2026-05-03 15:33:22,018 # load:0x40080400,len:2840
2026-05-03 15:33:22,021 # entry 0x40080410
2026-05-03 15:33:22,068 # Pro cpu up.
2026-05-03 15:33:22,069 # Single core mode
2026-05-03 15:33:22,123 # 
2026-05-03 15:33:22,130 # Help: Press s to start test, r to print it is ready
S
2026-05-03 15:33:57,455 # Help: Press s to start test, r to print it is ready
s
2026-05-03 15:35:39,844 # START
2026-05-03 15:35:39,850 # main(): This is RIOT! (Version: 2017.01-devel-30144-gc4f769-develop)
2026-05-03 15:35:39,853 # xtimer benchmark application.
2026-05-03 15:35:39,853 # 
2026-05-03 15:35:39,859 #                      set() one     8829 / 1000 = 8
2026-05-03 15:35:39,864 #                   remove() one     1178 / 1000 = 1
2026-05-03 15:35:39,873 #           set() + remove() one    13280 / 1000 = 13
2026-05-03 15:35:39,996 #   set() many increasing target   121929 / 1000 = 121
2026-05-03 15:35:40,005 #                re-set()  first     8981 / 1000 = 8
2026-05-03 15:35:40,213 #                re-set() middle   207155 / 1000 = 207
2026-05-03 15:35:40,616 #                re-set()   last   403268 / 1000 = 403
2026-05-03 15:35:40,631 #        remove() + set()  first    14803 / 1000 = 14
2026-05-03 15:35:40,846 #        remove() + set() middle   214713 / 1000 = 214
2026-05-03 15:35:41,257 #        remove() + set()   last   410660 / 1000 = 410
2026-05-03 15:35:41,346 #       remove() many decreasing    89643 / 1000 = 89
2026-05-03 15:35:41,352 #                   xtimer_now()     1326 / 1000 = 1
2026-05-03 15:35:41,355 #               sizeof(xtimer_t)    16000 / 1000 = 16
2026-05-03 15:35:41,357 # done.
2026-05-03 15:35:41,363 # { "threads": [{ "name": "idle", "stack_size": 2048, "stack_used": 2048 }]}
```

inside native computer :

```bash

# ./tests_bench_xtimer.elf 
RIOT native interrupts/signals initialized.
RIOT native board initialized.
RIOT native hardware initialization complete.

Help: Press s to start test, r to print it is ready
s
START
main(): This is RIOT! (Version: 2017.01-devel-30144-gc4f769-develop)
xtimer benchmark application.

                     set() one     5807 / 1000 = 5
                  remove() one     1719 / 1000 = 1
          set() + remove() one     7973 / 1000 = 7
  set() many increasing target     3429 / 1000 = 3
               re-set()  first     6254 / 1000 = 6
               re-set() middle     4285 / 1000 = 4
               re-set()   last     6057 / 1000 = 6
       remove() + set()  first    10848 / 1000 = 10
       remove() + set() middle    10208 / 1000 = 10
       remove() + set()   last    12421 / 1000 = 12
      remove() many decreasing     6152 / 1000 = 6
                  xtimer_now()       77 / 1000 = 0
              sizeof(xtimer_t)    16000 / 1000 = 16
done.
{ "threads": [{ "name": "idle", "stack_size": 8192, "stack_used": 436 }]}
{ "threads": [{ "name": "main", "stack_size": 12288, "stack_used": 2620 }]}
^C
native: exiting

```

## Conclusion
| Operation | PC (native) | ESP32-WROOM-32 | Ratio |
|-----------|-------------|----------------|-------|
| set() one | 5 µs | 8 µs | 1.6x |
| remove() one | 1 µs | 1 µs | 1x |
| set() + remove() | 7 µs | 13 µs | 1.9x |
| set() many increasing | 3 µs | 121 µs | **40x** |
| re-set() middle | 4 µs | 207 µs | **52x** |
| re-set() last | 6 µs | 403 µs | **67x** |

*Note: The PC outperforms the ESP32 significantly on list-based operations due to its higher clock speed (3+ GHz vs 240 MHz) and optimized memory access.*
