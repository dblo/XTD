
#ifndef _TOWER_H
#define _TOWER_H

#include "resources.h"
#include "monster.h"
#include "tile.h"
#include "trackingShot.h"
#include "path_Grass_listener.h"

class Tower : public TowerListener, public Tile, public ObjectWithCenter
{
	bool mobTable[MAX_MONSTER_COUNT];
	int reloadStatus;
	int target; 
	//int killCount; //Number of monsters killed by this tower

	static int s_as; //Attackspeed in ms
	static int s_dmg;

public:
	Tower(int posX, int posY, int LeftX, int LeftY, int tileSize);
	~Tower() {};
	bool operator== (const Tower* t) const;
	int aquireTarget(int numCurrWaveMobs);
	bool armed() const;
	void initTower(int _dmg, int _range, int _attSpeed);	
	void mobLeft(int mobId);
	void mobEntered(int mobId);
	void reloadTick();

	// Returns damage done
	int shoot();

	static void setAttSpeed(int _as);
	static void setDmg(int _dmg);
	static void incDmg(int _dmg);

};

#endif //_TOWER_H
