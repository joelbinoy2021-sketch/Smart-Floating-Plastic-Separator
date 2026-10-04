/*
 * vision.h — ESP32-CAM Object Detection Stubs
 * Smart Floating Plastic Waste Separator
 *
 * In real implementation, run a TFLite Micro model trained on:
 *   - PET plastic bottles (COLLECT)
 *   - Organic leaves (REJECT)
 *   - Fish (AVOID)
 *
 * For now: placeholder functions that return simulated detections.
 * Replace with actual ESP32-CAM stream + TensorFlow Lite inference.
 */

#ifndef VISION_H
#define VISION_H

#include "esp_camera.h"

// ──────────────────────────────────────────────
// CAMERA PIN MAP (AI Thinker ESP32-CAM)
// ──────────────────────────────────────────────
#define CAM_PIN_PWDN    32
#define CAM_PIN_RESET   -1
#define CAM_PIN_XCLK     0
#define CAM_PIN_SIOD    26
#define CAM_PIN_SIOC    27
#define CAM_PIN_D7      35
#define CAM_PIN_D6      34
#define CAM_PIN_D5      39
#define CAM_PIN_D4      36
#define CAM_PIN_D3      21
#define CAM_PIN_D2      19
#define CAM_PIN_D1      18
#define CAM_PIN_D0       5
#define CAM_PIN_VSYNC   25
#define CAM_PIN_HREF    23
#define CAM_PIN_PCLK    22

// Detection confidence thresholds
#define PLASTIC_THRESHOLD  0.75f
#define FISH_THRESHOLD     0.70f

static bool _plasticInView    = false;
static bool _plasticAtIntake  = false;
static bool _fishInView       = false;
static unsigned long _lastFrame = 0;

void initVisionCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = CAM_PIN_D0;
  config.pin_d1       = CAM_PIN_D1;
  config.pin_d2       = CAM_PIN_D2;
  config.pin_d3       = CAM_PIN_D3;
  config.pin_d4       = CAM_PIN_D4;
  config.pin_d5       = CAM_PIN_D5;
  config.pin_d6       = CAM_PIN_D6;
  config.pin_d7       = CAM_PIN_D7;
  config.pin_xclk     = CAM_PIN_XCLK;
  config.pin_pclk     = CAM_PIN_PCLK;
  config.pin_vsync    = CAM_PIN_VSYNC;
  config.pin_href     = CAM_PIN_HREF;
  config.pin_sscb_sda = CAM_PIN_SIOD;
  config.pin_sscb_scl = CAM_PIN_SIOC;
  config.pin_pwdn     = CAM_PIN_PWDN;
  config.pin_reset    = CAM_PIN_RESET;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size   = FRAMESIZE_QVGA;  // 320×240
  config.jpeg_quality = 12;
  config.fb_count     = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("[CAMERA] Init failed: 0x%x\n", err);
    return;
  }
  Serial.println("[CAMERA] ESP32-CAM ready (QVGA 320×240)");
}

/**
 * Run one inference frame.
 * TODO: Replace body with actual TFLite Micro model call.
 */
void _runInference() {
  if (millis() - _lastFrame < 200) return; // Max 5 FPS inference
  _lastFrame = millis();

  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) { Serial.println("[CAMERA] Frame capture failed"); return; }

  // ── PLACEHOLDER ──────────────────────────────────────
  // Real implementation:
  //   1. Convert JPEG → RGB888
  //   2. Resize to model input (e.g. 96×96)
  //   3. Run tflite_interpreter->Invoke()
  //   4. Parse output tensor for class scores
  // Placeholder: simulate random detections for testing
  _plasticInView   = (random(100) < 60); // 60% chance plastic visible
  _plasticAtIntake = (random(100) < 30); // 30% chance at intake proximity
  _fishInView      = (random(100) < 10); // 10% chance fish nearby
  // ─────────────────────────────────────────────────────

  esp_camera_fb_return(fb);
}

bool visionPlasticDetected() {
  _runInference();
  return _plasticInView;
}

bool visionPlasticAtIntake() {
  _runInference();
  return _plasticAtIntake;
}

bool visionCheckFishPresent() {
  _runInference();
  return _fishInView;
}

#endif // VISION_H
