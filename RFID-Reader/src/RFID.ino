#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <WiFiManager.h>

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 20
#define SS_PIN 2
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

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

  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory;
  MFRC522::MIFARE_Key key;
  for(byte i=0; i<6; i++) {
    key.keyByte[i] = 0xFF;
  }

  //some variable we need
  byte block;
  byte len;
  MFRC522::StatusCode status;

  //--------------------

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println(F("**Card Detected:**"));

  //====================

  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); // dump some details about the card

  //====================

  Serial.print(F("Name: "));

  byte buffer1[18];

  block = 4;
  len = 18;

  //==================== Get first name
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // Print first name
  for (byte i = 0; i < 16; i++ ) {
    if (buffer1[i] != 32) {
      Serial.write(buffer1[i]);
    }
  }
  Serial.println("");
  Serial.println("Finished reading");
  Serial.println("");

  //--------------------

  Serial.println(F("\n**End Reading**\n"));

  delay(1000); // change value if you want to read cards faster

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  
}
    


      
