# ⚡ AuraLight — ESP32 Smart Lighting Automation System

A modern, local IoT lighting control system built for home automation using an **ESP32** microcontroller, a **5V Relay Module**, and an embedded web server featuring a creative glassmorphism user interface.

## ✨ Features
* **Wireless Web Dashboard:** Control your lights and adjust timers directly from any smartphone, tablet, or PC connected to your local Wi-Fi.
* **Modern UI:** Built with HTML5, CSS3 (Glassmorphism design), and Vanilla JavaScript featuring smooth visual status rings and ambient lighting effects.
* **Flexible Timer Control:** Set automatic shut-off delays up to 5 minutes (300 seconds) via an interactive slider.
* **Local Hosting:** Fully self-hosted locally on the ESP32 with zero dependency on external cloud servers, ensuring fast response times and complete privacy.

---

## 🛠️ Hardware Requirements
* ESP32 NodeMCU / DevKit Board
* 5V 1-Channel Relay Module
* AC/DC Lamp or LED Load
* Jumper wires & Breadboard
* External power source / USB cable

---

## 🔌 Pin Configuration

| Component Pin | ESP32 Pin | Description |
| :--- | :--- | :--- |
| **Relay VCC** | `VIN` or `5V` | Power supply |
| **Relay GND** | `GND` | Common Ground |
| **Relay IN** | `GPIO 23` | Control Signal Pin |

> ⚠️ **High Voltage Safety Warning:** Exercise extreme caution when working with AC mains voltage (230V/110V). Ensure power is disconnected before wiring the relay switch to your lighting circuit.

---

## 📂 Project Structure
```text
/esp32-smart-lighting-system
├── webapp/
│   ├── index.html     # Frontend UI structure
│   ├── style.css      # Glassmorphism styling and animations
│   └── script.js      # Asynchronous fetch logic and polling
├── firmware/
│   └── main.ino       # ESP32 Web Server & Relay Controller firmware
└── README.md
