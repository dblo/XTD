#include "monster.h"

#include <iostream>

std::string Monster::mobPath = "11114441110";

Monster::Monster()
{ 
	x = 0;
	y = 0;
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

}

void Monster::initMonster(int _x, int _y, int _hp, int _ms, int _waveId, int _mobId)
{
	x = _x;
	y = _y;
	hp = _hp;
	ms = _ms;
	waveId = _waveId;
	mobId = _mobId;
	alive = true;
	updateGridPos = true;

	//set dir
}

void Monster::updateDirection()
{
	char c = (mobPath)[nextInstr];
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
		alive = false; //Using death to deal with exit grid
	}
}

void Monster::move()
{
	if (moveCounter == 0)
	{
		if(inNewSquare)
		{
			//Will get towers in range of this mob updated

			switch(movingDir)
			{
			case '1':
				currentGridPos.incX();
				break;
			case '2':
				currentGridPos.decY();
				break;
			case '3':
				currentGridPos.decX();
				break;
			case '4':
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
		moveCounter =  g_TileSize / 2*ms;
	}

	if(movingDir == RIGHT)
		x += ms ;
	else if(movingDir == UP)
		y -= ms;
	else if(movingDir == LEFT)
		x -= ms;
	else if(movingDir == DOWN)
		y += ms;
	moveCounter--;

}

