// SA3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include <RtMidi.h>
#include <RtAudio.h>
#include <atomic>

#include "NoiseGenerator.h"
#include "AudioEngine.h"
#include "Sample.h"
#include "DynamicSynth.h"
#include "SineWaveOsc.h"
#include "Granulator.h"
#include "GrainSynth.h"
#include "GrainEngine1.h"

using namespace std;

volatile bool state = false;

atomic<int> queueNumber = 0;
Noise noiseGen;


int _tmain(int argc, _TCHAR* argv[])
{

	auto gs1 = new DynamicSynth<VoiceRender<GrainSynth1>>();



	
	AudioEngine* engine = new AudioEngine();
	
	DynamicSynth<NoiseGen>* noiseGen = new DynamicSynth<NoiseGen>();
	DynamicSynth<VoiceRender<SamplePlayer>>* sampler = new DynamicSynth<VoiceRender<SamplePlayer>>(8);
	auto sineSynth = new DynamicSynth<VoiceRender<MonoTick<SineWaveOsc>>>();

	//auto granular = new DynamicSynth<VoiceRender<MultiTimber<Granulator, SamplePlayer>>>();
	auto granular = new DynamicSynth<VoiceRender<Granulator>>();
	granular->SetParam("sample", "c:\\temp\\pwm_ts2.wav");

	auto s2 = new DynamicSynth<VoiceRender<MultiTimber<SamplePlayer, MonoTick<SineWaveOsc>>>>();

	sampler->SetParam("sample", "c:\\temp\\loop7m.wav");
	s2->SetParam("sample", "c:\\temp\\loop7m.wav");
	
	gs1->SetParam("sample", "c:\\temp\\wanndancechopped.wav");




	engine->SetDevice(gs1);
	engine->Init();

	getchar();
	
	engine->Shutdown();
	delete engine;
	
	
	return 0;
}

