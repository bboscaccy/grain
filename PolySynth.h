#pragma once

#include "ISynth.h"


//T just needs a render method in here....
template<class T>
struct SynthVoice
{
	bool isActive;
	unsigned char boundNote;
	T item;

	MidiEvent eventBuffer[16];
	unsigned char eventCount;
	
	float* buffer;

};




template<class T>
class PolySynth : public ISynth
{

public:
	PolySynth()
	{
		voiceCount = 1;
		//voices = new SynthVoice<T>[1]();
		//voices[0] = new SynthVoice<T>();
		//voices[0].isActive = false;
		//create voice buffers...
	}

	~PolySynth()
	{
		//delete[] voices; //thats leaking the buffers fyi...
	}

	void SetVoiceCount(size_t n)
	{
		//delete[] voices;
		voiceCount = n;
		voices = new SynthVoice<T>[n];
		for(size_t i = 0; i < voiceCount; i++)
		{
			//voices[i] = SynthVoice<T>();
			voices[i].eventCount = 0;
			voices[i].buffer = nullptr;
			memset(voices[i].eventBuffer, 0, sizeof(MidiEvent) * 16);
			
		}

		//create voice buffers...
	}

	size_t GetVoiceCount()
	{
		return voiceCount;
	}

	void SetEngineInfo(size_t bufferSize, size_t channelCount, size_t sampleRate)
	{
		bs = bufferSize;
		numberOfChannels = channelCount;

		for(size_t i = 0; i < voiceCount; i++)
		{
			voices[i].buffer = new float[bufferSize * channelCount];
			memset(voices[i].buffer, 0, sizeof(float) * bufferSize * channelCount);
			voices[i].isActive = false;
			voices[i].item.Init();
		}

	}


	int Render(MidiEvent* eventList, size_t numberOfEvents, float* buffer, size_t frameSize, size_t channelCount)
	{
		//we are going to make a special version of this for monophonic...
		if(numberOfEvents > 0)
		{
			for(size_t i = 0; i < numberOfEvents; i++)
			{
				if(eventList[i].midiData[0] == 144)
				{
					//this works for ignore mode
					//for steal mode we need to implement an counter and assign it to mod vc
					for(size_t v = 0; v < voiceCount; v++)
					{
						if(!voices[v].isActive)
						{
							//we found a voice
							voices[v].boundNote = eventList[i].midiData[1];
							voices[v].isActive = true;
							voices[v].eventBuffer[voices[v].eventCount] = eventList[i];
							voices[v].eventCount++;
							break;
						}
					}
				}
				else if(eventList[i].midiData[0] == 128)
				{
					//find the active voice bound to this note and send it this event...
					for(size_t v = 0; v < voiceCount; v++)
					{
						if(voices[v].isActive && voices[v].boundNote == eventList[i].midiData[1])
						{
							//we found our voice....
							voices[v].eventBuffer[voices[v].eventCount] = eventList[i];
							voices[v].eventCount++;
						}
					}
				}
				else
				{
					//pass this event on to all active voices here...
					for(size_t v = 0; v < voiceCount; v++)
					{
						if(voices[v].isActive)
						{
							voices[v].eventBuffer[voices[v].eventCount] = eventList[i];
							voices[v].eventCount++;
						}
					}
				}
			}
		}

		for(size_t v = 0; v < voiceCount; v++)
		{
			if(voices[v].isActive)
			{
				int res = voices[v].item.Render(voices[v].eventBuffer, voices[v].eventCount, voices[v].buffer, frameSize, channelCount); //... fill this in
				voices[v].eventCount = 0;
				voices[v].isActive = (res == 0);
			}
		}

		//now here sum all active voices...
		for(unsigned int i = 0; i < frameSize * channelCount; i+= channelCount)
		{
			buffer[i] = 0;
			buffer[i + 1] = 0;

			for(size_t v = 0; v < voiceCount; v++)
			{
				if(voices[v].isActive)
				{
					buffer[i] += voices[v].buffer[i];
					buffer[i + 1] += voices[v].buffer[i + 1];
				}
			}
		}
		
		return 0;
	}

private:
	SynthVoice<T>* voices;
	size_t voiceCount;
	
	size_t bs;
	size_t numberOfChannels;
};