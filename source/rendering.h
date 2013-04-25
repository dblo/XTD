
#ifndef _RENDERING_H
#define _RENDERING_H

void cleanUpImages();
void drawBG(int tileSize);
void setUpImages(int tileSize);
int UpdateScreenSize();
void drawTile(int colour, int x, int y);
void drawTile(int colour, int x, int y, int wi, int hi);

#endif /* !_RENDERING_H */
