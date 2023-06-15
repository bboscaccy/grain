#include "AudioEngine.h"

using namespace std;


AudioEngine::AudioEngine(void)
{
	debugMidi = true;
	synth = NULL;
}


AudioEngine::~AudioEngine(void)
{
}


void AudioEngine::Init()
{
	//set up audio
	cout << "Configuring Audio\n";
	audio = make_shared<RtAudio>(RtAudio::WINDOWS_ASIO);

	int devCount = audio->getDeviceCount();
	for(int i = 0; i < devCount; i++)
	{
		auto info = audio->getDeviceInfo(i);
		cout << info.name << '\n';

	}
	cout << '\n';

	RtAudio::StreamParameters outParams;
	outParams.deviceId = 0;
	outParams.firstChannel = 0;
	outParams.nChannels = 2;

	bufferSize = 64;



	audio->openStream(&outParams, &outParams, RTAUDIO_FLOAT32, 44100, &bufferSize, &AudioCallback, this);
	
	if(synth != nullptr)
	{
		synth->SetEngineInfo(bufferSize, 2, 44100);
	}
	if(device != nullptr)
	{
		device->SetEngineInfo(bufferSize, 44100);
	}

	//do any other pre setup in here BEFORE you start the stream.....
	
	audio->startStream();


	cout << "Configuring Midi\n";

	//set up midi
	midiIn = make_shared<RtMidiIn>(RtMidi::WINDOWS_MM);

	auto portCount = midiIn->getPortCount();
	for(unsigned int x = 0; x < portCount; x++)
	{
		auto s = midiIn->getPortName(x);
		cout << x << ": " << s << '\n';
	}

	midiIn->openPort(3);
	
	midiIn->setCallback( &MidiCallback, this );
	midiIn->ignoreTypes( false, false, false );
}


void AudioEngine::Shutdown()
{
	audio->stopStream();
	midiIn->closePort();
}

//this dude is sending back interleaeved buffers
int AudioCallback(void* outbuffer, void* inBuffer, unsigned int nFrames, double streamTime, RtAudioStreamStatus status, void* userData)
{
	//now we can diverge from the vsti interface here or keep with it...
	AudioEngine* engine = (AudioEngine*)userData;

	if(engine->device != NULL)
	{
		size_t midiEventCount = engine->midiEvents.DataAvailableCount();
		MidiEvent* eventList = NULL;	
		if(midiEventCount > 0)
		{
			eventList = engine->midiEvents.GetCurrentList();
		}

		//engine->synth->Render(eventList, midiEventCount, (float*)outbuffer, nFrames, 2);

		Buffer out(outbuffer, nFrames, 2);
		out.Clear();
		
		Buffer in(inBuffer, nFrames, 2);
		engine->device->Render(eventList, midiEventCount, in, out);

		engine->midiEvents.IncrementReader(midiEventCount);
	}
	
	if(status == RTAUDIO_OUTPUT_UNDERFLOW)
	{
		cout << "!!Audio Underflow!!\n";
	}

	return 0;
}



void MidiCallback(double deltaTime, std::vector<unsigned char>* message, void* userData)
{


	unsigned int nBytes = message->size();
	AudioEngine* engine = (AudioEngine*)userData;

	for ( unsigned int i=0; i<nBytes && i < 4; i++ )
	{
		engine->GetEventQueue().GetCurrentItem().midiData[i] = message->at(i);
	}
	engine->GetEventQueue().IncrementWriter();

	
	
	if(engine->debugMidi)
	{
		for ( unsigned int i=0; i<nBytes; i++ )
		{
			std::cout << "Byte " << i << " = " << (int)message->at(i) << ", ";
		}
	
		if (nBytes > 0)
		{
			std::cout << "stamp = " << deltaTime << std::endl;
		}
	}
}

