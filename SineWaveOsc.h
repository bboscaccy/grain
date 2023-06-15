#pragma once

#include <string>
#define _USE_MATH_DEFINES
#include <math.h>

#include "DynamicSynth.h"
#include "ADSR.h"



class SineWaveOsc
{
public:
	SineWaveOsc(void);
	~SineWaveOsc(void);

	inline void SetParam(const std::string& paramName, int value){}
	inline void SetParam(const std::string& paramName, float value) {}
	inline void SetParam(const std::string& paramName, const std::string& value) {}

	void Signal(MidiEvent& evt);

	inline float Tick()
	{
		float res = sin(iter);
		res *= adsr.Tick();
		iter+=step;
		if(iter >= 2.0f * M_PI)
		{
			iter -= 2.0f * M_PI;
		}
		return res;
	}

	inline bool GetState()
	{
		return adsr.IsActive();
	}

private:
	float w;
	float step;
	float iter;
	ADSR adsr;
};

