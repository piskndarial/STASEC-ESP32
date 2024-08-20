#include <Arduino.h>
#include "esp_camera.h"
#include <WiFi.h>

#define CAMERA_MODEL_WROVER_KIT
#include "camera_pins.h"
#define LEDC_CHANNEL 0  
#define LEDC_TIMER  0

// WiFi credentials
const char* ssid = "SJ Wifi #27@unifi";
const char* password = "SJfreewifi2023";

// Camera configuration
void startCameraServer();

void setup() {
  Serial.begin(115200);
  
  // Initialize Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize the camera
  camera_config_t config;
  config.ledc_channel = (ledc_channel_t)LEDC_CHANNEL;
  config.ledc_timer = (ledc_timer_t)LEDC_TIMER;
  config.pin_d0 = 32;
  config.pin_d1 = 33;
  config.pin_d2 = 25;
  config.pin_d3 = 26;
  config.pin_d4 = 27;
  config.pin_d5 = 14;
  config.pin_d6 = 12;
  config.pin_d7 = 13;
  config.pin_xclk = 19;
  config.pin_pclk = 22;
  config.pin_vsync = 5;
  config.pin_href = 4;
  config.pin_sscb_sda = 21;
  config.pin_sscb_scl = 22;
  config.pin_pwdn = -1;
  config.pin_reset = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // Initialize the camera
  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera init failed");
    return;
  }

  // Start the camera server
  startCameraServer();
}

void loop() {
  // Main loop can be used for other tasks
}

void startCameraServer() {
  // Implement your server code here to handle image streaming or capture
}