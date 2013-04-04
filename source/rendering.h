
#ifndef _RENDERING_H
#define _RENDERING_H

#include "game.h"

enum TileType
{
	GRASS,
	WATER,
	TOWER,
	SPAWN,
	PURMON,
};

void CleanupImages();
void DrawBG(int wid, int hi);
void SetupImages();
void UpdateScreenSize();
void DrawTile(int colour, int x, int y, int size);
void drawMonster(int x, int y, int size);

// Global variable used by the rendering functions to scale graphics.
extern int g_TileSize;

#endif /* !_RENDERING_H */
