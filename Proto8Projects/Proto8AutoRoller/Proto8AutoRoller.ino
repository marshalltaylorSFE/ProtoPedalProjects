#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2SQuad        i2s_quad1;      //xy=114,128
AudioFilterBiquad        biquad2;        //xy=304,108
AudioFilterBiquad        biquad1;        //xy=367,70
AudioFilterBiquad        biquad3;        //xy=379,152
AudioMixer4              mixer1;         //xy=600,111
AudioOutputI2SQuad       i2s_quad2;      //xy=705,299
AudioConnection          patchCord1(i2s_quad1, 0, biquad1, 0);
AudioConnection          patchCord2(i2s_quad1, 0, biquad2, 0);
AudioConnection          patchCord3(i2s_quad1, 0, biquad3, 0);
AudioConnection          patchCord4(biquad2, 0, mixer1, 1);
AudioConnection          patchCord5(biquad1, 0, mixer1, 0);
AudioConnection          patchCord6(biquad3, 0, mixer1, 2);
AudioConnection          patchCord7(mixer1, 0, i2s_quad2, 0);
AudioConnection          patchCord8(mixer1, 0, i2s_quad2, 1);
AudioConnection          patchCord9(mixer1, 0, i2s_quad2, 2);
AudioConnection          patchCord10(mixer1, 0, i2s_quad2, 3);
AudioControlSGTL5000     sgtl5000_1;     //xy=441,306
AudioControlSGTL5000     sgtl5000_2;     //xy=441,345
// GUItool: end automatically generated code





//**********************************************************************//
//
//  Top-level for the Bendvelope panel, 16 button/led bar w/ knob
//  and scanned matrix 7 segment display.
//  
//  The serial debug shows internal variables
//
//  Written by:  Marshall Taylor
//  Changelog (YYYY/MM/DD):
//    2016/2/24: Created
//
//**********************************************************************//


#include "proto-8Hardware.h"
#include "VoltageMonitor.h"

//**Timers and stuff**************************//
#include "timerModule32.h"

#include "timeKeeper.h"
//**Panels and stuff**************************//
#include "P8Panel.h"

//**Panel State Machine***********************//
#include "P8Interface.h"
P8Interface p8hid;

//**Timers and stuff**************************//
IntervalTimer myTimer;

//HOW TO OPERATE
//  Make TimerClass objects for each thing that needs periodic service
//  pass the interval of the period in ticks
//  Set MAXINTERVAL to the max foreseen interval of any TimerClass
//  Set MAXTIMER to overflow number in the header.  MAXTIMER + MAXINTERVAL
//    cannot exceed variable size.
//Globals
uint32_t maxTimer = 60000000;
uint32_t maxInterval = 2000000;

TimerClass32 panelUpdateTimer(10000);
TimerClass32 LEDsTimer(200);
TimerClass32 switchesTimer(500);
TimerClass32 knobsTimer(500);



TimerClass32 ledToggleTimer( 333000 );
uint8_t ledToggleState = 0;
TimerClass32 ledToggleFastTimer( 100000 );
uint8_t ledToggleFastState = 0;

TimerClass32 debounceTimer(5000);

TimerClass32 debugTimer(333000);

//tick variable for interrupt driven timer1
uint32_t usTicks = 0;
uint8_t usTicksMutex = 1; //start locked out





int8_t loopCount = 0;
int8_t maxLoopCount = 20;

//Names use in P8PanelComponents.cpp and .h
VoltageMonitor LEDs;
AnalogMuxTree knobs;
SwitchMatrix switches;
//End used names



void setup()
{
	//**** Audio Section ****//
	AudioMemory(35);
	
	sgtl5000_1.setAddress(LOW);
	sgtl5000_1.enable();
	sgtl5000_1.volume(0.5);
	sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
	sgtl5000_1.unmuteHeadphone();
	
	sgtl5000_2.setAddress(HIGH);
	sgtl5000_2.enable();
	sgtl5000_2.volume(0.5);
	sgtl5000_2.unmuteHeadphone();

	sgtl5000_1.volume((float)p8hid.p8Param[0] / 256);
	sgtl5000_2.volume((float)p8hid.p8Param[0] / 256);
	biquad1.setLowpass(0, 200 + 5000 * ((float)p8hid.p8Param[1] / 256), 0.707);
	mixer1.gain(0, (float)p8hid.p8Param[2] / 256);
	biquad2.setBandpass(0, 400 + 15000 * ((float)p8hid.p8Param[3] / 256), 0.7-(0.69*(float)p8hid.p8Param[4] / 256));
	mixer1.gain(1, (float)p8hid.p8Param[5] / 256);
	biquad3.setHighpass(0, 400 + 15000 * ((float)p8hid.p8Param[6] / 256), 0.707);
	mixer1.gain(2, (float)p8hid.p8Param[7] / 256);
	
	LEDs.begin();
	knobs.begin();
	switches.begin();
	delay(1000);

	p8hid.init();
	
	Serial.println("Program Started");
	delay(1000);
	// initialize IntervalTimer
	myTimer.begin(serviceUS, 1);  // serviceMS to run every 0.001 seconds
	
}

