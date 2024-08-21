#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_camera.h"
#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "amir's lappy";
const char* password = "12345678";

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = 32;
    config.pin_d1 = 33;
    config.pin_d2 = 34;
    config.pin_d3 = 35;
    config.pin_xclk = 23;
    config.pin_pclk = 25;
    config.pin_vsync = 26;
    config.pin_href = 27;
    config.pin_sccb_sda = 22;
    config.pin_sccb_scl = 21;
    config.pin_reset = -1;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12; // 0-63 lower number means higher quality
    config.fb_count = 2; // if more than one, will use more RAM

    // Initialize the camera
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x", err);
        return;
    }
}

void loop() {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Camera capture failed");
        delay(1000);
        return;
    }

    // Send image to server
    WiFiClient client;
    if (client.connect("your.server.com", 80)) {
        String header = "--boundary\r\n";
        header += "Content-Disposition: form-data; name=\"file\"; filename=\"image.jpg\"\r\n";
        header += "Content-Type: image/jpeg\r\n\r\n";
        client.print(header);
        client.write(fb->buf, fb->len);
        client.print("\r\n--boundary--\r\n");
        Serial.println("Image sent");
    } else {
        Serial.println("Failed to connect to server");
    }

    esp_camera_fb_return(fb);
    delay(5000); // Capture every 5 seconds
}
