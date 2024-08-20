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
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// Initialize Telegram BOT
String BOTtoken = "XXXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";  // your Bot Token (Get from Botfather)

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
String CHAT_ID = "XXXXXXXXXX";

bool sendPhoto = false;

WiFiClientSecure clientTCP;
UniversalTelegramBot bot(BOTtoken, clientTCP);

#define FLASH_LED_PIN 4
bool flashState = LOW;

//Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

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
  Serial.println("Initializing camera...");
  Wire.beginTransmission(OV7670_I2C_ADDRESS);
  Serial.println("  Sending reset command...");
  Wire.write(0x12); // Reset the camera
  Wire.write(0x80);
  Wire.endTransmission();
  
  Serial.println("  Waiting for reset...");
  delay(100); // Wait for reset
  
  Serial.println("  Initialization complete.");
  
  // More initialization commands can be added here
}

void captureImage() 
{
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
  
//init with high specs to pre-allocate larger buffers
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;  //0-63 lower number means higher quality
    config.fb_count = 1;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;  //0-63 lower number means higher quality
    config.fb_count = 1;
  }
  
  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    delay(1000);
    ESP.restart();
  }
}

void handleNewMessages(int numNewMessages) 
{
  Serial.print("Handle New Messages: ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);
    
    String from_name = bot.messages[i].from_name;
    if (text == "/start") {
      String welcome = "Welcome , " + from_name + "\n";
      welcome += "Use the following commands to interact with the ESP32-CAM \n";
      welcome += "/photo : takes a new photo\n";
      welcome += "/flash : toggles flash LED \n";
      bot.sendMessage(CHAT_ID, welcome, "");
    }
    if (text == "/flash") {
      flashState = !flashState;
      digitalWrite(FLASH_LED_PIN, flashState);
      Serial.println("Change flash LED state");
    }
    if (text == "/photo") {
      sendPhoto = true;
      Serial.println("New photo request");
    }
  }
}

String sendPhotoTelegram() 
{

  const char* myDomain = "api.telegram.org";
  String getAll = "";
  String getBody = "";

  //Dispose first picture because of bad quality
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  esp_camera_fb_return(fb); // dispose the buffered image
  
  // Take a new photo
  fb = NULL;  
  fb = esp_camera_fb_get();  
  if(!fb) {
    Serial.println("Camera capture failed");
    delay(1000);
    ESP.restart();
    return "Camera capture failed";
  }  
  
  Serial.println("Connect to " + String(myDomain));


  if (clientTCP.connect(myDomain, 443)) {
    Serial.println("Connection successful");
    
    String head = "--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"chat_id\"; \r\n\r\n" + CHAT_ID + "\r\n--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"photo\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--RandomNerdTutorials--\r\n";

    size_t imageLen = fb->len;
    size_t extraLen = head.length() + tail.length();
    size_t totalLen = imageLen + extraLen;
  
    clientTCP.println("POST /bot"+BOTtoken+"/sendPhoto HTTP/1.1");
    clientTCP.println("Host: " + String(myDomain));
    clientTCP.println("Content-Length: " + String(totalLen));
    clientTCP.println("Content-Type: multipart/form-data; boundary=RandomNerdTutorials");
    clientTCP.println();
    clientTCP.print(head);
  
    uint8_t *fbBuf = fb->buf;
    size_t fbLen = fb->len;
    for (size_t n=0;n<fbLen;n=n+1024) {
      if (n+1024<fbLen) {
        clientTCP.write(fbBuf, 1024);
        fbBuf += 1024;
      }
      else if (fbLen%1024>0) {
        size_t remainder = fbLen%1024;
        clientTCP.write(fbBuf, remainder);
      }
    }  
    
    clientTCP.print(tail);
    
    esp_camera_fb_return(fb);
    
    int waitTime = 10000;   // timeout 10 seconds
    long startTimer = millis();
    boolean state = false;
    
    while ((startTimer + waitTime) > millis()){
      Serial.print(".");
      delay(100);      
      while (clientTCP.available()) {
        char c = clientTCP.read();
        if (state==true) getBody += String(c);        
        if (c == '\n') {
          if (getAll.length()==0) state=true; 
          getAll = "";
        } 
        else if (c != '\r')
          getAll += String(c);
        startTimer = millis();
      }
      if (getBody.length()>0) break;
    }
    clientTCP.stop();
    Serial.println(getBody);
  }
  else {
    getBody="Connected to api.telegram.org failed.";
    Serial.println("Connected to api.telegram.org failed.");
  }
  return getBody;
}

void setup()
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 
  // Init Serial Monitor
  Serial.begin(115200);

  // Set LED Flash as output
  pinMode(FLASH_LED_PIN, OUTPUT);
  digitalWrite(FLASH_LED_PIN, flashState);

  // Config and init the camera
  configInitCamera();

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  clientTCP.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("ESP32-CAM IP Address: ");
  Serial.println(WiFi.localIP()); 
}

void loop() 
{

 if (digitalRead(VSYNC_PIN) == HIGH) {
    captureImage();
    delay(1000);
  }
 
  if (sendPhoto) {
    Serial.println("Preparing photo");
    sendPhotoTelegram(); 
    sendPhoto = false; 
  }
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}

