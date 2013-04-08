#ifndef _TRACKINGSHOT_H
#define _TRACKINGSHOT_H

#include "circularObject.h"
#include <iostream>
#include "monster.h"

class TrackingShot : public  CircularObject
{
	Monster &target;
	int dmg;
public:
	TrackingShot(const Point &p, Monster &_target, int _dmg);
	void updateTopLeft();
	void move();
	bool colliding() const;
	Monster& getTarget() const;
	int getDmg() const;
};


inline Monster& TrackingShot::getTarget() const
{
	return target;
}

inline int TrackingShot::getDmg() const
{
	return dmg;
}

inline void TrackingShot::updateTopLeft()
{
	topLeft.setPoint(center.getX() - radius,
		center.getY() - radius);
}
#endif //_TRACKINGSHOT_H
