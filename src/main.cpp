/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/telegram-esp32-motion-detection-arduino/
  
  Project created using Brian Lough's Universal Telegram Bot Library: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
  Recontribute by piskndar for STASEC Project.
*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
/*
#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {14, 27, 26, 25};
byte colPins[COLS] = {33, 32, 35, 32};

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);

unsigned long loopCount;
unsigned long startTime;
String msg;
*/
// Replace with your network credentials
//const char* ssid = "SJ Wifi #27@unifi";
//const char* password = "SJfreewifi2023";

// Initialize Telegram BOT
#define BOTtoken "7323854577:AAGJfewY7Eb0pknO1BTAQ2vdo-YETaCj5nM"  // your Bot Token (Get from Botfather)

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "1823422500" //shahril
//#define CHAT_ID1 "62086994" //sod
//#define CHAT_ID2 "5189525146" //adi

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

const int motionSensor = 13; // PIR Motion Sensor
bool motionDetected = false;

// Indicates when motion is detected
void IRAM_ATTR detectsMovement() {
  //Serial.println("MOTION DETECTED!!!");
  motionDetected = true;
}

void setup() {
  Serial.begin(115200);
  /*
  loopCount = 0;
  startTime = millis();
  msg = "";
  */
  pinMode(2 , OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(4, OUTPUT);
  /* Merah = 2
     Hijau = 15
     Biru = 4*/
  

  WiFiManager wm;
  bool res;
  res = wm.autoConnect("SJ Legacy ESP32 Free WiFi!","password");
  if(!res) {
        Serial.println("Failed to connect");

  }
  else {
    Serial.println("Already connected!");
    digitalWrite(2 ,false);
    digitalWrite(15, false);
    digitalWrite(4, true);
    delay(1000);
    digitalWrite(4, false);
    digitalWrite(15, true);
  }

  // PIR Motion Sensor mode INPUT_PULLUP
  pinMode(motionSensor, INPUT_PULLUP);
  // Set motionSensor pin as interrupt, assign interrupt function and set RISING mode
  attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);

  // Attempt to connect to Wifi network:
  //Serial.print("Connecting Wifi: ");
  //Serial.println(ssid);

  //WiFi.mode(WIFI_STA);
  //WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  
  //while (WiFi.status() != WL_CONNECTED) {
    //Serial.print(".");
    //delay(500);
  //}

  //Serial.println("");
  //Serial.println("WiFi connected");
  //Serial.print("IP address: ");
  //Serial.println(WiFi.localIP());

  bot.sendMessage(CHAT_ID, "Bot started up", "");
  //bot.sendMessage(CHAT_ID1, "Bot started up", "");
  //bot.sendMessage(CHAT_ID2, "Bot started up", "");
  digitalWrite(15, true);
}

void loop() {
  if(motionDetected){
    bot.sendMessage(CHAT_ID, "Motion detected!!", "");
    //bot.sendMessage(CHAT_ID1, "Motion detected!!", "");
    //bot.sendMessage(CHAT_ID2, "Motion detected!!", "");
    Serial.println("Motion Detected");
    motionDetected = false;
    digitalWrite(15, false);
    for (int i = 0; i < 5; i++) {
      digitalWrite(2, true);
      delay(100);
      digitalWrite(2, false);
      delay(100);
    }
    digitalWrite(15, true);
  }

  /*
  if (kpd.getKeys())
  {
    for (int i=0; i<LIST_MAX; i++)
    {
      if (kpd.key[i].stateChanged)
      {
        switch (kpd.key[i].kstate) {
          case PRESSED:
          msg = "PRESSED.";
        break;
          case HOLD:
          msg = "HOLD.";
        break;
          case RELEASED:
          msg = "RELEASED.";
        break;
          case IDLE:
          msg = "IDLE.";
        }
        Serial.print("KEY ");
        Serial.print(kpd.key[i].kchar);
        Serial.println(msg); 
      }
    }
  }*/
}