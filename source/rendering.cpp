
#include "rendering.h"
#include "Iw2D.h"
#include "IwResManager.h"

// Pointers to images used by the rendering code
CIw2DImage* towerImage;
CIw2DImage* freeImage;
CIw2DImage* blockedImage;
CIw2DFont* font;

int g_TileSize = 0;

void CleanupImages()
{
	delete towerImage;
	delete freeImage;
	delete blockedImage;

    delete font;
}

// Draws a background by tiling the specified material to fill the specified area
void DrawBG(int wid, int hi)
{
	Iw2DSetColour(0);
	Iw2DFillRect(CIwSVec2(0,0), CIwSVec2(wid, hi));
	Iw2DSetColour(0xffffffff);
}

void SetupImages(int tileSize)
{
    CleanupImages();
	
	freeImage = Iw2DCreateImageResource("tiles32#");
	towerImage = Iw2DCreateImageResource("tiles32#tower");
	blockedImage = Iw2DCreateImageResource("tiles32#blocked");
}

void UpdateScreenSize()
{
	// Look up materials by name
	SetupImages(g_TileSize);

	// Choose font 

	font = Iw2DCreateFontResource("font");

	//Set this font as the current one
	Iw2DSetFont(font);

}

