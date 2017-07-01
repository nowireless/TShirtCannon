/* Sweep
 by BARRAGAN <http://barraganstudio.com> 
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/ 

#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
                // twelve servo objects can be created on most boards
 
int pos = 0;    // variable to store the servo position 

const int SERVO_IN_PIN = 21;

volatile int pwm_value = 0;
volatile int prev_time = 0;

void rising() {
  prev_time = micros();
  attachInterrupt(digitalPinToInterrupt(SERVO_IN_PIN), falling, FALLING);
  
}

void falling() {
  pwm_value = micros() - prev_time;
  attachInterrupt(digitalPinToInterrupt(SERVO_IN_PIN), rising, RISING);
}

 
void setup() 
{ 
  Serial.begin(9600);
  pinMode(SERVO_IN_PIN, INPUT);
  
  attachInterrupt(digitalPinToInterrupt(SERVO_IN_PIN) , rising, RISING);

  myservo.attach(9);  // attaches the servo on pin 9 to the servo object 
} 
 
void loop() 
{ 
  for(int i = 1000; i <= 2000; i += 100) {
    myservo.writeMicroseconds(i);
    for(int j = 0; j <= 50; j++) {
      Serial.print(i);
      Serial.print(": ");
      Serial.println(pwm_value);

     delay(20);
    }
  }
} 

