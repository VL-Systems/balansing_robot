/*
  A complementary filter class used for sensor fusion 
  of accelerometer and gryo data in a balancing robot
  
  by Axel Brinkeby
  2016-08-19
*/

#ifndef complementaryFilter_h
#define complementaryFilter_h

#include "Arduino.h"

class ComplementaryFilter
{
  public: 
    ComplementaryFilter(); 
    float calculate(float newAngle, float newRate, float dt); 
    void resetValues(); 
      
  private: 
    float _angle; 
};

#endif
