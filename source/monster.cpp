#include "monster.h"

#include <iostream>

//=============================================================================
Monster::Monster() : GridPosObject(0, 0)
{ 
	hp = 0;
	mobId = 0;	
	alive = false;
}
//=============================================================================
void Monster::init(int _gridPosX, int _gridPosY, int _topLeftX, int _topLeftY,
				   int _hp, int _ms, int _mobId, int rad)
{
	gridPosX = _gridPosX;
	gridPosY = _gridPosY;
	topLeftX = _topLeftX;
	topLeftY = _topLeftY;
	hp = _hp;
	ms = _ms;
	radius = rad;
	mobId = _mobId;
	alive = true;
	nextInstr = 0;
	moveCounter = 0;
	movingDir = STILL; 
	updateGridPos = false;
	inNewSquare = false;

	updateCenter();
}
//=============================================================================
void Monster::updateCenter()
{
	centerX = topLeftX + radius;
	centerY = topLeftY + radius;
}
//=============================================================================
void Monster::updateDirection(const std::string &path)
{
	char c = (path)[nextInstr];
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
//=============================================================================
bool Monster::move(const std::string &path, int tileSize)
{
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
		moveCounter =  tileSize;
	}
	else if(moveCounter == tileSize / 2)
	{
		updateDirection(path);
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
	else if(!alive)
		return false; //Monster reached finish

	moveCounter -= ms;
	updateCenter();
	return true;
}
//=============================================================================
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
//=============================================================================
