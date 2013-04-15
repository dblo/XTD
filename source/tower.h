
#ifndef _TOWER_H
#define _TOWER_H

#include "resources.h"
#include "monster.h"
#include "tile.h"
#include "trackingShot.h"
#include "path_grass_listener.h"

class Tower : public TowerListener, public Tile, public Object, public ObjectWithCenter
{
	int dmg;		 
	bool mobTable[NUM_MAX_MOBS];
	int reloadStatus;
	int target; 
	//int killCount; //Number of monsters killed by this tower

	static int s_attSpeed; //Attackspeed in ms

public:
	Tower(int _x, int _y); 
	~Tower() {};
	int aquireTarget(int numCurrWaveMobs);
	bool armed() const;
	int getDmg() const;
	void incDmg(int _dmg);
	void initTower(int _dmg, int _range, int _attSpeed);	
	void mobLeft(int mobId);
	void mobEntered(int mobId);
	void reloadTick();
	void initiateReload();

	static void initAttSpeed(int _attSpeed);
	static void setAttSpeed(int _attSpeed);
};

#endif //_TOWER_H
