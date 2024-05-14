/*
  WriteMultipleFields

  Description: Writes values to fields 1,2,3,4 and status in a single ThingSpeak update every 20 seconds.

  Hardware: ESP32 based boards

  !!! IMPORTANT - Modify the secrets.h file for this project with your network connection and ThingSpeak channel details. !!!

  Note:
  - Requires installation of EPS32 core. See https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md for details.
  - Select the target hardware from the Tools->Board menu
  - This example is written for a network using WPA encryption. For WEP or WPA, change the WiFi.begin() call accordingly.

  ThingSpeak ( https://www.thingspeak.com ) is an analytic IoT platform service that allows you to aggregate, visualize, and
  analyze live data streams in the cloud. Visit https://www.thingspeak.com to sign up for a free account and create a channel.

  Documentation for the ThingSpeak Communication Library for Arduino is in the README.md folder where the library was installed.
  See https://www.mathworks.com/help/thingspeak/index.html for the full ThingSpeak documentation.

  For licensing information, see the accompanying license file.

  Copyright 2020, The MathWorks, Inc.
*/

#include <WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

// Initialize our values
float vef;
float freqv;
float ief;
float freqi;
float p;
float s;
float phi;

void getfreqv() {
  freqv = random(0, 100);

}
void getvef() {
  vef = random(0, 100);
}

void getief() {
  ief = random(0, 100);
}

void getfreqi() {
  freqi = random(0, 100);
}

void getactivepower() {
  p = random(0, 100);
}

void getapparentpower() {
  s = random(0, 100);
}

void getphi() {
  phi = random(0, 100);
}

float readanalog(int Pin){
  int x = analogRead(Pin);
  float voltx = (x*3.3)/4096;
  return voltx;
}

void setup() {
  Serial.begin(115200);  //Initialize serial
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }

  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {
  unsigned long int auxTimer = micros();
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }

  getfreqv();
  getvef();
  getfreqi();
  getief();
  getactivepower();
  getapparentpower();
  getphi();

  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Channel update successful.");
  }
  else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  while (micros() - auxTimer < 60000000) {}

  // set the fields with the values
  ThingSpeak.setField(1, vef);
  ThingSpeak.setField(2, freqv);
  ThingSpeak.setField(3, ief);
  ThingSpeak.setField(4, freqi);
  ThingSpeak.setField(5, p);
  ThingSpeak.setField(6, s);
  ThingSpeak.setField(7, phi);

}
