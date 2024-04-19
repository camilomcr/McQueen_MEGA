#include <Arduino.h>

const int pinPWMA = 2;
const int pinAIN2 = 3;
const int pinAIN1 = 4;
const int pinBIN1 = 6;
const int pinBIN2 = 7;
const int pinPWMB = 8;
const int pinSTBY = 5;

const int waitTime = 250; 
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


void setup()
{ Serial.begin(9600);
  pinMode(pinAIN2, OUTPUT);
  pinMode(pinAIN1, OUTPUT);
  pinMode(pinPWMA, OUTPUT);
  pinMode(pinBIN1, OUTPUT);
  pinMode(pinBIN2, OUTPUT);
  pinMode(pinPWMB, OUTPUT);
  
  enableMotors();
  move(forward, speed);
  delay(waitTime);
  fullStop();
  delay(waitTime);
}

void loop()
{
  if (Serial.available() > 0) {
    int numbers[6]; // Array to hold the split numbers
    String data = Serial.readStringUntil('\n');
    int count = 0; // Number of parts split
    splitStringToInt(data, ',', numbers, count);
    if (numbers[0]>0){
      Serial.print("Forward ");
      Serial.println(numbers[0]);
      enableMotors();
      move(forward, abs(numbers[0]));
      delay(waitTime);
    }else if (numbers[0]<0){
      Serial.print("backwards ");
      Serial.println(numbers[0]);
      enableMotors();
      move(backward, abs(numbers[0]));
      delay(waitTime);
    }else if (numbers[5]>0){
      Serial.print("Clockwise ");
      Serial.println(numbers[5]);
      enableMotors();
      turn(clockwise, abs(numbers[5]));
      delay(waitTime);
    }else if (numbers[5]<0){
      Serial.print("Counterclockwise ");
      Serial.println(numbers[5]);
      enableMotors();
      turn(counterClockwise, abs(numbers[5]));
      delay(waitTime);
    }else{
      Serial.println("Stopped");
      fullStop();
      delay(waitTime);
    }
  }
}
