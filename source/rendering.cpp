#include <iostream>

#include "rendering.h"
#include "resources.h"

#include "Iw2D.h"
#include "IwResManager.h"

// Pointers to images used by the rendering code
CIw2DImage* tileImage[NUM_TILE_TYPES];
CIw2DImage* purpleMonster;
CIw2DFont* font;

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
	for(int i=0; i < wid; i++)
		for(int j=0; j < hi; j++)
			Iw2DDrawImage(
			tileImage[WATER],
			CIwSVec2(i*g_TileSize, j*g_TileSize)
			);
}

void DrawTile(int colour, int x, int y, int size) //skip size
{
	Iw2DDrawImage(
		tileImage[colour],
		CIwSVec2(x, y)
		);
}

void drawMonster(int x, int y, int size) //skip size
{
	Iw2DDrawImage(
		purpleMonster,
		CIwSVec2(x + VERTICAL_BORDER, y + HORIZONTAL_BORDER)
		);
}

void SetupImages()
{
	CleanupImages();

	tileImage[GRASS] = Iw2DCreateImageResource("tiles32grass");
	tileImage[WATER] = Iw2DCreateImageResource("tiles32water");
	tileImage[TOWER] = Iw2DCreateImageResource("tiles32tower");
	tileImage[SPAWN] = Iw2DCreateImageResource("tiles32spawn");
	tileImage[EXIT] = Iw2DCreateImageResource("tiles32exit");

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

