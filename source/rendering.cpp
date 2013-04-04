#include <iostream>

#include "rendering.h"
#include "resources.h"

#include "Iw2D.h"
#include "IwResManager.h"

// Pointers to images used by the rendering code
CIw2DImage* tileImage[NUM_TILE_TYPES];
CIw2DImage* purpleMonster;
CIw2DFont* font;

int g_TileSize = 20;

void CleanupImages()
{
	for(int i=0; i < NUM_TILE_TYPES; i++)
		delete tileImage[i];

	delete purpleMonster;
    delete font;
}

// Draws a background by tiling the specified material to fill the specified area
void DrawBG(int wid, int hi)
{
	Iw2DSetColour(0xffccab66);
	Iw2DFillRect(CIwSVec2(0,0), CIwSVec2(wid, hi));
	Iw2DSetColour(0xffffffff);
}

void DrawTile(int colour, int x, int y, int size) //skpi size
{
	 Iw2DDrawImage(
        tileImage[colour],
        CIwSVec2(x, y),
        CIwSVec2(g_TileSize, g_TileSize)
        );
}

void drawMonster(int x, int y, int size) //skip size
{
	Iw2DDrawImage(
		purpleMonster,
        CIwSVec2(x + VERTICAL_BORDER, y + HORIZONTAL_BORDER)
        //,CIwSVec2(g_TileSize, g_TileSize)
        );
		
}

void SetupImages()
{
    CleanupImages();
	
	tileImage[0] = Iw2DCreateImageResource("tiles32grass");
	tileImage[1] = Iw2DCreateImageResource("tiles32water");
	tileImage[2] = Iw2DCreateImageResource("tiles32tower");
	tileImage[3] = Iw2DCreateImageResource("tiles32spawn");

	purpleMonster = Iw2DCreateImageResource("tiles32purmon");
}

void UpdateScreenSize()
{
	// Look up materials by name
	SetupImages();

	// Choose font 

	font = Iw2DCreateFontResource("font");

	//Set this font as the current one
	Iw2DSetFont(font);

}

