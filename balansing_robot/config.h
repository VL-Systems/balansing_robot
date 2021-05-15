//Переменные для ПИД регулятора скорости
// *********************************************************************
double input_a, output_a;
double Kp_a = 350;   //300 Это примеры моих значений 
double Kd_a = 0.0045;    //0.045
double Ki_a = 0.009;    //0.09
const float limit_a = 14000;  

//Переменные для ПИД регулятора угла наклона
// *********************************************************************
double input_s, output_s;
double Kp_s = 0.0006;   //0.0003 Это примеры моих значений 
double Kd_s = 0.00001;    //0.00001
double Ki_s = 0.00002;    //0.00002
const float limit_s = 5;  

//Переменные для ПИД регулятора нулевого положения
// *********************************************************************
const float Kp_p = 2;          
const float Ki_p = 0.12;          
const float Kd_p = 0.05;          
const float limit_p = 400;  



// частота обновления данных, частота общего цикла. 
const float STD_LOOP_TIME = 10000.0;  
const float TARGET_DELTA_TIME = STD_LOOP_TIME / 1000000.0; // в секунду

// I2C адресс IMU датчика
const int MPU6050_ADDRESS = 0x68;


// 16 single cycle instructions is 1 microsecond at 16Mhz
void delay_05us()
{
  __asm__ __volatile__ (
    "nop" "\n\t"    "nop" "\n\t"    "nop" "\n\t"    "nop" "\n\t"
    "nop" "\n\t"    "nop" "\n\t"    "nop" "\n\t"    "nop");
}
