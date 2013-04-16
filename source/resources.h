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
	CONTWAVES,
	INCOME,
	PAUSE,
};

enum Buttons {
	BUYTOWERBUTTON,
	SPEEDBUTTON,
	PAUSEBUTTON,
	INCOMEBUTTON,
	CONTWAVESBUTTON,
	BUYTOWERBUTTONBOT,
	SPEEDBUTTONBOT,
	PAUSEBUTTONBOT,
	INCOMEBUTTONBOT,
	CONTWAVESBUTTONBOT
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

enum Texts {
	CREDITSTEXT,
	INCOMETEXT,
	WAVETEXT,
	UPGRADETEXT
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

extern unsigned int g_tileSize;
extern std::string *g_mobPath;
extern unsigned int g_verticalBar;
extern unsigned int g_horizontalBar;

#endif //_RESOURCES_H
