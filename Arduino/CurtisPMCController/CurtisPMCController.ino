#include <Adafruit_MCP4725.h>

#include <Servo.h>
#include <Wire.h>

#include "Protocol.h"

#define MCP4725_ADDR 0x62

const int KEY_SWITCH = P2_7;
const int FORWARD_SWITCH = P1_6;
const int REVERSE_SWITCH = P6_6;
const int INHIBIT_SWITCH = P3_2;

Adafruit_MCP4725 dac;

volatile int pwm_value = 0;

const int SERIAL_TIMEOUT_MS = 60;
const int WATCHDOG_TIMEOUT_MS = 100; 
volatile int watchdog_fed = 0;

/*
 * Watchdog
 * 
 * The must watchdog must be fed every 100 ms, or it kills/disables the controller
 */
void feedWatchdog() {
  //  Serial.println("Feeding");
  watchdog_fed = millis();
}

bool isWatchdogAlive() {
  int delta = (millis() - watchdog_fed);
  //  Serial.print("WD:");
  //  Serial.println(delta);
  return delta < WATCHDOG_TIMEOUT_MS;
}

/*
 * Converts a hobby servo style pwm singal pulse time into a speed
 * value between -1.0 and 1.0. Where -1.0 is full reverse, and 1.0 is full forward.
 * Returns 2 when the value is out of range
 */
const double SERVO_MIN_WIDTH = 1000;
const double SERVO_NEU_WIDTH = 1500;
const double SERVO_MAX_WIDTH = 2000;
double pwmToSpeed(int value) {
  if(value < 900 || 2100 < value ) {
    // The value is out of range
    return 2;
  }
  
  if(value < SERVO_NEU_WIDTH) {
    return ((value - SERVO_NEU_WIDTH) / (SERVO_NEU_WIDTH - SERVO_MIN_WIDTH));
  } else {
    return ((value - SERVO_NEU_WIDTH) / (SERVO_MAX_WIDTH - SERVO_NEU_WIDTH));
  }  
}

void setKeySwitch(bool on) { 
  if(on) {
    digitalWrite(KEY_SWITCH, LOW);  
  } else {
    digitalWrite(KEY_SWITCH, HIGH);
  }
}

void setFwdSwitch(bool on) { 
  if(on) {
    digitalWrite(FORWARD_SWITCH, LOW);  
  } else {
    digitalWrite(FORWARD_SWITCH, HIGH);
  }
}

void setRevSwitch(bool on) { 
  if(on) {
    digitalWrite(REVERSE_SWITCH, LOW);  
  } else {
    digitalWrite(REVERSE_SWITCH, HIGH);
  }
}

void setInhibitSwitch(bool on) { 
  if(on) {
    digitalWrite(INHIBIT_SWITCH, HIGH);  
  } else {
    digitalWrite(INHIBIT_SWITCH, LOW);
  }
}

/*
 * Pass a value of 0 to 1, and it will set the dac to this percentage
 */
void setOutputVoltage(float percent) {
  int output = (int)(percent * 4095);
  if(output < 0) output = 0;
  if(output > 4095) output = 4095;
  dac.setVoltage(output, true);
}

void setup() {
  // Setup serial
  Serial.begin(9600);
  Serial.setTimeout(SERIAL_TIMEOUT_MS);
  Serial.println("Hello");
  Serial1.begin(9600);

  // Initialize watchdog

  // For Adafruit MCP4725A1 the address is 0x62 (default) or 0x63 (ADDR pin tied to VCC)
  // For MCP4725A0 the address is 0x60 or 0x61
  // For MCP4725A2 the address is 0x64 or 0x65
  dac.begin(MCP4725_ADDR);
  dac.setVoltage(0, true);

  // Put the controller into a known state
  pinMode(KEY_SWITCH, OUTPUT);
  pinMode(FORWARD_SWITCH, OUTPUT);
  pinMode(REVERSE_SWITCH, OUTPUT);
  pinMode(INHIBIT_SWITCH, OUTPUT);

  digitalWrite(KEY_SWITCH, HIGH);
  digitalWrite(FORWARD_SWITCH, HIGH);
  digitalWrite(REVERSE_SWITCH, HIGH);
  digitalWrite(INHIBIT_SWITCH, HIGH);

  // Turn on Controller, and inhibit until we are ready
  setKeySwitch(true);
  setInhibitSwitch(true);
}

const float DEADBAND = 0.05;
char protocol_buffer[64];
void loop() {
  bool found_start_of_message = false;
  while(Serial1.available() > 0) {
    char rcv_byte = Serial1.peek();
    if(rcv_byte != PACKET_START_CHAR) {
      Serial1.read();
    } else {
      delay(2);
      found_start_of_message = true;
      break;
    }
  }

  // If sufficient bytes have been received, process the data and
  // if a valid message is received, handle it.
  double pwm_speed = 0;
  bool pwm_speed_okay = false;
    
  if(found_start_of_message && (Serial1.available() >= CMD_MESSAGE_LENGTH)) {
    int bytes_read = 0;
    while(Serial1.available()) {
      if(bytes_read >= sizeof(protocol_buffer)) {
        break;
      }
      protocol_buffer[bytes_read++] = Serial1.read();
      int value;
    }
    int i = 0;
    while(i < bytes_read) {
      int bytes_remaining = bytes_read - i;
      uint16_t value = 0;
      int packet_length = decodeCommand(&protocol_buffer[i], bytes_remaining, value);
      if(packet_length > 0) {
        Serial.print("PWM:");
        Serial.println(value);
        double speed = 0;
        speed = pwmToSpeed(value);
        Serial.println(speed);
        pwm_speed_okay = fabs(speed) < 1.1;  
        if(pwm_speed_okay) {
          //We got a good pwm value, store it
          pwm_value = value;
          pwm_speed = speed;
          feedWatchdog();
        }
        
        i += packet_length;
      } 
      else { 
        // current index is not the start of a valid packet; increment
        i++;
      }
    }
  }

  // Main control logic
  if(isWatchdogAlive()) {
     //Serial.println("Alive");
    // We are receiving a PWM signal
    if(pwm_speed_okay) {
      feedWatchdog();
      Serial.print("Output:");
      Serial.println(pwm_speed);
  
      if(pwm_speed < DEADBAND) {
        //Reverse
        setFwdSwitch(false);
        setRevSwitch(true);
      } else if (DEADBAND < pwm_speed) {
        //Forward
        setFwdSwitch(true);
        setRevSwitch(false);
      } else {
        // Neutral
        setFwdSwitch(false);
        setRevSwitch(false);
        pwm_speed = 0;
      }
  
      pwm_speed = fabs(pwm_speed);
  
      setOutputVoltage(pwm_speed);
  
      setInhibitSwitch(false);
    } else {
      Serial.println("PWM Speed is not okay");
    }
  } else {
    //Serial.println("Dead");

    // The watchdog is dead
    // We are not currently receiving a PWM signal
    // disable controller is wait for the signal to comeback up
    setOutputVoltage(0.0);
    setInhibitSwitch(true);
    setFwdSwitch(false);
    setRevSwitch(false);
  }
}

