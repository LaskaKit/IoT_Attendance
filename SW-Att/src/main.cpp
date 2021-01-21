/* LasKKit IoT Attendance. 
 * Google Sheet edition
 * Read RFID tag, compare and write enter and exit to Google Sheet
 * Google Script: src/google_script.gs
 * For settings see config.h
 * 
 * Email:obchod@laskarduino.cz
 * Web:laskarduino.cz
 * 
 *Google Sheet How To https://arduinodiy.wordpress.com/2019/11/18/sending-esp8266-sensordata-to-googlesheet/
 */

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <PN532_SWHSU.h>
#include <PN532.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h> //Include the SSL client
#include "config_my.h"           // change to config.h and fill the file.

const char* fingerprint = "46 B2 C3 44 9C 59 09 8B 01 B6 F8 BD 4C FB 00 74 91 2F EF F6";
const char* host = "script.google.com";
const int httpsPort = 443;

SoftwareSerial SWSerial( 5, 4 ); // RX, TX    
PN532_SWHSU pn532hsu(SWSerial);
PN532 nfc(pn532hsu);

WiFiClientSecure client;

const byte buzzIntervalMs = 100;
#define BUZZ 12
#define LED 13

void buzz() {

  Serial.println("--buuuuzz--");
  analogWrite(BUZZ, 200);
  digitalWrite(LED, HIGH);
  delay(buzzIntervalMs);
  digitalWrite(BUZZ, LOW);
  digitalWrite(LED, LOW);
}

void sendData(String string_x) {

  Serial.print("connecting to ");
  Serial.println(host);
  client.setInsecure();
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    buzz();
    buzz();
    buzz();
    return;
  }

  if (client.verify(fingerprint, host))
    Serial.println("certificate matches");
  else
    Serial.println("certificate doesn't match");
  
  Serial.println(string_x);

  String url = "/macros/s/" + GAS_ID + "/exec?name=" + string_x;
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "User-Agent: BuildFailureDetectorESP8266\r\n" +
                 "Connection: close\r\n\r\n");

  Serial.println("request sent");

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  Serial.println(line);

  if (line.startsWith("{\"state\":\"success\""))
    Serial.println("esp8266/Arduino CI successfull!");
  else
    Serial.println("esp8266/Arduino CI has failed");

  Serial.println("reply was: closing connection");
}

void getCard() {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;               // Length of the UID (4 bytes )
    
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
    buzz();
    nfc.setRFField(0x02, 0x00);
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
  
    bool found = false;
    byte count = 0;
    for (int i=0; i < sizeof(target_tag)/4; i++) {  // number of rows
      if (!found) {
        for (int j=0; j < 4; j++){
          if (uid[j]==target_tag[i][j]) {
            count++;
          } else break;
          if(count == 4){
            sendData(names[i]);   // TAG_ID [i]
            buzz();
            found = true;
          }
        }
      }
    }
    if (!found){
      Serial.println("Access not granted");
      buzz();
      buzz();
      buzz();
    }
  }

}

void setup() {

  Serial.begin(9600);
  while(!Serial) {} // Wait

  pinMode(BUZZ, OUTPUT_OPEN_DRAIN);
  digitalWrite(BUZZ, LOW);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  
  // pripojeni k WiFi
  Serial.println();
  Serial.print("Connecting to...");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Wi-Fi connected successfully");
  
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }

  // configure board to read RFID tags
  nfc.SAMConfig();
  Serial.println("Waiting for an ISO14443A Card ...");
}

void loop(){
  getCard();
  delay(1000);
}