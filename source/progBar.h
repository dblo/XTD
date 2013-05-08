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
	int counter;
	bool active;
	int startTime;
	float duration;

	void reset() ;
public:
	ProgBar(int _topLeftX, int _topLeftY, int width, int height);
	bool tick(int currTime);

	// _currTime in ms. _duration in seconds.
	void start(int _currTime, int _duration);
	int getWidth() const;
	int getHeight() const;
	int getProgress() const;
	bool isActive() const;
};
#endif _PROGBAR_H_
