#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Arduino_OV767X.h>
#include <OV767X.h>
#include "ov767X.h"

// Define pin constants
const int XCLK = 5;
const int PCLK = 18;
const int VSYNC = 19;
const int HREF = 23;
const int D0 = 25;
const int D1 = 26;
const int D2 = 27;
const int D3 = 14;
const int D4 = 15;
const int D5 = 4;
const int D6 = 12;
const int D7 = 13;
const int FIFO_WE = 17;

// Initialize OV7670
OV7670 camera(
  XCLK, PCLK, VSYNC, HREF, D0, D1, D2, D3, D4, D5, D6, D7, FIFO_WE
);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  SPI.begin(5, 2, 1, 0); // MOSI, MISO, SCK, SS

  camera.init();
  camera.set_xclk_rate(20000000); // Set clock rate
  camera.set_format(OV7670_JPEG); // Set JPEG format
}

void loop() {
  camera.capture();
  camera.save_image("image.jpg");
  delay(5000); // Wait for 5 seconds before capturing another image
}