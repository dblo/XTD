#include "monster.h"

#include <iostream>

//=============================================================================
Monster::Monster() : GridPosObject(0, 0)
{ 
	update = false;
	hp = 0;
}
//=============================================================================
void Monster::spawn(int _gridPosX, int _gridPosY, int _topLeftX, int _topLeftY,
					int _hp, int _mobId, int rad, int tileSize, 
					const std::string &path)
{
	gridPosX = _gridPosX;
	gridPosY = _gridPosY;
	topLeftX = _topLeftX + (tileSize-2*rad)/2;
	topLeftY = _topLeftY + (tileSize-2*rad)/2;
	hp = _hp;
	radius = rad;
	mobId = _mobId;
	nextInstr = 0;
	savedMove = 0;
	update = false;
	updateCenter();

	moveCounter =  tileSize / 2;
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
	char c = path[nextInstr];
	nextInstr++;

	switch(c)
	{
	case 'r':
		movingDir = monster::RightDirection;
		break;
	case 'u':
		movingDir = monster::UpDirection;
		break;
	case 'l':
		movingDir = monster::LeftDirection;
		break;
	case 'd':
		movingDir = monster::DownDirection;
		break;
	default:
		movingDir = monster::StillDirection;
		hp = 0; //Using fake death to deal with reached exit
	}
}
//=============================================================================
bool Monster::move(const std::string &path, int tileSize)
{
	if(moveMe)
	{
		int moveLen = ms;

		if(savedMove > 0)
		{
			moveLen += savedMove;
			savedMove = 0;
		}

		if(moveCounter == tileSize/2)
		{
			updateDirection(path);

			if(movingDir == monster::StillDirection)
				return false; 
		}
		else if(moveCounter > tileSize / 2 && moveCounter - moveLen <= tileSize / 2)
		{ // Will reach tile middle
			savedMove = moveLen - (moveCounter - tileSize/2);
			moveLen -= savedMove;
		}
		else if(moveLen >= moveCounter)
		{ // Will reach new tile
			savedMove	= moveLen - moveCounter;
			moveLen		= moveCounter;
			moveCounter += tileSize;
			updateGridPos();
		}

		if(movingDir == monster::RightDirection)
			topLeftX += moveLen;
		else if(movingDir == monster::UpDirection)
			topLeftY -= moveLen;
		else if(movingDir == monster::DownDirection)
			topLeftY += moveLen;
		else //if(movingDir == monster::LeftDirection)
			topLeftX -= moveLen;

		moveCounter -= moveLen;
		updateCenter();
	}
	moveMe = !moveMe;
	return true;
}
//=============================================================================
bool Monster::takeDamage(int dmg)
{
	hp -= dmg;
	if(hp < 1)
	{
		return true;
	}
	return false;
}
//=============================================================================
bool Monster::despawned()
{
	if(hp > 0)
	{
		hp = 0;
		return true;
	}
	return false;
}

void Monster::updateGridPos()
{
	switch(movingDir)
	{
	case monster::RightDirection:
		gridPosX++;
		break;
	case monster::UpDirection:
		gridPosY--;
		break;
	case monster::DownDirection:
		gridPosY++;
		break;
	case monster::LeftDirection:
		gridPosX--;
		break;
	}
	update = true;
}
