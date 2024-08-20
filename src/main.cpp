#include <Arduino.h>
#include <Wire.h>
#include "esp_camera.h"
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <string.h>
#include "freertos/semphr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <WiFiManager.h>

static const char *TAG = "OV7670";

#define OV7670_I2C_ADDRESS 0x21 // OV7670 default I2C address

// Define OV7670 pin connections
#define VSYNC_PIN 5
#define HREF_PIN 4
#define PCLK_PIN 22
#define XCLK_PIN 19
#define SIOD_GPIO_NUM    21
#define SIOC_GPIO_NUM    22
#define D0_PIN 32
#define D1_PIN 33
#define D2_PIN 25
#define D3_PIN 26
#define D4_PIN 27
#define D5_PIN 14
#define D6_PIN 12
#define D7_PIN 13

void initializeCamera();
void captureImage();

void setup() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = D0_PIN;
  config.pin_d1 = D1_PIN;
  config.pin_d2 = D2_PIN;
  config.pin_d3 = D3_PIN;
  config.pin_d4 = D4_PIN;
  config.pin_d5 = D5_PIN;
  config.pin_d6 = D6_PIN;
  config.pin_d7 = D7_PIN;
  config.pin_xclk = XCLK_PIN;
  config.pin_pclk = PCLK_PIN;
  config.pin_vsync = VSYNC_PIN;
  config.pin_href = HREF_PIN;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = -1;    // Power down pin (-1 if not used)
  config.pin_reset = -1;   // Reset pin (-1 if not used)
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; // or PIXFORMAT_RGB565 for raw data
  
  Wire.begin();
  Serial.begin(115200);

  // Initialize OV7670 camera
  initializeCamera();
  
  // Initialize GPIO for camera data pins
  pinMode(VSYNC_PIN, INPUT);
  pinMode(HREF_PIN, INPUT);
  pinMode(PCLK_PIN, INPUT);
  for (int i = D0_PIN; i <= D7_PIN; i++) {
    pinMode(i, INPUT);
  }

  // Initialize XCLK for camera
  ledcAttachPin(XCLK_PIN, 0);  // Use LEDC to generate clock signal
  ledcSetup(0, 20000000, 1);   // 20MHz clock
  ledcWrite(0, 1);

  // Ready to capture image
  Serial.println("Camera Initialized");
}

void initializeCamera() {
  Wire.beginTransmission(OV7670_I2C_ADDRESS);
  // Send initialization commands to the OV7670
  Wire.write(0x12); // Reset the camera
  Wire.write(0x80);
  Wire.endTransmission();
  
  delay(100); // Wait for reset
  
  // More initialization commands can be added here
}

void captureImage() {
  Serial.println("Capturing Image...");

  while (digitalRead(VSYNC_PIN) == LOW); // Wait for VSYNC to go high

  while (digitalRead(VSYNC_PIN) == HIGH) { // Image capture loop
    if (digitalRead(HREF_PIN) == HIGH) {
      // Read pixel data
      uint8_t byte = 0;
      for (int i = 0; i < 8; i++) {
        byte |= digitalRead(D0_PIN + i) << i;
      }
      Serial.write(byte); // Send pixel byte to Serial for now (or save to buffer)
    }

    while (digitalRead(PCLK_PIN) == LOW); // Wait for PCLK high
    while (digitalRead(PCLK_PIN) == HIGH); // Wait for PCLK low
  }

  Serial.println("Image Captured");
}

void loop() {
  if (digitalRead(VSYNC_PIN) == HIGH) {
    captureImage();
    delay(1000);
  }
}
