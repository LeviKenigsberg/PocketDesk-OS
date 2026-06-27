# PocketDesk OS

🚧 Status: Active Development
Current Version: v0.1.0

PocketDesk OS is an open-source touchscreen operating system for the ESP32 Cheap Yellow Display (CYD). The goal of the project is to transform the CYD into a portable desk companion with useful everyday tools while remaining easy to install, customize, and expand.

Features

Current Features (v0.1.0)

Touch calibration on first boot
Calibration saved to flash memory
Home launcher
Stopwatch
Countdown timer
Dice roller
Counter
Screen light
Settings
About page
Planned Features

Wi-Fi setup wizard
Internet clock
Calculator
Notes
Alarm
Themes
Plugin/App system
SD card support
Additional utilities
Requirements

Hardware

ESP32 Cheap Yellow Display (CYD)
MicroSD card

Software

Arduino IDE
ESP32 Arduino Board Package
Required Libraries

Install these libraries using the Arduino Library Manager:

TFT_eSPI
XPT2046_Touchscreen
The following libraries are included with the ESP32 Arduino Board Package:

WiFi
SPI
Preferences
time
Installation

Install the Arduino IDE.
Install the ESP32 Arduino Board Package.
Install the required libraries.
Open PocketDesk_OS.ino.
Select your ESP32 CYD board.
Upload the sketch.
On first boot, complete the touch calibration.
PocketDesk OS will save the calibration automatically.
Why PocketDesk OS?

PocketDesk OS is designed to be more than a collection of demos. The goal is to create a practical operating system for the CYD with tools that are genuinely useful, while making it easy for anyone to build on and contribute to the project.

Roadmap

Touch calibration
Home launcher
Stopwatch
Timer
Dice roller
Counter
Screen light
Wi-Fi setup
Internet clock
Calculator
Notes
Alarm
Themes
Plugin system
Contributing

Suggestions, bug reports, and pull requests are welcome. If you have an idea that would make PocketDesk OS better, feel free to open an issue or contribute.

License

PocketDesk OS is released under the MIT License.
