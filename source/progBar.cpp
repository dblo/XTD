#include "progBar.h"
#include<iostream>
ProgBar::ProgBar(int _topLeftX, int _topLeftY, int width, int height)
	: Object(_topLeftX, _topLeftY)
{
	wid				= width;
	hi				= height;
	progress		= 0;
	counter			= 1;
	active			= false;
}

bool ProgBar::tick(int currTime)
{
	if(active)
	{
		progress = (int)(((float)(currTime - startTime) / duration) * wid);

		if(progress >= wid)
		{
			active = false;
			reset();
			return true;
		}
		else
			counter++;
	}
	return false;
}

void ProgBar::reset()
{
	progress = 0;
}

int ProgBar::getHeight() const
{
	return hi;
}

int ProgBar::getWidth() const
{
	return wid;
}

void ProgBar::start(int _currTime, int _duration)
{
	active = true;
	startTime = _currTime;
	duration = (int)_duration * 1000;
	counter = 1;
}

int ProgBar::getProgress() const
{
	return progress;
}

bool ProgBar::isActive() const
{
	return active;
}

