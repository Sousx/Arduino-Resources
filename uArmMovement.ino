
#include "uArm.h"
#include "uArmAPI.h"
#define USE_SERIAL_CMD  1 // 1: use serial for control  0: just use arduino to control(release ROM and RAM space)

unsigned long tickStartTime = millis(); // get timestamp;
static void Init();

int box1[] = {-151.5, 130, 155};
int box2[] = {-154, 234, 156};
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
 
   
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    // this temporary copy is necessary to protect the original data
    //   because strtok() used in parseData() replaces the commas with \0
    parseData();
    showParsedData();
    
    //Serial.print("Choose box ... ");
    //Serial.println(receivedChars);
    //Serial.println(receivedChars[1] - '0');
    if (boxFromMega == 1){
      Serial.print("Get food from box 1, cut ");
      Serial.println(cutFromMega);
      //getFood(box1, cutFromMega);
    }else if (boxFromMega == 2){
      Serial.print("Get food from box 2, cut ");
      Serial.println(cutFromMega);
      //getFood(box2, cutFromMega);
    } else {
      Serial.println("You didn't pick a food box, jackass.");
    }
    Serial.println("Please input 1 or 2");
    newData = false;
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

void getFood(int storage[])
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
  moveTo(207.5, 128.6, 200, 100); //To the cutting Module
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
