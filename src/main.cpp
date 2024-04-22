#include <Arduino.h>

const int pinPWMA = 2;
const int pinAIN2 = 3;
const int pinAIN1 = 4;
const int pinBIN1 = 6;
const int pinBIN2 = 7;
const int pinPWMB = 8;
const int pinSTBY = 5;

const int MOTR_P1 = 9;
const int MOTR_P2 = 10;
const int MOTL_P1 = 11;
const int MOTL_P2 = 12;
volatile int MOTR_TICKS =0;
volatile int MOTL_TICKS =0;

volatile int LAST_MOTR =0;
volatile int LAST_MOTL =0;

const int waitTime = 1; 
const int speed = 80;  // 0-255

const int pinMotorA[3] = { pinPWMA, pinAIN2, pinAIN1 };
const int pinMotorB[3] = { pinPWMB, pinBIN1, pinBIN2 };

enum moveDirection {
  forward,
  backward
};

enum turnDirection {
  clockwise,
  counterClockwise
};


//Funciones que controlan los motores
void moveMotorForward(const int pinMotor[3], int speed)
{
  digitalWrite(pinMotor[1], HIGH);
  digitalWrite(pinMotor[2], LOW);

  analogWrite(pinMotor[0], speed);
}

void moveMotorBackward(const int pinMotor[3], int speed)
{
  digitalWrite(pinMotor[1], LOW);
  digitalWrite(pinMotor[2], HIGH);

  analogWrite(pinMotor[0], speed);
}

void stopMotor(const int pinMotor[3])
{
  digitalWrite(pinMotor[1], LOW);
  digitalWrite(pinMotor[2], LOW);

  analogWrite(pinMotor[0], 0);
}

void enableMotors()
{
  digitalWrite(pinSTBY, HIGH);
}

void disableMotors()
{
  digitalWrite(pinSTBY, LOW);
}


//Funciones que controlan el vehiculo
void move(int direction, int speed)
{
  if (direction == forward)
  {
    moveMotorForward(pinMotorA, speed);
    moveMotorForward(pinMotorB, speed);
  }
  else
  {
    moveMotorBackward(pinMotorA, speed);
    moveMotorBackward(pinMotorB, speed);
  }
}

void turn(int direction, int speed)
{
  if (direction == clockwise)
  {
    moveMotorForward(pinMotorA, speed);
    moveMotorBackward(pinMotorB, speed);
  }
  else
  {
    moveMotorBackward(pinMotorA, speed);
    moveMotorForward(pinMotorB, speed);
  }
}

void fullStop()
{
  disableMotors();
  stopMotor(pinMotorA);
  stopMotor(pinMotorB);
}


void splitStringToInt(String str, char delimiter, int numbers[], int& count) {
    int idx = 0;
    int start = 0;
    int len = str.length();
    count = 0; // This will count how many numbers we have

    while (idx >= 0 && idx < len) {
        idx = str.indexOf(delimiter, start);
        String part = (idx >= 0) ? str.substring(start, idx) : str.substring(start, len);
        numbers[count++] = part.toInt(); // Convert the substring to an integer and store it
        start = idx + 1; // Move start up past the delimiter
    }
}

void INTERRUPT_MOTR(){
  if (digitalRead(MOTR_P1)==digitalRead(MOTR_P2)){
    MOTR_TICKS++;
  }else{
    MOTR_TICKS--;
  }
}

void INTERRUPT_MOTL(){
  if (digitalRead(MOTL_P1)==digitalRead(MOTL_P2)){
    MOTL_TICKS++;
  }else{
    MOTL_TICKS--;
  }
}


void setup()
{ Serial.begin(57600);
  pinMode(pinAIN2, OUTPUT);
  pinMode(pinAIN1, OUTPUT);
  pinMode(pinPWMA, OUTPUT);
  pinMode(pinBIN1, OUTPUT);
  pinMode(pinBIN2, OUTPUT);
  pinMode(pinPWMB, OUTPUT);
  pinMode(MOTR_P1, INPUT_PULLUP);
  pinMode(MOTR_P2, INPUT_PULLUP);
  pinMode(MOTL_P1, INPUT_PULLUP);
  pinMode(MOTL_P2, INPUT_PULLUP);
  
  enableMotors();
}

void loop()
{ 
  int motrp1 = digitalRead(MOTR_P1);
  int motrp2 = digitalRead(MOTR_P2);
  int motlp1 = digitalRead(MOTL_P1);
  int motlp2 = digitalRead(MOTL_P2);
  if (motrp1!=LAST_MOTR){
    if (motrp1==motrp2){
      MOTR_TICKS++;
    }else{
      MOTR_TICKS--;
    }
    LAST_MOTR = motrp1;
  }

  if (motlp1!=LAST_MOTL){
    if (motlp1==motlp2){
      MOTL_TICKS++;
    }else{
      MOTL_TICKS--;
    }

    LAST_MOTL = motlp1;
  }
  if (Serial.available() > 0) {
    int numbers[6]; // Array to hold the split numbers
    String data = Serial.readStringUntil('\n');
    int count = 0; // Number of parts split
    splitStringToInt(data, ',', numbers, count);
    if (numbers[0]>0){
      Serial.print("Forward,");
      Serial.print(numbers[0]);
      Serial.print(",");
      Serial.print(MOTR_TICKS);
      Serial.print(",");
      Serial.println(MOTL_TICKS);
      move(forward, abs(numbers[0]));
    }else if (numbers[0]<0){
      Serial.print("Backwards,");
      Serial.print(numbers[0]);
      Serial.print(",");
      Serial.print(MOTR_TICKS);
      Serial.print(",");
      Serial.println(MOTL_TICKS);
      move(backward, abs(numbers[0]));
    }else if (numbers[5]>0){
      Serial.print("Clockwise,");
      Serial.print(numbers[5]);
      Serial.print(",");
      Serial.print(MOTR_TICKS);
      Serial.print(",");
      Serial.println(MOTL_TICKS);
      turn(clockwise, abs(numbers[5]));
    }else if (numbers[5]<0){
      Serial.print("Counterclockwise,");
      Serial.print(numbers[5]);
      Serial.print(",");
      Serial.print(MOTR_TICKS);
      Serial.print(",");
      Serial.println(MOTL_TICKS);
      turn(counterClockwise, abs(numbers[5]));
    }else{
      Serial.print("Stopped");
      Serial.print(",");
      Serial.print(MOTR_TICKS);
      Serial.print(",");
      Serial.println(MOTL_TICKS);
      stopMotor(pinMotorA);
      stopMotor(pinMotorB);
    }
  }
  delay(waitTime);
}
