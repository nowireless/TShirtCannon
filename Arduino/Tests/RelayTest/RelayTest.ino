const int KEY_SWITCH = P2_7;
const int FORWARD_SWITCH = P1_6;
const int REVERSE_SWITCH = P6_6;
const int INHIBIT_SWITCH = P3_2;

void setup() {
  // put your setup code here, to run once:

  pinMode(KEY_SWITCH, OUTPUT);
  pinMode(FORWARD_SWITCH, OUTPUT);
  pinMode(REVERSE_SWITCH, OUTPUT);
  pinMode(INHIBIT_SWITCH, OUTPUT);
  
  digitalWrite(KEY_SWITCH, HIGH);
  digitalWrite(FORWARD_SWITCH, HIGH);
  digitalWrite(REVERSE_SWITCH, HIGH);
  digitalWrite(INHIBIT_SWITCH, HIGH);

}

void loop() {
  // put your main code here, to run repeatedly: 
  digitalWrite(KEY_SWITCH, LOW);
  digitalWrite(FORWARD_SWITCH, HIGH);
  digitalWrite(REVERSE_SWITCH, HIGH);
  digitalWrite(INHIBIT_SWITCH, HIGH);
  delay(1000);
  digitalWrite(KEY_SWITCH, HIGH);
  digitalWrite(FORWARD_SWITCH, LOW);
  digitalWrite(REVERSE_SWITCH, HIGH);
  digitalWrite(INHIBIT_SWITCH, HIGH);
  delay(1000);
  digitalWrite(KEY_SWITCH, HIGH);
  digitalWrite(FORWARD_SWITCH, HIGH);
  digitalWrite(REVERSE_SWITCH, LOW);
  digitalWrite(INHIBIT_SWITCH, HIGH);
  delay(1000);
  digitalWrite(KEY_SWITCH, HIGH);
  digitalWrite(FORWARD_SWITCH, HIGH);
  digitalWrite(REVERSE_SWITCH, HIGH);
  digitalWrite(INHIBIT_SWITCH, LOW);
  delay(1000);
}
