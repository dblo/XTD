#include "tower.h"

Tower::Tower(int leftX, int leftY, int tileSize, int _value)
	: Object(leftX, leftY), 
	ObjectWithCenter(leftX + tileSize / 2, leftY + tileSize / 2)
{
	for(int i=0; i < MAX_MONSTER_COUNT; i++)
		mobTable[i] = false;

	value = _value;
	reloadStatus = 0;
	target = MAX_MONSTER_COUNT;
}

Tower::~Tower() {}

void Tower::buffRange(int newRange)
{
	range = newRange;
}

void Tower::buffDamage(int buff)
{
	damage += damage;
}

void Tower::buffSpeed(int newSpeed)
{
	speed = newSpeed;
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

bool Tower::armed() const
{
	return reloadStatus == 0;
}

int Tower::acquireTarget(int numWaveMobs)
{
	int i = target;
	for(; i < numWaveMobs; i++)
		if(mobTable[i])
			break;

	target = i;
	return target;
}

int Tower::acquireTarget(int numWaveMobs, int scanFrom)
{
	int i = scanFrom;
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

void Tower::setFastSpeed()
{
	speed = speed/2;
}

void Tower::setSlowSpeed()
{
	speed = speed*2;
}

/*
* 
*/
bool Tower::targetInRange(int targetX, int targetY, int targetRad)
{
	int deltaX = targetX - centerX;
	int deltaY = targetY - centerY;
	int hyp = range + targetRad;
	return hyp*hyp >= deltaX*deltaX + deltaY*deltaY; 
}

int Tower::getSellValue() const
{
	return value/2;
}

bool Tower::upgrade1Available() const
{
	return upgPath1Counter < UPGRADE_PATH_LEN;
}

bool Tower::upgrade2Available() const
{
	return upgPath2Counter < UPGRADE_PATH_LEN;
}

bool Tower::upgrade3Available() const
{
	return upgPath3Counter < UPGRADE_PATH_LEN;
}

int Tower::getRange() const
{
	return range;
}
