#include "GrainEngine1.h"


GrainEngine1::GrainEngine1(void)
{
	adsr.SetAttack(2);
	adsr.SetSustainLevel(1);
	adsr.SetReleaseTime(.01);
	adsr.SetAttackLevel(1);

	locAdsr.SetAttack(.40);
	locAdsr.SetAttackLevel(1);
	locAdsr.SetReleaseTime(.01);
	locAdsr.SetSustainLevel(1);

}



GrainEngine1::~GrainEngine1(void)
{
}


void GrainEngine1::Signal(MidiEvent& evt)
{
	if(evt.IsNoteOn())
	{
		adsr.TriggerOn();
		locAdsr.TriggerOn();
		t = 0;
	}
	else if(evt.IsNoteOff())
	{
		adsr.TriggerOff();
		locAdsr.TriggerOn();
	}
}

