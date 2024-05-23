#include <Encoder.h>

#define EncoderPinA 2
#define EncoderPinB 3
#define MotorPin 5

// Create encoder object
Encoder motorEncoder(EncoderPinA, EncoderPinB);

// Variables for finding servo position
int count = 0;
int PPR = 924; // 924 pulses per revolution
int deg = 0;

// For speed iteration
int servoSpeedControl[] = {50, 100, 150, 200, 255};
int current = 0;
int start = 0;
int index = 0;

void setup() {
  Serial.begin(9600);
  pinMode(MotorPin, OUTPUT);  // sets the pin as output
}

void loop() {
  //Indexes the different speeds every second
  current = millis();
  if((current - start) >= 1000) {
    index++;
    start = current;
  }
  
  setServoSpeed(servoSpeedControl[index]);
  Serial.print(servoSpeedControl[index]);
  Serial.print(servoPosition());
}

void setServoSpeed(int speedPWM){
  // This function sets the speed of the servo to the inputted PWM value.
  // Input: speedPWM 

  if (speedPWM > 255) speedPWM = 255;
  if (speedPWM < 0) speedPWM = 0;
  
  analogWrite(MotorPin, speedPWM); //input ranges from 0-255 as duty cycle

  //TODO find out what duty cycle corrisponds to what speed/voltage
  
}

float servoPosition(){
  // This function outputs the servo position.
  // Output: deg (degrees)
  
  count = motorEncoder.read() % 360;
  deg = count * (360 / PPR); // converts count to degrees 
  deg = deg % 360;  // keeps the value within the range of -359 to 359.
  if (deg < 0) deg += 360; // keeps the value positive
  return deg;
}
