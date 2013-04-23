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

//void drawBG()
//{
//	for(int i=GRID_COLUMNS; i < GRID_COLUMNS + 3; i++) //MAGIC NUM
//		for(int j=0; j < GRID_ROWS; j++)
//			Iw2DDrawImage(
//			tileImage[WATER],
//			CIwSVec2(i*tileSize, j*tileSize) //bars uncude
//			);
//}

void drawTile(int colour, int x, int y)
{
	Iw2DDrawImage(
		tileImage[colour],
		CIwSVec2(x, y)
		);
}

void drawTile(int colour, int x, int y, int wi, int hi)
{
	Iw2DDrawImage(
		tileImage[colour],
		CIwSVec2(x, y),
		CIwSVec2(wi, hi)
		);
}

void drawPhasedTile(int colour, int x, int y)
{
	CIwColour c;
	c.Set(255,255,255,150); //store somewhere? 0x00000000 = 0
	Iw2DSetColour(c);
	drawTile(colour, x, y);
	Iw2DSetColour(0xFFFFFF);
}

void setupImages(int tileSize)
{
	cleanupImages();

	tileImage[GRASS] = Iw2DCreateImageResource("tilesgrass");
	tileImage[WATER] = Iw2DCreateImageResource("tileswater");
	tileImage[SPAWN] = Iw2DCreateImageResource("tilesspawn");
	tileImage[EXIT] = Iw2DCreateImageResource("tilesexit");
	tileImage[MONSTER] = Iw2DCreateImageResource("tilespurmon");
	tileImage[BUYTOWER] = Iw2DCreateImageResource("tilesbuy_tower");
	tileImage[SPEED] = Iw2DCreateImageResource("tilesspeed");
	tileImage[CONTWAVES] = Iw2DCreateImageResource("tilescontwaves");
	tileImage[INCOME] = Iw2DCreateImageResource("tilesincome");
	tileImage[PAUSE] = Iw2DCreateImageResource("tilespause");

	/*const char* imgType[] = 
	{
	"tower",
	"shot",
	"horwall",
	"vertwall"
	"diag14wall",
	"diag23wall"
	};*/

	//int len = sizeof(imgType) / sizeof(char);
	char temp[32];
	//for(int i=0; i < len; i++)
	//	{
	//		sprintf(temp, "tiles%d%s", tileSize, imgType[i]);	//change filename with # for the faster pack?
	//		//tileImage[i]
	//}
	sprintf(temp, "tiles%dtower", tileSize);	
	tileImage[TOWER] = Iw2DCreateImageResource(temp);

	sprintf(temp, "tiles%dshot", tileSize);	
	tileImage[SHOT] = Iw2DCreateImageResource(temp);

	sprintf(temp, "tiles%dhorwall", tileSize);	
	tileImage[HORWALL] = Iw2DCreateImageResource(temp);

	sprintf(temp, "tiles%dvertwall", tileSize);	
	tileImage[VERWALL] = Iw2DCreateImageResource(temp);

	sprintf(temp, "tiles%ddiag14wall", tileSize);	
	tileImage[WALL14] = Iw2DCreateImageResource(temp);

	sprintf(temp, "tiles%ddiag23wall", tileSize);	
	tileImage[WALL23] = Iw2DCreateImageResource(temp);
}

int updateScreenSize()
{
	int tileSize;
	int widSize = Iw2DGetSurfaceWidth() / GRID_COLUMNS;
	int hisize = Iw2DGetSurfaceHeight() / GRID_ROWS;
	
	if(MIN(widSize, hisize) < 40)
	{
		tileSize = 20;

	}
	else
	{
		tileSize = 40;
	}
	
	setupImages(tileSize);

	if(tileSize < 40)
		font = Iw2DCreateFontResource("font8");
	else
		font = Iw2DCreateFontResource("font18");

	Iw2DSetFont(font);

	return tileSize;
}
