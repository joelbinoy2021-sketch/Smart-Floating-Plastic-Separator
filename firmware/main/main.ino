/*
 * Smart Floating Plastic Waste Separator & Flood Warning System
 * Main Firmware — ESP32-CAM
 * 
 * Hardware:
 *   - ESP32-CAM (AI Thinker)
 *   - 2× IP68 Brushless Thrusters (via ESC)
 *   - MG996R Servo (sorting gate)
 *   - HC-SR04 Ultrasonic (flood level)
 *   - IR Bin Sensor
 *   - LiFePO4 Battery + BMS
 * 
 * State Machine:
 *   PATROL → APPROACH_TRASH → DOCKING → DOCKED_CHARGING → PATROL
 */

#include <Arduino.h>
#include <ESP32Servo.h>
#include "motor_control.h"
#include "vision.h"
#include "flood_sensor.h"

// ──────────────────────────────────────────────
// PIN DEFINITIONS
// ──────────────────────────────────────────────
#define PIN_THRUSTER_LEFT    12
#define PIN_THRUSTER_RIGHT   13
#define PIN_SERVO_GATE       14
#define PIN_BIN_IR_SENSOR    15
#define PIN_BATTERY_ADC      34   // Analog read for voltage divider

// ──────────────────────────────────────────────
// CONSTANTS
// ──────────────────────────────────────────────
#define BATTERY_LOW_PCT      20
#define BIN_FULL_PCT         90
#define PATROL_SPEED         60    // PWM 0–100
#define APPROACH_SPEED       80
#define DOCK_SPEED           50

// ──────────────────────────────────────────────
// STATE MACHINE
// ──────────────────────────────────────────────
enum VesselState {
  STATE_PATROL,
  STATE_APPROACH_TRASH,
  STATE_DOCKING,
  STATE_DOCKED_CHARGING,
  STATE_AVOID_FISH
};

VesselState currentState = STATE_PATROL;

// ──────────────────────────────────────────────
// GLOBAL VARIABLES
// ──────────────────────────────────────────────
float batteryPct   = 100.0;
float binPct       = 0.0;
float waterDepthM  = 0.0;
bool  fishDetected = false;

unsigned long lastTelemetryMs = 0;
const unsigned long TELEMETRY_INTERVAL = 2000; // ms

// ──────────────────────────────────────────────
// SETUP
// ──────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  Serial.println("[BOOT] Smart Floating Separator v1.0");

  initMotors(PIN_THRUSTER_LEFT, PIN_THRUSTER_RIGHT, PIN_SERVO_GATE);
  initVisionCamera();
  initFloodSensor();

  pinMode(PIN_BIN_IR_SENSOR, INPUT);
  pinMode(PIN_BATTERY_ADC, INPUT);

  Serial.println("[BOOT] All systems ready. Starting PATROL.");
}

// ──────────────────────────────────────────────
// MAIN LOOP
// ──────────────────────────────────────────────
void loop() {
  // Read sensors every cycle
  batteryPct  = readBatteryPercent(PIN_BATTERY_ADC);
  binPct      = readBinPercent(PIN_BIN_IR_SENSOR);
  waterDepthM = readWaterDepthMeters();
  fishDetected = visionCheckFishPresent();

  // Flood warning check
  if (waterDepthM > 1.80) {
    Serial.println("[FLOOD WARNING] Water level critical: " + String(waterDepthM) + " m");
    // TODO: Send MQTT alert to dashboard
  }

  // State machine transitions
  switch (currentState) {

    case STATE_PATROL:
      motorPatrol(PATROL_SPEED);
      if (batteryPct <= BATTERY_LOW_PCT || binPct >= BIN_FULL_PCT) {
        Serial.println("[STATE] → DOCKING (low battery or bin full)");
        currentState = STATE_DOCKING;
      } else if (visionPlasticDetected()) {
        Serial.println("[STATE] → APPROACH_TRASH");
        currentState = STATE_APPROACH_TRASH;
      }
      break;

    case STATE_APPROACH_TRASH:
      if (fishDetected) {
        Serial.println("[STATE] → AVOID_FISH (safe swerve)");
        currentState = STATE_AVOID_FISH;
        break;
      }
      motorApproachTarget(APPROACH_SPEED);
      if (visionPlasticAtIntake()) {
        conveyorRun(true);
        servoGateSetPlastic(); // Open to bin
        Serial.println("[ACTION] Plastic scooped into bin.");
        currentState = STATE_PATROL;
      }
      if (batteryPct <= BATTERY_LOW_PCT) {
        currentState = STATE_DOCKING;
      }
      break;

    case STATE_AVOID_FISH:
      motorSwerveRight(PATROL_SPEED);
      delay(1200);
      currentState = STATE_PATROL;
      break;

    case STATE_DOCKING:
      motorNavigateToDock(DOCK_SPEED);
      if (dockContactDetected()) {
        motorStop();
        Serial.println("[STATE] → DOCKED_CHARGING");
        currentState = STATE_DOCKED_CHARGING;
      }
      break;

    case STATE_DOCKED_CHARGING:
      conveyorRun(false);
      motorStop();
      Serial.println("[CHARGING] Battery: " + String(batteryPct) + "% | Bin: " + String(binPct) + "%");
      if (batteryPct >= 98 && binPct <= 5) {
        Serial.println("[STATE] → PATROL (fully charged, bin cleared)");
        currentState = STATE_PATROL;
      }
      break;
  }

  // Telemetry printout every 2 seconds
  if (millis() - lastTelemetryMs >= TELEMETRY_INTERVAL) {
    lastTelemetryMs = millis();
    printTelemetry();
  }

  delay(50); // 20Hz main loop
}

// ──────────────────────────────────────────────
// TELEMETRY
// ──────────────────────────────────────────────
void printTelemetry() {
  Serial.println("─────────────────────────────");
  Serial.println("  STATE:   " + String(currentState));
  Serial.println("  BATTERY: " + String(batteryPct, 1) + "%");
  Serial.println("  BIN:     " + String(binPct, 1) + "%");
  Serial.println("  DEPTH:   " + String(waterDepthM, 2) + " m");
  Serial.println("  FISH:    " + String(fishDetected ? "YES - SAFE SWERVE" : "No"));
  Serial.println("─────────────────────────────");
}
