#include "trackingShot.h"

TrackingShot::TrackingShot(int x, int y, Monster *_target, int _dmg, int _ms) 
	: target(_target), dmg(_dmg)
{
	centerX = x;
	centerY = y;
	radius = g_tileSize / 5;
	updateTopLeft();
	ms = _ms;
}

void TrackingShot::move()
{
	if(centerX < target->getCenterX())
		centerX += ms;
	else if(centerX > target->getCenterX())
		centerX -= ms;

	if(centerY < target->getCenterY())
		centerY += ms;
	else if(centerY > target->getCenterY())
		centerY -= ms;
	updateTopLeft();
}

bool TrackingShot::colliding() const
{
	int deltaX = target->getCenterX() - centerX;
	int deltaY = target->getCenterY() - centerY;
	int hyp = radius + target->getRadius();
	return hyp*hyp >= deltaX*deltaX + deltaY*deltaY; 
}
