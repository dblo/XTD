#include "monster.h"

#include <iostream>

//=============================================================================
Monster::Monster() : GridPosObject(0, 0)
{ 
	gridPosX = 0;
	gridPosY = 0;
	topLeftX = 0;
	topLeftY = 0;
	hp = 0;
	ms = 0;
	radius = 0;
	mobId = 0;
	nextInstr = 0;
	moveCounter =  0;
	movingDir = monster::StillDirection; 
	alive = false;
	UpdateGridPos = false;
	inNewSquare = false;

	UpdateCenter();
}
//=============================================================================
void Monster::init(int _gridPosX, int _gridPosY, int _topLeftX, int _topLeftY,
				   int _hp, int _ms, int _mobId, int rad, int tileSize)
{
	gridPosX = _gridPosX;
	gridPosY = _gridPosY;
	topLeftX = _topLeftX;
	topLeftY = _topLeftY;
	hp = _hp;
	ms = _ms;
	radius = rad;
	mobId = _mobId;
	nextInstr = 0;
	moveCounter =  tileSize / 2;
	movingDir = monster::StillDirection; 
	alive = true;
	UpdateGridPos = false;
	inNewSquare = false;

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
	char c = (path)[nextInstr];
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
		alive = false; //Using death to deal with reached exit
	}
}
//=============================================================================
bool Monster::move(const std::string &path, int tileSize)
{
	int moveLen = ms;

	if(moveCounter > tileSize/2)
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
		UpdateGridPos = true;	
		moveCounter =  tileSize;
	}
	else if(moveCounter < tileSize / 2)
	{
		if(moveCounter < ms)
			moveLen = ms - moveCounter;
	}
	else
		UpdateDirection(path);
	
	inNewSquare = !inNewSquare;

	if(movingDir == monster::RightDirection)
		topLeftX += moveLen;
	else if(movingDir == monster::UpDirection)
		topLeftY -= moveLen;
	else if(movingDir == monster::LeftDirection)
		topLeftX -= moveLen;
	else if(movingDir == monster::DownDirection)
		topLeftY += moveLen;
	else if(!alive)
		return false; //Monster reached finish

	moveCounter -= moveLen;
	UpdateCenter();
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
bool Monster::despawned()
{
	if(hp > 0)
	{
		hp = 0;
		return true;
	}
	return false;
}
//=============================================================================
void Monster::updateMs(Image terrain, int baseMs)
{
	if(ms > baseMs)
	{
		if(terrain == GrassImage)
			ms /= 2;
		else if(terrain == MudImage)
			ms /= 4;
	}
	else if(ms < baseMs)
	{
		if(terrain == GrassImage)
			ms *= 2;
		else if(terrain == IceImage)
			ms *= 4;
	} 
	else if(terrain != GrassImage)
	{
		if(terrain == MudImage)
			ms /= 2;
		else
			ms *= 2;
	}
}
