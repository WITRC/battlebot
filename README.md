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

## Pinout

```
                              Pico 2 W
                          ┌───────────────┐
  Left Throttle    GP0 ←──┤ 1          40 ├── VBUS
  Left Reverse     GP1 ←──┤ 2          39 ├── VSYS ───── Step-down 5V (power in)
                   GND ←──┤ 3          38 ├── GND ────── Common GND / SD Card GND
  Right Throttle   GP2 ←──┤ 4          37 ├── 3V3_EN
  Right Reverse    GP3 ←──┤ 5          36 ├── 3V3 ────── SD Card 3V3
  DHT11 Data       GP4 ───┤ 6          35 ├── ADC_VREF
  Fan 1 Tach       GP5 ───┤ 7          34 ├── GP28
                   GND ───┤ 8          33 ├── GND
  Weapon           GP6 ───┤ 9          32 ├── GP27
  Fan 2 Tach       GP7 ───┤ 10         31 ├── GP26 ←──── Battery ADC
  LED Strip DIN    GP8 ───┤ 11         30 ├── RUN
                   GP9 ───┤ 12         29 ├── GP22
                   GND ───┤ 13         28 ├── GND
                  GP10 ───┤ 14         27 ├── GP21
                  GP11 ───┤ 15         26 ├── GP20
  IMU TX          GP12 ───┤ 16         25 ├── GP19 ←──── SD Card MOSI (SPI0)
  IMU RX          GP13 ───┤ 17         24 ├── GP18 ←──── SD Card CLK  (SPI0)
                   GND ───┤ 18         23 ├── GND
  Fan 1 PWM       GP14 ───┤ 19         22 ├── GP17 ←──── SD Card CS   (SPI0)
  Fan 2 PWM       GP15 ───┤ 20         21 ├── GP16 ←──── SD Card MISO (SPI0)
                          └───────────────┘
```

### Pin Assignment Summary

| GPIO | Function              | Peripheral   | Status  |
|------|-----------------------|--------------|---------|
| GP0  | Left Motor Throttle   | PWM Slice 0A | Wired   |
| GP1  | Left Motor Reverse    | PWM Slice 0B | Wired   |
| GP2  | Right Motor Throttle  | PWM Slice 1A | Wired   |
| GP3  | Right Motor Reverse   | PWM Slice 1B | Wired   |
| GP4  | DHT11 Temp/Humidity   | GPIO         | Wired   |
| GP5  | Fan 1 Tach (RPM)      | GPIO Input   | Planned |
| GP6  | Weapon Motor          | PWM Slice 3A | Wired   |
| GP7  | Fan 2 Tach (RPM)      | GPIO Input   | Planned |
| GP8  | WS2812B LED Strip DIN | PIO          | Planned |
| GP14 | Fan 1 PWM             | PWM Slice 7A | Planned |
| GP15 | Fan 2 PWM             | PWM Slice 7B | Planned |
| GP12 | IMU TX                | UART0 TX     | Planned |
| GP13 | IMU RX                | UART0 RX     | Planned |
| GP16 | SD Card MISO          | SPI0 RX      | Planned |
| GP17 | SD Card CS            | SPI0 CSn     | Planned |
| GP18 | SD Card CLK           | SPI0 SCK     | Planned |
| GP19 | SD Card MOSI          | SPI0 TX      | Planned |
| GP26 | Battery Voltage       | ADC0         | Planned |

### ESC Wiring

Each bidirectional ESC has 3 wires:
- **Signal** (white/yellow) → Connect to GPIO pin (two signals per ESC: throttle + reverse)
- **Ground** (black/brown) → Connect to Pico GND
- **Power** (red) → Leave disconnected (ESCs powered directly from battery)

### WS2812B LED Strip (~75 LEDs, ~50 inches)

| Strip Wire | Wire To                         |
|------------|---------------------------------|
| DIN        | GP8 (Pin 11)                    |
| +5V        | Step-down 5V output (direct)    |
| GND        | Common GND (shared with Pico)   |

> ⚠️ Do NOT power the strip through the Pico — wire 5V directly from the LM2596S step-down output. Brightness capped at 50% in software to stay within the step-down's 3A limit.

### Noctua NF-A4x10 5V PWM Fans

| Fan Wire       | Fan 1 → Pico      | Fan 2 → Pico      |
|----------------|--------------------|--------------------|
| +5V (yellow)   | VSYS (Pin 39)      | VSYS (Pin 39)      |
| GND (black)    | GND                | GND                |
| PWM (blue)     | GP14 (Pin 19)      | GP15 (Pin 20)      |
| Tach (green)   | GP5 (Pin 7)        | GP7 (Pin 10)       |

> Both fans on PWM Slice 7 (25kHz) — independent duty cycles, shared frequency. Tach outputs ~2 pulses per revolution.

### DHT11 Temperature/Humidity Sensor

| Module Pin | Pico Pin          |
|------------|-------------------|
| + (VCC)    | 3V3 (Pin 36)      |
| out (DATA) | GP4 (Pin 6)       |
| - (GND)    | GND               |

### SD Card Module Wiring

| Module Pin | Pico Pin          |
|------------|-------------------|
| 3V3        | 3V3 (Pin 36)      |
| CS         | GP17 (Pin 22)     |
| MOSI       | GP19 (Pin 25)     |
| CLK        | GP18 (Pin 24)     |
| MISO       | GP16 (Pin 21)     |
| GND        | GND  (Pin 38)     |

### Power

- 3S LiPo (11.1V nominal) powers ESCs directly
- Step-down converter takes battery voltage → 5V into VSYS (pin 39)
- Pico's onboard regulator converts 5V → 3.3V for logic and peripherals

## Connecting Your Pico

To put your Pico in bootloader mode (needed for flashing):

1. Hold the BOOTSEL button
2. Connect USB while holding the button
3. Release the button after connecting

The flash scripts will automatically detect and flash your Pico.