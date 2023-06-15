#pragma once

class ADSR 
{
public:
	ADSR(void);
	~ADSR(void);

	inline void TriggerOn()
	{
		state = 1;
		gainLevel = 0;
	}
	inline void TriggerOff()
	{
		state = 4;
	}

	inline bool IsActive()
	{
		return (state != 0);
	}

	inline void SetSampleRate(size_t rate)
	{
		sampleRate = rate;
	}

	inline void SetAttack(float time)
	{
		attackTime = time;
		attackSamples = (int)(sampleRate * attackTime);
		attackStep = 1.0f / (float)attackSamples;
	}

	inline void SetAttackLevel(float v)
	{
		attackLevel = v;
	}

	inline void SetDecayTime(float time)
	{
		decayTime = time;
		decaySamples = (int)(sampleRate * decayTime);
		decayStep = (1.0f - sustainLevel) / (float)decaySamples;
	}

	inline void SetSustainLevel(float level)
	{
		sustainLevel = level;
		SetDecayTime(decayTime); //recalc that 
	}

	inline void SetReleaseTime(float time)
	{
		releaseTime = time;
		releaseSamples = (int)(sampleRate * releaseTime);
		releaseStep = sustainLevel / (float)releaseSamples;

	}
	
	inline float Tick()
	{
		
		if(state == 0)
		{
			return 0.0f;
		}
		else if(state == 1)
		{
			gainLevel += attackStep;
			if(gainLevel >= attackLevel)
			{
				state = 2;
				gainLevel = attackLevel;
			}
			return gainLevel;
		}
		else if(state == 2)
		{
			gainLevel -= decayStep;
			if(gainLevel <= sustainLevel)
			{
				state = 3;
				gainLevel = sustainLevel;
			}
			return gainLevel;
		}
		else if(state == 3)
		{
			return sustainLevel;
		}
		else if(state == 4)
		{
			gainLevel -= releaseStep;
			if(gainLevel <= 0.0f)
			{
				state = 0;
				gainLevel = 0.0f;
			}
			return gainLevel;
		}
		return 0;
	}

private:
	size_t sampleRate;
	float attackTime;
	int attackSamples;
	float attackStep;
	float attackLevel;

	float decayTime;
	int decaySamples;
	float decayStep;

	float sustainLevel;

	float releaseTime;
	int releaseSamples;
	float releaseStep;

	char state; //0 = off 1 = attack, 2 = decay, 3 = sustain, 4 = release

	float gainLevel;

};

