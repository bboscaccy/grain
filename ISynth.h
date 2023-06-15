#pragma once

#include "EventQueue.h"
#include "Buffer.h"


class ISynth
{
public:	
	ISynth();
	virtual ~ISynth();
	
	virtual int Render(MidiEvent* eventList, size_t numberOfEvents, float* buffer, size_t frameSize, size_t channelCount);
	
	virtual void SetEngineInfo(size_t bufferSize, size_t channelCount, size_t sampleRate);
	//add sample rate and some reset deallys...

};




class IDevice
{
public:
	IDevice();
	virtual ~IDevice();
	virtual void Render(MidiEvent* eventList, size_t eventCount, Buffer& input, Buffer& output) = 0;
	virtual void SetEngineInfo(size_t bufferSize, size_t sampleRate);

	virtual size_t GetInputCount(); //default is 0-
	virtual size_t GetOutputCount(); //default is..?

};