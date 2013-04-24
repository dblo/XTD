
#include "tower.h"

int Tower::s_attSpeed = 0;

Tower::Tower(int posX, int posY, int leftX, int leftY, int tileSize) //TODO remove posxy
	: reloadStatus(0), dmg(BASE_DAMAGE), target(MAX_MONSTER_COUNT), 
	Tile(TOWER, leftX, leftY), 
	ObjectWithCenter(leftX + tileSize / 2, leftY + tileSize / 2)
{
	for(int i=0; i < MAX_MONSTER_COUNT; i++)
		mobTable[i] = false;
}

bool Tower::operator== (const Tower* t) const
{
	return topLeftX == t->topLeftX && topLeftY == t->topLeftY;
}

int Tower::getDmg() const
{
	return dmg;
}

void Tower::incDmg(int _dmg)
{
	dmg += _dmg;
}

void Tower::initAttSpeed(int _attSpeed)
{
	s_attSpeed = _attSpeed;
}

void Tower::setAttSpeed(int _attSpeed)
{
	s_attSpeed = _attSpeed;
}

void Tower::mobLeft(int mobId)
{
	mobTable[mobId] = false;
}

void Tower::mobEntered(int mobId)
{
	mobTable[mobId] = true;
	if(mobId < target)
		target = mobId;
}

void Tower::initiateReload()
{
	reloadStatus = s_attSpeed*2;//change, make var
}

bool Tower::armed() const
{
	return reloadStatus == 0;
}

int Tower::aquireTarget(int numWaveMobs)
{
	int i = target;
	for(; i < numWaveMobs; i++)
		if(mobTable[i])
			break;

	target = i;
	return target;
}

void Tower::reloadTick()
{
	reloadStatus-= 1;
}

