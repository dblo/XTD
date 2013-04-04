
#ifndef _TOWER_H
#define _TOWER_H

#include "resources.h"
#include "monster.h"

struct Tower
{
	static int dmg;		 
	static int range;  //Range in pixels
	static int attSpeed; //Attackspeed in ms
	static int sellVal; //50% of tower value
	int x, y; //Pixel-coordinates of tower center

	//
	bool* rangeTables[SIMUL_WAVES_ALLOWED]; 

	int wavePrio; //Index of oldest wave in rangeTables

	//int killCount; //Number of monsters killed by this tower

	Tower(int _x, int _y, int sellVal) : x(_x), y(_y) {}
	~Tower();

	void initTower(int _dmg, int _range, int _attSpeed);
	
	static void incDmg(int _dmg);
	static void incRange(int _range);
	static void intAttSpeed(int _attSpeed);
	static void setTowerStatics(int _dmg, int _range, int _attSpeed, int _sellVal);
	
};

int Tower::dmg = 0;
int Tower::range = 0;
int Tower::attSpeed = 0;
int Tower::sellVal = 0;

#endif _TOWER_H
