#include <iostream>

#include "rendering.h"
#include "resources.h"

#include "Iw2D.h"
#include "IwResManager.h"

// Pointers to images used by the rendering code
CIw2DImage* tileImage[NUM_TILE_TYPES];
CIw2DImage* purpleMonster;
CIw2DFont* font;

void cleanUpImages()
{
	for(int i=0; i < NUM_TILE_TYPES; i++)
		delete tileImage[i];

	delete font;
}

void drawBG(int tileSize)
{
	Iw2DSetColour(0xffffffff);
	Iw2DSetAlphaMode(IW_2D_ALPHA_HALF);
	/*
	Iw2DFillRect(CIwSVec2(0, 0), 
		CIwSVec2((int)Iw2DGetSurfaceWidth(),
			(int)Iw2DGetSurfaceHeight()));*/

	//for(int i=0; i < GRID_COLUMNS; i++)
		//for(int j=0; j < GRID_ROWS; j++)
		
			/*drawTile(GrassImage, 
			0, 0,
			(int)Iw2DGetSurfaceWidth(),
			(int)Iw2DGetSurfaceHeight());*/
	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
}

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

void setUpImages(int tileSize)
{
	cleanUpImages();

	tileImage[GrassImage]		= Iw2DCreateImageResource("tilesGrass");
	tileImage[WaterImage]		= Iw2DCreateImageResource("tilesWater");
	tileImage[SpawnImage]		= Iw2DCreateImageResource("tilesspawn");
	tileImage[ExitImage]		= Iw2DCreateImageResource("tilesexit");
	tileImage[MonsterImage]		= Iw2DCreateImageResource("tilespurmon");
	tileImage[BuyImage]			= Iw2DCreateImageResource("tilesbuy");
	tileImage[SpeedImage]		= Iw2DCreateImageResource("tilesspeed");
	tileImage[ContWavesImage]	= Iw2DCreateImageResource("tilescontwaves");
	tileImage[IncomeImage]		= Iw2DCreateImageResource("tilesincome");
	tileImage[PauseImage]		= Iw2DCreateImageResource("tilespause");
	tileImage[UndoImage]		= Iw2DCreateImageResource("tilesundo");
	tileImage[DamageImage]		= Iw2DCreateImageResource("tilesDamage");

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
	tileImage[TowerImage] = Iw2DCreateImageResource(temp);

	sprintf(temp, "tiles%dshot", tileSize);	
	tileImage[ShotImage] = Iw2DCreateImageResource(temp);

	sprintf(temp, "tiles%dhorwall", tileSize);	
	tileImage[HorWallImage] = Iw2DCreateImageResource(temp);

	sprintf(temp, "tiles%dvertwall", tileSize);	
	tileImage[VerWallImage] = Iw2DCreateImageResource(temp);

	sprintf(temp, "tiles%ddiag14wall", tileSize);	
	tileImage[Wall14Image] = Iw2DCreateImageResource(temp);

	sprintf(temp, "tiles%ddiag23wall", tileSize);	
	tileImage[Wall23Image] = Iw2DCreateImageResource(temp);
}

int UpdateScreenSize()
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
	
	setUpImages(tileSize);

	if(tileSize < 40)
		font = Iw2DCreateFontResource("font8");
	else
		font = Iw2DCreateFontResource("font18");

	Iw2DSetFont(font);
	return tileSize;
}
