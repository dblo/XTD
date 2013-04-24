#ifndef _RESOURCES_H
#define _RESOURCES_H

#include <string>

//add namespace?
enum Image {
	GRASS,
	WATER,
	TOWER,
	SPAWN,
	EXIT,
	SHOT,
	MONSTER,
	HORWALL,
	VERWALL,
	WALL14,
	WALL23,
	BUYTOWER,
	SPEED,
	CONTWAVES,
	INCOME,
	PAUSE,
	UNDO
};

const unsigned int BASE_MONSTER_COUNT		= 7;
const unsigned int MAX_MONSTER_COUNT		= 22;
const unsigned int GRID_COLUMNS				= 24;
const unsigned int GRID_ROWS				= 14;
const unsigned int NUM_TILE_TYPES			= 17;
const unsigned int INCOME_PRICE				= 100;
const unsigned int TOWER_PRICE				= 10;
const unsigned int SHOT_RADIUS				= 6;
const unsigned int BASE_DAMAGE				= 1;
const unsigned int GAMESPEED				= 16;
const unsigned int MAX_RESOURCE				= 999;
const unsigned int MAX_SCORE				= 999999;
const unsigned int MONSTER_SPAWN_INTERVAL	= 10;
const unsigned int FINAL_WAVE				= 50;

#endif //_RESOURCES_H
