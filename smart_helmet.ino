/*
  SMART HELMET FOR VISUALLY IMPAIRED PEOPLE
  Board: ESP32 DevKit V1

  Prototype hardware:
    - 3 x HC-SR04 ultrasonic sensors (front, left, right)
    - 1 x vibration motor (through transistor/MOSFET driver)
    - 1 x active buzzer (through transistor driver if required)

  Behaviour:
    - Measures distance in three directions.
    - Gives a slow alert for medium-range obstacles.
    - Gives a faster/stronger alert for close obstacles.
    - The closest detected obstacle determines the alert pattern.

  SAFETY:
  This is an educational assistive prototype, NOT a certified mobility aid.
  It must not replace a white cane, guide dog, training or safe judgement.
  Test in a controlled environment with a sighted assistant. Ultrasonic
  sensors can miss soft, narrow, angled or low/high obstacles and are
  affected by weather, surfaces and sensor placement.
*/

#include <Arduino.h>

// ---------- ESP32 pin assignment ----------
const int FRONT_TRIG = 5;
const int FRONT_ECHO = 18;

const int LEFT_TRIG  = 19;
const int LEFT_ECHO  = 21;

const int RIGHT_TRIG = 22;
const int RIGHT_ECHO = 23;

const int BUZZER_PIN = 25;
const int VIBRATION_PIN = 26;

// ---------- Distance thresholds (centimetres) ----------
const float NEAR_DISTANCE_CM = 50.0;
const float MEDIUM_DISTANCE_CM = 120.0;
const float MAX_VALID_DISTANCE_CM = 350.0;

// Sensor timing
const unsigned long ECHO_TIMEOUT_US = 25000UL;
const unsigned long SENSOR_INTERVAL_MS = 80;
const unsigned long ALERT_INTERVAL_MS = 100;

// Change this if your buzzer module is active-low.
const bool BUZZER_ACTIVE_HIGH = true;

// Use transistor/MOSFET driver for the vibration motor.
// Do not connect a motor directly to an ESP32 GPIO.
const bool MOTOR_ACTIVE_HIGH = true;

struct SensorReading {
  const char* name;
  int trigPin;
  int echoPin;
  float distanceCm;
};

SensorReading frontSensor = {"FRONT", FRONT_TRIG, FRONT_ECHO, -1};
SensorReading leftSensor  = {"LEFT", LEFT_TRIG, LEFT_ECHO, -1};
SensorReading rightSensor = {"RIGHT", RIGHT_TRIG, RIGHT_ECHO, -1};

unsigned long lastSensorRead = 0;
unsigned long lastAlertUpdate = 0;
bool alertState = false;

void setBuzzer(bool on) {
  digitalWrite(BUZZER_PIN,
    (on == BUZZER_ACTIVE_HIGH) ? HIGH : LOW);
}

void setVibration(bool on) {
  digitalWrite(VIBRATION_PIN,
    (on == MOTOR_ACTIVE_HIGH) ? HIGH : LOW);
}

float measureDistanceCm(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(3);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long duration = pulseIn(echoPin, HIGH, ECHO_TIMEOUT_US);

  if (duration == 0) {
    return -1.0; // No echo / out of range
  }

  // Speed of sound approximation: distance = time * 0.0343 / 2
  float distance = (duration * 0.0343f) / 2.0f;

  if (distance < 2.0f || distance > MAX_VALID_DISTANCE_CM) {
    return -1.0;
  }

  return distance;
}

void readSensor(SensorReading &sensor) {
  sensor.distanceCm = measureDistanceCm(sensor.trigPin, sensor.echoPin);
}

float closestValidDistance() {
  float closest = -1.0;
  SensorReading* sensors[] = {&frontSensor, &leftSensor, &rightSensor};

  for (SensorReading* sensor : sensors) {
    if (sensor->distanceCm > 0 &&
        (closest < 0 || sensor->distanceCm < closest)) {
      closest = sensor->distanceCm;
    }
  }
  return closest;
}

const char* closestDirection(float closest) {
  if (closest < 0) return "NONE";

  SensorReading* sensors[] = {&frontSensor, &leftSensor, &rightSensor};
  for (SensorReading* sensor : sensors) {
    if (sensor->distanceCm > 0 &&
        fabs(sensor->distanceCm - closest) < 0.1f) {
      return sensor->name;
    }
  }
  return "UNKNOWN";
}

void printReadings(float closest) {
  Serial.println("\n--- SMART HELMET ---");

  Serial.print("Front: ");
  if (frontSensor.distanceCm < 0) Serial.println("No echo");
  else { Serial.print(frontSensor.distanceCm, 1); Serial.println(" cm"); }

  Serial.print("Left: ");
  if (leftSensor.distanceCm < 0) Serial.println("No echo");
  else { Serial.print(leftSensor.distanceCm, 1); Serial.println(" cm"); }

  Serial.print("Right: ");
  if (rightSensor.distanceCm < 0) Serial.println("No echo");
  else { Serial.print(rightSensor.distanceCm, 1); Serial.println(" cm"); }

  if (closest < 0) {
    Serial.println("Status: No obstacle detected within sensor range");
  } else {
    Serial.print("Closest obstacle: ");
    Serial.print(closest, 1);
    Serial.print(" cm | Direction: ");
    Serial.println(closestDirection(closest));
  }
}

void updateAlerts(float closest) {
  if (closest < 0 || closest > MEDIUM_DISTANCE_CM) {
    setBuzzer(false);
    setVibration(false);
    return;
  }

  // Near obstacle: continuous alert
  if (closest <= NEAR_DISTANCE_CM) {
    setBuzzer(true);
    setVibration(true);
    return;
  }

  // Medium range: intermittent alert. Closer objects cause faster pulses.
  unsigned long interval = map(
    (long)closest,
    (long)NEAR_DISTANCE_CM,
    (long)MEDIUM_DISTANCE_CM,
    150,
    650
  );
  interval = constrain(interval, 150UL, 650UL);

  if (millis() - lastAlertUpdate >= interval) {
    lastAlertUpdate = millis();
    alertState = !alertState;
    setBuzzer(alertState);
    setVibration(alertState);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(FRONT_TRIG, OUTPUT);
  pinMode(FRONT_ECHO, INPUT);
  pinMode(LEFT_TRIG, OUTPUT);
  pinMode(LEFT_ECHO, INPUT);
  pinMode(RIGHT_TRIG, OUTPUT);
  pinMode(RIGHT_ECHO, INPUT);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(VIBRATION_PIN, OUTPUT);

  digitalWrite(FRONT_TRIG, LOW);
  digitalWrite(LEFT_TRIG, LOW);
  digitalWrite(RIGHT_TRIG, LOW);
  setBuzzer(false);
  setVibration(false);

  Serial.println("Smart Helmet for Visually Impaired People");
  Serial.println("System started. Checking obstacles...");
}

void loop() {
  if (millis() - lastSensorRead >= SENSOR_INTERVAL_MS) {
    lastSensorRead = millis();

    // Read sensors sequentially to reduce ultrasonic cross-talk.
    readSensor(frontSensor);
    delay(30);
    readSensor(leftSensor);
    delay(30);
    readSensor(rightSensor);

    float closest = closestValidDistance();
    printReadings(closest);
    updateAlerts(closest);
  }
}