void loop()
{
//**Copy to make a new timer******************//  
//   msTimerA.update(usTicks);
	ledToggleTimer.update(usTicks);
	ledToggleFastTimer.update(usTicks);
	panelUpdateTimer.update(usTicks);
	debounceTimer.update(usTicks);
	debugTimer.update(usTicks);
	LEDsTimer.update(usTicks);
	switchesTimer.update(usTicks);
	knobsTimer.update(usTicks);
	
//**Copy to make a new timer******************//  
	//  if(msTimerA.flagStatus() == PENDING)
	//  {
	//    digitalWrite( LEDPIN, digitalRead(LEDPIN) ^ 1 );
	//  }
	
	//**Debounce timer****************************//  
	if(debounceTimer.flagStatus() == PENDING)
	{
		p8hid.timersMIncrement(5);
	
	}
	//**Debounce timer****************************//  
	if(LEDsTimer.flagStatus() == PENDING)
	{
		LEDs.tickSeg();
	
	}
	//**Debounce timer****************************//  
	if(switchesTimer.flagStatus() == PENDING)
	{
		switches.tick();
	
	}
	//**Debounce timer****************************//  
	if(knobsTimer.flagStatus() == PENDING)
	{
		knobs.tick();
	
	}	
	//**Process the panel and state machine***********//  
	if(panelUpdateTimer.flagStatus() == PENDING)
	{
		p8hid.processMachine();
	}
	//**Fast LED toggling of the panel class***********//  
	if(ledToggleFastTimer.flagStatus() == PENDING)
	{
		p8hid.toggleFastFlasherState();
		
	}
	//**LED toggling of the panel class***********//  
	if(ledToggleTimer.flagStatus() == PENDING)
	{
		p8hid.toggleFlasherState();
		
	}
	//**Debug timer*******************************//  
	if(debugTimer.flagStatus() == PENDING)
	{
		Serial.println("**************************Debug Service**************************");
		Serial.println("Reading Knobs.");
		//
		//int temp;
		//for(int j = 0; j < 8; j++)
		//{
		//	for(int i = 0; i < 8; i++)
		//	{
		//		temp = (j * 8) + i + 1;
		//		Serial.print(knobs.fetch(temp));
		//		Serial.print(", ");
		//	}
		//	Serial.print("\n");
		//}
		//
		//Serial.print("\nReading Switches.");
		//for(int i = 1; i < 65; i++)
		//{
		//	if((i == 1)||(i == 17)||(i == 33)||(i == 49))
		//	{
		//		Serial.println();
		//	}
		//	Serial.print(switches.fetch(i));
		//	//Serial.print(i);
		//	Serial.print(", ");
		//}
		//Serial.println();
		//
		//Serial.println("\nLED data");
		//for(int i = 0; i < 8; i++)
		//{
		//	uint8_t ledpack = LEDs.LEDData[i];
		//	if(ledpack < 0x80) Serial.print("0");
		//	if(ledpack < 0x40) Serial.print("0");
		//	if(ledpack < 0x20) Serial.print("0");
		//	if(ledpack < 0x10) Serial.print("0");
		//	if(ledpack < 0x08) Serial.print("0");
		//	if(ledpack < 0x04) Serial.print("0");
		//	if(ledpack < 0x02) Serial.print("0");
		//	Serial.println(ledpack, BIN);
		//	//Serial.print(i);
		//}
		Serial.println();		
		Serial.println();

	}
	
	
}

void serviceUS(void)
{
  uint32_t returnVar = 0;
  if(usTicks >= ( maxTimer + maxInterval ))
  {
    returnVar = usTicks - maxTimer;

  }
  else
  {
    returnVar = usTicks + 1;
  }
  usTicks = returnVar;
  usTicksMutex = 0;  //unlock
}