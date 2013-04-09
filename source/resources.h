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
	WALL23
};

enum UpgradeLevel
{
	LEVEL1,
	LEVEL2,
	LEVEL3
};

enum Direction
{
	STILL,
	RIGHT,
	UP,
	LEFT,
	DOWN,
	
	UNDEF
};

/*
enum GameMode
{
MODE_TITLE,
MODE_GAMEPLAY,
};

extern GameMode g_GameMode;
*/

#define NUM_MAX_MOBS 20
#define GRID_COLUMNS  22
#define GRID_ROWS 13
#define NUM_TILE_TYPES 11

extern unsigned int g_tileSize;

extern unsigned int g_gameSpeed;

extern std::string g_mobPath;

#endif //_RESOURCES_H
