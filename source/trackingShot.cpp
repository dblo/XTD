#include "trackingShot.h"

TrackingShot::TrackingShot(int x, int y, Monster *_target, int _dmg, int _ms, int rad) 
	: target(_target), dmg(_dmg)
{
	centerX = x;
	centerY = y;
	radius = rad;
	UpdateTopLeft();
	ms = _ms;
}

/*
 * Apply ms in horizontal and vertical direction to approach target.
 * If no more movement in one direction is needed, apply double in other.
 */
void TrackingShot::move()
{
	int move = ms,
		delta = radius / 2;

	if(centerX < target->getCenterX() - delta)
		centerX += ms;
	else if(centerX > target->getCenterX() + delta)
		centerX -= ms;
	else
		move = ms*2;

	if(centerY < target->getCenterY() - delta)
		centerY += move;
	else if(centerY > target->getCenterY() + delta)
		centerY -= move;
	else
	{
		if(centerX < target->getCenterX() - delta)
			centerX += ms;
		else if(centerX > target->getCenterX() + delta)
			centerX -= ms;
	}
	UpdateTopLeft();
}

/*
 * 
 */
bool TrackingShot::colliding() const
{
	int deltaX = target->getCenterX() - centerX;
	int deltaY = target->getCenterY() - centerY;
	int hyp = radius + target->getRadius();
	return hyp*hyp >= deltaX*deltaX + deltaY*deltaY; 
}
