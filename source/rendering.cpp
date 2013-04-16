#include <iostream>

#include "rendering.h"
#include "resources.h"

#include "Iw2D.h"
#include "IwResManager.h"

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
	//for(int i=GRID_COLUMNS; i < GRID_COLUMNS + 3; i++) //MAGIC NUM
	//	for(int j=0; j < GRID_ROWS; j++)
	//		Iw2DDrawImage(
	//		tileImage[WATER],
	//		CIwSVec2(i*g_tileSize, j*g_tileSize) //bars uncude
	//		);
}

void drawTile(int colour, int x, int y)
{
	Iw2DDrawImage(
		tileImage[colour],
		CIwSVec2(x + g_verticalBar, y + g_horizontalBar)
		);
}

void drawTile(int colour, int x, int y, int wi, int hi)
{
	Iw2DDrawImage(
		tileImage[colour],
		CIwSVec2(x + g_verticalBar, y + g_horizontalBar),
		CIwSVec2(wi, hi)
		);
}

void drawPhasedTile(int colour, int x, int y)
{
	CIwColour c;
	c.Set(255,255,255,150); //store somewhere?
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
	int widSize = Iw2DGetSurfaceWidth() / GRID_COLUMNS;
	int hisize = Iw2DGetSurfaceHeight() / GRID_ROWS;
	g_tileSize = MIN(widSize, hisize);
	g_tileSize -= g_tileSize % 20;
	
	g_horizontalBar = (Iw2DGetSurfaceHeight() - GRID_ROWS*g_tileSize) / 2;
	g_verticalBar = g_horizontalBar;// fix

	/*std::cout << "tile: " << g_tileSize << ", hor: " << g_horizontalBar
		<< ", ver: " << g_verticalBar << std::endl;*/

	setupImages();
	font = Iw2DCreateFontResource("font_small");
	Iw2DSetFont(font);
}