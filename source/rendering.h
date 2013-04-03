
#ifndef _RENDERING_H
#define _RENDERING_H

#include "game.h"

void CleanupImages();
void DrawBG(int wid, int hi);
void SetupImages();
void UpdateScreenSize();
void DrawTile(int colour, int x, int y, int size);

// Global variable used by the rendering functions to scale graphics.
extern int g_TileSize;

#endif /* !_RENDERING_H */
