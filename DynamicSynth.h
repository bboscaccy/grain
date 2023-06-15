#pragma once

#include <string>
#include <vector>
#include <map>

#include "Buffer.h"
#include "EventQueue.h"
#include "AudioEngine.h"

//synth architecture class for dynamically creating voices
//synth voices are always stereo...? or can they be mono...

/*
	class VoiceProto
	{
		void SetParam(string, float);
		void SetParam(string, int);
		void SetParam(string, string);

		void Render(buffer); 
		void Tick(size_t channel);
		void Signal(midievent); //this method assumes NOW....
		bool GetState(); //return true if you haev more data and false if you don't...
	};


*/


//template mixin class for mono synth voices to pretend to be stereo
//with messing with all of the 
template<class T>
class MonoTick : public T
{
public:

	inline float Tick(size_t channel = 0)
	{
		if(channel == 0)
		{
			last = T::Tick();
		}
		return last;
	}
private:
	float last;
};


//template mixin class that implements the default render loop
//so the devices are free to just implement a single "tick" interface...
template<class T>
class VoiceRender : public T
{
public:	
	inline void Render(Buffer& buffer)
	{
		for(unsigned int i = 0; i < buffer.GetSize(); i++)
		{
			for(unsigned int c = 0; c < buffer.GetChannels(); c++)
			{
				buffer.at(i,c) = Tick(c);
			}
		}
	}
};

template<class T>
class DummyParam : public T
{
public:	
	inline void SetParam(const std::string& paramName, int value) {}
	inline void SetParam(const std::string& paramName, float value) {}
	inline void SetParam(const std::string& paramName, const std::string& value) {}

};

//multitimberal voice that sums n voices together
template<class V0, class V1>
class MultiTimber
{

public:
	inline void SetParam(const std::string& paramName, int value) 
	{
		v0.SetParam(paramName, value);
		v1.SetParam(paramName, value);
	}
	inline void SetParam(const std::string& paramName, float value) 
	{
		v0.SetParam(paramName, value);
		v1.SetParam(paramName, value);
	}
	inline void SetParam(const std::string& paramName, const std::string& value)
	{
		v0.SetParam(paramName, value);
		v1.SetParam(paramName, value);
	}

	inline float Tick(unsigned int channel = 0)
	{
		return v0.Tick(channel) + v1.Tick(channel);
	}

	inline bool GetState()
	{
		return v0.GetState() || v1.GetState();
	}

	void Signal(MidiEvent& evt)
	{
		v0.Signal(evt);
		v1.Signal(evt);
	}

private:
	V0 v0;
	V1 v1;
};


//wait the number of channels an actual voice generator has
//should be a constant....?
//i hope dynamic channels are just wierd.....
//how are we handling buffer creation in the voices
//the engine knows how many channels we need
//the voice is either 1 or 2 channels depending on the patch
//
template<class T>
class Voice
{
public:
	Voice() 
	{
		eventCount = 0;
		memset(eventBuffer, 0, sizeof(eventBuffer));
		state = false;
	}
	~Voice() {}

	inline void Bind(unsigned char n)
	{
		note = n;
		state = true;
	}

	void SetBufferSize(size_t sz)
	{
		data.Resize(sz, 2);
	}

	void SetSampleRate(size_t fs)
	{
		sampleRate = fs;
	}

	inline bool IsBoundTo(unsigned char n)
	{
		return n == note;
	}

	void SetParam(const std::string& name, float value)
	{
		item.SetParam(name, value);
	}
	void SetParam(const std::string& name, int value)
	{
		item.SetParam(name, value);
	}
	void SetParam(const std::string& name, const std::string& value)
	{
		item.SetParam(name, value);
	}

	inline void AddEvent(const MidiEvent& evt)
	{
		eventBuffer[eventCount] = evt;
		eventCount++;
	}

	inline Buffer& GetBuffer() { return data; }

	//true means this guy is busy false means its available...
	bool GetState() { return state; }
	

	inline void Render(size_t frameSize)
	{
		if(frameSize > data.GetSize())
		{
			data.Resize(frameSize); //goofy hosts might change that although its rare...
		}
		//just fire it...
		//are we sample accurate here or not
		//well currently no...
		//but with a groove playback we can be.....?
		if(eventCount == 0)
		{
			item.Render(data);
		}
		else
		{
			//sort the eventBuffer based on timestamps...
			int evtOffset = 0;
			for(size_t i = 0; i < frameSize; i++)
			{
				while(eventCount > 0 && eventBuffer[evtOffset].frameOffset == i)
				{
					item.Signal(eventBuffer[evtOffset]);
					eventCount--;
					evtOffset++;
				}

				for(size_t c = 0; c < data.GetChannels(); c++)
				{
					data.at(i, c) = item.Tick(c);
				}
			}
		}
		state = item.GetState();
		if(eventCount != 0)
		{
			cout << "wtfdude...\n";
		}
	}

private:
	Buffer data;
	T item;

