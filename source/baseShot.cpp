#include "baseShot.h"

BaseShot::BaseShot(int x, int y, Image _img, Monster *_target, int _dmg, int rad) 
	: target(_target), dmg(_dmg)
{
	img = _img;
	centerX = x;
	centerY = y;
	radius = rad;
	UpdateTopLeft();
}

/*
* Apply ms in horizontal and vertical direction to approach target.
* If no more movement in one direction is needed, apply double in other.
*/
void BaseShot::move()
{
	if(moveMe)
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
	moveMe = !moveMe;
}

/*
* 
*/
bool BaseShot::colliding() const
{
	int deltaX = target->getCenterX() - centerX;
	int deltaY = target->getCenterY() - centerY;

	//Divides target radius by 2 to delay collision -> looks better
	int hyp = radius + target->getRadius() / 2;
	return hyp*hyp >= deltaX*deltaX + deltaY*deltaY; 
}

Image BaseShot::getImage() const
{
	return img;
}
