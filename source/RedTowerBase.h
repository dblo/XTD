#ifndef _REDTOWERBASE_H
#define _REDTOWERBASE_H
#include "tower.h"

class RedTowerBase : public Tower
{
public:
	RedTowerBase(int leftX, int leftY, int tileSize, int _value,
		int dmg, int spd);
	virtual ~RedTowerBase() {};
	virtual void shoot(std::list<BaseShot*> &shots, Monster *tarMon);
	virtual Image getImage() const;
	virtual Image getUpg1Image() const;
	virtual Image getUpg2Image() const;
	virtual Image getUpg3Image() const;
	virtual int getUpg1Price() const;
	virtual int getUpg2Price() const;
	virtual int getUpg3Price() const;
	virtual const char* getDescription(int upgNum) const;
private:
	static const int upgPath1Prices[UPGRADE_PATH_LEN];
	static const int upgPath2Prices[UPGRADE_PATH_LEN];
	static const int upgPath3Prices[UPGRADE_PATH_LEN];
	static const char UPG_PATH_1_TEXTS[1][20];
	static const char UPG_PATH_2_TEXTS[1][20];
	static const char UPG_PATH_3_TEXTS[1][20];
};

const int RedTowerBase::upgPath1Prices[UPGRADE_PATH_LEN] = {150};
const int RedTowerBase::upgPath2Prices[UPGRADE_PATH_LEN] = {100};
const int RedTowerBase::upgPath3Prices[UPGRADE_PATH_LEN] = {200};
const char RedTowerBase::UPG_PATH_1_TEXTS[1][20] = {{"Tier2 Red tower"}};
const char RedTowerBase::UPG_PATH_2_TEXTS[1][20] = {{"Upgrade2"}};
const char RedTowerBase::UPG_PATH_3_TEXTS[1][20] = {{"Upgrade3"}};

void RedTowerBase::shoot(std::list<BaseShot*> &shots, Monster *tarMon)
{
	shots.push_back(new BaseShot(
		getCenterX(),
		getCenterY(),
		RedShotImage,
		tarMon, 
		damage,
		shotRadius));

	reloadStatus = speed;
}

RedTowerBase::RedTowerBase( int leftX, int leftY, int tileSize, int _value,
						   int dmg, int spd)
: Tower(leftX, leftY, tileSize, _value) 
{
	upgPath1Counter = 0;
	upgPath2Counter = 0;
	upgPath3Counter = 0;
	shotRadius = (tileSize*2) / 5;
	range = tileSize;
	damage = dmg;
	speed = spd;
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

const char* RedTowerBase::getDescription(int upgNum) const
{
	switch(upgNum)
	{
	case 1:
		return UPG_PATH_1_TEXTS[upgPath1Counter];
	case 2:
		return UPG_PATH_2_TEXTS[upgPath2Counter];
	}
	return UPG_PATH_3_TEXTS[upgPath3Counter];
}
#endif // _REDTOWERBASE_H