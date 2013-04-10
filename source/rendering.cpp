#include <iostream>

#include "rendering.h"
#include "resources.h"

#include "Iw2D.h"
#include "IwResManager.h"
#include "point.h"

// Pointers to images used by the rendering code
CIw2DImage* tileImage[NUM_TILE_TYPES];
CIw2DImage* purpleMonster;
CIw2DFont* font;

void cleanupImages()
{
	for(int i=0; i < NUM_TILE_TYPES; i++)
		delete tileImage[i];

	delete font;
}

void drawBG()
{
	int columns = Iw2DGetSurfaceWidth() / g_tileSize;
	int rows = Iw2DGetSurfaceHeight() / g_tileSize;

	for(int i=0; i < columns; i++)
		for(int j=0; j < rows; j++)
			Iw2DDrawImage(
			tileImage[WATER],
			CIwSVec2(i*g_tileSize, j*g_tileSize)
			);
}

void drawTile(int colour, const Point &p, int size)
{
	Iw2DDrawImage(
		tileImage[colour],
		CIwSVec2(p.getX() + g_tileSize, p.getY() + g_tileSize),
		CIwSVec2(size, size)
		);
}

void drawTile(int colour, int x, int y, int size)
{
	Iw2DDrawImage(
		tileImage[colour],
		CIwSVec2(x + g_tileSize, y + g_tileSize),
		CIwSVec2(size, size)
		);
}

void drawTile(int colour, const Point &p)
{
	Iw2DDrawImage(
		tileImage[colour],
		CIwSVec2(p.getX() + g_tileSize, p.getY() + g_tileSize)
		);
}

void drawTile(int colour, int x, int y)
{
	Iw2DDrawImage(
		tileImage[colour],
		CIwSVec2(x + g_tileSize, y + g_tileSize)
		);
}

void setupImages()
{
	cleanupImages();

	tileImage[GRASS] = Iw2DCreateImageResource("tiles32grass");
	tileImage[WATER] = Iw2DCreateImageResource("tiles32water");
	tileImage[TOWER] = Iw2DCreateImageResource("tiles32tower");
	tileImage[SPAWN] = Iw2DCreateImageResource("tiles32spawn");
	tileImage[EXIT] = Iw2DCreateImageResource("tiles32exit");
	tileImage[SHOT] = Iw2DCreateImageResource("tiles32shot");
	tileImage[MONSTER] = Iw2DCreateImageResource("tiles32purmon");
	tileImage[HORWALL] = Iw2DCreateImageResource("tiles32horwall");
	tileImage[VERWALL] = Iw2DCreateImageResource("tiles32vertwall");
	tileImage[WALL14] = Iw2DCreateImageResource("tiles32diag14wall");
	tileImage[WALL23] = Iw2DCreateImageResource("tiles32diag23wall");
}

void updateScreenSize()
{
	setupImages();
	font = Iw2DCreateFontResource("font");
	Iw2DSetFont(font);
}
