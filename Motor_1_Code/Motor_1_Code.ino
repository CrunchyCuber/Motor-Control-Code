#include <Servo.h> 
#define speedPin 3
#define positionPin 2

// Create a servo object 
Servo ServoSpeed; 

// Global variables for pulses function
volatile long StartTime = 0;
volatile long CurrentTime = 0;
volatile long Pulses = 0;
int PulseWidth = 0;
int tCycle = 0; 

// Variables for servo position
int timeHigh, timeLow, timeCycle, dutyCycle, deg, level;
int dutyCycleMin = 29;
int dutyCycleMax = 971;
int servoSpeedControl[] = {1500, 1520, 1540, 1560, 1580, 1600};
int index;

int start, current; // For speed iteration

void setup() {
  Serial.begin(9600);
  ServoSpeed.attach(speedPin); 
  attachInterrupt(digitalPinToInterrupt(positionPin), PulseTimer, CHANGE);
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

void PulseTimer(){
  // This interrupt function times the length of either a high or low pulse.
  // Output: Pulses in microseconds
  CurrentTime = micros();
  if(CurrentTime > StartTime){
    Pulses = CurrentTime - StartTime;
    StartTime = CurrentTime;
  }
}

void setServoSpeed(int speedPWMMicroseconds){
  // This function sets the speed of the servo to the inputted microseconds corelating to the PWM value.
  // Input: speedPWMMicroseconds (values corelate to speeds below)
  // 1280us...1480us = 140rpm...0rpm, 
  // 1480us...1520us = 0rpm, 
  // 1520us...1720us = 0rpm...-140rpm
  if (speedPWMMicroseconds > 1720) speedPWMMicroseconds = 1720;
  if (speedPWMMicroseconds < 1280) speedPWMMicroseconds = 1280;
  ServoSpeed.writeMicroseconds(speedPWMMicroseconds); 
}

int servoPosition(){
  // This function outputs the servo position.
  // Output: deg (degrees)
  
  // Is my digitalRead going to be after the change or before the change? I think after. When a change is detected in the position, 
  // our current position will be of that new position, not of what was previously measured. So pulses will be one interrupt behind
  // the current Level. So our tHigh will be the pulse when our current level is low and vica versa. 
  // Does it matter which is read first? No?
  level = digitalRead(positionPin);
  if (level == LOW){
    timeHigh = Pulses;
  } else if (level == HIGH){
    timeLow = Pulses;
  }
  timeCycle = timeHigh + timeLow;  // Calculate cycle time 
  
  dutyCycle = (1000 * timeHigh) / timeCycle; // Calculate duty cycle. In terms of Microseconds

  //Calculate theta in terms of degrees
  deg = (360 - 1) - ((dutyCycle - dutyCycleMin) * 360) / (dutyCycleMax - dutyCycleMin + 1); 

  //Clamp invalid values of theta
  if(deg < 0) deg = 0; 
  else if(deg > (360 - 1)) deg = 360 - 1;   

  return deg;
}
