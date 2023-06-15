#pragma once

#include <atomic>
#include <math.h>

//we can do some ifdefs or a traits class for if we actually need thread safety up in here
//with vst we really dont since they do it already


struct MidiEvent
{
	size_t frameOffset; //defaults to zero...
	unsigned char midiData[4];

	inline float GetVelocity()
	{
		return ((float)midiData[2]) / 127.0f;
	}

	inline bool IsNoteOn()
	{
		//yeah i'll fix this after my coffee
		return midiData[0] == 144;
	}

	inline bool IsNoteOff()
	{
		return midiData[0] == 128;
	}

	inline bool IsControl()
	{
		return !IsNoteOff() && !IsNoteOn();
	}

	inline float GetNoteFrequency()
	{
		return pow(2.0f, (float)(midiData[1] - 69) / (float)12) * 440.0f;
	}

};



template <class T, int N>
class EventQueue
{
public:
	EventQueue(void)
	{
		readCursor.store(0);
		writeCursor.store(0);
		memset(data, 0, sizeof(T) * N);
	}
	
	~EventQueue(void)
	{

	}

	//writer methods
	inline T& GetCurrentItem()
	{
		return data[writeCursor.load() % N];
	}

	inline T* GetCurrentList()
	{
		return &data[readCursor.load() % N];
	}

	inline void IncrementWriter()
	{
		writeCursor.fetch_add(1);
		memset(&data[writeCursor.load() % N], 0, sizeof(T));
	}

	//reader methods
	inline bool HasDataAvailable()
	{
		return readCursor.load() < writeCursor.load();
	}

	inline size_t DataAvailableCount()
	{
		return (size_t)(writeCursor.load() - readCursor.load());
	}


	inline T& ReadCurrentItem()
	{
		return data[writeCursor.load() % N];
	}

	inline void IncrementReader(int n = 1)
	{
		readCursor.fetch_add(n);
	}

private:
	std::atomic<unsigned long long> readCursor;
	std::atomic<unsigned long long> writeCursor;

	T data[N];
};

typedef EventQueue<MidiEvent, 1024> MidiEventQueue;