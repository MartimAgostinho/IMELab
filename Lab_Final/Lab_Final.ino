#include <WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

#define VOLTPIN 34
#define CURRENTPIN 35
#define VOLTAGEGAIN 264
//#define VOLTAGEOFFSET 1.5
#define SAMPLEPERIOD 200      // unidade - us
//#define CURRENTOFFSET 1.5
#define CURRENTGAIN 20
#define MINPERIOD 10000
#define SENDPERIOD 60000000

/***********WIFI***********/
char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;
unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;
/***********WIFI***********/

bool voltSig    = false; // false = low part of the amplitute true = high
bool currentSig = false;

// Initialize our values
float instVolt;
float instCurrent;

float avgRealPower;
float apparentPower;

float ief;
float vef;

float freqc;
float freqv;

float phi;


unsigned long int nSamples = 0;
unsigned long int auxTimer;
unsigned long int timeStart;


unsigned int nPeriodV;
unsigned int nPeriodC;
unsigned long int periodV;
unsigned long int periodC;
unsigned long int avgPeriodV;
unsigned long int avgPeriodC;
float averageC;
float averageV;
unsigned long int periodStartV;
unsigned long int periodStartC;
float currentOffset = 1.5;
float voltageOffset = 1.5;

float ReadVoltage() {
  int x = analogRead(VOLTPIN);
  float voltx = x * 3.3 / 4095;
  return (voltx - voltageOffset) * VOLTAGEGAIN;
}

float ReadCurrent() {
  int x = analogRead(CURRENTPIN);
  float ic = x * 3.3 / 4095;
  return (ic - currentOffset) * CURRENTGAIN;
}

void getfreqv() {
  freqv = 1000000.0 / avgPeriodV;
}

void getfreqc() {
  freqc = 1000000.0 / avgPeriodC;
}

void getvef() {
  vef = sqrt( vef );
}

void getief() {
  ief = sqrt( ief );
}

void getRealpower() {
  avgRealPower = (avgRealPower * nSamples + (instCurrent * instVolt)) / (nSamples + 1);
}

void getapparentpower() {
  apparentPower = vef * ief;
}

void getphi() {
  phi = avgRealPower / apparentPower;
}

void setup() {
  Serial.begin(115200);  //Initialize serial
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo natinstVolte USB port only
  }

  /***********WIFI***********/
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  /***********WIFI***********/

  voltSig    = true; //start allways in the beggining of a up wave
  currentSig = true;

  auxTimer = micros();
  timeStart = micros();
  periodC = 0;
  periodV = 0;
}

void loop() {
  Serial.println("Hello");

  //Sync to voltage period
  while (instVolt > 0.05) {
    instVolt = ReadVoltage();
  };
  while (instVolt < -0.05) {
    instVolt = ReadVoltage();
  };

  auxTimer      = micros(); //Used to count 60 seconds
  nSamples      = 0;
  nPeriodV      = 0;
  nPeriodC      = 0;
  ief           = 0;
  vef           = 0;
  avgPeriodV    = 0;
  avgPeriodC    = 0;
  periodStartV  = auxTimer;
  periodStartC  = auxTimer;

  while (micros() - auxTimer < SENDPERIOD) {
    timeStart   = micros();
    instVolt    = ReadVoltage();
    instCurrent = ReadCurrent();

    vef = (vef * nSamples + pow(instVolt, 2)) / (nSamples + 1);
    ief = (ief * nSamples + pow(instCurrent, 2)) / (nSamples + 1);

    if ((instVolt > 0.05) && !voltSig) { // means it got positinstVolte

      periodV  = micros() - periodStartV;
      //if ( periodv > MINPERIOD  ) { Serve para existir uma frequencia minima

      periodStartV = periodStartV + periodV;
      avgPeriodV = (avgPeriodV * nPeriodV + periodV) / (nPeriodV + 1);
      nPeriodV++;
      voltSig = true;
      //}
    }
    if ((instVolt < -0.05) && voltSig)
      voltSig = false;

    //Current
    if ((instCurrent > 0.01) && !currentSig) { // means it got positinstVolte

      periodC  = micros() - periodStartC;
      //if ( periodC > MINPERIOD  ) { Serve para existir uma frequencia minima

      periodStartC = periodStartC + periodC;
      avgPeriodC = (avgPeriodC * nPeriodC + periodC) / (nPeriodC + 1);
      nPeriodC++;
      currentSig = true;
      //}
    }
    if ((instCurrent < -0.01) && currentSig)
      currentSig = false;

    getRealpower(); // add the read value to the avg Real Power value

    while ( micros() - timeStart < SAMPLEPERIOD ) {}
    nSamples++;
  }
  
  getfreqv();
  getfreqc();
  getief();
  getvef();
  getapparentpower();
  getphi();

  /***********WIFI***********/
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

  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Channel update successful.");
  }
  else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  ThingSpeak.setField(1, vef);
  ThingSpeak.setField(2, freqv);
  ThingSpeak.setField(3, ief);
  ThingSpeak.setField(4, freqc);
  ThingSpeak.setField(5, avgRealPower);
  ThingSpeak.setField(6, apparentPower);
  ThingSpeak.setField(7, phi);
  /***********WIFI***********/


}
