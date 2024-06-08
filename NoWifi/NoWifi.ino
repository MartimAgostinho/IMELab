#define VOLTPIN 34
#define CURRENTPIN 35
#define VOLTAGEGAIN 264
//#define VOLTAGEOFFSET 1.5
#define SAMPLEPERIOD 200      // unidade - us
//#define CURRENTOFFSET 1.5
#define CURRENTGAIN 20
#define MINPERIOD 10000
#define SENDPERIOD 15000000

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
  Serial.print("freqV: ");
  Serial.println(freqv);
  getfreqc();
  Serial.print("FreqI: ");
  Serial.println(freqc);
  getief();
  Serial.print("Ief: ");
  Serial.println(ief);
  getvef();
  Serial.print("Vef: ");
  Serial.println(vef);
  getapparentpower();
  Serial.print("Apparent Power: ");
  Serial.println(apparentPower);
  getphi();
  Serial.print("Cos phi: ");
  Serial.println(phi);

  Serial.print("Real Power: ");
  Serial.println(avgRealPower);
}
