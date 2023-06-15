#pragma once


#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <iostream>

#include "Sample.h"
#include "ADSR.h"
#include "DynamicSynth.h"


//this class is depreciated bro..

struct Grain
{
	float pos; //scrub position from the clip...
	int size; //this is a counter that goes from size of clip to zero its in samples...
	bool isAlive;
	int cursor[2]; //current location....
	
	inline float CalcEnvelope(int channel)
	{
		int fs = size / 2;
		if(cursor[channel] > fs)
		{
			return ((float)(cursor[channel] - fs) / (float)fs) * 0.5f;
		}
		else
		{
			return ((float)(fs - cursor[channel]) / (float)fs) * 0.5f;
		}
	}

	
	
};

inline float LinearTransform(float t)
{
	return min(abs(t), 1.0f);
}

inline float SinTransform(float t)
{
	return min(abs(sin(t * 2.0f * (float)M_PI)), 1.0f);
}

inline float SincTransform(float t)
{
	float v = t * 2.0f * (float)M_PI * (float)M_PI;
	return min(abs( sin(v) / v), 1.0f);
}


//hmm our main problem here is that we are currently firing all the grains at once here...


class RandomGenerator
{
public:	
	//returns a value between 0 and 1 for location in the clip
	inline float NextOffset()
	{
		return (float)rand() / ((float)RAND_MAX);
	}

	//returns a value between 0 and 1 where v * MAX_GRAIN_LENGTH = duration
	inline float NextDuration()
	{
		auto res = (float)rand() / ((float)RAND_MAX);
		if(res < 0.2f)
		{
			return res;
		}
	}
};




class Granulator
{
public:
	Granulator(void);
	~Granulator(void);


	void Signal(MidiEvent& evt);

	inline void SetParam(const std::string& paramName, int value) {}
	void SetParam(const std::string& paramName, float value);
	void SetParam(const std::string& paramName, const std::string& value);


	inline bool GetState() { return adsr.IsActive(); }

	inline float Tick(unsigned channel = 0)
	{
		
		//
		if(channel == 0)
		{
			t++;
			ScheduleGrains();
		}
		
		float res = 0;

		for(int i = 0; i < maxGrains; i++)
		{
			if(grains[i].isAlive)
			{
				s.Scrub(grains[i].pos, grains[i].cursor[channel], channel);
				res += s.Tick(channel) * grains[i].CalcEnvelope(channel);
				++grains[i].cursor[channel];
				if(grains[i].cursor[channel] >= grains[i].size)
				{
					grains[i].isAlive = false;
					--activeGrains;
				}
			}
		}

		//adsr.Tick(channel);
		//we are just returning the sample at cursor * envelope then right...?
		return res * adsr.Tick(channel);
	}

	//test method that schedules the number of grains based on the sinc function....
	inline void ScheduleGrains()
	{
		int target = maxGrains; //(int)ceilf((float)maxGrains * LinearTransform(period / t)); //target number of grains we should have
		int numberToActive = target - activeGrains;
		while(numberToActive > 0)
		{
			//find first active 
			for(int i = 0; i < maxGrains; i++)
			{
				if(!grains[i].isAlive) //disco
				{
					grains[i].isAlive = true;
					grains[i].pos = gen.NextOffset();
					if(grains[i].pos > 0.8f)
					{
						grains[i].pos = 0.8f; //we need to make sure we don't go past the wave form as a grain is ramping up you get wierd clipping...
					}
					grains[i].size = grainSize;// * gen.NextDuration();
					grains[i].cursor[0] = 0;
					grains[i].cursor[1] = 0;
					--numberToActive;
					++activeGrains;
					i=maxGrains;
				}
			}
		}
	}


private:
	SamplePlayer s;
	float duration; //max grain duration in ms
	size_t grainSize; //max grain size in samples....
	size_t iter;
	MonoTick<ADSR> adsr;

	unsigned int t;

	static const int maxGrains = 15;
	Grain grains[maxGrains];
	size_t period;
	int activeGrains;

	RandomGenerator gen;

	//so we are going to need an array of envelopes?
	//and an array of scrub positions for the actual sample...

};

//so we need a function or a class that can generate
//grain offset, grain duration, and grain envelope.....
//density...? i.e how many grains are playing at once 
//so the densisity calculation is a function that returns between 1 and N over T
//so we set a MAX_GRAIN parameter and we have something that can take any set of parameters
//that we can have here...? i guess...

//t can be "wired" up to pitch, velocity an adsr, whatever you want
//these functions expect a number between -1 and 1


class StaticGenerator
{
public:	
	inline float NextOffset() { return 0;}
	inline float NextDuration() { return 1;}
};

class LinearGenerator
{
public:
	LinearGenerator() : offset(0), step(0)
	{

	}

	inline float NextOffset()
	{
		auto res = offset;
		offset += step;
		if(offset > 1)
		{
			offset -= 1;
		}
	}

	inline float NextDuration()
	{
		return 1;
	}
	
	float offset;
	float step;
};

