
#include "rendering.h"
#include "Iw2D.h"
#include "IwResManager.h"

// Pointers to images used by the rendering code
CIw2DImage* tileImage[3];
CIw2DFont* font;

int g_TileSize = 32;

void CleanupImages()
{
	for(int i=0; i < 3; i++) //magic num !!
		delete tileImage[i];

    delete font;
}

// Draws a background by tiling the specified material to fill the specified area
void DrawBG(int wid, int hi)
{
	Iw2DSetColour(0xffccab66);
	Iw2DFillRect(CIwSVec2(0,0), CIwSVec2(wid, hi));
	Iw2DSetColour(0xffffffff);
}

void DrawTile(int colour, int x, int y, int size)
{
	 Iw2DDrawImage(
        tileImage[colour],
        CIwSVec2(x, y),
        CIwSVec2(g_TileSize, g_TileSize)
        );
}

void SetupImages()
{
    CleanupImages();
	
	tileImage[0] = Iw2DCreateImageResource("tiles32grass");
	tileImage[1] = Iw2DCreateImageResource("tiles32water");
	tileImage[2] = Iw2DCreateImageResource("tiles32tower");
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

