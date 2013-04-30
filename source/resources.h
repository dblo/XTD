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

const int ALLOWED_UNDOS				= 3;
const int AS_BUFF					= 6;
const int BASE_CREDITS				= 70;
const int BASE_DAMAGE				= 1;
const int BASE_INCOME				= 5;
const int BASE_MONSTER_COUNT		= 7;
const int GAME_SPEED				= 16;
const int INCOME_PRICE				= 100;
const int MONSTER_BASE_HP			= 5;
const int MONSTER_SPAWN_INTERVAL	= 10;
const int MAX_INCOME				= 999;
const int MAX_WAVE					= 99;
const int MAX_CREDITS				= 9999;
const int MAX_SCORE					= 999999;
const int MAX_MONSTER_COUNT			= 22;
const int NUM_BUTTON_YPOS			= 18;
const int MAX_SPEED_LEVEL			= 3;
const int MAX_RANGE_LEVEL			= 3;
const int NUM_TILE_TYPES			= 22;
const int SHOT_RADIUS				= 6;
const int TOUCH_INTERVAL			= 200;
const int TOWER_BASE_SPEED			= 1400/GAME_SPEED;
const int TOWER_BASE_DMG			= 5;
const int TOWER_PRICE				= 10;

#endif //_RESOURCES_H

// grass		0xFF36BE10
// Light green	0xFF40C020
// Dark green	0xFF0C5907
// Light blue	0xffff9900
