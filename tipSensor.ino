
#include "uArm.h"
#define buttonState
#define USE_SERIAL_CMD  1 // 1: use serial for control  0: just use arduino to control(release ROM and RAM space)
unsigned long tickStartTime = millis(); // get timestamp;
static void Init();
void setup() {
        Wire.begin();      // join i2c bus (address optional for master)
        Serial.begin(9600); // start serial port at 9600 bps
        pinMode(2,INPUT);
        Init(); // Don't remove
       
}

void loop() {
        run();
          moveTo(0,150,150);
          if (!getTip()){
            Serial.println("I am being PUSHED!!!!!");
          }else{
            Serial.println("No Touchy");
          }
          delay(500);
      

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
