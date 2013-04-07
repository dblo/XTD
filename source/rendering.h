
#ifndef _RENDERING_H
#define _RENDERING_H

void CleanupImages();
void DrawBG(int wid, int hi);
void SetupImages();
void UpdateScreenSize();
void DrawTile(int colour, int x, int y, int size);
void drawMonster(int x, int y, int size);

#endif /* !_RENDERING_H */
