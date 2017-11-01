/*
  ==============================================================================

    MyoData.cpp
    Created: 8 Feb 2016 7:15:55pm
    Author:  Billy

  ==============================================================================
*/
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <cmath>

#include "MyoData.h"
#include <myo/myo.hpp>
#include "../JuceLibraryCode/JuceHeader.h"


bool sendData = true;
bool messageStop = false;
float roll;
float pitch;
float yaw;



MyoData::MyoData()
{
	emgSamples = { { 0, 0, 0, 0, 0, 0, 0, 0 } };

}

void MyoData::onPose(myo::Myo* myo, uint64_t timrstamp, myo::Pose pose)
//OnPose() will be called each time the Myo detects a different pose. NOT USING, avoid locking by bypassing with EMG data instead 
{
	currentPose = pose;
	/*if (pose != myo::Pose::unknown && pose != myo::Pose::rest) {
		// Tell the Myo to stay unlocked until told otherwise. We do that here so you can hold the poses without the
		// Myo becoming locked.
		myo->unlock(myo::Myo::unlockHold);
		// Notify the Myo that the pose has resulted in an action, in this case changing
		// the text on the screen. The Myo will vibrate.
		myo->notifyUserAction();
	}
	else {
		// Tell the Myo to stay unlocked only for a short period. This allows the Myo to stay unlocked while poses
		// are being performed, but lock after inactivity.
		myo->unlock(myo::Myo::unlockTimed);
	}*/
	/*myo->unlock(myo::Myo::unlockHold);
	if (pose == myo::Pose::waveIn || pose==myo::Pose::waveOut)
	{
		DBG(" pose in wave in or wave out");

	}*/
	/*if (pose == myo::Pose::unknown || pose == myo::Pose::rest)
	{
		DBG(" play available/reset");
		oneTimePlay = false;
	}*/
	myo->lock();
}

// onUnpair() will be called if the Myo is disconnected from Myo Connect
void MyoData::onUnpair(myo::Myo* myo, uint64_t timestamp)
{
	// We have lost the Myo, fill the array of the EMG Data to 0
	emgSamples.fill(0);
}

//onEmgData() will be called everytime the MYO provides new EMGdata
// and if streaming EMG is available
void MyoData::onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg)
{
	// Will store the data from the EMG sensor to the array emgSamples
	// The 8 values correspond to only one position
	for (int i = 0; i < 8; i++)
	{
		emgSamples[i] = emg[i];
	}
}

//onGyroscopeData() will be called everytime the MYO provides new gyroscope data 
// Active only if EMG streaming is enable
void MyoData::onGyroscopeData(myo::Myo* myo, uint64_t timestamp, const myo::Vector3<float>& gyro)
{
	gyroData = gyro;
}

//onOrientationData() will be called everytime the MYO provides data for the Orientaion
void MyoData::onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& rotation)
{
	//orientationData = quat;
	//orientAngle = orientationData.normalized();
	using std::atan2;
	using std::asin;
	using std::sqrt;
	using std::max;
	using std::min;

	// Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
	 roll = atan2(2.0f * (rotation.w() * rotation.x() + rotation.y() * rotation.z()),
		1.0f - 2.0f * (rotation.x() * rotation.x() + rotation.y() * rotation.y()));
	 pitch = asin(max(-1.0f, min(1.0f, 2.0f * (rotation.w() * rotation.y() - rotation.z() * rotation.x()))));
	 yaw = atan2(2.0f * (rotation.w() * rotation.z() + rotation.x() * rotation.y()),
		1.0f - 2.0f * (rotation.y() * rotation.y() + rotation.z() * rotation.z()));

}

void MyoData::printEMGData()
{
	//Print the EMG data, and/or other data
	for (unsigned int i = 0; i < emgSamples.size(); i++)
	{
		emgInt[i] = (int)emgSamples[i];
		emgString[i] = to_string(abs(emgInt[i]));
	}
	//DBG(emgString[0] + String("  ") + emgString[1] + String("  ") + emgString[2] + String("  ") + emgString[3] + String("  ") + emgString[4] + String("  ") + emgString[5] + String("  ") + emgString[6] + String("  ") + emgString[7] + String("  "));
	
	//DBG(String(gyroData.x()) + String("  ") +String(gyroData.y()) + String("  ") + String(gyroData.z()) + String("  "));

		//orientAngle = orientationData.normalized();
	//DBG(orientAngle.x());
	//DBG(String(roll) + String("  ") + String(pitch) + String("  ") + String(yaw) + String("  "));


}

//------------------------------ Return Functions to use on the MainComponent class ------------------------------//
bool MyoData::getMyoDataSend()
{
	return sendData;
}
bool MyoData::getMyoMessage()
{
	return messageStop;
}
myo::Pose MyoData::getMyoPose()
{
	return currentPose;
}

// --------------------------------------- Return EMG data for each sensor individually --------------------------//
int MyoData::getEMGZero()
{
	return abs(int(emgSamples[0]));
}
int MyoData::getEMGOne()
{
	return abs(int(emgSamples[1]));
}
int MyoData::getEMGTwo()
{
	return abs(int(emgSamples[2]));
}
int MyoData::getEMGThree()
{
	return abs(int(emgSamples[3]));
}
int MyoData::getEMGFour()
{
	return abs(int(emgSamples[4]));
}
int MyoData::getEMGFive()
{
	return abs(int(emgSamples[5]));
}
int MyoData::getEMGSix()
{
	return abs(int(emgSamples[6]));
}
int MyoData::getEMGSeven()
{
	return abs(int(emgSamples[7]));
}


//--------------------------------------------- Return Function from the Orientation of the Myo ---------------------//
float MyoData::getOrientationPitch()
{
	return abs(pitch);
}

float MyoData::getOrientationRoll()
{
	return abs(roll);
}

float MyoData::getOrientationYaw()
{
	return abs(yaw);
}

