/*
 * flood_sensor.h — HC-SR04 Ultrasonic Water Depth Sensor
 * Smart Floating Plastic Waste Separator
 */

#ifndef FLOOD_SENSOR_H
#define FLOOD_SENSOR_H

#include <NewPing.h>

// HC-SR04 wired inside a waterproof pipe pointing downward
#define FLOOD_TRIG_PIN    18
#define FLOOD_ECHO_PIN    19
#define CANAL_BOTTOM_CM   200  // Calibrated distance from sensor to canal floor (cm)
#define MAX_PING_CM       400

NewPing sonar(FLOOD_TRIG_PIN, FLOOD_ECHO_PIN, MAX_PING_CM);

void initFloodSensor() {
  Serial.println("[FLOOD] Ultrasonic sensor ready on pins T=" + String(FLOOD_TRIG_PIN) + " E=" + String(FLOOD_ECHO_PIN));
}

/**
 * Returns current water depth in meters.
 * Logic: sensor reads distance to water surface from fixed mount.
 * Depth = CANAL_BOTTOM_CM - distance_to_surface_cm.
 */
float readWaterDepthMeters() {
  unsigned int distanceCm = sonar.ping_cm();
  if (distanceCm == 0) distanceCm = MAX_PING_CM; // timeout fallback

  float depthCm = (float)(CANAL_BOTTOM_CM - distanceCm);
  depthCm = constrain(depthCm, 0, CANAL_BOTTOM_CM);

  return depthCm / 100.0; // Convert to meters
}

#endif // FLOOD_SENSOR_H
