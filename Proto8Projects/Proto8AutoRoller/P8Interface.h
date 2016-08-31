//**********************************************************************//
//
//  Interface example for the Bendvelope panel, 16 button/led bar w/ knob
//  and scanned matrix 7 segment display.
//  
//  This file defines the human interaction of the panel parts
//
//  Written by:  Marshall Taylor
//  Changelog (YYYY/MM/DD):
//    2016/2/24: Created
//    2016/2/29: Moved seven segment stuff to voltage monitor
//
//**********************************************************************//
#ifndef P8INTERFACE_H
#define P8INTERFACE_H

#include "stdint.h"
#include "timeKeeper.h"
#include "P8PanelComponents.h"
#include "P8Panel.h"
#include "flagMessaging.h"


enum PStates
{
	PInit,
	PIdle,

};

class P8Interface : public P8Panel
{
public:
	P8Interface( void );
	void reset( void );
	//State machine stuff  
	void processMachine( void );
	void tickStateMachine( void );

	void timersMIncrement( uint8_t );
	
	//Flags coming in from the system
	
	//Internal - and going out to the system - Flags
	
	//  ..and data.
	
	//Template Parameters
	//  Set initial values here
	uint8_t p8Param[15] = {190,4,210,240,73,50,45,255,255,0,0,0,0,255,0}; //Save 16 for idle
	//  p8ParamSync used for knob resyncing
	uint8_t p8ParamSync = 0;
	//  Choose how many params for the project
	uint8_t p8ParamActive[15] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,0};
	uint8_t activeParam = 0;
	
	float modFreqVar = 0;
	float modFreqDepth = 0;
	
	
private:
	//Internal Flags
	//MessagingFlag quantizeHoldOffFlag;
	//  ..and data
	
	P8PanelButton * buttonAccess[16];
	P8PanelLed * ledAccess[16];
	
	//State machine stuff  
	PStates state;
	

};


#endif