#ifndef _YELLOWTOWERBASE_H
#define _YELLOWTOWERBASE_H
#include "tower.h"

class YellowTowerBase : public Tower
{
public:
	YellowTowerBase(int leftX, int leftY, int tileSize, int _value, 
		int dmg, int spd);
	virtual ~YellowTowerBase() {};
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

const int YellowTowerBase::upgPath1Prices[UPGRADE_PATH_LEN] = {150};
const int YellowTowerBase::upgPath2Prices[UPGRADE_PATH_LEN] = {100};
const int YellowTowerBase::upgPath3Prices[UPGRADE_PATH_LEN] = {200};
const char YellowTowerBase::UPG_PATH_1_TEXTS[1][20] = {{"Tier2 Yellow tower"}};
const char YellowTowerBase::UPG_PATH_2_TEXTS[1][20] = {{"Upgrade2"}};
const char YellowTowerBase::UPG_PATH_3_TEXTS[1][20] = {{"Upgrade3"}};

void YellowTowerBase::shoot(std::list<BaseShot*> &shots, Monster *tarMon)
{
	shots.push_back(new BaseShot(
		getCenterX(),
		getCenterY(),
		YellowShotImage,
		tarMon, 
		damage,
		shotRadius));

	reloadStatus = speed;
}

YellowTowerBase::YellowTowerBase( int leftX, int leftY, int tileSize, int _value, 
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

Image YellowTowerBase::getImage() const
{
	return YellowTowerImage;
}

Image YellowTowerBase::getUpg1Image() const
{
	return YellowTowerImage;
}

Image YellowTowerBase::getUpg2Image() const
{
	return YellowTowerImage;
}

Image YellowTowerBase::getUpg3Image() const
{
	return YellowTowerImage;
}

int YellowTowerBase::getUpg1Price() const
{
	return upgPath1Prices[upgPath1Counter];
}

int YellowTowerBase::getUpg2Price() const
{
	return upgPath2Prices[upgPath2Counter];
}

int YellowTowerBase::getUpg3Price() const
{
	return upgPath3Prices[upgPath3Counter];
}

const char* YellowTowerBase::getDescription(int upgNum) const
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
#endif // _YELLOWTOWERBASE_H
