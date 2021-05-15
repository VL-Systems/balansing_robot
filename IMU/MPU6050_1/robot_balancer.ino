int IN1 = 7; // Input1 подключен к выводу 5 
int IN2 = 6;
int IN3 = 5;
int IN4 = 4;
int ENA = 9;
int ENB = 3;
int i;
void Motor()
{
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(IN3, OUTPUT);
  
  
  
  analogWrite(ENA, 255);
    analogWrite(ENB, 255);
}
void MinAlfa(){digitalWrite(IN2, HIGH);
  digitalWrite(IN1, LOW);
  digitalWrite(IN4, HIGH);
  digitalWrite(IN3, LOW);
  }
  void MaxAlfa(){
    digitalWrite(IN2, LOW);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN4, LOW);
  digitalWrite(IN3, HIGH);}
