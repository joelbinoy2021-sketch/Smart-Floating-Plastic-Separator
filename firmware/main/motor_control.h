/*
 * motor_control.h — Thruster, Conveyor & Servo Gate Control
 * Smart Floating Plastic Waste Separator
 */

#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <ESP32Servo.h>

// ──────────────────────────────────────────────
// SERVO & ESC OBJECTS
// ──────────────────────────────────────────────
Servo escLeft;
Servo escRight;
Servo servoGate;

// ESC PWM: 1000µs = stop, 1500µs = mid, 2000µs = full throttle
#define ESC_STOP       1000
#define ESC_MID        1500
#define ESC_MAX        2000

// Servo Gate Positions
#define GATE_PLASTIC   90    // Degrees — direct to bin
#define GATE_ORGANIC   30    // Degrees — eject back to canal

// ──────────────────────────────────────────────
// INIT
// ──────────────────────────────────────────────
void initMotors(int pinLeft, int pinRight, int pinServo) {
  escLeft.attach(pinLeft, ESC_STOP, ESC_MAX);
  escRight.attach(pinRight, ESC_STOP, ESC_MAX);
  servoGate.attach(pinServo);
  servoGate.write(GATE_ORGANIC);
  delay(500);
  Serial.println("[MOTORS] Initialized");
}

// ──────────────────────────────────────────────
// SPEED HELPER: pct (0–100) → ESC µs (1000–2000)
// ──────────────────────────────────────────────
int pctToEsc(int pct) {
  pct = constrain(pct, 0, 100);
  return map(pct, 0, 100, ESC_STOP, ESC_MAX);
}

// ──────────────────────────────────────────────
// MOVEMENT COMMANDS
// ──────────────────────────────────────────────
void motorStop() {
  escLeft.writeMicroseconds(ESC_STOP);
  escRight.writeMicroseconds(ESC_STOP);
}

void motorPatrol(int speedPct) {
  // Slow gentle sine-based patrol — left/right same speed
  int us = pctToEsc(speedPct);
  escLeft.writeMicroseconds(us);
  escRight.writeMicroseconds(us);
}

void motorApproachTarget(int speedPct) {
  int us = pctToEsc(speedPct);
  escLeft.writeMicroseconds(us);
  escRight.writeMicroseconds(us);
}

void motorSwerveRight(int speedPct) {
  // Turn right — reduce left thruster
  escLeft.writeMicroseconds(pctToEsc(speedPct));
  escRight.writeMicroseconds(pctToEsc(speedPct / 3));
}

void motorNavigateToDock(int speedPct) {
  // Hardcoded heading logic — in real build, use compass + PID
  int us = pctToEsc(speedPct);
  escLeft.writeMicroseconds(us);
  escRight.writeMicroseconds(us);
}

bool dockContactDetected() {
  // In real hardware: read digital pin wired to magnetic contact switch
  // Placeholder — returns false unless pin goes HIGH
  return digitalRead(26) == HIGH;
}

// ──────────────────────────────────────────────
// CONVEYOR BELT
// ──────────────────────────────────────────────
void conveyorRun(bool enable) {
  // Drive conveyor motor via digital pin 27 → relay or motor driver
  digitalWrite(27, enable ? HIGH : LOW);
}

// ──────────────────────────────────────────────
// SERVO GATE
// ──────────────────────────────────────────────
void servoGateSetPlastic() {
  servoGate.write(GATE_PLASTIC);
  delay(600);
}

void servoGateSetOrganic() {
  servoGate.write(GATE_ORGANIC);
  delay(600);
}

// ──────────────────────────────────────────────
// BATTERY ADC
// ──────────────────────────────────────────────
float readBatteryPercent(int pin) {
  // 12.8V LiFePO4 — voltage divider: R1=30kΩ, R2=10kΩ → ADC sees 3.2V at full
  // ESP32 ADC: 0–4095 for 0–3.3V
  int raw = analogRead(pin);
  float voltage = (raw / 4095.0) * 3.3 * 4.0; // ×4 for divider ratio
  // 12.8V = 100%, 10.0V = 0%
  float pct = ((voltage - 10.0) / (12.8 - 10.0)) * 100.0;
  return constrain(pct, 0, 100);
}

// ──────────────────────────────────────────────
// BIN IR SENSOR
// ──────────────────────────────────────────────
float readBinPercent(int pin) {
  // Reflective IR sensor: HIGH = full, LOW = empty
  // Simple implementation — in real build use calibrated analog IR
  return digitalRead(pin) == HIGH ? 95.0 : 25.0;
}

#endif // MOTOR_CONTROL_H
