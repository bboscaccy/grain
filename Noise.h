#pragma once

#include <stdlib.h>

class Noise
{
public:
	Noise(void);
	~Noise(void);


	inline float Tick(unsigned int channel = 0)
	{
		return (((float)rand()) - basis) / basis ;
	}
	

private:
	float basis;

};