	MidiEvent eventBuffer[128];
	int eventCount;

	unsigned char note;

	bool state;
	size_t sampleRate;

};



template<class T>
class DynamicSynth : public IDevice
{
public:
	typedef Voice<T> VoiceT;

	DynamicSynth() : freeVoiceCount(0), defaultBufferSize(0), fs(0)
	{
		onEvents = 0;
		offEvents = 0;
	}
	DynamicSynth(size_t n)
	{
		freeVoiceCount = n;
		for(size_t i = 0; i < n; i++)
		{
			voices.push_back(VoiceT());
		}
	}

	size_t GetOutputCount() { return 2; }
	size_t GetInputCount() { return 0; }

	 void SetEngineInfo(size_t bufferSize, size_t sampleRate)
	 {
		 defaultBufferSize = bufferSize;
		 fs = sampleRate;
		 for(auto it = voices.begin(); it != voices.end(); it++)
		 {
			 it->SetBufferSize(bufferSize);
			 it->SetSampleRate(sampleRate);
		 }
	 }

	~DynamicSynth()
	{

	}

	void SetParam(const std::string& name, int value)
	{
		intParams[name] = value;
		for(auto v = voices.begin(); v != voices.end(); v++)
		{
			v->SetParam(name, value);
		}
		
	}

	void SetParam(const std::string& name, float value)
	{
		floatParams[name] = value;
		for(auto v = voices.begin(); v != voices.end(); v++)
		{
			v->SetParam(name, value);
		}
		
	}

	void SetParam(const std::string& name, const std::string& value)
	{
		stringParams[name] = value;
		for(auto v = voices.begin(); v != voices.end(); v++)
		{
			v->SetParam(name, value);
		}
		
	}


	//midievent is easiest to use a pointer since that data really isn't dynamically
	//created but is just chilling inside of a queue
	//we can make a lightwiegt class later to manage it if it makes you feel better
	inline void Render(MidiEvent* eventList, size_t numberOfEvents, Buffer& input, Buffer& output )
	{
		for(size_t i = 0; i < numberOfEvents; i++)
		{
			if(eventList[i].IsNoteOn())
			{
				onEvents++;
				if(freeVoiceCount > 0)
				{
					//find the first free voice
					for(auto it = voices.begin(); it != voices.end(); it++)
					{
						if(!it->GetState())
						{
							freeVoiceCount--;
							it->AddEvent(eventList[i]);
							it->Bind(eventList[i].midiData[1]);
							break;
						}
					}
				}
				else
				{
					//we need to create a new voice...
					voices.push_back(VoiceT());
					auto last = voices.size() - 1;
					
					for(auto it = intParams.begin(); it != intParams.end(); it++)
					{
						voices[last].SetParam(it->first, it->second);
					}
					
					for(auto it = floatParams.begin(); it != floatParams.end(); it++)
					{
						voices[last].SetParam(it->first, it->second);
					}

					for(auto it = stringParams.begin(); it != stringParams.end(); it++)
					{
						voices[last].SetParam(it->first, it->second);
					}
					voices[last].SetBufferSize(defaultBufferSize);
					voices[last].SetSampleRate(fs);
					voices[last].AddEvent(eventList[i]);
					voices[last].Bind(eventList[i].midiData[1]);


					 //?? anything else???
				}
			}
			else if(eventList[i].IsNoteOff())
			{
				
				for(auto it = voices.begin(); it != voices.end(); it++)
				{
					if(it->GetState() && it->IsBoundTo(eventList[i].midiData[1]))
					{
						offEvents++;
						it->AddEvent(eventList[i]);  //are multiple voices being bound to a single note...?
					}
				}
			}
			else
			{
				for(auto it = voices.begin(); it != voices.end(); it++)
				{
					it->AddEvent(eventList[i]);
				}
			}
		}


		//iterate through each voice and render it if its available
		for(auto it = voices.begin(); it != voices.end(); it++)
		{
			if(it->GetState())
			{
				it->Render(output.GetSize());
				output += it->GetBuffer();
				if(!it->GetState())
				{
					freeVoiceCount++;
				}
			}
		}

	}


private:
	std::vector<VoiceT> voices;	
	int freeVoiceCount;

	size_t defaultBufferSize;
	size_t fs;

	std::map<std::string, int> intParams;
	std::map<std::string, float> floatParams;
	std::map<std::string, std::string> stringParams;

	int offEvents;
	int onEvents;
};
