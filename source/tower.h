
#ifndef _TOWER_H
#define _TOWER_H

#include "resources.h"
#include "monster.h"
#include "tile.h"

struct Tower : public TowerListener, public Tile
{
	static int dmg;		 
	static int range;  //Range in pixels
	static int attSpeed; //Attackspeed in ms
	static int sellVal; //50% of tower value
	int x, y; //Pixel-coordinates of tower center

	//
	bool mobTable[NUM_MAX_MOBS];

	int wavePrio; //Index of oldest wave in rangeTables

	//int killCount; //Number of monsters killed by this tower

	Tower(int _x, int _y); 
	~Tower() {};

	void initTower(int _dmg, int _range, int _attSpeed);
	
	static void incDmg(int _dmg);
	static void incRange(int _range);
	static void intAttSpeed(int _attSpeed);
	static void setTowerStatics(int _dmg, int _range, int _attSpeed, int _sellVal);
	void mobLeft(int mobId);
	void mobEntered(int mobId);
};

#endif _TOWER_H
