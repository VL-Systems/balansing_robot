//Подключаем библиотеки и файлы из текущей папки
#include "mpu6050.h"
#include "complementaryFilter.h"
#include "pinConfig.h"
#include "config.h"
#include <Wire.h>
#include "pid.h"

//Вызов IMU датчика
Mpu6050 imu = Mpu6050(MPU6050_ADDRESS);
//Вызов комплиментарного фильтра
ComplementaryFilter angleFilter; 

//Вызов ПИД регулятора для скорости
Pid pid_a(Kp_a, Ki_a, Kd_a,limit_a);
//Вызов ПИД регулятора для угла
Pid pid_s(Kp_s, Ki_s, Kd_s,limit_s);
//Вызов ПИД регулятора для нулевого положения
Pid pid_p(Kp_p, Ki_p, Kd_p, limit_p);

int t=0;
float zero=-2.3;//Смещение нулевого угла из за неидеальности робота, вводиться индивидуально
float targetAngle = 0;//Желаемый угол 
float currentLeanAngle = 0.0f;//Текущий угол
int8_t directionMotor1 = 0;//Направление мотора 1
int8_t directionMotor2 = 0;//Направление мотора 2
int targetSpeed = 0;//Желаемая линейная скорость 
int motorSpeed = 0;//Текущая линейная скорость
int turningSpeed = 0; //Желаемая угловая скорость
float targetPosition = 0; //Желаемое положение    
float currentPosition = 0; //Текущее положение 
boolean balancing = false; //Статус балансировки


void setup() {
  Serial.begin(115200);
  pinMode(STEPPER_1_STEP_PIN, OUTPUT);
  pinMode(STEPPER_1_DIR_PIN, OUTPUT);
  pinMode(STEPPER_2_STEP_PIN, OUTPUT);
  pinMode(STEPPER_2_DIR_PIN, OUTPUT);
  pinMode(led_indicator, OUTPUT);
  
  digitalWrite(led_indicator, LOW);  
    
  digitalWrite(STEPPER_1_ENABLE_PIN, HIGH); // Выключаем мотор 1
  digitalWrite(STEPPER_2_ENABLE_PIN, HIGH); // Выключаем мотор 2

  Wire.begin();                         
  Wire.setClock(400000); 
  imu.init(); 
  imu.calibradeGyro();
  Serial.print("ok");

 // TIMER1 controls motor 1
  TCCR1A = 0;                             // Timer1 CTC mode 4, OCxA,B outputs disconnected
  TCCR1B = (1 << WGM12) | (1 << CS11);    // Prescaler=8, => 2Mhz
  OCR1A = 65535;                          // longest period, motor stopped
  TCNT1 = 0;

  // TIMER3 controls motor 2
  TCCR3A = 0;                             // Timer1 CTC mode 4, OCxA,B outputs disconnected
  TCCR3B = (1 << WGM12) | (1 << CS11);    // Prescaler=8, => 2Mhz
  OCR3A = 65535;                          // longest period, motor stopped
  TCNT3 = 0;

  TIMSK1 |= (1 << OCIE1A);    // Enable TIMER1 interrupt
  TIMSK3 |= (1 << OCIE3A);    // Enable TIMER3 interrupt

}

// TIMER 1: controlling motor 1
ISR(TIMER1_COMPA_vect)
{
  TCNT1 = 0;  
  if (directionMotor1 == 0)
    return;
    
  PORTB |= (1<<PORTB5); //set bit       // same as digitalWrite(STEPPER_1_STEP_PIN, HIGH); 
  delay_05us();
  PORTB &= ~(1<<PORTB5); //clear bit    // same as digitalWrite(STEPPER_1_STEP_PIN, LOW);
}

