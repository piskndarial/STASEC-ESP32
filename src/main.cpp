#include <esp_camera.h>
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
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

#define OV7670_I2C_ADDRESS 0x21 // OV7670 default I2C address

// Define OV7670 pin connections
#define VSYNC_PIN 25
#define HREF_PIN 23
#define PCLK_PIN 19
#define XCLK_PIN 18
#define D0_PIN 5
#define D1_PIN 17
#define D2_PIN 16
#define D3_PIN 4
#define D4_PIN 0
#define D5_PIN 2
#define D6_PIN 15
#define D7_PIN 13
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

void initializeCamera() {
  Wire.beginTransmission(OV7670_I2C_ADDRESS);
  // Send initialization commands to the OV7670
  Wire.write(0x12); // Reset the camera
  Wire.write(0x80);
  Wire.endTransmission();
  
  delay(100); // Wait for reset
  
  // More initialization commands can be added here
}
void setup() {
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

void loop() {
  if (digitalRead(VSYNC_PIN) == HIGH) {
    captureImage();
  }
}



