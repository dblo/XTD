#include "monster.h"

#include <iostream>

Monster::Monster() : GridPosObject(0, 0)
{ 
	hp = 0;
	mobId = 0;	
	alive = false;
}

void Monster::init(int _x, int _y, int _hp, int _ms, int _mobId)
{
	gridPosX = _x;
	gridPosY = _y;
	topLeftX = _x * g_tileSize;
	topLeftY = _y * g_tileSize;
	hp = _hp;
	ms = _ms;
	radius = g_tileSize / 2 - 2; //MAGIC
	mobId = _mobId;
	alive = true;
	nextInstr = 0;
	moveCounter = 0;
	movingDir = STILL; 
	updateGridPos = false;
	inNewSquare = false;

	updateCenter();
}

void Monster::updateCenter()
{
	centerX = topLeftX + radius;
	centerY = topLeftY + radius;
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
			gridPosX++;
			break;
		case UP:
			gridPosY--;
			break;
		case LEFT:
			gridPosX--;
			break;
		case DOWN:
			gridPosY++;
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
		topLeftX += ms;
	else if(movingDir == UP)
		topLeftY -= ms;
	else if(movingDir == LEFT)
		topLeftX -= ms;
	else if(movingDir == DOWN)
		topLeftY += ms;
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

