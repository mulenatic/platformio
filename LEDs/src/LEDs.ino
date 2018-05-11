#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

const byte led_red_pin = D2;
const byte led_green_pin = D3;
const byte led_blue_pin = D4;

ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

  pinMode(led_red_pin, OUTPUT);
  pinMode(led_green_pin, OUTPUT);
  pinMode(led_blue_pin, OUTPUT);

  digitalWrite(led_red_pin, LOW);
  digitalWrite(led_green_pin, LOW);
  digitalWrite(led_blue_pin, LOW);
  

  WiFiManager wifiManager;
  //wifiManager.autoConnect("esp8266-setup");

  // Port defaults to 8266
  ArduinoOTA.setPort(8266);

  server.on("/red", HTTP_GET, handleRedGet);
  server.on("/green", HTTP_GET, handleGreenGet);
  server.on("/blue", HTTP_GET, handleBlueGet);
  server.begin();
  Serial.println("ESP8266WebServer started");

  // Hostname default to esp8266-[ChipId]
  //ArduinoOTA.setHostname("NodeMCU1");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // ArduinoOTA.setPasswordHash(xxx);

  ArduinoOTA.onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
	type == "sketch";
      } else {
	type = "filesystem";
      }

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating" + type);
    });

  ArduinoOTA.onEnd([]() {
      Serial.println("\nEND");
    });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / ( total / 100 )));
    });

  ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) {
	Serial.println("Auth failed");
      } else if (error == OTA_BEGIN_ERROR) {
	Serial.println("Begin failed");
      } else if (error == OTA_CONNECT_ERROR) {
	Serial.println("Connect failed");
      } else if (error == OTA_RECEIVE_ERROR) {
	Serial.println("Receive failed");
      } else if (error == OTA_END_ERROR) {
	Serial.println("End failed");
      }
    });

  ArduinoOTA.begin();
  Serial.println("Ready for business OTA");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());		   
}

void loop() {
  ArduinoOTA.handle();

  server.handleClient();
  
}
    
void handleRedGet() {
  digitalWrite(led_red_pin, HIGH);
  delay(5000);
  digitalWrite(led_red_pin, LOW);

  server.send(200, "text/json", "{ \"success\": \"true\" }");
}

void handleGreenGet() {
  digitalWrite(led_green_pin, HIGH);
  delay(5000);
  digitalWrite(led_green_pin, LOW);

  server.send(200, "text/json", "{ \"success\": \"true\" }");
}

void handleBlueGet() {
  digitalWrite(led_blue_pin, HIGH);
  delay(5000);
  digitalWrite(led_blue_pin, LOW);

  server.send(200, "text/json", "{ \"success\": \"true\" }");
}


      
