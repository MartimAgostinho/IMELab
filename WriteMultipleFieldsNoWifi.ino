
#define VOLTPIN 34
#define CURRENTPIN 35
#define VOLTAGEGAIN 1320
#define VOLTAGEOFFSET 1.5
#define SAMPLEPERIOD 200      // unidade - us
#define CURRENTOFFSET 1.5
#define CURRENTGAIN 20

bool voltSig = false; // false = low part of the amplitute true = high
bool powerSig = false; // false = low part of the amplitute true = high

// Initialize our values
float instVolt;
float instCurrent;
float RealPower;
float vef;
float freqv;
float ief;
float freqi;
float p;
float s;
float phi;
float avgVef;
float avgIef;

unsigned long int nSamples = 0;
unsigned long int auxTimer;
unsigned long int timeStart;

unsigned long int period;
unsigned long int nPeriod;
unsigned long int periodStart;
unsigned long int periodStartPower;
int SampleTime;
int avgSample;
int avgPeriod;
int sampleTimePower;

float getfreqv() {
  return 1000000/avgSample;
}

float getVef() {
  return ReadVoltage()/sqrt(2);
}

float getMedVef() {
  return (avgVef * nSamples + getVef())/(nSamples + 1);
}

float getief() {
  return ReadCurrent()/sqrt(2);
}

float getMedIef() {
  return (avgIef * nSamples + getief())/(nSamples + 1);
}

float getapparentpower() {
  return avgIef * avgVef;
}

void getfreqi() {
  freqi = random(0, 100);
}

float getActivePower() {
  return avgIef * avgVef * cos(phi);
}

void getphi() {
  phi = random(0, 100);
}

float ReadVoltage() {
  int x = analogRead(VOLTPIN);
  float voltx = x * 3.3 / 4095;
  return voltx - VOLTAGEOFFSET * VOLTAGEGAIN;
}

float ReadCurrent() {
  int x = analogRead(CURRENTPIN);
  float ic = x * 3.3 / 4095;
  return ic - CURRENTOFFSET * CURRENTGAIN;
}

void setup() {
  Serial.begin(115200);  //Initialize serial
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo natinstVolte USB port only
  }
  //wait to have the star of a wave
  while (instVolt > 0) {
    instVolt = ReadVoltage();
  };
  while (instVolt < 0) {
    instVolt = ReadVoltage();
  };
  voltSig = true; //start allways in the beggining of a up wave
  
  auxTimer = micros();
  timeStart = micros();
  period = 0;
  SampleTime = 0;
  avgSample = 0;
}

void loop() {

  float Saux;
  
  auxTimer = micros();
  nSamples = 0;
  nPeriod  = 0;
  avgSample = 0;
  periodStart = micros();
  periodStartPower = micros();
  while (micros() - auxTimer < 60000000) {
    timeStart = micros();
    instVolt = ReadVoltage();
    instCurrent = ReadCurrent();

    if ((instVolt > 0) && !voltSig) { // means it got positinstVolte
      voltSig = true;
      
      SampleTime = micros() - periodStart;
      periodStart = periodStart + SampleTime;
      
      avgPeriod = (avgPeriod * nPeriod + periodStart)/(nPeriod + 1);
      nPeriod++;
    }
    if ((instVolt < 0) && voltSig)
      voltSig = false;

    Saux = instVolt * instCurrent;

    if( S > 0){
      RealPower += S*SAMPLEPERIOD/;
    }
    else{
      ReactivePower -= S*SAMPLEPERIOD;
    }
    
    while ( micros() - timeStart < SAMPLEPERIOD ) {}
    nSamples++;
  }
  getfreqv();
  getapparentpower();
}
