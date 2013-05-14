#ifndef _TRACKINGSHOT_H
#define _TRACKINGSHOT_H

#include "circularObject.h"
#include <iostream>
#include "monster.h"

class BaseShot : public  CircularObject
{
	Monster *target; //switch to const ref?
	int dmg;
	Image img;
public:
	BaseShot(int x, int y, Image _img, Monster *_target, int _dmg, int rad);
	void UpdateTopLeft();
	void move();
	bool colliding() const;
	Monster* getTarget() const;
	int getDmg() const;
	Image getImage() const;
};

inline Monster* BaseShot::getTarget() const
{
	return target;
}

inline int BaseShot::getDmg() const
{
	return dmg;
}

inline void BaseShot::UpdateTopLeft()
{
	topLeftX = centerX - radius;
	topLeftY = centerY - radius;
}
#endif //_TRACKINGSHOT_H
