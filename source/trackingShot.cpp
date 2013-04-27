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
		eps = radius / 2;

	if(centerX < target->getCenterX() - eps)
		centerX += ms;
	else if(centerX > target->getCenterX() + eps)
		centerX -= ms;
	else
		move = ms*2;

	if(centerY < target->getCenterY() - eps)
		centerY += move;
	else if(centerY > target->getCenterY() + eps)
		centerY -= move;
	else
	{
		if(centerX < target->getCenterX() - eps)
			centerX += ms;
		else if(centerX > target->getCenterX() + eps)
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

	//Divides target radius by 2 to delay collision -> looks better
	int hyp = radius + target->getRadius() / 2;
	return hyp*hyp >= deltaX*deltaX + deltaY*deltaY; 
}
