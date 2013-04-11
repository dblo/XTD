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

const int SURROUNDINGBAR = 10;

void cleanupImages()
{
	for(int i=0; i < NUM_TILE_TYPES; i++)
		delete tileImage[i];

	delete font;
}

void drawBG()
{
	for(int i=GRID_COLUMNS; i < GRID_COLUMNS + 3; i++) //MAGIC NUM
		for(int j=0; j < GRID_ROWS; j++)
			Iw2DDrawImage(
			tileImage[WATER],
			CIwSVec2(i*g_tileSize, j*g_tileSize)
			);
}

void drawTile(int colour, const Point &p, int size)
{
	Iw2DDrawImage(
		tileImage[colour],
		CIwSVec2(p.getX() + SURROUNDINGBAR, p.getY() + SURROUNDINGBAR),
		CIwSVec2(size, size)
		);
}

void drawTile(int colour, const Point &p)
{
	Iw2DDrawImage(
		tileImage[colour],
		CIwSVec2(p.getX() + SURROUNDINGBAR, p.getY() + SURROUNDINGBAR)
		);
}

void drawTile(int colour, int x, int y)
{
	Iw2DDrawImage(
		tileImage[colour],
		CIwSVec2(x + SURROUNDINGBAR, y + SURROUNDINGBAR)
		);
}

void drawPhasedTile(int colour, int x, int y)
{
	CIwColour c;
	c.Set(255,255,255,150);
	Iw2DSetColour(c);
	drawTile(colour, x, y);
	Iw2DSetColour(0xFFFFFF);
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
	tileImage[BUYTOWER] = Iw2DCreateImageResource("tiles32buy_tower");
	tileImage[SPEED] = Iw2DCreateImageResource("tiles32speed");
	tileImage[CONTWAVES] = Iw2DCreateImageResource("tiles32contwaves");
	tileImage[INCOME] = Iw2DCreateImageResource("tiles32income");
	tileImage[PAUSE] = Iw2DCreateImageResource("tiles32pause");

}

void updateScreenSize()
{
	setupImages();
	font = Iw2DCreateFontResource("font_small");
	Iw2DSetFont(font);
}
