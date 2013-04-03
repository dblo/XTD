

#include "game.h"
#include "rendering.h"


#include "Iw2D.h"
#include "s3eKeyboard.h"
#include "s3ePointer.h"

#include <time.h>

//
// Grid class ////////////////////////////////////////////////////////////////////////
//

const Tile & Grid::Get(int x, int y) const
{
    //IwAssertMsg(APP, Valid(x,y), ("Coordinate out of range for Grid (%d,%d)", x, y));
    return tiles[x + y*width];
}


void Grid::Resize(int newWidth, int newHeight)
{
    if (width!=newWidth || height!=newHeight)
    {
        width = newWidth;
        height = newHeight;
        delete [] tiles;
        tiles = new Tile[width*height];
    }
}

//
// Game class ////////////////////////////////////////////////////////////////////////
//

Game::Game()
{
	grid.Resize(GAME_WIDTH, GAME_HEIGHT);
}

void Game::Render()
{
    int displayWidth = Iw2DGetSurfaceWidth();
    int displayHeight = Iw2DGetSurfaceHeight();

	// Draw overall background
    DrawBG(displayWidth, displayHeight);
  
    // Draw playing area and active piece
    //grid.Render(0, 0);
    		
    // Reset screen space origin
}

void Game::Update(int deltaTimeMs)
{

}
