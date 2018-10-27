// Arduino Micro P  ro
#define USE_SERIAL

#include <Wire.h>

#include <Bounce2.h>
#include "adxl335.h" 

#ifdef USE_SERIAL
#define serialOut(x) Serial.print(x)
#define serialOutln(x) Serial.println(x)
#else
#define serialOut(x) (void)0
#define serialOutln(x) (void)0
#endif
// Indices pointing to key position in 'keys' array
#define KEY_PLUNGER      2

// Number of keys in arrays
#define CONTROL_KEY_COUNT 6

// When this pin is pulled low, no keys are scanned for, or written to OS
// TODO: IMPLEMENT DISABLE SWITCH
#define PIN_DISABLE_CONTROLLER 8

#define PIN_CALIBRATE_ALLCELEROMETER 10
#define UNUSED_PIN_COUNT 4

int debounceTime = 5;
int ledPin = 17;
int pins[CONTROL_KEY_COUNT] = {
  14, // Left Flipper
  15, // Right Flipper
  4,  // Plunger
  5,  // Escape
  6,  // Player
  7  // Coin
};

// Set ununsed inputs as input with pullup resisitor
int unused_pins[UNUSED_PIN_COUNT] = {
0,1,9,16
};
JoyState_t joySt;

int states[CONTROL_KEY_COUNT] = {LOW,LOW,LOW,LOW,LOW,LOW};
Bounce debouncers[CONTROL_KEY_COUNT];
byte keys[9] = {
  0x81,  // Left flipper  (Left shift)
  0x85,  // Right flipper (Right shift)
  0xB0,  // Plunger       (Enter)
  0xB1,  // Escape
  '1',   // Player
  '5',   // Coin
  'Z',   // Left nudge
  0x2F,   // Right nudge '-'
  ' '    // Tilt center
};

Adxl335 adxl335 ;
int minPlungerReading ;
int maxPlungerReading ;
float filteredPlungerReading;
void setup()
{
  #ifdef USE_SERIAL
  Serial.begin(115200);
  #endif
  
  Keyboard.begin();
  minPlungerReading = 5000;
  maxPlungerReading = -5000;
  filteredPlungerReading = 0;

   pinMode(2, INPUT_PULLUP);
   pinMode(3, INPUT_PULLUP);

  for (int cnt=0; cnt < CONTROL_KEY_COUNT; cnt++) {
    pinMode(pins[cnt], INPUT_PULLUP);
    debouncers[cnt] = Bounce();
    debouncers[cnt].attach(pins[cnt]);
    debouncers[cnt].interval(debounceTime);
  }
 
   // Analog pins
   pinMode(18, INPUT_PULLUP);
   digitalWrite(18, HIGH);


  for (int cnt=0; cnt < UNUSED_PIN_COUNT;cnt++){
       pinMode(unused_pins[cnt], INPUT_PULLUP);
  }
  pinMode(PIN_DISABLE_CONTROLLER, INPUT_PULLUP);
  pinMode(PIN_CALIBRATE_ALLCELEROMETER, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  //digitalWrite(ledPin, HIGH);
}

boolean ledOn = false ;
int ledCounter = 0 ;

void loop()
{
  int rawJoyReading = analogRead(0);
  filteredPlungerReading = (float)rawJoyReading * FILTER_ALPHA +(filteredPlungerReading * (1.0 - FILTER_ALPHA));
  if (digitalRead(PIN_CALIBRATE_ALLCELEROMETER) == LOW) {
    // Re-calibrate accelerometer and just return
    adxl335.calibrate();
    return;
  }

  boolean accelValuesUpdated = adxl335.updateReadings();
  if (!accelValuesUpdated) {
    return;
  }
  // mul by 200 and then div by 200 to get better resolution

  // Sensor is mounted wrong way, so compensate for that by inverting axis
  //float joyValueX = map(adxl335.accelX*200, -400, 400, 0, 255) ;
  float oldxAxis = joySt.xAxis;
  float oldyAxis = joySt.yAxis ;
  float joyValueX = map(adxl335.accelX*200, -400, 400, 255, 0) ;
  joySt.xAxis = constrain(joyValueX, 0, 255);

  float joyValueY = map(adxl335.accelY*200, -400, 400, 0, 255) ;
  joySt.yAxis = constrain(joyValueY, 0, 255);
  if (oldxAxis != joySt.xAxis || oldyAxis != joySt.yAxis) {
    serialOut(joySt.xAxis); serialOut(", "); serialOutln(joySt.yAxis);
  }
  //Serial.print(joySt.xAxis); Serial.print(", "); Serial.print(joySt.yAxis); Serial.print(", "); Serial.print(adxl335.accelZ);
  //Serial.println();
  // Do nothing while disabled
  //When unconnected, pin PIN_DISABLE_CONTROLLER is high. Pull low to disable controller
  if (digitalRead(PIN_DISABLE_CONTROLLER) == LOW) {
    //digitalWrite(ledPin, LOW);
    return;
  }

  
  filteredPlungerReading = (float)rawJoyReading * .01 +(filteredPlungerReading * (1.0 - .01));
  minPlungerReading = min((int)filteredPlungerReading, minPlungerReading);
  maxPlungerReading = max((int)filteredPlungerReading, maxPlungerReading);
  int joyReading = map(filteredPlungerReading, minPlungerReading, maxPlungerReading, 255, 0);
  joyReading = constrain(joyReading, 0, 255);
  joySt.zAxis = joyReading;
  /*Serial.print(filteredPlungerReading);
  Serial.print(", ");
  Serial.print(minPlungerReading);
  Serial.print(", ");
  Serial.print(maxPlungerReading);
  Serial.print(", ");
  Serial.println(joyReading);*/

    Joystick.setState(&joySt);

    ledCounter ++ ;
    if (ledOn && ledCounter > 25) {
      //digitalWrite(ledPin, HIGH);
      ledOn = false ;
    } else if (ledCounter > 50) {
      ledOn = true ;
      //digitalWrite(ledPin, LOW);
      ledCounter = 0;
    }
  
  
  boolean anyKeyDown = false;
  for (int cnt=0; cnt < CONTROL_KEY_COUNT; cnt++) {
    debouncers[cnt].update();
    int state = debouncers[cnt].read();
    //int state = digitalRead(pins[cnt]);
    if (state == LOW) {
      anyKeyDown = true ;
    }
    if (state != states[cnt]) {
      states[cnt] = state;
      if (state == LOW) {
        Keyboard.press(keys[cnt]);
      } else {
        Keyboard.release(keys[cnt]);
      }
    }
    if (anyKeyDown) {
      //digitalWrite(ledPin, LOW);
    } else {
      if (!ledOn) {
        //digitalWrite(ledPin, HIGH);
      }
    }
  }
}

