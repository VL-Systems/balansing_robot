/*
  A basic PID class. 
  by Axel Brinkeby.
  2016-05-08
*/

#include "Arduino.h"
#include "pid.h"

Pid::Pid(float p, float i, float d, float limit)
{
  _P = p; 
  _I = i; 
  _D = d; 
  _integratedError = 0; 
  _lastError = 0; 
  _limit = limit;
  _result = 0;
}

float Pid::updatePID(float target, float current, float deltaTime)
{
  float error = (target - current) * deltaTime; 
  
  float pPart = _P * (target - current);
  
  _integratedError += error;    
  float iPart = _I * _integratedError; 
  
  float dPart = _D * (error - _lastError)/deltaTime;    
  
  _lastError = error;
  _result = (pPart + iPart + dPart);
   if (_result > _limit)
   {_result = _limit;}
   else if (_result < -_limit)
   {_result = -_limit;}
	
	return _result; 
}

void Pid::resetPID()
{
  _integratedError = 0; 
  _lastError = 0; 
}

void Pid::setP(float p)
{
  _P = p; 
}

void Pid::setI(float i)
{
  _I = i; 
} 

void Pid::setD(float d)
{
  _D = d; 
}


float Pid::getP()
{
  return _P;  
}

float Pid::getI()
{
  return _I;  
} 

float Pid::getD()
{
  return _D;  
}
 
   
