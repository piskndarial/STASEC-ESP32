#include <esp_camera.h>
#include <Arduino.h>

#define D0_PIN 32
#define D1_PIN 33
#define D2_PIN 25
#define D3_PIN 26
#define D4_PIN 27
#define D5_PIN 14
#define D6_PIN 12
#define D7_PIN 13
#define XCLK_PIN 19
#define PCLK_PIN 23
#define VSYNC_PIN 5
#define HREF_PIN 4
#define SIOD_GPIO_NUM 21  // SDA
#define SIOC_GPIO_NUM 22  // SCL

void setup() {
  Serial.begin(115200);

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
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = -1;    // Power down pin (-1 if not used)
  config.pin_reset = -1;   // Reset pin (-1 if not used)
  config.xclk_freq_hz = 20000000;  // XCLK 20MHz
  config.pixel_format = PIXFORMAT_RGB565;  // Use RGB565 format for OV7670
  config.frame_size = FRAMESIZE_QVGA;  // Set to QVGA (320x240)
  config.fb_count = 1;  // Single buffer

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    delay(1000);
    ESP.restart();
  } else {
    Serial.println("Camera initialized successfully");
  }
}

void loop() {
  // Nothing to do here
}
