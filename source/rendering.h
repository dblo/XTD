
#ifndef _RENDERING_H
#define _RENDERING_H

void cleanUpImages();
void drawBG(int tileSize);
void setUpGrapicRes(int tileSize);
void drawTile(int colour, int x, int y);
void drawTile(int colour, int x, int y, int wi, int hi);

#endif /* !_RENDERING_H */
