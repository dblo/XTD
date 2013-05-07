#ifndef _RESOURCES_H
#define _RESOURCES_H

#include <string>

enum Image {
	GrassImage,
	//WaterImage,
	TowerImage,
	SpawnImage,
	ExitImage,
	ShotImage,
	MonsterImage,
	RlWallImage,
	UdWallImage,
	SpeedImage,
	PauseImage,
	SellImage,
	BuyDamageImage,
	BuySpeedImage,
	BuyRangeImage,
	NormalSpeedImage,
	FastSpeedImage,
	IceImage,
	MudImage,
	CrossWallImage,
	RdWallImage,
	RuWallImage,
	LuWallImage,
	LdWallImage,
	RluWallImage,
	RldWallImage,
	UdrWallImage,
	UdlWallImage
};

const int ALLOWED_UNDOS				= 3;
const int AS_BUFF					= 6;
const int BASE_CREDITS				= 150;
const int BASE_DAMAGE				= 1;
const int BASE_MONSTER_COUNT		= 7;
const int GAME_SPEED				= 16;
const int MONSTER_BASE_HP			= 5;
const int MONSTER_SPAWN_INTERVAL	= 5;
const int MAX_WAVE					= 99;
const int MAX_CREDITS				= 99999;
const int MAX_MONSTER_COUNT			= 22;
const int NUM_BUTTON_YPOS			= 18;
const int MAX_DAMAGE_LEVEL			= 3;
const int MAX_SPEED_LEVEL			= 3;
const int MAX_RANGE_LEVEL			= 3;
const int NUM_TILE_TYPES			= 35;
const int SHOT_RADIUS				= 6;
const int TOUCH_INTERVAL			= 200;
const int TOWER_BASE_SPEED			= 1300/GAME_SPEED;
const int TOWER_BASE_DMG			= 5;
const int TOWER_PRICE				= 10;
const int NORMAL_SPEED				= 1;
const int FAST_SPEED				= 2;

#endif //_RESOURCES_H

// Grass		0xff10be36
// Dark green	0xFF046b0a
// Water		0xffff9900
// button bg    0xff09ab12
// tower grey	0xFF4E4949
