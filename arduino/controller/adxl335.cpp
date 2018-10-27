#include "adxl335.h"
#include "Arduino.h"
//#define USE_SERIAL
Adxl335::Adxl335() {
/*  value_x = 0.0;
  value_y = 0.0;
  value_z = 0.0;
  
  calibrating = true ;
  calibrateCnt = 0;*/
  calibrate();
}


void Adxl335::calibrate() {
  value_x = 0.0;
  value_y = 0.0;
  value_z = 0.0;
  calibrating = true;
  calibrateCnt = 0;
}

boolean Adxl335::updateReadings() {

  

  
  rawValueX = analogRead(1);
  rawValueY = analogRead(2);
  rawValueZ = analogRead(3);
  
  #ifdef USE_SERIAL
  Serial.print ("Raw: ");
  Serial.print ("x= ");
  Serial.print (rawValueX);
  Serial.print(", ");
  Serial.print ("y= ");

  Serial.print (rawValueY);
  Serial.print(", ");
  Serial.print ("z= ");
  Serial.print (rawValueZ);
  
  Serial.print("\n");
  #endif
  // Apply low pass filter and store value
  value_x = rawValueX * FILTER_ALPHA +(value_x * (1.0 - FILTER_ALPHA));
  value_y = rawValueY * FILTER_ALPHA +(value_y * (1.0 - FILTER_ALPHA));
  value_z = rawValueZ * FILTER_ALPHA +(value_z * (1.0 - FILTER_ALPHA));
  
  
  #ifdef USE_SERIAL
  Serial.print ("Raw: ");
  Serial.print ("rx= ");
  Serial.print (value_x);
  Serial.print(", ");
  Serial.print ("ry= ");

  Serial.print (value_y);
  Serial.print(", ");
  Serial.print ("rz= ");
  Serial.print (value_z);
  
  Serial.print("\n");
  #endif

  accelX=(value_x/1024.0*ADC_ref-zero_x)/sensitivity_x;
  accelY=(value_y/1024.0*ADC_ref-zero_y)/sensitivity_y;
  accelZ=(value_z/1024.0*ADC_ref-zero_z)/sensitivity_z;
 
  #ifdef USE_SERIAL
  Serial.print ("Accel: ");
  Serial.print ("x= ");
  Serial.print (xv);
  Serial.print(" g ");
  Serial.print ("y= ");

  Serial.print (yv);
  Serial.print(" g ");
  Serial.print ("z= ");
  Serial.print (zv);
  Serial.print(" g ");
  
  Serial.print("\n");
  #endif


  #ifdef USE_SERIAL
  Serial.print ("Roll, Pitch: ");
  Serial.print (rollDegrees);
  Serial.print(", ");
  Serial.print (pitchDegrees);
  Serial.print("\n");
  #endif


  #ifdef USE_SERIAL
  Serial.print("Calibrate: ");
  Serial.print("pOff= ");
  Serial.print(pitchOffset);
  Serial.print(" rOff= ");
  Serial.print(rollOffset);
  Serial.print("\n");
  #endif
  
  #ifdef USE_SERIAL
  delay(300);
  #endif
  
  
  if (calibrating) {
    calibrateCnt++ ;
   
    if (calibrateCnt > 100) {
      offsetX = accelX;
      offsetY = accelY;
      offsetZ = accelZ;
      calibrating = false ;
    }
    return false ;
  }
  accelX -= offsetX;
  accelY -= offsetY;
  accelZ -= offsetZ;
  
  return true ;
}

