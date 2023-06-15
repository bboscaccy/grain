#pragma once


#include <math.h>
#include <vector>
#include <deque>

#include "DynamicSynth.h"
#include "Sample.h"
#include "ADSR.h"


struct GrainData
{
	float pos;
	int length;
	int period; //if this is a -1 it does not get replayed...
	int delay; //current delay counter....
	size_t cursor[2];

	float maxAmplitude;

	inline bool IsCompleted()
	{
		return (cursor[0] >= length) && (cursor[1] >= length);
	}

	//real simple triangle window for right now
	//this will become a template...
	inline float Envelope(unsigned int channel)
	{
		//return 0.5f;
		int fs = length / 2;
		if(cursor[channel] > fs)
		{
			return ((float)(cursor[channel] - fs) / (float)fs) * maxAmplitude;
		}
		else
		{
			return ((float)(fs - cursor[channel]) / (float)fs) * maxAmplitude;
		}
	}
};



template<class T>
class GrainSynth
{
public:
	GrainSynth() : maxGrains(10), grains(100), minLength(10), maxLength(200), activeGrains(0), sampleRate(44100)
	{
		for(int i = 0; i < maxGrains; i++)
		{
			freeGrains.push_back(i);
		}
		adsr.SetAttack(2);
		adsr.SetAttackLevel(.5);
		adsr.SetSustainLevel(.5);
		adsr.SetDecayTime(5);
	}

	//we are probably going to need param forwarding and whatnot...

	void SetParam(const std::string& paramName, int value) {}
	void SetParam(const std::string& paramName, float value) {}
	void SetParam(const std::string& paramName, const std::string& value) 
	{
		sampler.SetParam(paramName, value);
	}

	inline bool GetState() { return adsr.IsActive(); }

	void Signal(MidiEvent& evt)
	{
		scheduler.Signal(evt);
		sampler.Signal(evt);

		if(evt.IsNoteOn())
		{
			adsr.TriggerOn();
		}
		else if(evt.IsNoteOff())
		{
			adsr.TriggerOff();
		}

	}

	//there are also 2 grain reclemation algorithms here
	//newest die or oldest die.....

	inline float Tick(unsigned int channel = 0)
	{
		if(channel == 0)
		{
			auto target = ceil(scheduler.Tick() * (float)maxGrains);

			if(target < activeGrains)
			{
				//we are killing the oldest grains first to have a more "evolving" texture here....
				//but there is nothing stopping this from using using the newest as welll
				auto it = usedGrains.begin();
				if(it != usedGrains.end()) //
				{
					for(int i = 0; i < activeGrains - target; i++)
					{
						grains[*it].period = -1; //this effectivly kills it after it playback is completed...
						it++;
						if(it == usedGrains.end())
						{
							break; //huh....?
						}
					}
				}
				

			}

			while(target > activeGrains)
			{
				if(freeGrains.size() == 0)
				{
					//we have no free grains currently do to grain entropy or whatever try again at the next tick when some should be available....
					break;
				}
				auto index = freeGrains.back();
				freeGrains.pop_back();

				grains[index].pos = scheduler.NextPos(); //make that guy scaled back so it doesn't go past the end
				//sample.length / stepSize = maxNumber of samples
				//so pos needs to be scaled back after duration is calclulated to check for clipping...
				auto len = maxLength - minLength;
				auto timeInMs = (scheduler.NextDuration() * len) + minLength;
				grains[index].length = (int)(sampleRate * (timeInMs / 1000.0f));


				//convert ms to samples


				//scale back position if needed
				auto ratio = (float)(sampler.TransposedLength() - grains[index].length) / (float) sampler.TransposedLength();
				grains[index].pos *= ratio;

				grains[index].period = scheduler.NextPeriod(); //that should be in hertz you can use this to generate harmonics...
				grains[index].delay = scheduler.NextDelay(); //this should be in ms and we will convert it...
				grains[index].maxAmplitude = scheduler.NextAmplitude(); //float 
				grains[index].cursor[0] = 0;
				grains[index].cursor[1] = 0;
				activeGrains++;
				usedGrains.push_back(index);
			}
		}

		float result = 0.0f;
		auto it = usedGrains.begin();
		while(it != usedGrains.end())
		{
			auto index = *it;
			auto& g = grains[index];
			//render the grain...
			if(grains[index].delay > 0)
			{
				--g.delay;
			}
			else //we should play it
			{
				//if period = -1 we need to remove this guy from the used grains queue and place it into the free grains queue...
				//if and only if its done playing....
				sampler.Scrub(g.pos, g.cursor[channel], channel);
				result += sampler.Tick(channel) * g.Envelope(channel);
				++g.cursor[channel];
				
				if(grains[index].IsCompleted())
				{
					//now we can either A reschedule this guy... or b remove it if needed
					if(g.period == -1) //its dead jim...
					{
						it = usedGrains.erase(it);
						freeGrains.push_back(index); //garabage collection...
						--activeGrains;
					}
					else
					{
						g.delay = g.period; //reschedule it at whatever....
						g.cursor[0] = 0; 
						g.cursor[1] = 0;
						it++;
					}
				}
				else
				{
					it++;
				}
			}
			

		}
		
		return result * adsr.Tick();
	}



private:
	SamplePlayer sampler;
	ADSR adsr;

	std::vector<GrainData> grains;
	std::deque<int> freeGrains;
	std::deque<int> usedGrains;

	float minLength; //in ms
	float maxLength; //in ms

	int maxGrains;
	int activeGrains;
	int sampleRate; 

	T scheduler;
};


