#pragma once

#include <memory>

//channel interleaved signal buffer
//make a couple of load ops for integrating with 
//vst or rtaudio buffer formats....
class Buffer
{
public:
	Buffer(void);
	Buffer(void* nativeBuffer, size_t size, size_t channels);
	Buffer(size_t size, size_t channels);
	~Buffer(void);

	Buffer& operator= (const Buffer& rhs);
	Buffer& operator= (Buffer&& rhs);

	Buffer(const Buffer& rhs);
	Buffer(Buffer&& rhs);


	void Resize(size_t size, size_t channels);
	void Resize(size_t size);

	inline float* GetData() { return data; }
	inline size_t GetSize() const { return bufferSize; } 
	inline size_t GetChannels() const { return channelCount; } 


	inline void Clear()
	{
		if(data != nullptr)
		{
			memset(data, 0, bufferSize * channelCount * sizeof(float));
		}
	}

	//do the rest of the arithmetic operators
	//for both vectors and scalars....
	inline Buffer& operator += (const Buffer& rhs)
	{
		//we have the same channel count
		if(rhs.channelCount == channelCount)
		{
			size_t it = bufferSize;
			if(rhs.bufferSize < it)
			{
				it = rhs.bufferSize;
			}

			for(size_t i = 0; i < it; i++)
			{
				for(size_t c = 0; c < channelCount; c++)
				{
					data[i * channelCount + c] += rhs.data[i * channelCount + c];
				}
			}
		}
		else
		{
			//sums n channels into 1
			if(channelCount == 1)
			{
				size_t it = bufferSize;
				if(rhs.bufferSize < it)
				{
					it = rhs.bufferSize;
				}

				for(size_t i = 0; i < it; i++)
				{
					for(size_t c = 0; c < rhs.channelCount; c++)
					{
						data[i] += rhs.data[i * channelCount + c];
					}
				}
			}
			else if(rhs.channelCount == 1)
			{
				size_t it = bufferSize;
				if(rhs.bufferSize < it)
				{
					it = rhs.bufferSize;
				}

				for(size_t i = 0; i < it; i++)
				{
					for(size_t c = 0; c < rhs.channelCount; c++)
					{
						data[i * channelCount + c] += rhs.data[i];
					}
				}
			}
			else //where punting here sum the minimum channel count ....
			{

			}
		}

		return *this;
	}

	inline float& operator[] (unsigned int i)
	{
		return data[i * channelCount];
	}

	
	//linear interoplation between values
	inline float operator[] (float f)
	{
		int i = (int)f;
		int n = i+1;

		float lmod = 1.0f - (f - (float)i);
		float rmod = 1.0f - ( ((float)n) - f);
		return (data[i * channelCount] * lmod) + (data[n * channelCount] * rmod);
	}
	//we need ops for add sub mult div between different buffer sizes and number of channels...

	inline float& at(unsigned int i, unsigned int c = 0)
	{
		return data[i * channelCount + c];
	}
	inline float& at(int i, int c = 0)
	{
		return data[i * channelCount + c];
	}
	inline float& at(unsigned int i, int c = 0)
	{
		return data[i * channelCount + c];
	}
	inline float& at(int i, unsigned int c = 0)
	{
		return data[i * channelCount + c];
	}

	//crap we need to do bounds checking up in here....? for edge cases where 
	//f + 1 > the end of the array
	inline float at(float f, unsigned int c = 0)
	{
		int i = (int)f;
		int n = i+1;

		float lmod = 1.0f - (f - (float)i);
		float rmod = 1.0f - ( ((float)n) - f);
		return (data[i * channelCount + c] * lmod) + (data[n * channelCount + c] * rmod);
	}

private:
	float* data;
	size_t bufferSize;
	size_t channelCount;
	bool shared;
};

