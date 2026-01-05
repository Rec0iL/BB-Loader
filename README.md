# 🎯 Airsoft BB Loader Pro

An automated, high-precision Airsoft BB loading machine built with recycled 3D printer hardware. This project uses a Creality V2.1 (ATmega2560) board to control a stepper-driven feeder with a custom Gradio web interface for configuration and operation.

## 🚀 Features

- **Precision Loading:** Exact BB count using stepper motor steps.
- **Dynamic Speed Ramping:** Smooth acceleration and deceleration to prevent motor stalling and BB jams.
- **Gradio Web UI:** Modern, easy-to-use interface for your PC.
- **EEPROM Persistence:** Settings like Steps/BB, Speed, and Direction are saved directly on the hardware.
- **Quick Presets:** One-click buttons for common magazine sizes (25, 30, 60, 100, 120, 150).
- **Live Progress:** Real-time feedback in the UI showing remaining BBs.

## 🛠 Hardware Requirements

- **Mainboard:** Creality V2.1 (from CR-10 / Ender 3) or any ATmega2560 based board.
- **Motor:** NEMA 17 Stepper motor (ideally with a gearbox/reduction).
- **Driver:** Integrated A4988 (on-board).
- **Power:** 12V or 24V DC power supply (depending on your board version).
- **Feeder:** 3D printed screw/auger feeder system.

## 🔌 Wiring (Creality V2.1)

Connect your stepper motor to the **X-Axis** port.

| Component | Port | Pins (Internal) |
| :--- | :--- | :--- |
| **Stepper Motor** | X-Axis Motor Port | STEP: 54, DIR: 55, EN: 38 |
| **PC Connection** | Mini-USB | Serial (115200 Baud) |

## 💻 Software Setup

### 1. Arduino Firmware
1. Open `bb_loader.ino` in the Arduino IDE.
2. Select **Board:** "Arduino Mega or Mega 2560".
3. Upload the code to your board via USB.

### 2. Gradio Configurator (Python)
Ensure you have Python 3.10+ installed.

Command to install dependencies:
pip install gradio pyserial

Run the application:
python app.py

The interface will open in your web browser (usually at http://127.0.0.1:7860).

## ⚙️ Configuration

1. **Connect:** Select your COM port and click "Connect".
2. **Steps per BB:** Calibrate this by running a test batch (Total Steps / BBs Loaded).
3. **Speed Ramp:** Increase "Ramp Steps" if your motor skips or grinds during startup.
4. **Save:** Click "Save to EEPROM" to make settings permanent on the board.

## 🎮 Usage

1. Select a **Preset** (e.g., 120 for a Midcap).
2. Use the **+/- buttons** for fine-tuning.
3. Click **🚀 START LOADING**.
4. Use **🛑 EMERGENCY STOP** if a jam occurs to immediately disable the motor.

---
*Note: This project is intended for hobbyist use. Ensure your feeder mechanism is clear of debris to prevent mechanical failure.*
TODO:
1. **Screen:** Add a screen
2. **Add Buttons n Stuff:** Add buttons to use the screen (maybe also a recycle 3D printer parts thing)
3. **Finalize 3D Design:** Finalize and test 3D Printed design.
4. **Release:** Release eveerything once working.
