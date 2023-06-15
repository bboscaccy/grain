#pragma once

#include "DynamicSynth.h"

#include "Noise.h"
#include "ADSR.h"

//were going to need a "voice" class that can maintain note/velocicy/whatever....?
class NoiseGenerator 
{
public:
	NoiseGenerator(void);
	~NoiseGenerator(void);
	
	inline bool GetState()
	{
		return adsr.IsActive();
	}

	void Signal(MidiEvent& evt);

	inline float Tick()
	{
		return noise.Tick() * adsr.Tick() * currentVelocity;
	}

private:
	bool state;
	Noise noise;
	float currentVelocity;
	ADSR adsr;
};

typedef DummyParam<VoiceRender<MonoTick<NoiseGenerator>>> NoiseGen;