#pragma once

#include "ADSR.h"
#include "EventQueue.h"
#include "GrainSynth.h"
//everything is based off of an adsr envelope
#include <stdlib.h>

class GrainEngine1
{
public:
	GrainEngine1(void);
	~GrainEngine1(void);

	void Signal(MidiEvent& evt);
	//tick is the DENSITY dude... some value between 0 and 1 * maxGrains = number of grains
	inline float Tick()
	{
		lastTick = adsr.Tick();
		t++;
		return lastTick;
	}

	inline float NextPos()
	{
		//return locAdsr.Tick();
		//return (float)rand() / (float)RAND_MAX;
		return lastTick;
	}

	inline float NextDuration()
	{
		//return lastTick * 5;
		auto res = locAdsr.Tick();
		if(res < .2)
		{
			return .2;
		}
		else
		{
			return res;
		}
		//return .50;

		//return (float)rand() / (float)RAND_MAX;
		//return .01;
	}
	//we can reverse scale this based on density...
	inline float NextAmplitude()
	{
		return .75;
	}

	inline int NextDelay()
	{
		return 0;
	}

	inline int NextPeriod()
	{
		//return 0;
		//return -1;
		return 0;
	}



private:
	ADSR adsr;
	size_t t;
	float lastTick;
	ADSR locAdsr;
};

typedef GrainSynth<GrainEngine1> GrainSynth1;