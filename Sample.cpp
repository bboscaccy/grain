#include "Sample.h"
#include <FileRead.h>

using namespace std;
using namespace stk;

Sample::Sample(void) : sampleRate(0)
{
	
}


Sample::~Sample(void)
{
	

}



void Sample::Open(const string& filePath)
{
	FileRead fi;
	fi.open(filePath);
	auto channels = fi.channels();
	auto size = fi.fileSize();
	sampleRate = fi.sampleRate();

	buffer.Resize(size, channels);
	fi.read(buffer.GetData(), fi.fileSize(), 0, true);
	
	fi.close();

}

SamplePlayer::SamplePlayer()
{
	basis = 261.626f; 	
}

void SamplePlayer::Open(const string& filePath)
{
	sample.Open(filePath);
	state = false;
}

void SamplePlayer::SetParam(const std::string& paramName, const std::string& value)
{
	if(paramName == "sample")
	{
		Open(value);
	}

}

void SamplePlayer::SetParam(const std::string& paramName, float value)
{
	if(paramName == "basis")
	{
		basis = value;
	}
}


void SamplePlayer::Signal(MidiEvent& evt)
{
	if(evt.IsNoteOn())
	{
		state = true;
		for(int i = 0; i < 8; i++) //this should  get unrolled
		{
			cursor[i] = 0.0f;
			step = evt.GetNoteFrequency() / basis;
		}
	}
	else if(evt.IsNoteOff())
	{
		state = false;
	}
}
