# AGENTS.md - Watchino Smartwatch OS Project Rules

## Project Context & Architecture
This repository contains a lightweight, custom Smartwatch Operating System built on the Arduino framework using the **pioarduino** IDE extension in VS Code. The system behaves as an embedded firmware loop optimizing for power-efficiency, instant wake-up, and real-time sensor polling. 

### Key Design Paradigm
- **Language Preference**: Strict preference for **Pure C** over C++ abstractions wherever possible. Avoid complex OOP design patterns, inheritance, or excessive dynamic memory allocations (`malloc`/`free`) outside of library requirements to prevent heap fragmentation.
- **Hardware Integration**: Prioritize handling direct hardware drivers, register maps, and lightweight state machines.

---

## Hardware Profile & Device Specs
Target Hardware: **Waveshare ESP32-C6-Touch-AMOLED-2.06 Dev Kit**
- **MCU**: ESP32-C6 (32-bit RISC-V Single-core @ 160MHz)
- **Power Management**: AXP2101 PMIC (handles deep sleep/wake loops)
- **6-Axis IMU**: QMI8658 (3-axis accelerometer, 3-axis gyroscope)

---

## Hardware Pin Configurations & Geometry
Always use the exact definitions matching `pin_config.h`. Never redefine these pins or guess GPIO numbers.

### Power Management (PMIC)
- **Chip Type**: `XPOWERS_CHIP_AXP2101`

### Display Architecture (QSPI Interface)
- **Display Dimensions**: Width: **410 px**, Height: **502 px**
- **LCD_SDIO0**: GPIO 1
- **LCD_SDIO1**: GPIO 2
- **LCD_SDIO2**: GPIO 3
- **LCD_SDIO3**: GPIO 4
- **LCD_SCLK**: GPIO 0
- **LCD_CS**: GPIO 5
- **LCD_RESET**: GPIO 11

### I2C / Touch Architecture (FT3168 Controller)
- **IIC_SDA**: GPIO 8
- **IIC_SCL**: GPIO 7
- **TP_INT**: GPIO 15 (Touch Interrupt line for wake routines)
- **TP_RESET**: GPIO 10

---

## Core Software Dependencies
1. **UI Layer**: LVGL v9.5 (Light and Versatile Graphics Library)
   - Configuration must be constrained to the 410×502 display boundary.
   - Use direct buffer flushing optimized for QSPI transfers using the specified `LCD_SDIO` data pins.
2. **Companion Protocol**: Bangle.js Bluetooth LE Emulation
   - Emulates Bangle.js Bluetooth protocol behaviors to bridge data seamlessly to the Android **Gadgetbridge** app.
   - Responsible for fetching incoming notifications, syncing time/weather data, and pushing step count metadata.

---

## Technical Constraints & Guardrails

### 1. Pure C vs C++
- Write all custom business logic, UI wrapper screens, step counter algorithms, and state machines in **pure C** strings and structures. 
- Use the minimal C++ required by the Arduino Core setup structure (`void setup()` and `void loop()`), but keep your modular code strictly inside `.c` files or wrapped properly with `extern "C"` if exposing to C++.

### 2. Touch Screen & Wake Loop
- The `TP_INT` (GPIO 15) touch controller line acts as an external interrupt to wake the ESP32-C6 from light/deep sleep modes.
- Do not add blocking logic inside touch handler routines. Touch inputs must feed cleanly into LVGL 9.5's input device pointer system (`lv_indev_t`).

### 3. Step Tracking & IMU
- Continuously poll or configure interrupt thresholds on the QMI8658 6-axis IMU over the shared `IIC_SDA`/`IIC_SCL` bus.
- Keep the step counting algorithm mathematically lean to fit the RISC-V processor cycle budgets. Store total tracking metrics inside a persistent struct to be relayed over the Bangle.js BLE emulator interface.

### 4. Gadgetbridge BLE Emulation
- Emulated Bangle.js JSON packets must strictly adhere to the expected Gadgetbridge string protocols over BLE UART.
- Keep parsing buffers flat, statically sized, and small to respect the ESP32-C6's memory limits. Avoid unbounded string operations (`sprintf`, `strcat`); use bounds-checked safe variants (`snprintf`, `strncpy`).

---

## Development & Build Environment
- **IDE Extension**: `pioarduino` (PlatformIO Fork optimized for ESP32/Arduino Ecosystem)
- **Primary Commands**:
  - Build firmware: Use PlatformIO build panel or `pio run`
  - Upload target: `pio run --target upload`
  - Monitor logs: `pio device monitor`
