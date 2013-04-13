#include "monster.h"

#include <iostream>

Monster::Monster()
{ 
	hp = 0;
	ms = 0;
	waveId = 0;
	mobId = 0;	
	alive = false;
	radius = 8;
}

void Monster::init(int _x, int _y, int _hp, int _ms, int _waveId, int _mobId)
{
	currentGridPos.setPoint(_x, _y);
	topLeft.setPoint(_x * g_tileSize, _y * g_tileSize);
	hp = _hp;
	ms = _ms;
	waveId = _waveId;
	mobId = _mobId;
	alive = true;
	nextInstr = 0;
	movingDir = STILL; 
	updateGridPos = false;
	moveCounter = 0;
	inNewSquare = false;
	updateCenter();
}

void Monster::updateCenter()
{
	center.setPoint(topLeft.getX() + radius,
		topLeft.getY() + radius);
}

void Monster::updateDirection()
{
	char c = (*g_mobPath)[nextInstr];
	nextInstr++;

	switch(c)
	{
	case 'r':
		movingDir = RIGHT;
		break;
	case 'u':
		movingDir = UP;
		break;
	case 'l':
		movingDir = LEFT;
		break;
	case 'd':
		movingDir = DOWN;
		break;
	default:
		movingDir = STILL;
		alive = false; //Using death to deal with reached exit
	}
}

void Monster::move()
{
	static int half = g_tileSize / 2;

	if(moveCounter == 0)
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
		moveCounter =  g_tileSize;
	}
	else if(moveCounter == half)
	{
		updateDirection();

	}
	inNewSquare = !inNewSquare;

	if(movingDir == RIGHT)
		topLeft.addToX(ms);
	else if(movingDir == UP)
		topLeft.addToY(-ms);
	else if(movingDir == LEFT)
		topLeft.addToX(-ms);
	else if(movingDir == DOWN)
		topLeft.addToY(ms);
	moveCounter -= ms;
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

