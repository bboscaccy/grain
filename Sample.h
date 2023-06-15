#pragma once
#include <string>
#include "EventQueue.h"
#include "Buffer.h"
//an in memory sample class
class Sample
{
public:
	Sample(void);
	~Sample(void);


	void Open(const std::string& filePath);

	inline unsigned int Channels() const
	{
		return buffer.GetChannels();
	}

	inline unsigned int Length() const
	{
		return buffer.GetSize();
	}

	inline unsigned int SampleRate() const
	{
		return sampleRate;
	}
	
	//interleaved floating point format...
	inline Buffer& Data() { return buffer; }

private:
	Buffer buffer;
	unsigned int sampleRate;
	
};

class SamplePlayer
{
public:
	SamplePlayer();
	
	inline void SetParam(const std::string& paramName, int value) {}
	void SetParam(const std::string& paramName, float value);
	void SetParam(const std::string& paramName, const std::string& value);

	void Open(const std::string& filePath);

	void Signal(MidiEvent& evt);

	inline float Tick(unsigned int channel = 0)
	{
		if(cursor[channel] < sample.Length() && cursor[channel] >= 0.0f) 
		{
			auto res = sample.Data().at(cursor[channel], channel) * .5f;
			cursor[channel] += step;
			return res;
		}
		else
		{
			return 0.0f;
		}
	}

	inline bool GetState() { return state; }

	inline void Scrub(float f, unsigned int channel)
	{
		float v = abs(f);
		if(v > 1.0f)
		{
			cursor[channel] = sample.Length();
		}
		else
		{
			cursor[channel] = sample.Length() * v;
		}
	}
	
	//scrub to a relative position N iterations forward...
	inline void Scrub(float f, int iter, unsigned int channel)
	{
		float v = abs(f);
		if(v > 1.0f)
		{
			cursor[channel] = sample.Length();
		}
		else
		{
			cursor[channel] = sample.Length() * v;
			cursor[channel] += step * iter;
		}

	}

	inline void Scrub(float f)
	{
		for(int i = 0; i < 8; i++)
		{
			Scrub(f, i);
		}
	}

	inline int TransposedLength()
	{
		return sample.Length() / step;
	}



private:
	Sample sample;
	float cursor[8]; //whats wrong with that we have very sane upper bounds on alot of these
	//guys here i mean wtf has an 8 channel sample....
	bool state;
	float step;
	float basis;
};