#ifndef _RESOURCES_H
#define _RESOURCES_H

#include <string>

#include "pathingVertex.h"

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
	PAUSE,
	INCOME,
	CONTWAVES
};

enum UpgradeLevel
{
	LEVEL1,
	LEVEL2,
	LEVEL3
};

enum GameSpeedMode
{
	NEWWAVE,
	NORMAL,
	FAST
};

//enum Direction
//{
//	NONE,
//	RIGHT,
//	UP,
//	LEFT,
//	DOWN
//};

/*
enum GameMode
{
MODE_TITLE,
MODE_GAMEPLAY,
};

extern GameMode g_GameMode;
*/

const int NUM_MAX_MOBS = 22;
const int GRID_COLUMNS = 20;
const int GRID_ROWS = 15;
const int NUM_TILE_TYPES = 16;
const int BUY_INCOME = 10;

extern unsigned int g_tileSize;

extern unsigned int g_gameSpeed;

extern std::string *g_mobPath;

#endif //_RESOURCES_H
