// Controlling a servo position using a potentiometer (variable resistor) 
// by Michal Rinott <http://people.interaction-ivrea.it/m.rinott> 

#include <Servo.h> 

const float MIN_REVERSE = 1460.0;
const float FULL_REVERSE = 999.0;
const float MIN_FORWARD = 1550.0;
const float MAX_FORWARD = 2003;
const float NEUTRAL = 1500;

#define DEBUG

void writeMotor(Servo &s, float speed) {
  if(speed > 0.001) {
     //Forward
    int out = (MAX_FORWARD-MIN_FORWARD)*speed+MIN_FORWARD;
    s.writeMicroseconds(out);

#ifdef DEBUG
    Serial.print("FOut:");
    Serial.println(out);
#endif
  } else if(speed < -0.001) {
    //Reverse
    int out = (FULL_REVERSE-MIN_REVERSE)*fabs(speed)+MIN_REVERSE;
    s.writeMicroseconds(out);

#ifdef DEBUG
    Serial.print("ROut:");
    Serial.println(out);
#endif
  } else {
    //Nuetral
    s.writeMicroseconds(NEUTRAL);
  }
}
 
Servo myservo;  // create servo object to control a servo 
 
int potpin = 0;  // analog pin used to connect the potentiometer
int val;    // variable to read the value from the analog pin 
 
void setup() 
{ 
  Serial.begin(9600);
  Serial.println("Hi!");
  myservo.attach(P1_2);  // attaches the servo on pin 9 to the servo object 
} 
 
void loop() 
{ 
  val = analogRead(P6_0);            // reads the value of the potentiometer (value between 0 and 1023) 
  val = map(val, 0, 4096, -100, 100);     // scale it to use it with the servo (value between 0 and 180) 
  double out = val / 100.0;
//  Serial.println(out);

  writeMotor(myservo, out);
  delay(15);                           // waits for the servo to get there 
} 
