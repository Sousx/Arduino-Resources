
#include "uArm.h"
#include "uArmAPI.h"
#include <SPI.h>
#define USE_SERIAL_CMD  1 // 1: use serial for control  0: just use arduino to control(release ROM and RAM space)
// Needed for Motors
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

unsigned long tickStartTime = millis(); // get timestamp;
static void Init();

int box1[] = {-151.5, 130, 155};
int box2[] = {-154, 234, 156};
int diceBox[] = {-100 , 182,155}; // directly left of the storage area
int sliceBox[] = {-50, 182,155};  // slightly more left than above coordinates
int pwm1, pwm2, pwm3, pwm4;
boolean dir1, dir2, dir3, dir4;
//int input = 0;

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing

      // variables to hold the parsed data
char cutFromMega[numChars] = {0};
int boxFromMega = 0;
//float floatFromPC = 0.0;

boolean newData = false;

void setup()
{
  unsigned int configWord;

  // put your setup code here, to run once for motors:
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
  Serial.begin(9600);
  Init(); // Don't remove
  //uarm.init();
  //debugPrint("debug start"); // uncomment DEBUG in uArmConfig.h to use debug function
  //pinMode(2,INPUT); //TipSensor
  // TODO
  moveTo(0, 150, 150, 50); //Neutral Position coordinates
  Serial.println("@1"); // report ready
  //reportPos(); //Serial Prints the current position
  Serial.println("Please input 1 or 2");
}

void loop(){
  run(); // Don't remove
  recvWithStartEndMarkers();
 int processLoc[] = {0,0,0};
   
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    // this temporary copy is necessary to protect the original data
    //   because strtok() used in parseData() replaces the commas with \0
    parseData();
    showParsedData();
    
    //Serial.print("Choose box ... ");
    //Serial.println(receivedChars);
    //Serial.println(receivedChars[1] - '0');
    if(strcmp(cutFromMega,"d")){
      processLoc = diceBox;
    } else if (strcmp(cutFromMega,"s")){
      processLoc = sliceBox;
    } else {
      Serial.print("Inoperable process selected you chose: ");
      Serial.println(cutFromMega);
    }
    if (boxFromMega == 1){
      Serial.print("Get food from box 1, cut ");
      Serial.println(cutFromMega);
      getFood(box1, processLoc);
    }else if (boxFromMega == 2){
      Serial.print("Get food from box 2, cut ");
      Serial.println(cutFromMega);
      getFood(box2, processLoc);
    } else {
      Serial.println("You didn't pick a food box, jackass.");
    }
    Serial.println("Please input 1 or 2");
    newData = false;
  }

  if (strcmp(cutFromMega,"d")){

    delay(1000);
    //move actuator 1 in then out
    dir1 = 1; 
    pwm1 = 255; //set direction and speed 
    digitalWrite(DIR_M1, dir1);
    analogWrite(PWM_M1, pwm1); // write to pins
    delay(5000);
    dir1 = 0;
    pwm1 = 255; //set direction and speed 
    digitalWrite(DIR_M1, dir1);
    analogWrite(PWM_M1, pwm1); // write to pins
    delay(5000); //allows enough time for actuator to retract
  }else if (strcmp(curFromMega,"s")){
    
    delay(1000);
    dir3 = 1;
    pwm3 = 255;
    digitalWrite(DIR_M3, dir3);
    analogWrite(PWM_M3, pwm3);
    delay(5000);
    dir3 = 0;
    pwm3 = 255;
    digitalWrite(DIR_M3, dir3);
    analogWrite(PWM_M3, pwm3);
    delay(5000);
  }
  
  //Serial.print("Input : "); 
  //Serial.println(atoi(input));    
}


void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial.available() == 0) {} //do nothing until input
  while ((Serial.available() > 0) && newData == false){
    rc = Serial.read();
    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      } else {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    } else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}

void parseData() {      // split the data into its parts

    char * strtokIndx; // this is used by strtok() as an index

    strtokIndx = strtok(tempChars,",");      // get the first part - the string
    strcpy(cutFromMega, strtokIndx); // copy it to messageFromPC
 
    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    boxFromMega = atoi(strtokIndx);     // convert this part to an integer

    //strtokIndx = strtok(NULL, ",");
    //floatFromPC = atof(strtokIndx);     // convert this part to a float

}

//============

void showParsedData() {
    Serial.print("Type of cut: ");
    Serial.println(cutFromMega);
    Serial.print("Food from box: ");
    Serial.println(boxFromMega);
    //Serial.print("Float ");
    //Serial.println(floatFromPC);
}

void getFood(int storage[], int processing[])
{
  //delay(1000);
  int storageZ = storage[2];
  moveTo(storage[0],storage[1],storageZ);  //Move to Box 1
  delay(1000);
  Serial.println("@1"); // report ready
  reportPos(); //Serial Prints the current position
  while (getTip()) {
    storageZ = storageZ - 1;
    moveTo(storage[0],storage[1],storageZ, 25);  //Move to Box 1
    //Serial.println("@1"); // report ready
    //reportPos(); //Serial Prints the current position
    delay(100);
  }
  delay(1000);
  pumpOn();
  reportPos();
  delay(1000);
  moveTo(storage[0],storage[1], 200, 100);
  delay(1000);
  moveTo(processing[0], processing[1], processing[2], 100); //To the cutting Modules
  delay(1000);
  pumpOff();
  delay(1000);
  moveTo(0, 150, 150); //Neutral Position coordinates
  Serial.println("@1"); // report ready
  reportPos(); //Serial Prints the current position
  

}
// time out every TICK_INTERVAL(50 ms default)
void tickTimeOut()
{
  
}

////////////////////////////////////////////////////////////
// DO NOT EDIT
static void Init()
{
  uArmInit(); // Don't remove
  service.init();

  #if USE_SERIAL_CMD == 1
  serialCmdInit();
  

  #endif
}

void run()
{
  #if USE_SERIAL_CMD == 1
  handleSerialCmd();
  #endif

  manage_inactivity(); // Don't remove
}

void tickTaskRun()
{
  tickTimeOut();

    buttonPlay.tick();
    buttonMenu.tick();
#ifdef MKII
    ledRed.tick();
    service.btDetect();
#endif    
}

void manage_inactivity(void)
{
#if USE_SERIAL_CMD == 1
  getSerialCmd(); // for serial communication
#endif
  service.run();  // for led, button, bt etc.

  // because there is no other hardware timer available in UNO, so use a soft timer
  // it's necessary for button,led, bt
  // so Don't remove it if you need them
  if(millis() - tickStartTime >= TICK_INTERVAL)
  {
    tickStartTime = millis();
    tickTaskRun();
  }   
}
