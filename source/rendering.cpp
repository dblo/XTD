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

void drawBG(int cols)
{
	Iw2DSetColour(0xef40C020);
	Iw2DFillRect(CIwSVec2(0, 0), 
		CIwSVec2(cols, Iw2DGetSurfaceHeight()));

	Iw2DSetColour(0xffffffff);
	Iw2DSetAlphaMode(IW_2D_ALPHA_HALF);

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

void setUpGrapicRes(int tileSize)
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
	tileImage[BuyDamageImage]	= Iw2DCreateImageResource("tilesDamage");
	tileImage[BuySpeedImage]	= Iw2DCreateImageResource("tilesBuySpeed");
	tileImage[BuyRangeImage]	= Iw2DCreateImageResource("tilesBuyRange");
	tileImage[NormalSpeedImage]	= Iw2DCreateImageResource("tilesNormalSpeed");
	tileImage[FastSpeedImage]	= Iw2DCreateImageResource("tilesFastSpeed");

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

	if(tileSize < 40)
		font = Iw2DCreateFontResource("font8");
	else
		font = Iw2DCreateFontResource("font18");

	Iw2DSetFont(font);
}
