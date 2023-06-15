#include "SineWaveOsc.h"



SineWaveOsc::SineWaveOsc(void)
{
	
}


SineWaveOsc::~SineWaveOsc(void)
{
}

void SineWaveOsc::Signal(MidiEvent& evt)
{
	if(evt.IsNoteOn())
	{
		w = 44100 / evt.GetNoteFrequency();
		
		step = (2.0f * M_PI) / w;
		iter = 0;
		adsr.TriggerOn();
	}
	else if(evt.IsNoteOff())
	{
		
		adsr.TriggerOff();
	}
}
