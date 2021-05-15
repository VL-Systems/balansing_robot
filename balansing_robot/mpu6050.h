
/*
    A class to read raw data from the MPU6050 IMU.
  this class also contains functionality to calibrate 
  the gyros and scale the data from the accelerometers 
  and gyros to G:s and degs/sek.

  The accelerometer can also be calibrated manually by 
  setting the initialization values in this header file. 
  
  Axel Brinkeby.
  2016-08-14
*/

#ifndef Mpu6050_h
#define Mpu6050_h

#include "Arduino.h"
#include <Wire.h>

class Mpu6050
{
  public: 
    Mpu6050(int addr); 

    void init(); 
    void calibradeGyro(); 
    void calibrateAccel(); 
    void updateIMUdata(); 
    void serialPrintDebug(); 

    float getAccelX(); 
    float getAccelY();
    float getAccelZ();
    float getGyroX();
    float getGyroY();
    float getGyroZ();
    float getTemp();
      
  private: 

    const int NUMEBR_OF_CALLIBATION_ITTERATIONS = 500; 
    const double GYRO_SCALE_FACTOR = 1 / (float)131;
    double _accScaleFactor = 1 / (float)8192;

    int _address; 

    float _AcXcalibationOffset = 0, _AcYcalibationOffset = -300, _AcZcalibationOffset = 0;
    float _GyXcalibationOffset = 0, _GyYcalibationOffset = 0, _GyZcalibationOffset = 0;
    int16_t _AcX = 0, _AcY = 0, _AcZ = 0, _Tmp = 0, _GyX = 0, _GyY = 0, _GyZ = 0;
    float _accelX = 0, _accelY = 0, _accelZ = 0, _gyroX = 0, _gyroY = 0, _gyroZ = 0; 
};

#endif
