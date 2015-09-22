// Geiger Counter Simulator
//
// Made by Jeff Kramer - jeffkramr@gmail.com
// License: CC-BY-SA 3.0

//Pin Assignation
int geigerOut = 9;
int dialControl = 10;
int stayAwake = 11;
int slider = A0;
int LEDFlash = 13;
int LEDFlash2 = A1;

//Variables
long sliderIn = 0;
long oldsliderIn = 0;
long cps = 685; //OK for yearly rad worker.
long oldcps = 0;

const long maxCPS = 2000; //A good number for "very hot rads"
const long minCPS = 100; //A good vague number for "some radiation"
const long cpsRange = maxCPS - minCPS;

long previousMicrosGeiger = 0; // will store last time we updated the geiger
const long intervalGeiger = 5;           // interval at which to wakeup (microseconds)
const long wps = 1000000 / intervalGeiger; //wakeups per second

long previousMillisAwake = 0;
const long intervalAwake = 10000; //interval to pulse the wakeup pin
const long timeToStayAwake = 50; //how long to keep the wakeup pin high
bool awakePinState = false; //What's the awake pin state?

long previousMillisDial = 0;
const long intervalDial = 5;
byte dialOutput = 0;
const byte dialMax = 255;
float dialLevel = 0;
const float dialDecayConstant = 0.99;



// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pins as an output.
  pinMode(geigerOut, OUTPUT);
  pinMode(stayAwake, OUTPUT);
  pinMode(LEDFlash, OUTPUT);
  pinMode(LEDFlash2, OUTPUT);
  // random setup
  randomSeed(42);

  //Serial
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  unsigned long currentMicrosGeiger = micros();
  unsigned long currentMillisAwake = millis();
  unsigned long currentMillisDial = millis();

  //Set the awake pin state
  if (awakePinState == true) {
    digitalWrite(stayAwake, HIGH);
  }
  else {
    digitalWrite(stayAwake, LOW);
  }

  if (currentMillisDial - previousMillisDial > intervalDial) {
    previousMillisDial = currentMillisDial;

    //Set the dial output
    if (dialLevel > (float)dialMax) {
      dialLevel = (float)dialMax;
    }
    dialLevel = dialLevel * dialDecayConstant;
    dialOutput = dialLevel;
    analogWrite(dialControl, dialOutput);
  }

  if (currentMicrosGeiger - previousMicrosGeiger > intervalGeiger) {
    // save the last time you blinked the LED
    previousMicrosGeiger = currentMicrosGeiger;
    sliderIn = analogRead(slider) + 1;
    long sliderChunk = ((float)sliderIn / 1024) * (cpsRange);
    cps = sliderChunk + minCPS;
    if (random(wps) <= cps) {
      //GOT A HIT
      dialLevel = dialLevel + 10.0;//(float)dialMax / 500;
      digitalWrite(geigerOut, HIGH);
      digitalWrite(LEDFlash, HIGH);
      digitalWrite(LEDFlash2, HIGH);
      delay(1);
      digitalWrite(geigerOut, LOW);
      digitalWrite(LEDFlash, LOW);
      digitalWrite(LEDFlash2, LOW);
    }
  }

  if (awakePinState == false && currentMillisAwake - previousMillisAwake > intervalAwake) {
    previousMillisAwake = currentMillisAwake;
    awakePinState = true;
  }

  if (awakePinState == true && currentMillisAwake - previousMillisAwake > timeToStayAwake) {
    previousMillisAwake = currentMillisAwake;
    awakePinState = false;
  }
}
