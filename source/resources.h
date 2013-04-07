#ifndef _RESOURCES_H
#define _RESOURCES_H

enum Image {
	GRASS,
	WATER,
	TOWER,
	SPAWN,
	EXIT,
	MONSTER
};

#define NUM_MAX_MOBS 20
#define NUM_MAX_PATH_IN_RANGE 12
#define FPS 20
#define GAME_WIDTH  22
#define GAME_HEIGHT 13
#define HORIZONTAL_BORDER 30
#define VERTICAL_BORDER 20
#define NUM_TILE_TYPES 5

// Global variable used by the rendering functions to scale graphics.
extern int g_TileSize;

#endif _RESOURCES_H
