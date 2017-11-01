/*
==============================================================================

This file was auto-generated!

==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <Windows.h>
#include <myo/myo.hpp>
#include "MyoData.h"
#include "MyoHub.h"
#include "wiimote.h"
#include <cstdlib>



using namespace std;

void on_state_change(wiimote &remote, state_change_flags  changed, const wiimote_state &new_state)
{
	// we use this callback to set report types etc. to respond to key events
	//  (like the wiimote connecting or extensions (dis)connecting).

	// NOTE: don't access the public state from the 'remote' object here, as it will
	//		  be out-of-date (it's only updated via RefreshState() calls, and these
	//		  are reserved for the main application so it can be sure the values
	//		  stay consistent between calls).  Instead query 'new_state' only.

	// the wiimote just connected
	if (changed & CONNECTED)
	{
		// ask the wiimote to report everything (using the 'non-continous updates'
		//  default mode - updates will be frequent anyway due to the acceleration/IR
		//  values changing):

		// note1: you don't need to set a report type for Balance Boards - the
		//		   library does it automatically.

		// note2: for wiimotes, the report mode that includes the extension data
		//		   unfortunately only reports the 'BASIC' IR info (ie. no dot sizes),
		//		   so let's choose the best mode based on the extension status:
		if (new_state.ExtensionType != wiimote::BALANCE_BOARD)
		{
			if (new_state.bExtension)
				remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR_EXT); // no IR dots
			else
				remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR);		//    IR dots
		}
	}
}
//==============================================================================
/*
This component lives inside our window, and this is where you should put all
your controls and content.
*/
class MainContentComponent : public AudioAppComponent,
	public Button::Listener,
	public ChangeListener
{
public:

	enum armPosition
	{
		FOLDED,
		EXTENDED
	};

	//==============================================================================
	MainContentComponent::MainContentComponent()
	{

		setSize(800, 300);
		// specify the number of input and output channels that we want to open
		setAudioChannels(0, 2);
		// Initialise Buttons
		buttPlay = new TextButton("Play");
		addAndMakeVisible(buttPlay);
		buttPlay->setBounds(20, 100, 750, 100);
		buttPlay->addListener(this);
		buttPlay->setToggleState(false, false);
		// Initialise Myo
		initialiseMyo();
		// Initialise Wii remote
		initialiseRemote();
	}

	MainContentComponent::~MainContentComponent()
	{
		delete buttPlay;
		transportSource.setSource(nullptr);
		transportSource.removeChangeListener(this);
		//delete readerSource;

		shutdownAudio();
	}

	//=======================================================================
	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override
	{
		// This function will be called when the audio device is started, or when
		// its settings (i.e. sample rate, block size, etc) are changed.

		// You can use this function to initialise any resources you might need,
		// but be careful - it will be called on the audio thread, not the GUI thread.

		// For more details, see the help for AudioProcessor::prepareToPlay()

		formatManager.registerBasicFormats(); //Allow AudioFormatManager object to detect different audio file formats and create appropriate file reader objects
		sourcePlayer.setSource(&transportSource); // Connect the AudioTransportSource object to AudioSourcePlayer object
		deviceManager.addAudioCallback(&sourcePlayer); // Connect AudioSourcePlayer object to AudioDeviceManager object
		deviceManager.initialise(0, 2, nullptr, false); // 0 input channels required, 1 for mono output, no saved state
		deviceManager.addChangeListener(this);
		//DBG(deviceManager.getCurrentAudioDevice()->getCurrentSampleRate());
		transportSource.addChangeListener(this);
		//state = Stopped;
		preparationFinished = true;


	}

	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override
	{
		// Your audio-processing code goes here!
		// For more details, see the help for AudioProcessor::getNextAudioBlock()

		// Right now we are not producing any data, in which case we need to clear the buffer
		// (to prevent the output of random noise)
		bufferToFill.clearActiveBufferRegion();
		//poseRest();
		switchEMGdata();
		refreshRemote();
		selectArmPosition();
		//DBG(myArm);
		//DBG(counter);
		if ((initialisationFinished==true) && (preparationFinished == true))
		{
			if (transportSource.isPlaying() == false)
			{
				hubInstance.MyoRun(200/20);
			}
			else
			{
				hubInstance.MyoRunOne(10);
			}
		}

		//hubInstance.MyoRun(1000 / 20);
		//myoInstance.printEMGData();
		//DBG(myoInstance.getEMGSeven());
		if (myArm == EXTENDED)
		{
			if ((myoInstance.getEMGSeven() > 70) && (availableEFour == true))
			{
				//myo->setStreamEmg(myo::Myo::streamEmgDisabled);
				//EMGStreamEnabled = false;
				availableCFour = true;
				availableFFour = true;
				availableAFour = true;
				availableBFour = true;
				availableDFour = true;
				availableEFour = false;
				oneTimePlay = false;
				counterPlay = 0;
				resetSource();
				openNote("E4.wav");
				noteName = "E4.wav";
				DBG("E4 loaded");
			}
			else if (((myoInstance.getEMGFour() >= 50) || (myoInstance.getEMGTwo() >= 50)) && (myoInstance.getEMGFive() >= 60) && (availableDFour == true))
			{
				//myo->setStreamEmg(myo::Myo::streamEmgDisabled);
				//EMGStreamEnabled = false;
				availableCFour = true;
				availableEFour = true;
				availableFFour = true;
				availableAFour = true;
				availableBFour = true;
				availableDFour = false;
				oneTimePlay = false;
				counterPlay = 0;
				resetSource();
				openNote("D4.wav");
				noteName = "D4.wav";
				DBG("D4 loaded");
			}
			else if ((myoInstance.getEMGOne() >= 65) && (myoInstance.getEMGZero() >= 55) && (availableCFour == true))
			{
				//myo->setStreamEmg(myo::Myo::streamEmgDisabled);
				//EMGStreamEnabled = false;
				availableDFour = true;
				availableEFour = true;
				availableFFour = true;
				availableAFour = true;
				availableBFour = true;
				availableCFour = false;
				oneTimePlay = false;
				counterPlay = 0;
				resetSource();
				openNote("C4.wav");
				noteName = "C4.wav";
				DBG(" C4 loaded");
			}
		}
		else if (myArm == FOLDED)
		{
			if ((myoInstance.getEMGSeven() > 70) && (availableFFour == true))
			{
				//myo->setStreamEmg(myo::Myo::streamEmgDisabled);
				//EMGStreamEnabled = false;
				availableCFour = true;
				availableAFour = true;
				availableBFour = true;
				availableDFour = true;
				availableEFour = true;
				availableFFour = false;
				oneTimePlay = false;
				counterPlay = 0;
				resetSource();
				openNote("F4.wav");
				noteName = "F4.wav";
				DBG("F4 loaded");
			}
			else if (((myoInstance.getEMGFour() >= 50) || (myoInstance.getEMGTwo() >= 50)) && (myoInstance.getEMGFive() >= 60) && (availableAFour == true))
			{
				//myo->setStreamEmg(myo::Myo::streamEmgDisabled);
				//EMGStreamEnabled = false;
				availableCFour = true;
				availableEFour = true;
				availableFFour = true;
				availableBFour = true;
				availableDFour = true;
				availableAFour = false;
				oneTimePlay = false;
				counterPlay = 0;
				resetSource();
				openNote("A4.wav");
				noteName = "A4.wav";
				DBG("A4 loaded");
			}
			else if ((myoInstance.getEMGOne() >= 65) && (myoInstance.getEMGZero() >= 55) && (availableBFour == true))
			{
				//myo->setStreamEmg(myo::Myo::streamEmgDisabled);
				//EMGStreamEnabled = false;
				availableDFour = true;
				availableEFour = true;
				availableFFour = true;
				availableAFour = true;
				availableCFour = true;
				availableBFour = false;
				oneTimePlay = false;
				counterPlay = 0;
				resetSource();
				openNote("B4.wav");
				noteName = "B4.wav";
				DBG(" B4 loaded");
			}
		}

		setSourceState();
		setSourceGain();
		rumbleSet();
		playVibrato(noteName);

		/*if ((myoInstance.getMyoPose()== myo::Pose::waveIn ) && (myoInstance.getOneTimePlay()== true))
		{
			//hubInstance.MyoRunOne(3000 / 20);
			//myo->setStreamEmg(myo::Myo::streamEmgDisabled);
			Sleep(5);
			resetSource();
			openNote("C4.wav");
			playNote();
			myoInstance.setOneTimePlay(false);
		}
		else if ((myoInstance.getMyoPose() == myo::Pose::waveOut) && (myoInstance.getOneTimePlay() == true))
		{
			//hubInstance.MyoRunOne(3000 / 20);
			//myo->setStreamEmg(myo::Myo::streamEmgDisabled);
			Sleep(5);
			resetSource();
			openNote("D4.wav");
			playNote();
			myoInstance.setOneTimePlay(false);
		}*/

	}

	void releaseResources() override
	{
		// This will be called when the audio device stops, or when it is being
		// restarted due to a setting change.

		// For more details, see the help for AudioProcessor::releaseResources()
		transportSource.setSource(nullptr);

		//delete readerSource;


	}

	void buttonClicked(Button* button)
	{
		if (button == buttPlay)
		{
			buttonCounter = buttonCounter + 1;
			if (buttonCounter % 2 == 0)
			{
				buttPlay->setToggleState(false, false);
				buttonCounter = 0;
			}
			else
			{
				buttPlay->setToggleState(true, false);
				resetSource();
				openNote("C4.wav");
				playNote();
			}
			/*while (buttPlay->getToggleState() == true)
			{

			/*resetSource();
			openNote();
			playNote();
			}*/
		}

	}

	void changeListenerCallback(ChangeBroadcaster * source)
	{

	}

	//========================= Personal functions===========================

	void openNote(juce::StringRef loadWav)
	{
		// Open the file depending of the finger pressed
		
		/*FileChooser chooser("select mp3 file");
		if (chooser.browseForFileToOpen())
		{
		File file(chooser.getResult());
		readerSource = new AudioFormatReaderSource(formatManager.createReaderFor(file), true);
		transportSource.setSource(readerSource);


		}*/

		File audioFile = File::getCurrentWorkingDirectory().getChildFile(loadWav);
		fileExist = audioFile.existsAsFile();
		readerSource = new AudioFormatReaderSource(formatManager.createReaderFor(audioFile), true);
		transportSource.setSource(readerSource,0,nullptr,sampleRate);
		//transportSource.setSource(readerSource, 0, nullptr, 20000);
		//reSampler =new ResamplingAudioSource(&transportSource,false);
		//reSampler->setResamplingRatio(0.9);
		//sourcePlayer.setSource(reSampler);
		//readerSource = new AudioFormatReaderSource(formatManager.createReaderFor(audioFile),true); // File pass to AudioFormatManager object to create a file reader object
		//transportSource.setSource(readerSource); // Set the source of the file to the transportSource
	}

	void playNote()
	{
		// Play the note from the start
		transportSource.setPosition(0);
		transportSource.start();
	}

	void resetSource()
	{
		// Reset buffer after changing note
		transportSource.setSource(nullptr);
		readerSource = nullptr;
		//reSampler = nullptr;
	}
	void setSourceState()
	{
		// Decide of the state of the audio (playing/mute) depending of the Wii remote movement
		YAccel = remote.Acceleration.Y * 100;
		if ((oldYAccel <= (YAccel + 3)) && (oldYAccel >= (YAccel - 3)))
		{
			counterMute = counterMute + 1;
			if (counterMute > 100)
			{
				counterPlay = 0;
				volSource = volSource - 0.1;
				//DBG(volSource);
				//transportSource.setGain(volSource);
				if ((volSource <= 0.21) && (oneTimeStop == false))
				{
					DBG(" volMute");
					volSource = 0;
					transportSource.stop();
					oneTimeStop = true;
				}
			}
		}
		else
		{
			volSource = abs(remote.Acceleration.X);
			counterMute = 0;
			counterPlay = counterPlay + 1;
			if ((counterPlay > 5) && (counterPlay < 7) && (oneTimePlay == false))
			{
				playNote();
				oneTimePlay = true;
				oneTimeStop = false;
				DBG("playNote");
			}
		}
		oldYAccel = YAccel;
	}

	void setSourceGain()
	{
		//SetSourceGain() set the volume of the audio
		if (volSource >= 1.5) // Fix limit of the audio value to sound natural
		{
			volSource = 1.5;
		}
		else if (volSource <= 0.3)
		{
			volSource = 0.3;
		}

		if (transportSource.isPlaying() == true)
		{
			transportSource.setGain(volSource);
		}
	}

	void playVibrato(juce::StringRef nameNote)
	{
		//DBG(myoInstance.getOrientationRoll());
		//DBG(myoInstance.getOrientationX());
		if (myoInstance.getOrientationRoll() >= 1.0) // Activate the vibrato effect only if the hand rotate to the left with an important degree
		{
			vibratoOn = true;
			counterVibrOff = 0;
		}
		else if (myoInstance.getOrientationRoll() < 0.4) // If the hand is not rotating for a certain time, disable the vibrato effect
		{
			counterVibrOff = counterVibrOff + 1;
			if (counterVibrOff > 15)
			{
				vibratoOn = false;
			}
		}
		if (vibratoOn == true)
		{
			if (fileExist == true)
			{
				vibratPos = transportSource.getCurrentPosition();
				resetSource();
				File audioFile = File::getCurrentWorkingDirectory().getChildFile(nameNote);
				fileExist = audioFile.existsAsFile();
				readerSource = new AudioFormatReaderSource(formatManager.createReaderFor(audioFile), true);
				if (myoInstance.getOrientationRoll() <= 0.4)
				{
					transportSource.setSource(readerSource, 0, nullptr, sampleRate);
				}
				else if (myoInstance.getOrientationRoll() > 0.4) //Increase Pitch of the note will depend of the degree of the hand rotation
				{
					transportSource.setSource(readerSource, 0, nullptr, sampleRate + (myoInstance.getOrientationRoll()/0.0005));
					//DBG(myoInstance.getOrientationRoll()/0.00042);
				}

				if (transportSource.isPlaying() == false)
				{
					transportSource.setPosition(vibratPos);
					//DBG(vibratPos);
					transportSource.start();
				}
			}
		}

	}
	//===================Myo related functions===============================
	
	void initialiseMyo()
	{
		// Initialise the Myo
		try
		{
			// Create a Hub with a application identifier
			DBG(" Attempting to find a Myo...");
			//DBG(String ("1") +String("  ") +String ("2"));
			// It needs to detect the Myo.
			// waitforMyo() will let some times to detect the Myo
			//It will return a null pointer if it fails
		    myo = hubInstance.MyoWait(10000);

			if (!myo)
			{
				throw runtime_error(" Did you buy a Myo?");
				DBG("Have you buy a Myo?");
			}
			// in case the Myo is founded
			DBG("Connected to a Myo wristband!");
			// Enable the EMG streaming in the detected Myo
			//myo->setStreamEmg(myo::Myo::streamEmgEnabled);
			//EMGStreamEnabled = true;
			hubInstance.addMyoListener(myoInstance);
			//hubInstance.removeMyoListener(myoInstance);
			initialisationFinished = true;

		}
		catch (const exception& e)
		{
			cerr << "Error:" << e.what() << endl;
			DBG("Error:");
			cerr << "Press enter to continue";
			cin.ignore();

		}

	}

	void switchEMGdata()
		// Will switch between enable EMG data streaming and disabling
		// Enable it for a too long time will cause the program to finish calculation of the position of the Myo and indirectly slowing the program (5 sec difference between sound and gesture)
		// Ideal setting : 1/11 Enable EMG stream
	{
		if (counter > 1 && EMGStreamEnabled == true)
		{
			myo->setStreamEmg(myo::Myo::streamEmgDisabled);
			EMGStreamEnabled = false;
			counter = 0;
		}
		else if (counter >11 && EMGStreamEnabled == false)
		{
			myo->setStreamEmg(myo::Myo::streamEmgEnabled);
			EMGStreamEnabled = true;
			counter = 0;
		}
		counter = counter + 1;
	}

	void poseRest()
	{
		if ((myoInstance.getEMGZero() < restThresh) && (myoInstance.getEMGOne() < restThresh) && (myoInstance.getEMGTwo() < restThresh) && (myoInstance.getEMGThree() < restThresh) && (myoInstance.getEMGFour() < restThresh) && (myoInstance.getEMGFive() < restThresh) && (myoInstance.getEMGSix() < restThresh) && (myoInstance.getEMGSeven() < restThresh))
		{
			//DBG(" play available");
			oneTimePlay= false;
		}
	}

	void selectArmPosition()
	{
		// Detect the position of the arm
		if (myoInstance.getOrientationPitch() <= 0.8)
		{
			myArm = EXTENDED;
		}
		else if (myoInstance.getOrientationPitch() > 0.8)
		{
			myArm = FOLDED;
		}
	}
	//================== Wii Remote functions related========================


	void initialiseRemote()
	{
		// Initialise the Wii remote
		remote.ChangedCallback = on_state_change;
		remote.CallbackTriggerFlags = (state_change_flags)(CONNECTED | EXTENSION_CHANGED | MOTIONPLUS_CHANGED);
		DBG("Looking for a Wiimote");
		while (!remote.Connect(wiimote::FIRST_AVAILABLE))
		{
		}
		remote.SetLEDs(0x0f);
		DBG("Wiimote connected");

	}

	void refreshRemote()
	{
		//Refresh the state of the Wii remote, inactive if no change
		if (remote.RefreshState() == NO_CHANGE)
		{
			Sleep(1);
		}
	}
	void rumbleSet()
	{
		//rumbleSet() is a function only active when B button of the Remote is pressed
		remote.SetRumble(false);
		//randTrem = ((float)rand() / (float)(RAND_MAX)) * 0.4; // Generate Random number between 0 and 0.4
		//DBG(counterTrem);
		//DBG(randTrem);
		if (remote.Button.B())
		{
			if (initialisedTrem == false)
			{
				//Initialise the fake triangular sine right after the button is pressed
				counterTrem = 0.35;
				initialisedTrem = true;
			}
			if (tremIncrease == false)
			{
				// Add to the original volume a triangular sine between 0.25 and 0.5 amplitude
				if (counterTrem > 0.25)
				{
					volSource = volSource - 0.05;
					counterTrem = counterTrem - 0.05;
				}
				else
				{
					tremIncrease = true;
				}
				
			}
			else if (tremIncrease == true)
			{
				if (counterTrem < 0.5)
				{
					volSource = volSource + 0.05;
					counterTrem = counterTrem + 0.05;
				}
				else
				{
					tremIncrease = false;
				}
			}
			//DBG(transportSource.getTotalLength());
			//playVibrato(noteName);
			//DBG(counterTrem);
		}
		else
		{
			initialisedTrem= false;
		}
	}
	//=======================================================================
	void paint(Graphics& g) override
	{
		// (Our component is opaque, so we must completely fill the background with a solid colour)
		g.fillAll(Colour(0xff60D8EB));

		g.setFont(Font(16.0f));
		g.setColour(Colours::black);


		// You can add your drawing code here!
	}

	void resized() override
	{
		// This is called when the MainContentComponent is resized.
		// If you add any child components, this is where you should
		// update their positions.
	}

	//======================Old code================================================
	
	//poseRest();
	//rumbleSet();
	//counter = counter + 1;
	//switchEMGdata();
	//refreshRemote();
	////DBG(counter);
	//if ((initialisationFinished == true) && (preparationFinished == true))
	//{
	//	if (transportSource.isPlaying() == false)
	//	{
	//		hubInstance.MyoRunOne(1000 / 20);
	//	}
	//	else
	//	{
	//		hubInstance.MyoRunOne(10);
	//	}
	//}

	//hubInstance.MyoRun(1000 / 20);
	//myoInstance.printEMGData();
	//DBG(myoInstance.getEMGSeven());
	//if (/*(myoInstance.getEMGZero() > 40) &&*/ ((myoInstance.getEMGOne() > 60) /*|| (myoInstance.getEMGZero() > 65)*/) && (oneTimePlay == false) && (remote.Button.B() == true))
	//{
	//	myo->setStreamEmg(myo::Myo::streamEmgDisabled);
	//	EMGStreamEnabled = false;
	//	DBG("thumb+ index");
	//	resetSource();
	//	openNote("C4.wav");
	//	playNote();
	//	oneTimePlay = true;
	//}
	//else if ((myoInstance.getEMGSeven() > 50)/* && (myoInstance.getEMGOne() > 20) */ && (oneTimePlay == false) && (remote.Button.B() == true))
	//{
	//	//myo->setStreamEmg(myo::Myo::streamEmgDisabled);
	//	//EMGStreamEnabled = false;
	//	DBG("thumb+baby");
	//	resetSource();
	//	openNote("E4.wav");
	//	playNote();
	//	oneTimePlay = true;
	//}
	//else if (((myoInstance.getEMGFour() > 40) && (myoInstance.getEMGFive() > 40)) && /*(myoInstance.getEMGOne() >30) &&*/ (oneTimePlay == false) && (remote.Button.B() == true))
	//{
	//	//myo->setStreamEmg(myo::Myo::streamEmgDisabled);
	//	//EMGStreamEnabled = false;
	//	DBG("thumb+ring");
	//	resetSource();
	//	openNote("D4.wav");
	//	playNote();
	//	oneTimePlay = true;
	//}

	//setSourceGain();

	/*if ((myoInstance.getMyoPose()== myo::Pose::waveIn ) && (myoInstance.getOneTimePlay()== true))
	{
	//hubInstance.MyoRunOne(3000 / 20);
	//myo->setStreamEmg(myo::Myo::streamEmgDisabled);
	Sleep(5);
	resetSource();
	openNote("C4.wav");
	playNote();
	myoInstance.setOneTimePlay(false);
	}
	else if ((myoInstance.getMyoPose() == myo::Pose::waveOut) && (myoInstance.getOneTimePlay() == true))
	{
	//hubInstance.MyoRunOne(3000 / 20);
	//myo->setStreamEmg(myo::Myo::streamEmgDisabled);
	Sleep(5);
	resetSource();
	openNote("D4.wav");
	playNote();
	myoInstance.setOneTimePlay(false);
	}*/
private:
	//==============================================================================

	// Your private member variables go here...
	//Create variables for reading audio file
	AudioFormatManager formatManager;
	ScopedPointer<AudioFormatReaderSource> readerSource;
	AudioTransportSource transportSource;
	AudioSourcePlayer sourcePlayer;
	//ResamplingAudioSource* reSampler;
	juce::StringRef noteName;
	bool fileExist = false; // Check if there is a file in the TransportSource
	bool initialisationFinished = false; // Check if the initialisation is finished
	bool preparationFinished = false; // Check if the function preparetoPlay() is done
	bool EMGStreamEnabled= true; // Check if the EMG streaming is enabled
	bool oneTimePlay = false; // Allow a note to played one time only
	bool oneTimeStop = false; // Allow the transportSource to stop only when needed
	bool vibratoOn = false; // Flag to allow activation of the vibrato effect
	bool oneTimeVibrato = false; 
	bool tremIncrease = false; // Flag to simulate the tremolo triangular signal, true if the signal has to go up, false if it has to go down
	bool initialisedTrem = false; // Check if the initialisation for the tremolo is done
	//-------------------------------- Check which note is ready to play------------------------------//
	bool availableCFour = true;
	bool availableDFour = true;
	bool availableEFour = true;
	bool availableFFour = true;
	bool availableAFour = true;
	bool availableBFour = true;

	short counterPlay = 0; // Act as a trigger to play a note
	short buttonCounter = 0; // Act as a switcher for the button NOT USED
	short counterMute = 0; // Act as a counter to trigger the muting of the sound 
	short counterVibrOff = 0; // Act as a counter before turn off the vibrato effect
	short restThresh = 13; // Every EMG sensor have to become lower than this number to pass on rest pose
	int counter = 0; // Counter which switch on/off the EMG streaming
	float volSource = 0; // Variable for the volume
	float oldYAccel = 0; // Old value of the Y axis of the accelerator
	float YAccel = 0; // Present value of the Y axis of the accelerator
	float counterTrem = 0; // Counter which help creating a fake triangular signal
	float randTrem = 0; // NOT USED, suppose to store a random number for the tremolo
	double vibratPos = 0; // Variable which takes the actual postion of the audio before deleting
	double sampleRate = 41100; // Sample rate initialisation of the audio
	int8_t emgThresh1 = 25;

	//Create Button variables
	TextButton* buttPlay;

	//Create instance for the Myo
	MyoData myoInstance;
	MyoHub  hubInstance;
	myo::Myo* myo;

	//Create instance for the Wii Remote
	wiimote remote;

	//Create instance for the position of the arm
	armPosition myArm;

	


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
