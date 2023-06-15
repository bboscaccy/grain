#include "Granulator.h"


Granulator::Granulator(void)
{
	duration = 50.0f;
	grainSize = 44100 * (duration / 1000.f) ;
	

	period = 44100; //we have a 1 second period here.....
	activeGrains = 0;

	adsr.SetAttack(.05);

}


Granulator::~Granulator(void)
{
}

void Granulator::Signal(MidiEvent& evt)
{
	s.Signal(evt);
	
	if(evt.IsNoteOn())
	{
		adsr.TriggerOn();
		//s.Scrub(evt.GetVelocity());
		//pos = evt.GetVelocity();
		t = 0;
		for(int i = 0; i < maxGrains; i++)
		{
			grains[i].isAlive = false;
		}
		activeGrains = 0;
	}
	else if(evt.IsNoteOff())
	{
		adsr.TriggerOff();
	}
}

void Granulator::SetParam(const std::string& name, const std::string& value)
{
	s.SetParam(name, value);
	//anything we need to do
}

void Granulator::SetParam(const std::string& name, float value)
{
	s.SetParam(name, value);
	//anything we need to do
}