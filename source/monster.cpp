#include "monster.h"

#include <iostream>

//=============================================================================
Monster::Monster() : GridPosObject(0, 0)
{ 
	updateGridPos = false;
	hp = 0;
}
//=============================================================================
void Monster::spawn(int _gridPosX, int _gridPosY, int _topLeftX, int _topLeftY,
				   int _hp, int _mobId, int rad, int tileSize)
{
	gridPosX = _gridPosX;
	gridPosY = _gridPosY;
	topLeftX = _topLeftX;
	topLeftY = _topLeftY;
	hp = _hp;
	radius = rad;
	mobId = _mobId;
	nextInstr = 0;
	moveCounter =  tileSize / 2;
	movingDir = monster::StillDirection; 
	updateGridPos = false;
	UpdateCenter();
}
//=============================================================================
void Monster::UpdateCenter()
{
	centerX = topLeftX + radius;
	centerY = topLeftY + radius;
}
//=============================================================================
void Monster::UpdateDirection(const std::string &path)
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
		hp = 0; //Using death to deal with reached exit
	}
}
//=============================================================================
bool Monster::move(const std::string &path, int tileSize)
{
	if(moveMe)
	{
		int moveLen = ms;

		if(moveCounter > tileSize / 2)
		{
			if(moveCounter - ms < tileSize / 2)
				moveLen = moveCounter - tileSize/2;
		}
		else if(moveCounter == 0)
		{
			switch(movingDir)
			{
			case monster::RightDirection:
				gridPosX++;
				break;
			case monster::UpDirection:
				gridPosY--;
				break;
			case monster::LeftDirection:
				gridPosX--;
				break;
			case monster::DownDirection:
				gridPosY++;
				break;
			}
			updateGridPos = true;	
			moveCounter =  tileSize;
		}
		else if(moveCounter < tileSize / 2)
		{
			if(moveCounter < ms)
				moveLen = ms - moveCounter;
		}
		else
			UpdateDirection(path);

		if(movingDir == monster::RightDirection)
			topLeftX += moveLen;
		else if(movingDir == monster::UpDirection)
			topLeftY -= moveLen;
		else if(movingDir == monster::LeftDirection)
			topLeftX -= moveLen;
		else if(movingDir == monster::DownDirection)
			topLeftY += moveLen;
		else if(!isAlive())
			return false; //Monster reached exit

		moveCounter -= moveLen;
		UpdateCenter();
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
