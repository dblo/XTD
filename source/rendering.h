
#ifndef _RENDERING_H
#define _RENDERING_H

void cleanupImages();
void drawBG(int tileSize);
void setupImages(int tileSize);
int updateScreenSize();
void drawTile(int colour, int x, int y);
void drawTile(int colour, int x, int y, int wi, int hi);
//void drawPhasedTile(int colour, int x, int y);

#endif /* !_RENDERING_H */
