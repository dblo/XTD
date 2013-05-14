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
	int builtWave;
	Image images[NUM_IMAGES];
	int value;

protected:
	int upgPath1Counter;
	int upgPath2Counter;
	int upgPath3Counter;
	int reloadStatus;
	int shotRadius;

	static int s_range;
	static int s_dmg;
	static int s_as; //Attack speed in ms

public:
	Tower(int leftX, int leftY, int tileSize, int _builtRound, int _value);
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
	//bool builtThisWave(int currWave) const;
	// Takes targets center coordinates
	bool targetInRange(int targetX, int targetY, int targetRad);

	virtual Image getImage() const = 0;
	virtual Image getUpg1Image() const = 0;
	virtual Image getUpg2Image() const = 0;
	virtual Image getUpg3Image() const = 0;
	virtual int getUpg1Price() const = 0;
	virtual int getUpg2Price() const = 0;
	virtual int getUpg3Price() const = 0;
	virtual const char* getDescription(int upgNum) const = 0;
	bool upgrade1Available() const;
	bool upgrade2Available() const;
	bool upgrade3Available() const;

	// Returns damage done
	virtual void shoot(std::list<BaseShot*> &shots, Monster *tarMon) = 0;

	virtual int getSellValue() const;
	static void resetTowers(int tileSize);
	static void buffAs();
	static void buffDmg(int _dmg);
	static void buffRange();
	static void fastAs();
	static void slowAs();

};

#endif //_TOWER_H
