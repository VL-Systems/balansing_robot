
/*
  A class to read raw data from the MPU6050 IMU.
  this class also contains functionality to calibrate 
  the gyros and scale the data from the accelerometers 
  and gyros to G:s and degs/sek.

  The accelerometer can also be calibrated manually by 
  setting the initialization values in the header file. 
  
  Axel Brinkeby.
  2016-08-14
*/

#include "Arduino.h"
#include "mpu6050.h"

Mpu6050::Mpu6050(int addr)
{
  _address = addr;   
}

void Mpu6050::init()
{  
  Wire.beginTransmission(_address);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}

void Mpu6050::calibradeGyro() 
{   
  // The IMU chuld not be moving while calling this callibration function. 
  for (int i = 0; i < NUMEBR_OF_CALLIBATION_ITTERATIONS; i++) {
     updateIMUdata();
     _GyXcalibationOffset += _GyX; 
     _GyYcalibationOffset += _GyY; 
     _GyZcalibationOffset += _GyZ; 
     delay(1);
  }  
  _GyXcalibationOffset = -_GyXcalibationOffset / NUMEBR_OF_CALLIBATION_ITTERATIONS;  
  _GyYcalibationOffset = -_GyYcalibationOffset / NUMEBR_OF_CALLIBATION_ITTERATIONS;
  _GyZcalibationOffset = -_GyZcalibationOffset / NUMEBR_OF_CALLIBATION_ITTERATIONS;
}

void Mpu6050::calibrateAccel() {
  // the IMU shuld be level and not moving while colling this callibration function.
  float xSum = 0, ySum = 0, zSum = 0; 
  for (int i = 0; i < NUMEBR_OF_CALLIBATION_ITTERATIONS; i++) {
     updateIMUdata();
     xSum += _AcX; 
     ySum += _AcY; 
     zSum += _AcZ; 
     delay(1);
  } 
  _AcXcalibationOffset = -xSum / NUMEBR_OF_CALLIBATION_ITTERATIONS; 
  _AcYcalibationOffset = -ySum / NUMEBR_OF_CALLIBATION_ITTERATIONS;   
  _accScaleFactor = 1.0f / (zSum / NUMEBR_OF_CALLIBATION_ITTERATIONS);  
}

void Mpu6050::updateIMUdata() 
{
  Wire.beginTransmission(_address);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  int numberOfBytes = Wire.requestFrom(_address, 14, true);  // request a total of 14 registers

  if (numberOfBytes == 14) {
    _AcX = Wire.read() << 8 | Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
    _AcY = Wire.read() << 8 | Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    _AcZ = Wire.read() << 8 | Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
    _Tmp = Wire.read() << 8 | Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    _GyX = Wire.read() << 8 | Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
    _GyY = Wire.read() << 8 | Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    _GyZ = Wire.read() << 8 | Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  
    _accelX = (_AcX + _AcXcalibationOffset) * _accScaleFactor;  // scale the accelerometer values to G:s and save as float
    _accelY = (_AcY + _AcYcalibationOffset) * _accScaleFactor; 
    _accelZ = (_AcZ + _AcZcalibationOffset) * _accScaleFactor; 
    
    _gyroX = (_GyX + _GyXcalibationOffset) * GYRO_SCALE_FACTOR;  // scale the gyros to degrees/second and save as float
    _gyroY = (_GyY + _GyYcalibationOffset) * GYRO_SCALE_FACTOR; 
    _gyroZ = (_GyZ + _GyZcalibationOffset) * GYRO_SCALE_FACTOR; 
  }
}

void Mpu6050::serialPrintDebug()
{
  Serial.print("AccelX: "); 
  Serial.print(getAccelX());
  Serial.print("  AccelY: "); 
  Serial.print(getAccelY());
  Serial.print("  AccelZ: "); 
  Serial.print(getAccelZ());

  Serial.print("  GyroX: "); 
  Serial.print(getGyroX());
  Serial.print("  GyroY: "); 
  Serial.print(getGyroY());
  Serial.print("  GyroZ: "); 
  Serial.println(getGyroZ()); 
}

float Mpu6050::getAccelX() 
{
  return _accelX;   
}

float Mpu6050::getAccelY() 
{
  return _accelY;   
}

float Mpu6050::getAccelZ() 
{
  return _accelZ;   
}

float Mpu6050::getGyroX() 
{
  return _gyroX;   
}

float Mpu6050::getGyroY() 
{
  return _gyroY;   
}

float Mpu6050::getGyroZ() 
{
  return _gyroZ;   
}

float Mpu6050::getTemp() {
  return (_Tmp / 340.00f) + 36.53f; 
}
