#ifndef _TOWER_H
#define _TOWER_H

#include <vector>
#include <list>
#include "resources.h"
#include "monster.h"
#include "tile.h"
#include "baseShot.h"
#include "pathGrassListener.h"
#include "baseShot.h"

const int NUM_IMAGES = 4;
const int UPGRADE_PATH_LEN = 1;

class Tower : public TowerListener, public Object, public ObjectWithCenter
{
	bool mobTable[MAX_MONSTER_COUNT];
	int target; 
	int value;
	Image images[NUM_IMAGES];

protected:
	int upgPath1Counter;
	int upgPath2Counter;
	int upgPath3Counter;
	int reloadStatus;
	int shotRadius;
	int range;
	int damage;
	int speed; //Attack speed in ms
public:
	Tower(int leftX, int leftY, int tileSize, int _value);
	virtual ~Tower();
	bool operator== (const Tower* t) const;

	// Returns target index or (number of monsters at current wave + 1)
	// if none is found
	int acquireTarget(int numCurrWaveMobs);
	int acquireTarget(int numCurrWaveMobs, int scanFrom);

	bool armed() const;
	void initTower(int _dmg, int _range, int _attSpeed);	
	void mobLeft(int mobId);
	void mobEntered(int mobId);
	void reloadTick();

	// Takes targets center coordinates
	bool targetInRange(int targetX, int targetY, int targetRad);

	bool upgrade1Available() const;
	bool upgrade2Available() const;
	bool upgrade3Available() const;
	void buffSpeed(int buff);
	void buffDamage(int buff);
	void buffRange();
	void setFastSpeed();
	void setSlowSpeed();

	// Returns damage done
	virtual void shoot(std::list<BaseShot*> &shots, Monster *tarMon) = 0;

	virtual int getSellValue() const;
	virtual Image getImage() const = 0;
	virtual Image getUpg1Image() const = 0;
	virtual Image getUpg2Image() const = 0;
	virtual Image getUpg3Image() const = 0;
	virtual int getUpg1Price() const = 0;
	virtual int getUpg2Price() const = 0;
	virtual int getUpg3Price() const = 0;
	virtual const char* getDescription(int upgNum) const = 0;
};
#endif //_TOWER_H
