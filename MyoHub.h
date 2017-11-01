/*
  ==============================================================================

    MyoHub.h
    Created: 11 Feb 2016 9:58:27pm
    Author:  Billy

  ==============================================================================
*/

#ifndef MYOHUB_H_INCLUDED
#define MYOHUB_H_INCLUDED

#include "MyoData.h"

class MyoHub : public myo::DeviceListener
{
public:
	~MyoHub();
	void initialise();
	void addMyoListener(MyoData &myoInstance);
	void removeMyoListener(MyoData &myoInstance);
    myo::Myo* MyoWait(int millisecond);
	void MyoRun(int millisecond);
	void MyoRunOne(int millisecond);

private:

};



#endif  // MYOHUB_H_INCLUDED