// TIMER 3: controlling motor 2
ISR(TIMER3_COMPA_vect)
{
  TCNT3 = 0;  
  if (directionMotor2 == 0)
    return;
    
  PORTE |= (1<<PORTE3); //set bit       // same as digitalWrite(STEPPER_2_STEP_PIN, HIGH); 
  delay_05us();
  PORTE &= ~(1<<PORTE3); //clear bit    // same as digitalWrite(STEPPER_2_STEP_PIN, LOW);
}
int new_speed = 0;
float angle = 0;
void loop() {
  imu.updateIMUdata(); 
  // Вычисляем угол отклонения, и передаем его в комплемемнтарный фильтр 
  //float accAngle = atan2(imu.getAccelY(), imu.getAccelX()) * 57;  
  float accAngle = atan(imu.getAccelY()/(sqrt(pow(imu.getAccelX(),2)+pow(imu.getAccelZ(),2))))* 57;   
  angle = angleFilter.calculate(accAngle, imu.getGyroY(), TARGET_DELTA_TIME); 
  currentLeanAngle = angle * 0.7 + currentLeanAngle * (1 - 0.7);
  // вычисляем скорости через 3 пид регулятора 
  //targetSpeed = pid_p.updatePID(targetPosition, currentPosition, TARGET_DELTA_TIME);
  float targetAngle = pid_s.updatePID(targetSpeed, motorSpeed, TARGET_DELTA_TIME)+zero; 
  motorSpeed = -pid_a.updatePID(targetAngle, currentLeanAngle, TARGET_DELTA_TIME);

   new_speed = angleFilter.calculate(accAngle, imu.getGyroX(), TARGET_DELTA_TIME);
    // motor speed is converted from rotation per second to steps per second
    int16_t leftMotorSpeed = motorSpeed;
    int16_t rightMotorSpeed = motorSpeed; 
    setMotorSpeed(leftMotorSpeed, 2);
    setMotorSpeed(rightMotorSpeed, 1);
    digitalWrite(STEPPER_1_ENABLE_PIN, LOW); // Включаем мотор 1
    digitalWrite(STEPPER_2_ENABLE_PIN, LOW); // Включаем мотор 2
  
  
  //Выводим различные параметры для отладки
    Serial.print("currentLeanAngle: ");
    Serial.print(currentLeanAngle);
    Serial.print(" t: ");
    Serial.print(t);
    Serial.print(" targetSpeed: ");
    Serial.print(targetSpeed);
    Serial.print(" targetAngle: ");
    Serial.print(targetAngle);
    Serial.print(" motorSpeed: ");
    Serial.println(motorSpeed);
}

// Функция управления моторами
void setMotorSpeed(int16_t tspeed, int motorID)
{
  long timer_period;
  int16_t motorspeed = tspeed;
  
  if (motorID == 1) {
    if (motorspeed > 0) {
      timer_period = 2000000 / motorspeed; // 2Mhz timer
      directionMotor1 = 1;
      digitalWrite(STEPPER_1_DIR_PIN, LOW);
    } else if (motorspeed < 0) {
      timer_period = 2000000 / -motorspeed;
      directionMotor1 = -1;
      digitalWrite(STEPPER_1_DIR_PIN, HIGH);
    } else {
      timer_period = 65535;
      directionMotor1 = 0;
    }

    if (timer_period > 65535)   // Check for maximun period without overflow
      timer_period = 65535;

    OCR1A = timer_period;  
    if (TCNT1 > OCR1A)    // Check  if we need to reset the timer...
      TCNT1 = 0;
    
  } else if (motorID == 2){
    if (motorspeed > 0) {
      timer_period = 2000000 / motorspeed; // 2Mhz timer
      directionMotor2 = 1;
      digitalWrite(STEPPER_2_DIR_PIN, HIGH);
    } else if (motorspeed < 0) {
      timer_period = 2000000 / -motorspeed;
      directionMotor2 = -1;
      digitalWrite(STEPPER_2_DIR_PIN, LOW);
    } else {
      timer_period = 65535;
      directionMotor2 = 0;
    }

    if (timer_period > 65535)   // Check for maximun period without overflow
      timer_period = 65535;

    OCR3A = timer_period;  
    if (TCNT3 > OCR3A)    // Check  if we need to reset the timer...
      TCNT3 = 0;
  } 
  
}
