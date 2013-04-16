#ifndef _TRACKINGSHOT_H
#define _TRACKINGSHOT_H

#include "circularObject.h"
#include <iostream>
#include "monster.h"

class TrackingShot : public  CircularObject
{
	Monster *target; //switch to const ref?
	int dmg;
public:
	TrackingShot(int x, int y, Monster *_target, int _dmg, int _ms);
	void updateTopLeft();
	void move();
	bool colliding() const;
	Monster* getTarget() const;
	int getDmg() const;
};

inline Monster* TrackingShot::getTarget() const
{
	return target;
}

inline int TrackingShot::getDmg() const
{
	return dmg;
}

inline void TrackingShot::updateTopLeft()
{
	topLeftX = centerX - radius;
	topLeftY = centerY - radius;
}
#endif //_TRACKINGSHOT_H
