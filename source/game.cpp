#include <iostream>

#include "game.h"
#include "rendering.h"
#include "resources.h"


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

void Grid::rollMap(int &spawnX, int &spawnY, int &goalX, int &goalY)
{
	for(int x=0; x < width; x++)
		for(int y=0; y < height; y++) 
		{
			Get(x,y).SetCol(0);
		}

		spawnX = spawnY = 0;
		goalX = (GAME_WIDTH-1) * g_TileSize;
		goalY = (GAME_HEIGHT-1) * g_TileSize;
		
		Get(spawnX, spawnY).SetCol(3);
		Get(goalX, goalY).SetCol(3);

		Get(0,1).SetCol(2);
		Get(1,1).SetCol(2);
		Get(2,1).SetCol(2);
		Get(3,1).SetCol(2);
		Get(3,2).SetCol(2);

		Get(5,0).SetCol(1);
		Get(5,1).SetCol(1);
		Get(5,2).SetCol(1);
		Get(6,0).SetCol(1);
		Get(6,1).SetCol(1);
}

//
// Game class ////////////////////////////////////////////////////////////////////////
//

Game::Game() : spawnNextWave(false), currWave(0)
{
	grid.Resize(GAME_WIDTH, GAME_HEIGHT);
	grid.rollMap(spawnX, spawnY, goalX, goalY);
	
	for(int i=0; i < SIMUL_WAVES_ALLOWED; i++)
		monsters[i] = 0;
}

Game::~Game()
{
	for(int i=0; i < SIMUL_WAVES_ALLOWED; i++)
	{
		delete[] monsters[i];
	}
}

void Game::spawnWave() 
{
	int waveArrayPos = currWave % SIMUL_WAVES_ALLOWED;

	monsters[waveArrayPos] = new Monster[currWave+1];
	for(int i=0; i <= currWave; i++)
		monsters[waveArrayPos][i].initMonster(spawnX, spawnY, mobHp
		, mobMoveSpeed, currWave, i);

	waveNumber[waveArrayPos] = currWave;
}

void Game::Render()
{
	int displayWidth = Iw2DGetSurfaceWidth();
	int displayHeight = Iw2DGetSurfaceHeight();

	// Draw overall background
	DrawBG(displayWidth, displayHeight);

	// Draw playing area and active piece
	grid.Render(VERTICAL_BORDER, HORIZONTAL_BORDER);

	renderMonsters();

	// Reset screen space origin
}

void Game::renderMonsters()
{
	for(int i=0; i < SIMUL_WAVES_ALLOWED; i++)
	{
		if(monsters[i] != 0)
		{
			for(int j=0; j < sizeof(monsters[i]) / sizeof(int); j++)
				drawMonster(monsters[i][j].getX(), monsters[i][j].getY(), g_TileSize);
		}
	}
}

void Game::Reset() 
{

}

void Game::Update(int deltaTimeMs)
{
	if(!spawnNextWave) {
		spawnWave();
		spawnNextWave = !spawnNextWave;
	}
	
}
