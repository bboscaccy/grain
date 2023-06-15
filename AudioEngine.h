#pragma once

#include <RtAudio.h>
#include <RtMidi.h>

#include <memory>

#include "EventQueue.h"
#include "ISynth.h"

//main audio and midi engine for the app
//yeah move some  of the other stuff up in here....

//the synth class is basically so you can just play with generators easily and wire them up to keyboard control....


class AudioEngine
{

public:
	AudioEngine(void);
	~AudioEngine(void);

	friend void MidiCallback(double deltaTime, std::vector<unsigned char>* message, void* userData);
	friend int AudioCallback(void* outbuffer, void* inBuffer, unsigned int nFrames, double streamTime, RtAudioStreamStatus status, void* userData);


	inline MidiEventQueue& GetEventQueue()
	{
		return midiEvents;
	}

	void Init();
	void Shutdown();

	//usefull for mapping control messages...?
	inline bool DebugMidi()
	{
		return debugMidi;
	}

	inline void SetSynth(ISynth* s)
	{
		synth = s;
	}

	inline void SetDevice(IDevice* d)
	{
		device = d;
	}

private:
	MidiEventQueue midiEvents;
	ISynth* synth;
	IDevice* device;


	std::shared_ptr<RtMidiIn> midiIn;
	std::shared_ptr<RtAudio> audio;

	unsigned int bufferSize;
	unsigned int sampleRate;
	bool debugMidi;

};

//ok i'm liking the whole inline tick function thing that works pretty well



