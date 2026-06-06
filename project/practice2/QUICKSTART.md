# Practice 2 - Quick Start Guide

## 📋 Overview

This directory contains 8 wireless communication exercises for the RIOT embedded operating system, targeting **nRF52-DK** development boards.

## 🚀 Quick Build

Each exercise follows the same build process:

```bash
# Navigate to exercise directory
cd practice2/exoN

# Build for nRF52-DK
make clean all BOARD=nrf52dk

# Flash and open serial terminal
make BOARD=nrf52dk flash term
```

## 📚 Exercise Directory

| # | Title | Focus | Difficulty |
|---|-------|-------|------------|
| 1 | LoRaWAN Random Retry | Protocol compliance, timing | ⭐ Basic |
| 2 | Legacy Version Issue | Security vulnerability, testing | ⭐ Basic |
| 3 | BLE Beacon Broadcasting | Wireless communication, RSSI | ⭐⭐ Intermediate |
| 4 | Cycling Speed & Cadence | BLE GATT, sensor integration | ⭐⭐ Intermediate |
| 5 | Heart Rate + ECG | Signal processing, data streaming | ⭐⭐⭐ Advanced |
| 6 | 6LoWPAN + CoAP | Mesh networking, IPv6 | ⭐⭐⭐ Advanced |
| 7 | Smart Device Network | IoT design, remote control | ⭐⭐⭐ Advanced |
| 8 | LoRaWAN Lock Security | Security analysis, attacks | ⭐⭐⭐ Advanced |

## 🛠️ Hardware Setup

**Minimum**: 1 nRF52-DK board  
**Recommended**: 2-3 nRF52-DK boards (for networked exercises)

```
nRF52-DK Board Features:
├── ARM Cortex-M4 (64 MHz)
├── 256 KB RAM / 512 KB Flash
├── Bluetooth 5 (BLE)
├── 20 GPIO pins
├── I2C, SPI, UART
└── On-board LED + Button
```

## 📖 Full Documentation

See `README.md` in this directory for:
- Complete exercise descriptions
- Hardware requirements
- Software stack details
- Troubleshooting guide
- Common build commands
- Code standards

## ✨ Key Features

✅ **All code in English** - Variable names, comments, and documentation  
✅ **Build-ready** - Each exercise has `main.c` and `Makefile`  
✅ **Realistic scenarios** - Based on real IoT applications  
✅ **Educational** - Detailed explanations of concepts  
✅ **Extensible** - Easy to modify and extend  
✅ **Security focus** - Includes vulnerability analysis  

## 🔗 Wireless Protocols Covered

- **LoRaWAN** - Long-range, low-power networking
- **BLE** - Bluetooth Low Energy with NimBLE
- **6LoWPAN** - IPv6 over low-power mesh
- **CoAP** - Constrained Application Protocol
- **GATT** - Generic Attribute Profile (BLE services)

## 📝 File Structure

```
practice2/
├── README.md                      # Complete guide
├── QUICKSTART.md                  # This file
├── exo1/
│   ├── main.c                     # Source code
│   └── Makefile                   # Build configuration
├── exo2/
│   ├── main.c
│   └── Makefile
└── ... (exo3 through exo8)
```

## 🎯 Learning Path

1. **Start with exo1 & exo2** - Understand basic RIOT concepts
2. **Move to exo3 & exo4** - Learn BLE and sensor integration
3. **Try exo5 & exo6** - Advanced networking and data processing
4. **Explore exo7 & exo8** - Real-world design and security

## 🐛 Troubleshooting

### Build Errors
```bash
# Clean and rebuild
make clean all BOARD=nrf52dk

# Check for missing dependencies
make info BOARD=nrf52dk
```

### Flash Issues
```bash
# Check device connection
ls /dev/ttyACM* /dev/ttyUSB*

# Use alternative port
make BOARD=nrf52dk PORT=/dev/ttyACM0 flash term
```

### Serial Terminal
```bash
# Monitor without flashing
miniterm.py /dev/ttyACM0 115200

# Or use screen
screen /dev/ttyACM0 115200
```

## 📚 Additional Resources

- RIOT OS Documentation: https://doc.riot-os.org/
- LoRaWAN Specification: https://lora-alliance.org/
- Bluetooth SIG: https://www.bluetooth.com/
- NimBLE: https://github.com/apache/mynewt-nimble
- CoAP (RFC 7252): https://tools.ietf.org/html/rfc7252

## 💡 Tips

- Start with simulated behavior before hardware
- Use `MODULE_SHELL` to debug interactively
- Check RIOT examples for reference implementations
- Test frequently, don't wait for "perfect" code
- Keep wireless power consumption in mind

## ✅ Verification Checklist

After building each exercise:

- [ ] Code compiles without warnings
- [ ] All required modules included in Makefile
- [ ] Device flashes successfully
- [ ] Serial output matches expectations
- [ ] Wireless functionality works (if applicable)
- [ ] Documentation updated

## 🚀 Next Steps

1. Review the full `README.md`
2. Choose an exercise matching your skill level
3. Study the provided code
4. Build and test on your board
5. Modify and extend with custom features
6. Document your changes

---

**Last Updated**: 2026-06-04  
**RIOT Version**: Latest development branch  
**Target Board**: nRF52-DK  
**Language**: C (English documentation)
