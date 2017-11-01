/*
  ==============================================================================

    MyoData.h
    Created: 8 Feb 2016 7:15:55pm
    Author:  Billy

  ==============================================================================
*/

#ifndef MYODATA_H_INCLUDED
#define MYODATA_H_INCLUDED

#include <myo/myo.hpp>

#include <array>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

using namespace std;

class MyoData : public myo::DeviceListener
{
public:
		MyoData();
		void onPose(myo::Myo* myo, uint64_t timrstamp, myo::Pose pose);
		void onUnpair(myo::Myo* myo, uint64_t timestamp);
		void onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg);
		void onGyroscopeData(myo::Myo* myo, uint64_t timestamp, const myo::Vector3<float>& gyro);
		void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& rotation);
		void printEMGData();
		bool getWaveInState();
		bool getMyoMessage();
		bool getMyoDataSend();
		int getEMGZero();
		int getEMGOne();
		int getEMGTwo();
		int getEMGThree();
		int getEMGFour();
		int getEMGFive();
		int getEMGSix();
		int getEMGSeven();
		float getOrientationRoll();
		float getOrientationPitch();
		float getOrientationYaw();
		myo::Pose getMyoPose();
private:

	    array<int8_t, 8> emgSamples;
		array<int, 8> emgInt;
		array<string, 8> emgString;
		myo::Vector3<float> gyroData;
		myo::Quaternion<float> orientationData;
		myo::Quaternion<float> orientAngle;

		myo::Pose currentPose;
		bool waveInPose = false;

};




#endif  // MYODATA_H_INCLUDED
