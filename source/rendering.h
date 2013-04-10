
#ifndef _RENDERING_H
#define _RENDERING_H

#include "point.h"
#include "resources.h"

void cleanupImages();
void drawBG();
void setupImages();
void updateScreenSize();
void drawTile(int colour, const Point &p, int size);
void drawTile(int colour, const Point &p);
void drawTile(int colour, int x, int y);

#endif /* !_RENDERING_H */
