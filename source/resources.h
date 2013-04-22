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
};
/*
enum GameMode
{
MODE_TITLE,
MODE_GAMEPLAY,
};

extern GameMode g_GameMode;
*/

const unsigned int NUM_MAX_MOBS = 22;
const unsigned int GRID_COLUMNS = 20;
const unsigned int GRID_ROWS = 15;
const unsigned int NUM_TILE_TYPES = 16;
const unsigned int BUY_INCOME = 10;
const unsigned int SHOT_RADIUS = 6;
const unsigned int BASE_DAMAGE = 1;
const unsigned int GAMESPEED= 17;
const unsigned int MAX_RESOURCE = 999;

extern unsigned int g_tileSize;
extern std::string *g_mobPath;
extern unsigned int g_verticalBorder;
extern unsigned int g_horizontalBorder;

#endif //_RESOURCES_H
