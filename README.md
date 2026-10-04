# 🚤 Smart Floating Plastic Waste Separator & Flood Warning System

<div align="center">

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-ESP32--CAM-blue.svg)](https://www.espressif.com/)
[![Demo](https://img.shields.io/badge/🎮%20Live%20Demo-Three.js%203D-brightgreen.svg)](https://joelbinoy2021-sketch.github.io/Smart-Floating-Plastic-Separator/)
[![Arduino](https://img.shields.io/badge/Firmware-Arduino%20IDE-teal.svg)](firmware/)
[![Hardware](https://img.shields.io/badge/Hardware-Open%20Source-orange.svg)]()

> **An autonomous solar-powered floating robot that collects plastic waste from rivers and canals, with an integrated real-time flood-level warning subsystem.**

[🎮 Live 3D Demo](#-live-3d-interactive-demo) · [🔩 Hardware](#-hardware-components) · [🧠 AI Vision](#-how-ai-vision-works) · [🚀 Setup](#-getting-started) · [📐 Architecture](#-system-architecture)

</div>

---

## 🌊 What It Does

This is a **student hardware innovation project** — a self-navigating catamaran-style pontoon robot designed for deployment in rivers, canals, and drainage waterways. It autonomously:

| Feature | Description |
|---|---|
| 🗑️ **Plastic Collection** | Scoops floating PET bottles & plastic wrappers using a mesh conveyor belt |
| 🧠 **AI-Powered Sorting** | ESP32-CAM identifies plastic vs organic leaves vs fish in real time |
| 🐟 **Fish Safe Mode** | Safe-zone ring detection — swerves away when fish are nearby |
| ☀️ **Auto Dock & Charge** | Returns to solar charging dock automatically when battery < 20% or bin full |
| 🌊 **Flood Warning** | HC-SR04 ultrasonic sensor monitors canal water depth and sends early alerts |

---

## 🎮 Live 3D Interactive Demo

👉 **[View the Photorealistic 3D Simulation →](https://joelbinoy2021-sketch.github.io/Smart-Floating-Plastic-Separator/)**

Built with **Three.js r128 WebGL** — runs in any modern browser, no install needed.

| Feature | Detail |
|---|---|
| 🌊 Animated Water | 96×96 vertex sine-wave water mesh updated per frame |
| 🚤 Full 3D Vessel | PBR materials — yellow HDPE pontoons, acrylic glass lid, spinning propellers |
| ⚡ Docking Cycle | Watch the robot auto-navigate to dock, laser charge, bin clear, then resume |
| 🔍 Exploded View | Slider separates the hull to reveal ESP32 PCB + LiFePO4 battery inside |
| 🚁 Camera Presets | Top View, Chase Cam, Front Intake, Inside Chassis, Dock Station |
| 📊 Live Telemetry | Real-time battery %, bin capacity %, and state machine status |

> **To run locally:** Clone the repo and open `index.html` in Chrome / Edge / Firefox.

---

## 🔩 Hardware Components

### Main Electronics

| Component | Model / Spec | Role |
|---|---|---|
| **Microcontroller** | ESP32-CAM (AI Thinker) | Main brain — WiFi, OV2640 camera, all motor I/O |
| **Motor Driver** | L298N / 30A ESC | Controls brushless thrusters |
| **Servo** | MG996R (Metal Gear) | Sorting gate — swing to plastic bin or eject organic |
| **Battery** | 12.8V 10Ah LiFePO4 | Main power — ~6–8h runtime |
| **BMS** | 4S LiFePO4 BMS 20A | Overcharge & deep-discharge protection |
| **Solar Panel** | 20W Monocrystalline | Mounted on top lid — charges at dock |
| **Flood Sensor** | HC-SR04 Ultrasonic | Waterproofed — measures canal water depth |
| **Bin Sensor** | IR Reflective Sensor | Detects when 12L waste bin is full |

### Hull & Mechanical

| Component | Spec | Role |
|---|---|---|
| **Pontoons** | HDPE 110mm dia × 600mm | Twin cylindrical floats — high buoyancy |
| **Frame** | 20×20mm Aluminum T-slot | Structural crossbar chassis |
| **Thrusters** | IP68 Brushless (×2) | Rear underwater propulsion & steering |
| **Conveyor Belt** | Mesh nylon on 3D-printed rollers | Angled scoop — lifts plastic from water surface |
| **Chassis Box** | IP67 ABS enclosure | Waterproof housing for all electronics |
| **Storage Bin** | 12L HDPE container | Collects scooped plastic waste |
| **Dock Platform** | Canal bank fixed mount | Solar panel + magnetic charging contacts |

---

## 🧠 How AI Vision Works

The **ESP32-CAM module** streams video from the OV2640 camera and runs object classification to decide what action to take:

```
Camera Frame (320×240)
        │
        ▼
  ┌─────────────────────────────────────────┐
  │         Object Classifier               │
  │                                         │
  │  PET Bottle / Plastic   → COLLECT ✅   │
  │  Organic Leaf / Debris  → REJECT  ❌   │
  │  Swimming Fish          → AVOID   🐟   │
  └─────────────────────────────────────────┘
        │              │              │
        ▼              ▼              ▼
  Steer toward    Servo gate     Swerve right
  target &        ejects item    (safe zone
  run conveyor    into canal     detection ring)
```

### Detection Steps
1. **Frame capture** — ESP32-CAM takes a JPEG frame at ~5 FPS
2. **Resize** — Frame downscaled to 96×96 pixels for model input
3. **Inference** — TensorFlow Lite Micro model runs fully on-device (no cloud needed)
4. **Decision** — Output scores compared against thresholds:
   - Plastic confidence ≥ **75%** → COLLECT
   - Fish confidence ≥ **70%** → SWERVE AWAY
   - Everything else → REJECT (servo gate ejects it)

> **Model:** Transfer learning on MobileNetV2 (quantized INT8 for ESP32), trained on a custom dataset of river plastic, organic debris, and fish images.

---

## 📐 System Architecture

```
┌───────────────────────────────────────────────────────────────┐
│                      ESP32-CAM (Brain)                        │
│                                                               │
│  ┌──────────────┐   ┌─────────────┐   ┌───────────────────┐  │
│  │  OV2640 Cam  │   │  WiFi/MQTT  │   │   Motor Driver    │  │
│  │  TFLite AI   │   │  Telemetry  │   │   L298N / ESC     │  │
│  └──────┬───────┘   └─────────────┘   └────────┬──────────┘  │
│         │ Object Classification                 │             │
│         ▼                                       ▼             │
│  ┌──────────────┐                   ┌───────────────────────┐ │
│  │ Plastic?     │──── COLLECT ────► │  Thrusters × 2        │ │
│  │ Leaf?        │──── REJECT  ────► │  + Conveyor Motor     │ │
│  │ Fish?        │──── SWERVE  ────► │  + Servo Sorting Gate │ │
│  └──────────────┘                   └───────────────────────┘ │
└───────────────────────────────────────────────────────────────┘
                              │
              ┌───────────────┴────────────────┐
              ▼                                ▼
  Battery < 20% or Bin Full          Water Depth > 1.8m
              │                                │
              ▼                                ▼
   Navigate to Solar Dock          FLOOD WARNING ALERT
   Magnetic contacts align          (WiFi push notification)
   LiFePO4 charges via BMS
   Bin cleared → Resume patrol
```

### Autonomous State Machine

```
  ┌─────────┐     plastic     ┌────────────────┐
  │  PATROL │─────detected───►│ APPROACH_TRASH │
  └────┬────┘                 └───────┬────────┘
       │                              │ in range
  batt <20%                           ▼
  or bin full              conveyor scoops bottle
       │                              │
       ▼                         back to PATROL
  ┌─────────┐
  │ DOCKING │
  └────┬────┘
       │ dock reached
       ▼
  ┌──────────────────┐
  │  DOCKED_CHARGING │
  │  batt charges    │
  │  bin drained     │
  └────────┬─────────┘
           │ fully charged & bin empty
           └──────────► PATROL resumes
```

---

## 🗂️ Repository Structure

```
smart-floating-separator/
│
├── index.html                   # 🎮 Three.js 3D photorealistic demo
│
├── firmware/
│   └── main/
│       ├── main.ino             # Main state machine & loop
│       ├── motor_control.h      # Thruster ESC, servo gate, conveyor
│       ├── vision.h             # ESP32-CAM init & TFLite inference
│       └── flood_sensor.h       # HC-SR04 water depth measurement
│
├── hardware/
│   ├── bom.csv                  # Bill of Materials (parts + prices)
│   ├── wiring_diagram.pdf       # Full circuit schematic
│   └── cad/                     # 3D printable STL parts
│
├── docs/
│   └── images/                  # Project photos & diagrams
│
├── .gitignore
├── LICENSE
└── README.md
```

---

## 🚀 Getting Started

### Prerequisites

- **Arduino IDE 2.x** — [Download here](https://www.arduino.cc/en/software)
- **ESP32 Board Package** — In Arduino IDE go to *File → Preferences → Additional Board Manager URLs*, add:
  ```
  https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
  ```
- **Required Libraries** (install via *Sketch → Include Library → Manage Libraries*):

  | Library | Install Name |
  |---|---|
  | ESP32 Servo | `ESP32Servo` |
  | NewPing (ultrasonic) | `NewPing` |
  | MQTT Client | `ArduinoMQTTClient` |

### Flash the Firmware

```bash
# 1. Clone the repo
git clone https://github.com/joelbinoy2021-sketch/Smart-Floating-Plastic-Separator.git
cd Smart-Floating-Plastic-Separator

# 2. Open in Arduino IDE:
#    File → Open → firmware/main/main.ino

# 3. Board settings:
#    Tools → Board        → ESP32 Arduino → ESP32 Dev Module
#    Tools → Port         → (select your COM port)
#    Tools → Upload Speed → 115200

# 4. Click ▶ Upload
```

### Run the 3D Demo Locally

```bash
# Just open index.html in any browser — no build tools needed

# Windows
start index.html

# macOS / Linux
open index.html
```

### Deploy to GitHub Pages (Free Hosting)

1. Push your code to GitHub
2. Go to **Settings → Pages**
3. Source: **Deploy from a branch** → branch `main`, folder `/root`
4. Click **Save** — live in ~60 seconds at:
   ```
   https://joelbinoy2021-sketch.github.io/Smart-Floating-Plastic-Separator/
   ```
5. Your live demo is now online! ✅

---

## 📄 License

This project is licensed under the **MIT License** — see [LICENSE](LICENSE) for details.
Free to use, remix, and share — please give credit! ⭐

---

## 👨‍💻 Author

Built as a student hardware innovation project.
Questions or feedback? Open a [GitHub Issue](../../issues) — happy to help!

---

<div align="center">

**⭐ Star this repo if it helped or inspired you! ⭐**

*Made with 💛 to keep our rivers clean.*

</div>
