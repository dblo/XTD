
#include "tower.h"

int Tower::s_as = 0;
int Tower::s_dmg = 0;

Tower::Tower(int posX, int posY, int LeftX, int LeftY, int tileSize) //TODO remove posxy, if tower not inherit gridpos
	: reloadStatus(0), target(MAX_MONSTER_COUNT), 
	Tile(TowerImage, LeftX, LeftY), 
	ObjectWithCenter(LeftX + tileSize / 2, LeftY + tileSize / 2)
{
	for(int i=0; i < MAX_MONSTER_COUNT; i++)
		mobTable[i] = false;
}

void Tower::setAttSpeed(int _as)
{
	s_as = _as;
}

void Tower::incDmg(int _dmg)
{
	s_dmg += _dmg;
}

void Tower::setDmg(int _dmg)
{
	s_dmg = _dmg;
}

void Tower::mobLeft(int mobId)
{
	mobTable[mobId] = false;
}

bool Tower::operator== (const Tower* t) const
{
	return topLeftX == t->topLeftX && topLeftY == t->topLeftY;
}

void Tower::mobEntered(int mobId)
{
	mobTable[mobId] = true;
	if(mobId < target)
		target = mobId;
}

int Tower::shoot()
{
	reloadStatus = s_as;
	return s_dmg;
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
	reloadStatus -= 1;
}
