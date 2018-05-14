
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <WiFiManager.h>

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 20
#define SS_PIN 2
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory;
MFRC522::MIFARE_Key key;

WiFiClientSecure client;
const char* host = "poc9.osnow.de";
const int httpsPort = 443;

const char* fingerprint = "dc a6 d8 b8 0a ea 7e 0b 32 77 14 05 2f 27 b2 90 32 b7 7e b2";

//ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

  for(byte i=0; i<6; i++) {
    key.keyByte[i] = 0xFF;
  }


  SPI.begin();
  mfrc522.PCD_Init();

  WiFiManager wifiManager;
  wifiManager.autoConnect();

  // Port defaults to 8266
  ArduinoOTA.setPort(8266);

  // Hostname default to esp8266-[ChipId]
  ArduinoOTA.setHostname("RFIDMCU");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well

  // ArduinoOTA.setPasswordHash(xxx);

  //server.on("/cardId", HTTP_GET, handleGetCardId);
  //server.begin();
  //Serial.println("ESP8266WebServer started");


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
  Serial.println("Ready for RFID reading");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());		   
}

void loop() {

  ArduinoOTA.handle();
  readCardID();
  
  //server.handleClient();
}

void handleGetCardId() {

  //String cardId = readCardID();
  //Serial.print("ID: ");
  //Serial.println(cardId);
  //server.send(200, "text/json", "{ \"success\": \"true\", \"cardId\": \"" + cardId + "\" }");

}

void readCardID() {

  String cardId = "";
  boolean isFinished = false;
  //  while(!isFinished) {

  yield();

  //--------------------

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    //continue;
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    //      continue;
    return;
  }

  Serial.println(F("**Card Detected:**"));

  //====================

  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); // dump some details about the card

  long code = 0;
  for (byte i = 0; i < mfrc522.uid.size; i++ ) {
    code = ((code + mfrc522.uid.uidByte[i]) * 10 );
  }

  cardId = String(code);

  //====================



  Serial.println(F("\n**End Reading**\n"));

  isFinished = true;
  yield();

  Serial.print("ID: ");
  Serial.println(cardId);

  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection to ServiceNow failed.");
    return;
  } else {
    Serial.println("Connection to ServiceNow succeded");
  }

  if ( client.verify(fingerprint, host)) {
    Serial.println("Certificates matches");
  } else {
    Serial.println("Certifiactes don't match");
  }


  String url = "/api/x_buergerbox/buergerboxrestapi/buergerbox/kartenid/" + cardId;

  Serial.print("Requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
	       "Host: " + host + "\r\n" +
	       "User-Agent: BuildFailureDetectorESP8266\r\n" + 
	       "Authorization: Basic bm9kZU1DVToxMTEx\r\n" +
	       "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while(client.available() == 0) {
    if(millis() - timeout > 5000) {
      Serial.println(">>> Client timeout!");
      client.stop();
      return;
    }
  }

  Serial.println("Send request, next wait for answer");

  while(client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("Closing connection");
  

  delay(1000);

    
    
  //  }
  //  return cardId;
  
}
      
