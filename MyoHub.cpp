/*
  ==============================================================================

    MyoHub.cpp
    Created: 11 Feb 2016 9:58:27pm
    Author:  Billy

  ==============================================================================
*/

#include "MyoHub.h"
#include "MyoData.h"
#include <myo/myo.hpp>

myo::Hub hub("com.Billy.MainComponent");


MyoHub::~MyoHub()
{
	hub.removeListener(this);
}
void MyoHub::addMyoListener(MyoData &myoInstance)
{
	// Connect the Myo to the Hub
	hub.addListener(&myoInstance);
}
void MyoHub::removeMyoListener(MyoData &myoInstance)
{
	//Disconnect the Myo from the Hub
	hub.removeListener(&myoInstance);
}

 myo::Myo* MyoHub::MyoWait(int millisecond)
{
	// Wait in millisecond for the detection of the Myo
	return hub.waitForMyo(millisecond);
}

 void MyoHub::MyoRun(int millisecond)
 {
	 // Loop an event to detect any information coming fron the Myo
	 hub.run(millisecond);
 }
 void MyoHub::MyoRunOne(int millisecond)
 {
	 hub.runOnce(millisecond);
 }