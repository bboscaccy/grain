#include "NoiseGenerator.h"
#include <iostream>

using namespace std;

NoiseGenerator::NoiseGenerator(void)
{
	state = false;
	adsr.SetSampleRate(44100);
	adsr.SetAttack(.25);
	adsr.SetSustainLevel(0.25f);
	adsr.SetReleaseTime(0.1f);
	adsr.SetDecayTime(0.5f);


}


NoiseGenerator::~NoiseGenerator(void)
{
}

void NoiseGenerator::Signal(MidiEvent& evt)
{
	if(evt.IsNoteOn())
	{
		currentVelocity = evt.GetVelocity();
		adsr.TriggerOn();
	}
	else if(evt.IsNoteOff())
	{
	
		adsr.TriggerOff();
	}

}

