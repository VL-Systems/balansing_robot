const int STEPPER_2_STEP_PIN = 11;   //4
const int STEPPER_2_DIR_PIN = 13;    //5
const int STEPPER_1_STEP_PIN = 5;   //6
const int STEPPER_1_DIR_PIN = 7;    //7
const int STEPPER_2_ENABLE_PIN = 15; //8
const int STEPPER_1_ENABLE_PIN = 9; //9


int8_t directionMotor1 = 0;
int8_t directionMotor2 = 0;

void setup() {
  pinMode(STEPPER_1_STEP_PIN, OUTPUT);
  pinMode(STEPPER_1_DIR_PIN, OUTPUT);
  pinMode(STEPPER_2_STEP_PIN, OUTPUT);
  pinMode(STEPPER_2_DIR_PIN, OUTPUT);

  digitalWrite(STEPPER_1_ENABLE_PIN, HIGH); // disable the stepper motors
  digitalWrite(STEPPER_2_ENABLE_PIN, HIGH); // disable the stepper motors

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

// 16 single cycle instructions is 1 microsecond at 16Mhz
void delay_05us()
{
  __asm__ __volatile__ (
    "nop" "\n\t"    "nop" "\n\t"    "nop" "\n\t"    "nop" "\n\t"
    "nop" "\n\t"    "nop" "\n\t"    "nop" "\n\t"    "nop");
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
void loop() {

  setMotorSpeed(-10500, 2);
  setMotorSpeed(500, 1);
    digitalWrite(STEPPER_1_ENABLE_PIN, LOW); // enable the stepper motors
    digitalWrite(STEPPER_2_ENABLE_PIN, LOW); // enable the stepper motors

}

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
