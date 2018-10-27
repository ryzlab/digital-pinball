#ifndef __adxl335_h__
#define __adxl335_h__

#include "Arduino.h"

// Simple angle meter using ADXL335 accelerometer
//from electronicsblog.net/


/*
Raw analog alues when resting flat:
x: 353
y: 354
z: 420

reading/1024.0*ADC_ref

gives:
Original value for all axis: 1.569
x: 353/1024*5 = 1.7236328125
y: 354/1024*5 = 1.728515625
z: 420/1024*5 = 2.05078125

x: 353/1024*3.3 = 1.13759765625
y: 354/1024*3.3 = 1.13759765625
z: 420/1024*3.3 = 1.353515625

*/
/*
#define ADC_ref 3.3
#define zero_x 1.13759765625
#define zero_y 1.13759765625
#define zero_z 1.353515625
*/

// Voltage measured Vs -> GND on accelerometer chiw was 3.274V
#define FILTER_ALPHA 0.05

// Raw analogRead() values from accelerometer when the accelerometer
// is oriented so that each axis is in zero-g
#define MEASURED_RAW_ZEROG_X 355
#define MEASURED_RAW_ZEROG_Y 355
#define MEASURED_RAW_ZEROG_Z 355

// Voltage range of the analogRead() values 0-1023 
#define ADC_ref 5

// Define voltage (in opposite to the analog readings) when the accelerometer
// is oriented so that each axis is in zero-g
#define zero_x (MEASURED_RAW_ZEROG_X/1024.0*5.0)
#define zero_y (MEASURED_RAW_ZEROG_Y/1024.0*5.0)
#define zero_z (MEASURED_RAW_ZEROG_Z/1024.0*5.0)

// 330mV / g is the approx sensitivity in V/g, value taken from the datasheet
#define sensitivity_x 0.33
#define sensitivity_y 0.33
#define sensitivity_z 0.33

class Adxl335 {
  
  public:
    Adxl335();
    // Initiates re-calibration of accelerometer. Can be called before re-calibration finished. If so, re-calibration is triggered again
    void calibrate();
    boolean updateReadings();
    
    // Accelerator values in g, calculated from filtered analog readings
    float accelX;
    float accelY;
    float accelZ;
  
    // Raw analog readings from accelerator
    float rawValueX ;
    float rawValueY ;
    float rawValueZ ;
  
  
    // Raw analog readings from accelerator with filter applied
    float value_x;
    float value_y;
    float value_z;
    
    // Values measured during initialization. These values are subtracted from reading to fix
    // if the sensor is not aligned perfectly horizontal
    float offsetX;
    float offsetY;
    float offsetZ;
  private:
    boolean calibrating;
    int calibrateCnt;
};
#endif

