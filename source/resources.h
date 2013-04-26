#ifndef _RESOURCES_H
#define _RESOURCES_H

#include <string>

enum Image {
	GrassImage,
	WaterImage,
	TowerImage,
	SpawnImage,
	ExitImage,
	ShotImage,
	MonsterImage,
	HorWallImage,
	VerWallImage,
	Wall14Image,
	Wall23Image,
	BuyImage,
	SpeedImage,
	ContWavesImage,
	IncomeImage,
	PauseImage,
	UndoImage,
	BuyDamageImage,
	BuySpeedImage,
	BuyRangeImage,
	NormalSpeedImage,
	FastSpeedImage
};

const unsigned int BASE_MONSTER_COUNT		= 7;
const unsigned int MAX_MONSTER_COUNT		= 22;
const unsigned int GRID_COLUMNS				= 24;
const unsigned int GRID_ROWS				= 14;
const unsigned int NUM_TILE_TYPES			= 22;
const unsigned int NUM_BUTTON_YPOS			= 18;
const unsigned int INCOME_PRICE				= 100;
const unsigned int TOWER_PRICE				= 10;
const unsigned int SHOT_RADIUS				= 6;
const unsigned int BASE_DAMAGE				= 1;
const unsigned int GAME_SPEED				= 16;
const unsigned int MAX_RESOURCE				= 999;
const unsigned int MAX_SCORE				= 999999;
const unsigned int MONSTER_SPAWN_INTERVAL	= 10;
const unsigned int FINAL_WAVE				= 50;
const unsigned int TOUCH_INTERVAL			= 200;
const unsigned int AS_BUFF					= 12;
const unsigned int NUM_OF_UPGRADE_LVLS		= 3;
const unsigned int ALLOWED_UNDOS			= 3;

#endif //_RESOURCES_H


// Light green	0xFF40C020
// Dark green	0xFF0C5907
// Light blue	0xffff9900
