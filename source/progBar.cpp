#include "progBar.h"
#include<iostream>
ProgBar::ProgBar(int _topLeftX, int _topLeftY, int width, int height)
	: Object(_topLeftX, _topLeftY)
{
	wid				= width;
	hi				= height;
	tickCounter		= 0;
}

bool ProgBar::tick(int currTime)
{
	if(tickCounter == TICKS_PER_INCREMENT)
	{
		progress = (int)(((float)(currTime - startTime) / duration) * wid);

		if(progress >= wid)
		{
			tickCounter = 0; // The bar goes inactive
			return true;
		}
		tickCounter = 1;
	}
	else
		tickCounter++;
	return false;
}

void ProgBar::start(int _currTime, int _duration)
{
	startTime		= _currTime;
	duration		= (float)(_duration * 1000);
	tickCounter		= 1;
	progress		= 0;
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
	return tickCounter > 0;
}

void ProgBar::abort()
{
	tickCounter = 0;
}
