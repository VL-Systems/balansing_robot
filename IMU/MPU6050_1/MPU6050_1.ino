#include <Wire.h> // подключение библиотеки для I2C 
#include "gyro_accel.h"// подключение библиотеки для гироскопа и акселирометра mpu 6050
// Defining constants
#define dt 20                       // промежуток времени в милисикундах 
#define rad2degree 57.3              // переменная для перевода из радиан в градусы 
#define Filter_gain 0.95             // переменная для фильтра angle = angle_gyro*Filter_gain + angle_accel*(1-Filter_gain)
// *********************************************************************
//    Global Variables
// *********************************************************************
unsigned long t=0; // Переменная времени 
float y=0,angle_x_gyro=0,angle_y_gyro=0,angle_z_gyro=0,angle_x_accel=0,angle_y_accel=0,angle_z_accel=0,angle_x=0,angle_y=0,angle_z=0;
// *********************************************************************
// Основной код 
void setup(){
  Serial.begin(115200);//Общение с компьютером при 115200
  Wire.begin();// Налало работы с I2C шиной , в (адресс устройства  ), если не назван адресс , то плата подключается к шина  как  мастер  
  MPU6050_ResetWake();// Сбрасывает настройки по умолчанию 
  MPU6050_SetGains(0,1);// Настраивает максимальные значения шкалы измерений гироскопа и акселерометра 
  MPU6050_SetDLPF(0); //Настройка фильтра низких частот 
  MPU6050_OffsetCal();// Каллебровка гироскопа и акселерометра 
  MPU6050_SetDLPF(6); // Настройка фильтра низких частот
  
  
  
  
  
  Serial.print("\tangle_x_accel");
  Serial.print("\tangle_y_accel");
  Serial.print("\tangle_z_accel");
  
  Serial.print("\tangle_x");
  Serial.print("\tangle_y");
  Serial.print("\tangle_z");
  
  Serial.println("\tLoad");

  
    
  t=millis(); 
}
void loop(){
  t=millis(); 
  
  MPU6050_ReadData();
 // Угловая скорость относительно всех осей 
  angle_x_gyro = (gyro_x_scalled*((float)dt/1000)+angle_x);
  angle_y_gyro = (gyro_y_scalled*((float)dt/1000)+angle_y);
  angle_z_gyro = (gyro_z_scalled*((float)dt/1000)+angle_z);
  //Угол поворота относительно всех осей 
  angle_z_accel = atan(accel_z_scalled/(sqrt(accel_y_scalled*accel_y_scalled+accel_x_scalled*accel_x_scalled)))*(float)rad2degree;
  angle_y_accel = -atan(accel_x_scalled/(sqrt(accel_y_scalled*accel_y_scalled+accel_z_scalled*accel_z_scalled)))*(float)rad2degree;
  angle_x_accel = atan(accel_y_scalled/(sqrt(accel_x_scalled*accel_x_scalled+accel_z_scalled*accel_z_scalled)))*(float)rad2degree;
  // Отфильтрованные значения угла поворота относительно всех осей 
  angle_x = Filter_gain*angle_x_gyro+(1-Filter_gain)*angle_x_accel;
  angle_y = Filter_gain*angle_y_gyro+(1-Filter_gain)*angle_y_accel;
  angle_z = Filter_gain*angle_z_gyro+(1-Filter_gain)*angle_z_accel;
  

 


  //Serial.print(angle_x);
 // Serial.print("\t");
  Serial.print(angle_y_accel);
 Serial.print("\t");
  //Serial.print(angle_z);
  //Serial.print("\t");
  
   
 //Вывод оси времени 
  Serial.println(((float)(millis()-t)/(float)dt)*100);

  
  while((millis()-t) < dt){ // Making sure the cycle time is equat to dt
  // Do nothing
  }
  
Motor();
if (angle_y_accel>80&&angle_y_accel<88){MinAlfa();}
else if (angle_y_accel<80&&angle_y_accel>70){MaxAlfa();}
}




  
