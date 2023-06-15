#include "ADSR.h"


ADSR::ADSR(void) : state(0), sampleRate(44100), attackLevel(0.25f), attackTime(.01f), decayTime(0.01f), sustainLevel(0.25f), releaseTime(.01f)
{
	attackSamples = (int)(sampleRate * attackTime);
	attackStep = 1.0f / (float)attackSamples;

	decaySamples = (int)(sampleRate * decayTime);
	decayStep = (1.0f - sustainLevel) / (float)decaySamples;

	releaseSamples = (int)(sampleRate * releaseTime);
	releaseStep = sustainLevel / (float)releaseSamples;

}


ADSR::~ADSR(void)
{
}
