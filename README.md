![set_warning](https://github.com/user-attachments/assets/cf7f0b35-19b9-46b3-9c35-6e1de6c87755)# Gas Detecting Telegram Bot

An ESP32-based smart gas detecting system that continuously monitors air quality using an MQ2 sensor. The system provides local visual/audio feedback through an LCD, LEDs, and a buzzer, and connects to WiFi to send real-time alerts and receive remote commands via a Telegram Bot.

Originally built in the Arduino IDE, this project has been refactored into a highly modular C++ structure using **PlatformIO**. It uses FreeRTOS tasks to ensure smooth multitasking between reading sensors, updating the UI, and communicating over the network.

## Features

- **Real-time Monitoring:** Continuously reads gas concentration (PPM) using an MQ2 sensor.
- **Local UI & Alerts:** Displays status on a 16x2 I2C LCD. Uses Green, Yellow, and Red LEDs alongside a buzzer for different warning levels.
- **Telegram Bot Integration:**
  - Receives automatic warnings if gas levels exceed thresholds.
  - Remote commands supported: `/status_gas`, `/set_warn <ppm>`, `/set_crit <ppm>`, `/start_gas`.
- **Interactive Menu:** Physical buttons to navigate screens, set thresholds locally, and trigger sensor calibration.
- **Non-volatile Storage:** Saves user-defined thresholds and calibration data (R0) to flash memory using the ESP32 `Preferences` library.
- **FreeRTOS Multitasking:** Separates the heavy Telegram network polling (Core 0) from the main UI and sensor reading loop (Core 1).

## Hardware Requirements

- **Microcontroller:** ESP32 Development Board
- **Sensor:** MQ2 Gas Sensor
- **Display:** 16x2 LCD with I2C module
- **Outputs:** 3x LEDs (Green, Yellow, Red), 1x Active Buzzer
- **Inputs:** 4x Push Buttons (Menu, Up, Down, Save)

## Project Structure

The codebase is strictly organized following the Single Responsibility Principle:

```text
src/ (and include/)
├── main.cpp          # System initialization and main loop
├── config.h          # Hardware pin definitions, constants, and global variables
├── mq2_sensor        # MQ2 gas calculations, smoothing, and auto-calibration logic
├── fsm_buttons       # State machine for the LCD screens and button debounce/handling
├── ui_display        # LCD rendering logic for different modes (Monitor, Settings, Calibrate)
├── outputs           # Control logic for LEDs and Buzzer
└── telegram_task     # FreeRTOS task handling WiFi connection, automated alerts, and Bot commands
```

## Setup and Installation

1. **Install PlatformIO:** Open the project folder in VSCode with the PlatformIO extension installed.
2. **Update Credentials:**
   Open `src/telegram_task.cpp` (or wherever you defined them) and update your WiFi and Telegram Bot credentials:
   ```cpp
   const char* WIFI_SSID     = "Your_WiFi_SSID";
   const char* WIFI_PASSWORD = "Your_WiFi_Password";
   const char* GAS_BOT_TOKEN = "Your_Bot_Token";
   const char* GAS_CHAT_ID   = "Your_Chat_ID";
   ```
3. **Build and Upload:** Click the PlatformIO **Build** and **Upload** buttons to flash the code to your ESP32.

## Demo

### 1. Safe State

![safe](https://github.com/user-attachments/assets/0acab6e3-5247-4247-add8-12de3581b571)

### 2. Warning State

![warning](https://github.com/user-attachments/assets/ed653840-3329-4768-9453-be823873354f)

### 3. Critical State

![critical](https://github.com/user-attachments/assets/b3a7e81d-0c9d-41be-9997-49bcb0ec46cc)

### 4. Calibrate State

![calibrate](https://github.com/user-attachments/assets/5c067943-8dc2-47ee-8495-6ab1f7535599)

### 5. Done Calibrate

![calibrate_done](https://github.com/user-attachments/assets/7d7e204e-214c-41ad-9edc-93cd093d1722)

### 6. Set Warning Threshold

![set_warning](https://github.com/user-attachments/assets/aa570301-bca7-4984-ad9b-b553489ffeb0)

### 7. Set Critical Threshold

![set_critical](https://github.com/user-attachments/assets/d5f93bf9-73c6-421a-875b-39489a66e285)

