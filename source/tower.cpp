
#include "tower.h"

int Tower::dmg = 0;
int Tower::range = 0;
int Tower::attSpeed = 0;
int Tower::sellVal = 0;

Tower::Tower(int _x, int _y) : x(_x), y(_y), Tile(TOWER)
{
	for(int i=0; i < NUM_MAX_MOBS; i++)
		mobTable[i] = false;
}

void Tower::incDmg(int _dmg)
{
	dmg += _dmg;
}

void Tower::incRange(int _range)
{
	range += range;
}

void Tower::intAttSpeed(int _attSpeed)
{
	attSpeed += _attSpeed;
}
void Tower::setTowerStatics(int _dmg, int _range, int _attSpeed, int _sellVal) 
{
	dmg =_dmg; 
	range = _range;
	attSpeed = attSpeed;
	sellVal = sellVal;
}

void Tower::mobLeft(int mobId)
{
	mobTable[mobId] = false;
}

void Tower::mobEntered(int mobId)
{
	mobTable[mobId] = true;
}
