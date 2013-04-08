#include "trackingShot.h"

TrackingShot::TrackingShot(const Point &p, Monster &_target, int _dmg) : target(_target), dmg(_dmg)
{
	center = p;
	radius = 2;
	ms = 3; //3 times faster than monsters
	updateTopLeft();
}

void TrackingShot::move()
{
	if(center.getX() < target.getCenter().getX())
		center.addToX(ms);
	else if(center.getX() > target.getCenter().getX())
		center.addToX(-ms);

	if(center.getY() < target.getCenter().getY())
		center.addToY(ms);
	else if(center.getY() > target.getCenter().getY())
		center.addToY(-ms);
	updateTopLeft();
}

bool TrackingShot::colliding() const
{
	int deltaX = target.getCenter().getX() - center.getX();
	int deltaY = target.getCenter().getY() - center.getY();
	int rad = radius + target.getRadius();
	return rad * rad >= deltaX*deltaX + deltaY*deltaY; 
}
