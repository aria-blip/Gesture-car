# Kinetic Drive: ESP32 Gesture Controlled Car

A robust, low-latency wireless control system for a robotic chassis using hand gestures. Powered by ESP32 microcontrollers and the ESP-NOW communication protocol.

## Features
-Low Latency: Uses ESP-NOW (connectionless Wi-Fi) for immediate response times (<10ms).
-Intuitive Control: 3-Axis MEMS Accelerometer (MPU6050) translates hand tilt into motor vectors.
-Failsafe Protocol: Receiver automatically halts motors if signal is lost for >500ms (Safety Stop).
* **Modular Design:** Separated logic for sensor acquisition and motor actuation.

## 🛠 Hardware Components
* **Microcontrollers:** 2x ESP32 Development Boards (WROOM-32)
* **Sensor:** MPU6050 (Accelerometer/Gyroscope)
* **Driver:** L293D or L298N H-Bridge Motor Driver
* **Actuators:** 2x DC Gear Motors + Wheels
* **Power:** 2x 18650 Li-Ion Batteries (Series) or 9V Block

##  Pin Configuration
![Transmitter Circuit Diagram](./assets/transmitter_wiring.png)
### Transmitter (Hand Unit)
| MPU6050 | ESP32 (GPIO) |
| :--- | :--- |
| VCC | 3.3V |
| GND | GND |
| SDA | 21 |
| SCL | 22 |

![Receiver Circuit Diagram](./assets/receiver_wiring.jpg)
### Receiver (Car Unit)
| L298N Driver | ESP32 (GPIO) |
| :--- | :--- |
| IN1 (Motor A) | 32 |
| IN2 (Motor A) | 33 |
| IN3 (Motor B) | 25 |
| IN4 (Motor B) | 26 |

## ⚙️ Installation

1.  **Dependencies:** Install the following libraries in Arduino IDE:
    * `MPU6050_light` by rfetick
    * `ESP32` Board Manager files

2.  **MAC Address Setup:**
    * Upload `MacAddressFinder.ino` to the Receiver ESP32.
    * Copy the MAC Address from the Serial Monitor.
    * Paste it into `Transmitter.ino` line 12: `uint8_t broadcastAddress[] = { ... };`

3.  **Flash:**
    * Upload `Transmitter.ino` to the Hand Unit.
    * Upload `Receiver.ino` to the Car Unit.


MIT License 
