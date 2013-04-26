
#include "tower.h"

int Tower::s_as = 0;
int Tower::s_dmg = 0;
int Tower::s_asCounter = 0;
int Tower::s_dmgCounter = 0;
int Tower::s_range = 0;
int Tower::s_rangeCounter = 0;

Tower::Tower(int posX, int posY, int LeftX, int LeftY, int tileSize) //TODO remove posxy, if tower not inherit gridpos
	: reloadStatus(0), target(MAX_MONSTER_COUNT), 
	Tile(TowerImage, LeftX, LeftY), 
	ObjectWithCenter(LeftX + tileSize / 2, LeftY + tileSize / 2)
{
	for(int i=0; i < MAX_MONSTER_COUNT; i++)
		mobTable[i] = false;
}

bool Tower::rangeUpgraded()
{
	return s_rangeCounter > 0;
}

void Tower::buffRange()
{
	s_range += s_range/3;
	s_rangeCounter++;
}

bool Tower::dmgUncapped()
{
	return s_dmgCounter < NUM_OF_UPGRADE_LVLS;
}

bool Tower::asUncapped()
{
	return s_asCounter < NUM_OF_UPGRADE_LVLS;
}

bool Tower::rangeUncapped()
{
	return s_rangeCounter < NUM_OF_UPGRADE_LVLS;
}

void Tower::resetTowers(int tileSize)
{
	s_range = tileSize;
	s_as = 1000/GAME_SPEED;
	s_dmg = 1;
	s_asCounter = 0;
	s_dmgCounter = 0;
	s_rangeCounter = 0;
}

void Tower::buffDmg(int _dmg)
{
	s_dmg += _dmg;
	s_dmgCounter++;
}

void Tower::buffAs()
{
	s_as -= AS_BUFF;
	s_asCounter++;
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

void Tower::fastAs()
{
	s_as = s_as*2;
}

void Tower::slowAs()
{
	s_as = s_as/2;
}

/*
 * 
 */
bool Tower::targetInRange(int targetX, int targetY, int targetRad)
{
	int deltaX = targetX - centerX;
	int deltaY = targetY - centerY;
	int hyp = s_range + targetRad;
	return hyp*hyp >= deltaX*deltaX + deltaY*deltaY; 
}