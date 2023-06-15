#include "ISynth.h"

ISynth::ISynth()
{

}

ISynth::~ISynth()
{

}

int ISynth::Render(MidiEvent* eventList, size_t numberOfEvents, float* buffer, size_t frameSize, size_t channelCount)
{
	return 0;
}

void ISynth::SetEngineInfo(size_t bufferSize, size_t channelCount, size_t sampleRate)
{

}

IDevice::IDevice()
{

}

IDevice::~IDevice()
{

}

void IDevice::SetEngineInfo(size_t bufferSize, size_t sampleRate)
{

}

size_t IDevice::GetInputCount()
{
	return 0;
}

size_t IDevice::GetOutputCount()
{
	return 0;
}