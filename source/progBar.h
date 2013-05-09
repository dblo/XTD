#ifndef _PROGBAR_H_
#define _PROGBAR_H_

#include "object.h"

const int TICKS_PER_INCREMENT = 10;

// Class only works properly for durations that are multiples of 10
class ProgBar : public Object
{
private:
	int wid;
	int hi;
	int progress;
	int tickCounter;
	int startTime;
	float duration;
public:
	ProgBar(int _topLeftX, int _topLeftY, int width, int height);

	// Returns true if the bar is filled
	bool tick(int currTime);

	// _currTime in ms. _duration in seconds.
	void start(int _currTime, int _duration);

	bool isActive() const;
	int getWidth() const;
	int getHeight() const;
	int getProgress() const;
};
#endif // _PROGBAR_H_
