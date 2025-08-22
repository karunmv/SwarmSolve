// L298N Motor Driver Test Code
// Connections:
// pwm  -> PWM pin 9
// dir  -> pin 8
// IN2  -> pin 7
// Motor connected to OUT1 and OUT2
// +12V, GND, and +5V connected properly on the L298N

int pwm = 11;   // pwmble pin (PWM)
int dir = 12;   // Input 1
//int in2 = 7;   // Input 2

void setup() {
  // Set pins as outputs
  pinMode(pwm, OUTPUT);
  pinMode(dir, OUTPUT);
  //pinMode(in2, OUTPUT);

  // Start with motor off
  digitalWrite(dir, LOW);
  //digitalWrite(in2, LOW);
}

void loop() {
  // Rotate motor forward
  // digitalWrite(dir, HIGH);
  // //digitalWrite(in2, LOW);
  // analogWrite(pwm, 200); // Speed (0-255)
  // delay(2000);

  // // Stop motor
  // digitalWrite(dir, LOW);
  // //digitalWrite(in2, LOW);
  // delay(1000);

  // // Rotate motor backward
  // digitalWrite(dir, LOW);
  // //digitalWrite(in2, HIGH);
  // analogWrite(pwm, 200); // Speed
  // delay(2000);

  // // Stop motor
  // digitalWrite(dir, LOW);
  // //digitalWrite(in2, LOW);
  // delay(1000);

  digitalWrite(dir, HIGH);

  for(int i=0; i<255; i++){
    analogWrite(pwm, i);
    delay(20);
  }

  digitalWrite(dir, LOW);

  for(int i=255; i>0; i--){
    analogWrite(pwm, i);
    delay(20);
  }

}
