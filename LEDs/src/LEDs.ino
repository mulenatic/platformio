#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

const byte pin_realais_1 = D5;
const byte pin_realais_2 = D6;
const byte pin_realais_3 = D7;
const byte pin_realais_4 = D8;

ESP8266WebServer server(80);

void handleLockOpenGet() {
  
  String numberArg = server.arg("number");
  if ( numberArg == "" ) {
    server.send(409, "textjson", "{ \"success\": \"false\" }");
  } else {

    int pin = -1;
    if (numberArg == "1" ) {
      pin = pin_realais_1;
    } else if (numberArg == "2") {
      pin = pin_realais_2;
    } else if (numberArg == "3" ) {
      pin = pin_realais_3;
    } else if (numberArg == "4" ) {
      pin = pin_realais_4;
    }

	
    if ( pin == -1 ) {
      server.send(409, "textjson", "{ \"success\": \"false\" }");
    } else {
      digitalWrite(pin, LOW);
      delay(5000);
      digitalWrite(pin, HIGH);
	

      server.send(200, "textjson", "{ \"success\": \"true\" }");
    }
  }
}


void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

  pinMode(pin_realais_1, OUTPUT);
  pinMode(pin_realais_2, OUTPUT);
  pinMode(pin_realais_3, OUTPUT);
  pinMode(pin_realais_4, OUTPUT);

  digitalWrite(pin_realais_1, HIGH);
  digitalWrite(pin_realais_2, HIGH);
  digitalWrite(pin_realais_3, HIGH);
  digitalWrite(pin_realais_4, HIGH);

  WiFiManager wifiManager;
  //wifiManager.autoConnect("esp8266-setup");

  // Port defaults to 8266
  ArduinoOTA.setPort(8266);

  server.on("/lock/open", HTTP_GET, handleLockOpenGet);
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

	  
