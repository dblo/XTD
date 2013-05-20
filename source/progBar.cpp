#include "progBar.h"
#include<iostream>
ProgBar::ProgBar(int _topLeftX, int _topLeftY, int width, int height)
	: Object(_topLeftX, _topLeftY)
{
	wid	= width;
	hi	= height;
}

bool ProgBar::tick(int currTime)
{
	if(active && currTime >= updateNextAt)
	{
		tickCounter++;
		progress = (int)((float)(tickCounter-1) * ((float)hi / 10));
		updateNextAt += updateInterval;
		if(tickCounter > 10)
		{
			tickCounter = 0;
			active = false;
			return true;
		}
	}
	return false;
}
void ProgBar::start(int currTime, int duration, bool delayStart)
{
	updateInterval	= duration * 100;
	updateNextAt	= currTime + updateInterval;
	progress		= 0;
	tickCounter		= 1;

	if(!delayStart)
		active = true;
}

int ProgBar::getHeight() const
{
	return hi;
}

int ProgBar::getWidth() const
{
	return wid;
}

int ProgBar::getProgress() const
{
	return progress;
}

bool ProgBar::isActive() const
{
	return active;
}

void ProgBar::abort()
{
	active = false;
	tickCounter = 0;
}

int ProgBar::getRemaining( int currtime ) const
{
	return updateNextAt - currtime;
}

void ProgBar::compensatePause( int newUpdateNext )
{
	updateNextAt = newUpdateNext;
}

void ProgBar::setActive(bool activeMode)
{
	active = activeMode;
}

bool ProgBar::upgrading() const
{
	return tickCounter > 0;
}
