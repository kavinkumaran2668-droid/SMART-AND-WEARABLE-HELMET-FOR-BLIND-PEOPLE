# Wiring Guide — Smart Helmet

## ESP32 pin map

| Function | ESP32 GPIO |
|---|---:|
| Front ultrasonic TRIG | 5 |
| Front ultrasonic ECHO | 18 (through divider) |
| Left ultrasonic TRIG | 19 |
| Left ultrasonic ECHO | 21 (through divider) |
| Right ultrasonic TRIG | 22 |
| Right ultrasonic ECHO | 23 (through divider) |
| Buzzer control | 25 |
| Vibration motor driver input | 26 |

## HC-SR04 connections (repeat for all 3 sensors)

```text
HC-SR04 VCC  -> 5V
HC-SR04 GND  -> GND
HC-SR04 TRIG -> assigned ESP32 TRIG GPIO
HC-SR04 ECHO -> voltage divider -> assigned ESP32 ECHO GPIO
```

**Never connect HC-SR04 ECHO directly to an ESP32 input:** it may output 5V. ESP32 GPIOs are not 5V tolerant.

## Vibration motor

Use a transistor/MOSFET driver and a suitable external motor supply. Connect the driver input to GPIO26. Connect the external supply ground to ESP32 GND. Do not drive the motor directly from GPIO26.

## Buzzer

Connect a compatible buzzer through a driver if its current exceeds the GPIO rating. Control input is GPIO25.

Check all module voltage/current specifications before powering the circuit.
