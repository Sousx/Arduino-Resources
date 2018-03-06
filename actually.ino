/*    Example code to control up to 4 actuators,using the Robot Power MultiMoto driver.
   Hardware:
    - Robot Power MultiMoto
    - Arduino Uno

    Wiring:
  - Connect actuators to the M1, M2, M3, M4 connections on the MultiMoto board.
  - Connect the negative (black) to the right connection, positive (red) to the left.
  - Connect a 12 volt source (minimum 1A per motor if unloaded, 8A per motor if fully loaded)to the BAT terminals. Ensure that positive and negative are placed in the correct spots.

   Code modified by Progressive Automations from the example code provided by Robot Power
     <a href="http://www.robotpower.com/downloads/" rel="nofollow"> http://www.robotpower.com/downloads/</a>

    Robot Power MultiMoto v1.0 demo
    This software is released into the Public Domain
*/

// include the SPI library:

#include <SPI.h>
// L9958 slave select pins for SPI
#define SS_M4 14
#define SS_M3 13
#define SS_M2 12
#define SS_M1 11
// L9958 DIRection pins
#define DIR_M1 2
#define DIR_M2 3
#define DIR_M3 4
#define DIR_M4 7
// L9958 PWM pins
#define PWM_M1 9
#define PWM_M2 10    // Timer1
#define PWM_M3 5
#define PWM_M4 6     // Timer0


// L9958 Enable for all 4 motors
#define ENABLE_MOTORS 8

int pwm1, pwm2, pwm3, pwm4;
boolean dir1, dir2, dir3, dir4;

void setup() {
  unsigned int configWord;

  // put your setup code here, to run once:
  pinMode(SS_M1, OUTPUT); digitalWrite(SS_M1, LOW);  // HIGH = not selected
  pinMode(SS_M2, OUTPUT); digitalWrite(SS_M2, LOW);
  pinMode(SS_M3, OUTPUT); digitalWrite(SS_M3, LOW);
  pinMode(SS_M4, OUTPUT); digitalWrite(SS_M4, LOW);

  // L9958 DIRection pins
  pinMode(DIR_M1, OUTPUT);
  pinMode(DIR_M2, OUTPUT);
  pinMode(DIR_M3, OUTPUT);
  pinMode(DIR_M4, OUTPUT);

  // L9958 PWM pins
  pinMode(PWM_M1, OUTPUT);  digitalWrite(PWM_M1, LOW);
  pinMode(PWM_M2, OUTPUT);  digitalWrite(PWM_M2, LOW);    // Timer1
  pinMode(PWM_M3, OUTPUT);  digitalWrite(PWM_M3, LOW);
  pinMode(PWM_M4, OUTPUT);  digitalWrite(PWM_M4, LOW);    // Timer0

  // L9958 Enable for all 4 motors
  pinMode(ENABLE_MOTORS, OUTPUT); 
 digitalWrite(ENABLE_MOTORS, HIGH);  // HIGH = disabled

/******* Set up L9958 chips *********
  ' L9958 Config Register
  ' Bit
  '0 - RES
  '1 - DR - reset
  '2 - CL_1 - curr limit
  '3 - CL_2 - curr_limit
  '4 - RES
  '5 - RES
  '6 - RES
  '7 - RES
  '8 - VSR - voltage slew rate (1 enables slew limit, 0 disables)
  '9 - ISR - current slew rate (1 enables slew limit, 0 disables)
  '10 - ISR_DIS - current slew disable
  '11 - OL_ON - open load enable
  '12 - RES
  '13 - RES
  '14 - 0 - always zero
  '15 - 0 - always zero
  */  // set to max current limit and disable ISR slew limiting
  configWord = 0b0000010000001100;

  SPI.begin();
  SPI.setBitOrder(LSBFIRST);
  SPI.setDataMode(SPI_MODE1);  // clock pol = low, phase = high

  // Motor 1
  digitalWrite(SS_M1, LOW);
  SPI.transfer(lowByte(configWord));
  SPI.transfer(highByte(configWord));
  digitalWrite(SS_M1, HIGH);
  // Motor 2
  digitalWrite(SS_M2, LOW);
  SPI.transfer(lowByte(configWord));
  SPI.transfer(highByte(configWord));
  digitalWrite(SS_M2, HIGH);
  // Motor 3
  digitalWrite(SS_M3, LOW);
  SPI.transfer(lowByte(configWord));
  SPI.transfer(highByte(configWord));
  digitalWrite(SS_M3, HIGH);
  // Motor 4
  digitalWrite(SS_M4, LOW);
  SPI.transfer(lowByte(configWord));
  SPI.transfer(highByte(configWord));
  digitalWrite(SS_M4, HIGH);

  //Set initial actuator settings to pull at 0 speed for safety
  dir1 = 0; dir2 = 0; dir3 = 0; dir4 = 0; // Set direction
  pwm1 = 0; pwm2 = 0; pwm3 = 0; pwm4 = 0; // Set speed (0-255)

digitalWrite(ENABLE_MOTORS, LOW);// LOW = enabled
} // End setup

void loop() {
    dir1 = 1;
    pwm1 = 255; //set direction and speed 
    digitalWrite(DIR_M1, dir1);
    analogWrite(PWM_M1, pwm1); // write to pins

    dir2 = 0;
    pwm2 = 128;
    digitalWrite(DIR_M2, dir2);
    analogWrite(PWM_M2, pwm2);

    dir3 = 1;
    pwm3 = 255;
    digitalWrite(DIR_M3, dir3);
    analogWrite(PWM_M3, pwm3);

    dir4 = 0;
    pwm4 = 128;
    digitalWrite(DIR_M4, dir4);
    analogWrite(PWM_M4, pwm4);

    delay(5000); // wait once all four motors are set

    dir1 = 0;
    pwm1 = 128;
    digitalWrite(DIR_M1, dir1);
    analogWrite(PWM_M1, pwm1);

    dir2 = 1;
    pwm2 = 255;
    digitalWrite(DIR_M2, dir2);
    analogWrite(PWM_M2, pwm2);

    dir3 = 0;
    pwm3 = 128;
    digitalWrite(DIR_M3, dir3);
    analogWrite(PWM_M3, pwm3);

    dir4 = 1;
    pwm4 = 255;
    digitalWrite(DIR_M4, dir4);
    analogWrite(PWM_M4, pwm4);

   delay(5000); 

    dir1 = 0;
    pwm1 = 255;
    digitalWrite(DIR_M1, dir1);
    analogWrite(PWM_M1, pwm1);

    dir2 = 0;
    pwm2 = 255;
    digitalWrite(DIR_M2, dir2);
    analogWrite(PWM_M2, pwm2);

    dir3 = 0;
    pwm3 = 255;
    digitalWrite(DIR_M3, dir3);
    analogWrite(PWM_M3, pwm3);

    dir4 = 0;
    pwm4 = 255;
    digitalWrite(DIR_M4, dir4);
    analogWrite(PWM_M4, pwm4);

   delay(5000); 

}//end void loop
