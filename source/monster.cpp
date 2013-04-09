#include "monster.h"

#include <iostream>

Monster::Monster()
{ 
	currentGridPos.setPoint(0,0);
	topLeft.setPoint(0, 0);
	hp = 0;
	ms = 0;
	waveId = 0;
	mobId = 0;
	nextInstr = 0;
	movingDir = STILL; 
	alive = false;
	updateGridPos = false;
	moveCounter = 0;
	inNewSquare = false;
	radius = 8;
}

void Monster::init(int _x, int _y, int _hp, int _ms, int _waveId, int _mobId)
{
	topLeft.setPoint(_x, _y);
	hp = _hp;
	ms = _ms;
	waveId = _waveId;
	mobId = _mobId;
	alive = true;
	updateCenter();
}

void Monster::updateCenter()
{
	center.setPoint(topLeft.getX() + radius,
		topLeft.getY() + radius);
}

void Monster::updateDirection()
{
	char c = g_mobPath[nextInstr];
	nextInstr++;

	switch(c)
	{
	case '1':
		movingDir = RIGHT;
		break;
	case '2':
		movingDir = UP;
		break;
	case '3':
		movingDir = LEFT;
		break;
	case '4':
		movingDir = DOWN;
		break;
	default:
		movingDir = STILL;
		alive = false; //Using death to deal with reached exit
	}
}

void Monster::move()
{
	if (moveCounter == 0)
	{
		if(inNewSquare)
		{
			switch(movingDir)
			{
			case RIGHT:
				currentGridPos.incX();
				break;
			case UP:
				currentGridPos.decY();
				break;
			case LEFT:
				currentGridPos.decX();
				break;
			case DOWN:
				currentGridPos.incY();
				break;
			}
			updateGridPos = true;	
		}
		else
		{
			updateDirection();
		}
		inNewSquare = !inNewSquare;
		moveCounter =  g_tileSize / 2*ms;
	}

	if(movingDir == RIGHT)
		topLeft.addToX(ms);
	else if(movingDir == UP)
		topLeft.addToY(-ms);
	else if(movingDir == LEFT)
		topLeft.addToX(-ms);
	else if(movingDir == DOWN)
		topLeft.addToY(ms);
	moveCounter--;
	updateCenter();
}

bool Monster::wasShot(int dmg)
{
	hp -= dmg;
	if(hp < 1)
	{
		alive = false;
		return true;
	}
	return false;
}

