/*
  A basic PID class. 
  by Axel Brinkeby.
  2016-05-08
*/

#ifndef Pid_h
#define Pid_h

#include "Arduino.h"

class Pid
{
  public: 
    Pid(float p, float i, float d, float limit); 
    float updatePID(float target, float current, float deltaTime); 
    void resetPID(); 
    
    void setP(float p); 
    void setI(float i); 
    void setD(float d); 


    float getP(); 
    float getI(); 
    float getD(); 
   

  private: 
    float _P; 
    float _I; 
    float _D; 
    float _limit; 
    float _integratedError; 
    float _lastError; 
    float _result;
};

#endif
