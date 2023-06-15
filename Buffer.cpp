#include "Buffer.h"
#include <memory>

Buffer::Buffer(void) : data(nullptr), bufferSize(0), channelCount(0), shared(false)
{
	
}

Buffer::Buffer(void* nativeBuffer, size_t size, size_t channels)
{
	shared = true;
	bufferSize = size;
	channelCount = channels;
	data = (float*)nativeBuffer;
	
}

Buffer::Buffer(size_t size, size_t channels) : bufferSize(size), channelCount(channels), shared(false)
{
	data = new float[bufferSize * channelCount];
	memset(data, 0, sizeof(float) * bufferSize * channelCount);
}


Buffer::Buffer(const Buffer& rhs)
{
	if(rhs.shared)
	{
		data = rhs.data;
		bufferSize = rhs.bufferSize;
		channelCount = rhs.channelCount;
		shared = true;
	}
	else if(rhs.data != nullptr) 
	{
		bufferSize = rhs.bufferSize;
		channelCount = rhs.channelCount;
		data = new float[bufferSize * channelCount];
		memcpy(data, rhs.data, bufferSize * channelCount * sizeof(float));
		shared = false;
	}
	else
	{
		data = nullptr;
		shared = false;
		bufferSize = rhs.bufferSize;
		channelCount = rhs.channelCount;
	}
}

Buffer& Buffer::operator=(const Buffer& rhs)
{
	if(&rhs == this)
	{
		return *this;
	}
	if(rhs.shared)
	{
		data = rhs.data;
		bufferSize = rhs.bufferSize;
		channelCount = rhs.channelCount;
		shared = true;
	}
	else if(rhs.data != nullptr)
	{
		bufferSize = rhs.bufferSize;
		channelCount = rhs.channelCount;
		data = new float[bufferSize * channelCount];
		memcpy(data, rhs.data, bufferSize * channelCount * sizeof(float));
		shared = false;
	}
	else
	{
		data = nullptr;
		shared = false;
		bufferSize = rhs.bufferSize;
		channelCount = rhs.channelCount;
	}
	return *this;
}

Buffer::Buffer(Buffer&& rhs)
{
	
	data = rhs.data;
	bufferSize = rhs.bufferSize;
	channelCount = rhs.channelCount;
	if(rhs.shared) //move is irrelvant here
	{
		shared = true;
	}
	else
	{
		shared = false;
		rhs.shared = true;
	}
}

Buffer& Buffer::operator=(Buffer&& rhs)
{
	if(&rhs == this)
	{
		return *this;
	}
	data = rhs.data;
	bufferSize = rhs.bufferSize;
	channelCount = rhs.channelCount;
	if(rhs.shared) //move is irrelvant here
	{
		shared = true;
	}
	else
	{
		shared = false;
		rhs.shared = true;
	}
	return *this;
}

void Buffer::Resize(size_t size)
{
	if(channelCount == 0)
	{
		channelCount = 1;
	}
	Resize(size, channelCount);
}

void Buffer::Resize(size_t size, size_t channels)
{
	if(shared)
		return;

	if(bufferSize == 0 && channelCount == 0)
	{
		if(data != nullptr)
		{
			delete [] data;
		}
		bufferSize = size;
		channelCount = channels; //good job blaise....
		data = new float[bufferSize * channelCount];
		memset(data, 0, sizeof(float) * bufferSize * channelCount);
		
	}
	else if(channels == channelCount)
	{
		if(size <= bufferSize)
		{
			//just truncate this bitch...		
			bufferSize = size;
		}
		else
		{
			auto tmp = new float[size * channels];
			memset(tmp, 0, sizeof(float) * size * channels);
			auto oldSize = sizeof(float) * bufferSize * channelCount;
			memcpy(tmp, data, oldSize);
			bufferSize = size;
			channelCount = channels;

			//cleaning up whould be nice here....
			delete [] data;
			data = tmp;

		}
	}
	else
	{
		//we just kill all the old data and realocate a new array here
		delete [] data;
		bufferSize = size;
		channelCount = channels;
		data = new float[bufferSize * channelCount];
		memset(data, 0, sizeof(float) * bufferSize * channelCount);

	}


}

Buffer::~Buffer(void)
{
	if(data != nullptr && !shared)
	{
		delete[] data;
	}
}
