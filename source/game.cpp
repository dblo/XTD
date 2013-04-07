#include <iostream>

#include "game.h"

#include "Iw2D.h"
#include "s3eKeyboard.h"
#include "s3ePointer.h"

#include <time.h>


Game::Game() : spawnNextWave(false), currWave(0), numOfCurrWaveMons(7), 
	mobMoveSpeed(1), spawnNextMobId(0)
{
	grid.rollMap(spawnX, spawnY, goalX, goalY);

}

Game::~Game()
{
}

void Game::buildTower(int x, int y)
{
	grid.releaseTile(x, y);
	grid.buildTower(x,y);
}

void Game::spawnMonster() 
{
	if(spawnNextMobId < numOfCurrWaveMons)
	{
		monsters[spawnNextMobId].initMonster(spawnX, spawnY, mobHp
			, mobMoveSpeed, currWave, spawnNextMobId);
		
		mobGridPos[spawnNextMobId].setX(spawnX);
		mobGridPos[spawnNextMobId].setY(spawnY);
		spawnNextMobId++;
	}
}

void Game::updateMobGrid()
{
	for(int i=0; i < numOfCurrWaveMons; i++)
	{
		if(monsters[i].updateGridPos)
		{
			grid.notifyTileExit(mobGridPos[i], i);
			mobGridPos[i] = monsters[i].getGridPos();
			grid.notifyTileEnter(mobGridPos[i], i);
			monsters[i].gridPosUpdated();
		}
	}
}

void Game::Render()
{
	int displayWidth = Iw2DGetSurfaceWidth() / g_TileSize;
	int displayHeight = Iw2DGetSurfaceHeight() / g_TileSize;

	// Draw overall background
	DrawBG(displayWidth, displayHeight);

	// Draw playing area and active piece
	grid.Render(VERTICAL_BORDER, HORIZONTAL_BORDER);

	renderMonsters();

	// Reset screen space origin
}

void Game::renderMonsters()
{
	for(int j=0; j < NUM_MAX_MOBS; j++)
		if(monsters[j].monsterIsAlive()) 
		{
			drawMonster(monsters[j].getX(), monsters[j].getY(), g_TileSize);
		}
}

void Game::Reset() 
{

}

void Game::Update(int deltaTimeMs)
{
	if(!spawnNextWave) {
		spawnNextMobId = 0;
		numOfCurrWaveMons += 3;
		findShortestPath();
		spawnNextWave = !spawnNextWave;
	}
	spawnMonster();
	moveMobs();
}

void Game::moveMobs()
{
	for(int i=0; i < NUM_MAX_MOBS; i++)
		if(monsters[i].monsterIsAlive())
			monsters[i].move();
}

void Game::findShortestPath()
{
	//(g_mobPath).append("11114441111111111111111110");

}
