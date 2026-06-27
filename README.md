# PocketDesk OS

PocketDesk OS is an open-source operating system for the ESP32 Cheap Yellow Display (CYD).

It transforms the CYD into a portable touchscreen desk gadget with built-in productivity tools, Wi-Fi features, and a web interface.

---

## Features

### Wi-Fi

- Wi-Fi setup portal (no hardcoded Wi-Fi credentials)
- Automatic Wi-Fi reconnect
- Built-in web server
- Network clock (NTP)

### Web Interface

- Send messages to the device
- Generate QR codes remotely
- Switch between apps

### Built-in Apps

- Wi-Fi Clock
- Stopwatch
- Timer
- Dice
- Counter
- Flashlight
- QR Code Generator
- Message Display
- Settings
- About

---

## Hardware

- ESP32 Cheap Yellow Display (CYD)
- 2.8" Touchscreen

---

## Software

- Arduino IDE
- ESP32 Board Package
- TFT_eSPI
- XPT2046_Touchscreen
- QRCode
- WiFi
- WebServer
- Preferences

---

## Installation

1. Install Arduino IDE.
2. Install the ESP32 board package.
3. Install all required libraries.
4. Open the PocketDesk OS project.
5. Upload it to your CYD.

On first boot, PocketDesk OS will:

- Run touch calibration (if needed)
- Ask for Wi-Fi setup
- Save Wi-Fi credentials
- Automatically reconnect on future boots

---

## Roadmap

### v0.3

- Better keyboard
- Calculator
- Alarm Clock
- Notes
- Wi-Fi scanner
- More built-in apps

---

## License

MIT License

