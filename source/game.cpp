#include <iostream>

#include "game.h"
#include "rendering.h"


#include "Iw2D.h"
#include "s3eKeyboard.h"
#include "s3ePointer.h"

#include <time.h>

//
// Grid class ////////////////////////////////////////////////////////////////////////
//

Tile & Grid::Get(int x, int y)
{
	//IwAssertMsg(APP, Valid(x,y), ("Coordinate out of range for Grid (%d,%d)", x, y));
	return tiles[x + y*width];
}

const Tile & Grid::Get(int x, int y) const
{
	//IwAssertMsg(APP, Valid(x,y), ("Coordinate out of range for Grid (%d,%d)", x, y));
	return tiles[x + y*width];
}

void Grid::Render(int rx, int ry) const
{
	for (int x=0; x<width; x++)
	{
		for (int y=0; y<height; y++)
		{
			DrawTile(
				Get(x,y).color,
				x*g_TileSize + rx,
				y*g_TileSize + ry,
				g_TileSize
				);
		}
	}
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

void Grid::rollMap()
{
	for(int x=0; x < width; x++)
		for(int y=0; y < height; y++) 
		{
			Get(x,y).SetCol((x+y)%3);
		}
		
}

//
// Game class ////////////////////////////////////////////////////////////////////////
//

Game::Game()
{
	grid.Resize(GAME_WIDTH, GAME_HEIGHT);
	grid.rollMap();
}

void Game::Render()
{
	int displayWidth = Iw2DGetSurfaceWidth();
	int displayHeight = Iw2DGetSurfaceHeight();

	// Draw overall background
	DrawBG(displayWidth, displayHeight);

	// Draw playing area and active piece
	grid.Render(0, 20);

	// Reset screen space origin
}

void Game::Reset() 
{

}

void Game::Update(int deltaTimeMs)
{

}
