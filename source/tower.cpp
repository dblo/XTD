
#include "tower.h"

int Tower::s_attSpeed = 0;

Tower::Tower(int _x, int _y) : reloadStatus(0), dmg(BASE_DAMAGE), target(NUM_MAX_MOBS), 
	Tile(TOWER), 
	Object(_x * g_tileSize, _y * g_tileSize),
	ObjectWithCenter(_x * g_tileSize + g_tileSize / 2, _y * g_tileSize + g_tileSize / 2)
{
	for(int i=0; i < NUM_MAX_MOBS; i++)
		mobTable[i] = false;
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
	s_attSpeed += _attSpeed;
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

void Tower::shoot()
{
	reloadStatus = s_attSpeed;
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
