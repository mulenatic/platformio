#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <WiFiManager.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

DHT dht(D5, DHT11);

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

  Serial.println("Starting DHT");
  dht.begin();

  WiFiManager wifiManager;
  wifiManager.autoConnect("esp8266-setup");
  
  
  // WiFi.mode(WIFI_STA);
  // WiFi.begin(ssid, password);
  // while (WiFi.waitForConnectResult() != WL_CONNECTED) {
  //   Serial.println("Connection Failed! Rebooting...");
  //   delay(5000);
  //   ESP.restart();
  // }

  // Port defaults to 8266
  ArduinoOTA.setPort(8266);

  // Hostname default to esp8266-[ChipId]
  ArduinoOTA.setHostname("NodeMCU1");

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

  if(millis() % 2000 == 0) {

    boolean measurementCompleted = false;
    while(!measurementCompleted) {
      float temperature = dht.readTemperature();

      if (!isnan(temperature)) {
	Serial.print("Temperature: ");
	Serial.println(temperature);

	measurementCompleted = true;
      }

    }

  }
}
    
