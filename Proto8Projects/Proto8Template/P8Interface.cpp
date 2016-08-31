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
#include "P8Interface.h"
#include "P8PanelComponents.h"
#include "P8Panel.h"
#include "Arduino.h"
#include "flagMessaging.h"
#include "proto-8Hardware.h"
#include "VoltageMonitor.h"

extern VoltageMonitor LEDs;
extern AnalogMuxTree knobs;
extern SwitchMatrix switches;


P8Interface::P8Interface( void )
{
	//Controls
	state = PInit;
	
	buttonAccess[0] = &button1;
	buttonAccess[1] = &button2;
	buttonAccess[2] = &button3;
	buttonAccess[3] = &button4;
	buttonAccess[4] = &button5;
	buttonAccess[5] = &button6;
	buttonAccess[6] = &button7;
	buttonAccess[7] = &button8;
	buttonAccess[8] = &button9;
	buttonAccess[9] = &button10;
	buttonAccess[10] = &button11;
	buttonAccess[11] = &button12;
	buttonAccess[12] = &button13;
	buttonAccess[13] = &button14;
	buttonAccess[14] = &button15;
	buttonAccess[15] = &button16;
	
	ledAccess[0] = &led1;
	ledAccess[1] = &led2;
	ledAccess[2] = &led3;
	ledAccess[3] = &led4;
	ledAccess[4] = &led5;
	ledAccess[5] = &led6;
	ledAccess[6] = &led7;
	ledAccess[7] = &led8;
	ledAccess[8] = &led9;
	ledAccess[9] = &led10;
	ledAccess[10] = &led11;
	ledAccess[11] = &led12;
	ledAccess[12] = &led13;
	ledAccess[13] = &led14;
	ledAccess[14] = &led15;
	ledAccess[15] = &led16;
	
}

void P8Interface::reset( void )
{
	//Set explicit states
	//Set all LED off
	LEDs.clear();
	
}

//---------------------------------------------------------------------------//
//
//  To process the machine,
//    take the inputs from the system
//    process human interaction hard-codes
//    process the state machine
//    clean up and post output data
//
//---------------------------------------------------------------------------//
void P8Interface::processMachine( void )
{
	//switches.scan();
	//knobs.scan();
	update();
	//Do small machines
//Example usage
//	if( trackDownButton.serviceRisingEdge() )
//	{
//		if( viewingTrack > 1 )
//		{
//			viewingTrack--;
//		}
//	}
//	if( quantizeSelector.serviceChanged() )
//	{
	uint8_t updateDisplay = 0;
	for(int i = 0; i < 16; i++)
	{
		if( buttonAccess[i]->serviceRisingEdge() )
		{
			updateDisplay = 1;
			//Set to number of 15 if not active
			if(p8ParamActive[i])
			{
				activeParam = i;
			}
			else
			{
				activeParam = 15;//Flip to idle
			}
			p8ParamSync = 0;
			
		}
	}
//	if( button16.serviceRisingEdge() )
//	{
//		updateDisplay = 1;
//		activeParam = 15;
//		p8ParamSync = 1;
//
//		
//	}
	if( fixtureKnob.serviceChanged() == 1 )
	{
		//LEDs.store( fixtureKnob.getState() + 1, 1 );
		//LEDs.store( attackBendKnob.getState() + 1, 0 );
		//LEDs.setNumber1( fixtureKnob.getState() );
		//Serial.println( fixtureKnob.getState() );
		
		//If param active check sync, else set 15 active
		if(p8ParamActive[activeParam])
		{
			//if sync
			if(p8ParamSync)
			{
				p8Param[activeParam] = fixtureKnob.getState();
				updateDisplay = 1;
			}
			else
			{
				int16_t upperRange = p8Param[activeParam] + 5;
				if(upperRange > 255) upperRange = 255;
				int16_t lowerRange = p8Param[activeParam] - 5;
				if(lowerRange < 0) lowerRange = 0;
				if((fixtureKnob.getState() < upperRange)&&(fixtureKnob.getState() > lowerRange))
				{
					//Close enough
					p8ParamSync = 1;
					p8Param[activeParam] = fixtureKnob.getState();
					updateDisplay = 1;
				}
			}
		}
		updateDisplay = 1;

	}
	if(updateDisplay)
	{
		//Serial.println(activeParam);
		//Serial.println()
		for( int i = 0; i < 15; i++ )
		{
			if( p8ParamActive[i] )
			{
				if( i == activeParam )
				{
					if( p8ParamSync )
					{
						//set normal flashing
						ledAccess[i]->setState( LEDFLASHING );
					}
					else
					{
						//no sync
						ledAccess[i]->setState( LEDFLASHINGFAST );
					}
				}
				else
				{
					ledAccess[i]->setState( LEDON );
				}
			}
			else
			{
				ledAccess[i]->setState( LEDOFF );
			}
		}
		if( activeParam == 15 )
		{
			//flash the idle
			ledAccess[15]->setState( LEDFLASHING );
		}
		else
		{
			ledAccess[15]->setState( LEDOFF );
		}
		updateDisplay = 0;
	}
	
	LEDs.setNumber1( fixtureKnob.getState() );
	float tempVoltage = 0;
	float tempFactor = 0; // n * factor = 2.5
	tempFactor = 2.5 / refKnob.getState();
	tempVoltage = (float)rail18Knob.getState() * tempFactor * 2;
	LEDs.setVoltage( tempVoltage, 0 );
	tempVoltage = (float)rail33Knob.getState() * tempFactor * 2;
	LEDs.setVoltage( tempVoltage, 3 );
	//'set' all the values
	//display1.update();
	//update();
	
	//Do main machine
	tickStateMachine();
	
	//Do pure LED operations first
	
	//System level LEDs
	//led10.setState(LEDON);
	update();
	//Panel level LEDs
	//LEDs.send();
}

void P8Interface::tickStateMachine()
{
	//***** PROCESS THE LOGIC *****//
    //Now do the states.
    PStates nextState = state;
    switch( state )
    {
    case PInit:
		nextState = PIdle;
		
		break;
	case PIdle:
		nextState = PIdle;
        break;
    default:
        nextState = PInit;
		//Serial.println("!!!DEFAULT STATE HIT!!!");
        break;
    }
	
    state = nextState;

}

void P8Interface::timersMIncrement( uint8_t inputValue )
{
	button1.buttonDebounceTimeKeeper.mIncrement(inputValue);
	button2.buttonDebounceTimeKeeper.mIncrement(inputValue);	
	button3.buttonDebounceTimeKeeper.mIncrement(inputValue);	
	button4.buttonDebounceTimeKeeper.mIncrement(inputValue);	
	button5.buttonDebounceTimeKeeper.mIncrement(inputValue);	
	button6.buttonDebounceTimeKeeper.mIncrement(inputValue);	
	button7.buttonDebounceTimeKeeper.mIncrement(inputValue);	
	button8.buttonDebounceTimeKeeper.mIncrement(inputValue);	
	button9.buttonDebounceTimeKeeper.mIncrement(inputValue);	
	button10.buttonDebounceTimeKeeper.mIncrement(inputValue);	
	button11.buttonDebounceTimeKeeper.mIncrement(inputValue);	
	button12.buttonDebounceTimeKeeper.mIncrement(inputValue);	
	button13.buttonDebounceTimeKeeper.mIncrement(inputValue);	
	button14.buttonDebounceTimeKeeper.mIncrement(inputValue);	
	button15.buttonDebounceTimeKeeper.mIncrement(inputValue);	
	button16.buttonDebounceTimeKeeper.mIncrement(inputValue);	
	

}
