#include <SoftwareSerial.h>
#include <PN532_SWHSU.h>
#include <PN532.h>

#include <ESP8266WiFi.h>
//Include the SSL client
#include <WiFiClientSecure.h>

String GAS_ID = "AKfycbygLIPks3_uJdokokiNyro0rE3mnl7gd55xrqINNTBUwGhR6DE";//getactivespreadsheetID
const char* fingerprint = "46 B2 C3 44 9C 59 09 8B 01 B6 F8 BD 4C FB 00 74 91 2F EF F6";
const char* host = "script.google.com";
const int httpsPort = 443;

SoftwareSerial SWSerial( D1, D2 ); // RX, TX    
PN532_SWHSU pn532hsu(SWSerial);
PN532 nfc(pn532hsu);

char ssid[] = "laskalab"; // your network SSID (name)
char pass[] = "laskaLAB754125"; // your network password

//char ssid[] = "Krewedko"; // your network SSID (name)
//char pass[] = "trzlaskova1"; // your network password

WiFiClientSecure client;

const byte buzzIntervalMs = 100;
#define BUZZ D5

void buzz(){
  Serial.println("--buuuuzz--");
  analogWrite(BUZZ, 200);
  delay(buzzIntervalMs);
  pinMode(BUZZ, INPUT_PULLUP);
  digitalWrite(BUZZ, LOW);
}

void sendData(String string_x)
{

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

  if (client.verify(fingerprint, host)) {
    Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
  }
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
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.println("reply was: closing connection");
}

void getCard() {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
    buzz();
    nfc.setRFField(0x02, 0x00);
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    
    if( uid[0]==0xC9 && uid[1]==0x4F && uid[2]==0x02 && uid[3]==0xB9 ) {  //C9 4F 02 B9
    sendData("Helena");
  }  else if( uid[0]==0x86 && uid[1]==0x2D && uid[2]==0x14 && uid[3]==0xCA ) {  //86 2D 14 CA
    sendData("Martin");
  } else if(uid[0]==0x76 && uid[1]==0x16 && uid[2]==0xB0 && uid[3]==0xCA) { //76 16 B0 CA
    sendData("Marek");
  } else if( uid[0]==0x2B && uid[1]==0xD0 && uid[2]==0x6B && uid[3]==0x21 ) { //
    sendData("Kosta");
  } else {
    Serial.println("Get the fuck outta my office");
    buzz();
    buzz();
    }
    buzz();
  }
}

void setup() {

  Serial.begin(9600);
  while(!Serial) {} // Wait

  pinMode(BUZZ, INPUT_PULLUP);
  digitalWrite(BUZZ, LOW);
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }

  // configure board to read RFID tags
  nfc.SAMConfig();
  Serial.println("Waiting for an ISO14443A Card ...");
  
  // pripojeni k WiFi
  Serial.println();
  Serial.print("Connecting to...");
  Serial.println(ssid);

 // WiFi.config(ip,gateway,subnet);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Wi-Fi connected successfully");
}

void loop(){
  getCard();
  delay(1000);

}