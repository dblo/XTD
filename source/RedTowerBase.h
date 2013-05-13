#ifndef _REDTOWERBASE_H
#define _REDTOWERBASE_H
#include "tower.h"

const int upgPath1Prices[UPGRADE_PATH_LEN] = {150};
const int upgPath2Prices[UPGRADE_PATH_LEN] = {100};
const int upgPath3Prices[UPGRADE_PATH_LEN] = {200};
const char UPG1PATHTEXTS[1][30] = {{"Tier2 red tower"}};
const char UPG2PATHTEXTS[1][30] = {{"Upgrade2"}};
const char UPG3PATHTEXTS[1][30] = {{"Upgrade3"}};

class RedTowerBase : public Tower
{
public:
	RedTowerBase(int leftX, int leftY, int tileSize, int _builtRound);
	virtual ~RedTowerBase() {};
	virtual int shoot();
	virtual Image getImage() const;
	virtual Image getUpg1Image() const;
	virtual Image getUpg2Image() const;
	virtual Image getUpg3Image() const;
	virtual int getUpg1Price() const;
	virtual int getUpg2Price() const;
	virtual int getUpg3Price() const;
};

int RedTowerBase::shoot()
{
	return Tower::shoot();
}

RedTowerBase::RedTowerBase( int leftX, int leftY, int tileSize, int _builtRound )
: Tower(leftX, leftY, tileSize, _builtRound) 
{
	upgPath1Counter = 0;
	upgPath2Counter = 0;
	upgPath3Counter = 0;
}

Image RedTowerBase::getImage() const
{
	return RedTowerImage;
}

Image RedTowerBase::getUpg1Image() const
{
	return RedTowerImage;
}

Image RedTowerBase::getUpg2Image() const
{
	return RedTowerImage;
}

Image RedTowerBase::getUpg3Image() const
{
	return RedTowerImage;
}

int RedTowerBase::getUpg1Price() const
{
	return upgPath1Prices[upgPath1Counter];
}

int RedTowerBase::getUpg2Price() const
{
	return upgPath2Prices[upgPath2Counter];
}

int RedTowerBase::getUpg3Price() const
{
	return upgPath3Prices[upgPath3Counter];
}


#endif // _REDTOWERBASE_H