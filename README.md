# Smart Helmet for Visually Impaired People

An ESP32-based educational prototype designed to detect nearby obstacles in three directions and alert the wearer using a buzzer and vibration motor.

## Features

- ESP32 microcontroller
- Three ultrasonic sensors for front, left and right obstacle detection
- Vibration motor alert
- Buzzer alert
- Alert pattern changes according to obstacle distance
- Serial Monitor displays measured distances and closest direction

## Hardware required

| Component | Quantity |
|---|---:|
| ESP32 DevKit V1 | 1 |
| HC-SR04 ultrasonic sensor | 3 |
| Vibration motor | 1 |
| Active buzzer | 1 |
| NPN transistor or logic-level MOSFET driver for motor | 1 |
| Flyback diode (for a brushed DC motor) | 1 |
| Resistors for driver and echo dividers | As required |
| Breadboard / PCB and jumper wires | As required |
| Suitable regulated power supply | 1 |
| Helmet or demonstration frame | 1 |

## Wiring — ESP32 DevKit V1

### Ultrasonic sensors

| Sensor | TRIG | ECHO |
|---|---:|---:|
| Front HC-SR04 | GPIO 5 | GPIO 18 |
| Left HC-SR04 | GPIO 19 | GPIO 21 |
| Right HC-SR04 | GPIO 22 | GPIO 23 |

For each HC-SR04:
- VCC → 5V (check your module specification)
- GND → ESP32 GND
- TRIG → listed ESP32 GPIO
- ECHO → listed ESP32 GPIO through a voltage divider / level shifter

**Critical:** HC-SR04 ECHO is commonly 5V. ESP32 GPIOs are 3.3V-only and are not 5V tolerant. Use a suitable divider on every ECHO signal. A typical divider can use 1kΩ from ECHO to the ESP32 input and 2kΩ from the input to GND, but verify the resulting voltage and your exact module.

### Outputs

| Component | ESP32 connection |
|---|---|
| Buzzer control | GPIO 25 through suitable driver if required |
| Vibration motor driver input | GPIO 26 |
| Motor supply | Separate suitable supply |
| Common ground | Connect supply GND to ESP32 GND |

Do not power a vibration motor directly from an ESP32 GPIO. Use a transistor or MOSFET driver. For a brushed DC motor, add a flyback diode across the motor (correct polarity). Check the buzzer's voltage/current and use a driver if it exceeds GPIO capability.

## Arduino IDE setup

1. Install Arduino IDE.
2. Install ESP32 board support by Espressif Systems using Boards Manager.
3. Open `smart_helmet.ino`.
4. Select **ESP32 Dev Module** and the correct COM port.
5. Upload the sketch.
6. Open Serial Monitor at **115200 baud**.

This sketch uses only the built-in Arduino/ESP32 functions; no extra Arduino libraries are required.

## How it works

1. The ESP32 reads the front, left and right ultrasonic sensors one after another.
2. Each sensor measures the distance to a nearby obstacle.
3. The program finds the closest valid distance.
4. If no obstacle is detected within the configured range, the buzzer and vibration motor remain off.
5. At medium distance (50–120 cm), the buzzer and vibration motor pulse intermittently. The pulses become faster as the obstacle gets closer.
6. At 50 cm or less, the buzzer and vibration motor remain on continuously.

## Adjusting detection distances

Open `smart_helmet.ino` and change:

```cpp
const float NEAR_DISTANCE_CM = 50.0;
const float MEDIUM_DISTANCE_CM = 120.0;
const float MAX_VALID_DISTANCE_CM = 350.0;
```

These are demonstration values. Test and adjust for the sensor model, mounting angle, environment and intended use.

## Repository structure

```text
smart-helmet-for-visually-impaired/
├── smart_helmet.ino
├── README.md
├── wiring.md
├── LICENSE
└── .gitignore
```

## Safety and limitations

This is a student prototype, not a certified mobility aid or medical device. It must not replace a white cane, guide dog, orientation-and-mobility training or other established assistance. Ultrasonic sensors can fail to detect soft, narrow, angled, absorbent or low/high obstacles; they can also produce false readings and have blind spots. The prototype does not identify stairs, drop-offs, moving traffic or safe crossing opportunities.

Test only in a controlled area with a sighted assistant. Do not rely on it near roads, stairs, platforms or other hazards. A real assistive product needs extensive human-factors testing, redundant sensing, weather protection, fault alerts, battery monitoring and professional safety evaluation.

## Future improvements

- Add downward-facing sensors for step/drop-off research (with careful validation).
- Add GPS and emergency location sharing, with user consent.
- Add battery-level monitoring and low-battery warning.
- Add waterproof sensor housings and robust mounting.
- Add haptic patterns that distinguish front, left and right directions.
- Conduct supervised user testing and accessibility evaluation.
