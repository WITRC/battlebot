# Monster Book of Monsters

Pico 2 W battlebot with Bluetooth controller support and Wi-Fi dashboard.

## Cross-Platform Setup

This project supports Windows, macOS, and Linux users:

### Windows Users

Follow instructions in [WINDOWS.md](WINDOWS.md)

- Use `scripts/setup.bat` or `scripts/setup-windows.ps1` to install dependencies
- Use `scripts/build.bat` or `scripts/build.ps1` to build the project
- Use `scripts/flash.bat` or `scripts/flash.ps1` to flash your Pico

### macOS Users

- Use `scripts/build.sh` to build the project
- Use `scripts/flash.sh` to flash your Pico
- Use `scripts/secrets.sh` to manage secrets

### Linux Users

Follow instructions in [LINUX.md](LINUX.md)

- Use `scripts/setup-linux.sh` to install dependencies
- Use `scripts/build-linux.sh` to build the project
- Use `scripts/flash-linux.sh` to flash your Pico
- Use `scripts/secrets-linux.sh` to manage secrets

## Documentation

- [Wiring & Pinout](docs/wiring.md) — pin assignments, ESC wiring, sensors, power
- [Controller Mapping](docs/controller.md) — Xbox button-to-function mapping and Bluepad32 constants

## Connecting Your Pico

To put your Pico in bootloader mode (needed for flashing):

1. Hold the BOOTSEL button
2. Connect USB while holding the button
3. Release the button after connecting

The flash scripts will automatically detect and flash your Pico.