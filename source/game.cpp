#include <iostream>

#include "game.h"
#include "tower.h"

#include "Iw2D.h"
#include "s3eKeyboard.h"
#include "s3ePointer.h"

#include <time.h>

Game::Game()
{
	Reset();
}

Game::~Game() 
{
	shots.clear();

	for(std::list<Wall*>::iterator it = walls.begin(); it != walls.end(); it++)
		delete (*it);
	walls.clear();
}

void Game::Reset()
{
	spawnPoint.setPoint(0, 0);
	exitPoint.setPoint(1, 0);
	spawnNextWave = true;
	currWave = 0;
	numOfCurrWaveMons = 17;
	mobMoveSpeed = 1;
	spawnNextMobId = 0;
	Tower::setAttSpeed(g_gameSpeed);
	towerRange = LEVEL1;
	mobHp = 3;

	generateMap();
}

void Game::buildTower(int x, int y)
{
	towers.push_back(grid.buildTower(x,y));
	buildWalls(x, y);
}

//bygger dubbel wall atm i varje riktning
void Game::buildWalls(int x, int y)
{
	int topLeftX = x * g_tileSize;
	int topLeftY = y * g_tileSize;

	if(isWallSpace(x, y-1))
		walls.push_back(new Wall(VERWALL, topLeftX + 9, topLeftY - 2));

	if(isWallSpace(x, y+1))
		walls.push_back(new Wall(VERWALL, topLeftX + 9, topLeftY + 18));

	if(isWallSpace(x-1, y))
		walls.push_back(new Wall(HORWALL, topLeftX - 2, topLeftY + 9));

	if(isWallSpace(x+1, y))
		walls.push_back(new Wall(HORWALL, topLeftX + 18, topLeftY + 9));

	if(isWallSpace(x-1, y-1))
		walls.push_back(new Wall(WALL14, topLeftX - 5, topLeftY - 5));

	if(isWallSpace(x+1, y+1))
		walls.push_back(new Wall(WALL14, topLeftX + 15, topLeftY - 5));

	if(isWallSpace(x+1, y-1))
		walls.push_back(new Wall(WALL23, topLeftX + 15, topLeftY - 5));

	if(isWallSpace(x-1, y+1))
		walls.push_back(new Wall(WALL23, topLeftX - 5, topLeftY + 15));
}

bool Game::isWallSpace(int x, int y)
{
	if(x>=0 && x < GRID_COLUMNS && y >= 0 && y < GRID_ROWS)
		if(dynamic_cast<Tower*>(grid.get(x,y)))
			return true;
	return false;
}

void Game::spawnMonster() 
{
	if(spawnNextMobId < numOfCurrWaveMons)
	{
		monsters[spawnNextMobId].init(spawnPoint.getX(), spawnPoint.getY(), mobHp
			, mobMoveSpeed, currWave, spawnNextMobId);

		mobGridPos[spawnNextMobId].setPoint(spawnPoint.getX(), spawnPoint.getY());
		spawnNextMobId++;
	}
}

void Game::updateMobGrid()
{
	for(int i=0; i < numOfCurrWaveMons; i++)
	{
		if(monsters[i].getUpdateGridPos())
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
	drawBG();

	grid.render();
	renderWalls();
	renderMonsters();
	renderShots();
}

void Game::renderMonsters()
{
	for(int j=0; j < numOfCurrWaveMons; j++)
		if(monsters[j].monsterIsAlive()) 
		{
			drawTile(MONSTER, monsters[j].getTopLeft());
		}
}

void Game::generateMap()
{
	buildTower(0,1);
	buildTower(2,1);
	buildTower(1,1);
	buildTower(2,1);
	buildTower(3,1);
	buildTower(3,2);
	buildTower(0,7);
	buildTower(1,6);
	buildTower(0,6);
	buildTower(3,2);
	buildTower(2,8);
	buildTower(1,10);
	buildTower(1,11);
	buildTower(1,12);
	buildTower(1,9);

	spawnPoint.setPoint(0, 0);

	exitPoint.setPoint(0, 12);

	grid.buildSpawn(spawnPoint.getX(), spawnPoint.getY());
	grid.buildExit(exitPoint.getX(), exitPoint.getY());
	
	grid.buildWater(5, 0);
	grid.buildWater(5, 1);
	grid.buildWater(5, 2);
	grid.buildWater(5, 3);
	grid.buildWater(1, 3);
	grid.buildWater(1, 4);
	grid.buildWater(2, 4);
	grid.buildWater(3, 4);
	grid.buildWater(4, 4);
	grid.buildWater(5, 4);
	grid.buildWater(3, 5);
	grid.buildWater(3, 6);
	grid.buildWater(3, 7);
	grid.buildWater(3, 8);
}

void Game::Update(int deltaTimeMs)
{
	if(spawnNextWave) {
		spawnNextMobId = 0;
		//numOfCurrWaveMons += 3;
		findShortestPath();
		spawnNextWave = false;

		setPathGrassListeners();
	}
	spawnMonster();
	moveMobs();
	updateMobGrid();

	shoot();
	moveShots();

	checkCollisions();
}

void Game::shoot()
{
	for(std::list<Tower*>::iterator it = towers.begin(); it != towers.end(); it++)
	{
		if((*it)->armed())
		{
			int target = (*it)->aquireTarget(numOfCurrWaveMons);
			if(target < numOfCurrWaveMons)
			{
				shots.push_back(new TrackingShot((*it)->getCenter(),
					monsters[target], (*it)->getDmg()));
				(*it)->shoot();
			}
		}
		else
		{
			(*it)->reloadTick();
		}
	}
}

void Game::moveMobs()
{
	for(int i=0; i < numOfCurrWaveMons; i++)
		if(monsters[i].monsterIsAlive())
			monsters[i].move();
}

void Game::setListener(Point &pathGrass, Tower *t)
{
	Grass* g;

	if(g = dynamic_cast<Grass*>(grid.get(pathGrass)))
		g->addListener(t);
}

void Game::setPathGrassListeners()
{
	Point pathTrav(spawnPoint.getX(), spawnPoint.getY());
	unsigned int nxtInstr = 0;

	while(nxtInstr < Monster::s_mobPath.length())
	{
		switch(Monster::s_mobPath[nxtInstr])
		{
		case '1':
			pathTrav.addToX(1);
			break;
		case '2':
			pathTrav.addToY(-1);
			break;
		case '3':
			pathTrav.addToX(-1);
			break;
		case '4':
			pathTrav.addToY(1);
			break;
		}
		nxtInstr++;

		int xLim = pathTrav.getX()+1;
		int yLim = pathTrav.getY()+1;
		Tower *t;

		for(int x=pathTrav.getX()-1; x <= xLim; x++)
			for(int y=pathTrav.getY()-1; y <= yLim; y++)
				if(x>=0 && x < GRID_COLUMNS && y >= 0 && y < GRID_ROWS)//valid method make
					if(t = dynamic_cast<Tower*>(grid.get(x,y)))
						setListener(pathTrav, t);
	}
}

void Game::findShortestPath()
{
	//(g_mobPath).append("11114441111111111111111110");
}

void Game::moveShots()
{
	for(std::list<TrackingShot*>::iterator it = shots.begin(); it != shots.end(); it++)
	{
		(*it)->move();
	}
}

void Game::checkCollisions()
{
	for(std::list<TrackingShot*>::iterator it = shots.begin(); it != shots.end();)
	{
		if((*it)->colliding())
		{
			Monster &getsShot = (*it)->getTarget();

			if(getsShot.wasShot((*it)->getDmg()))
			{
				//monster died
				grid.notifyTileExit(getsShot.getGridPos(), getsShot.getMobId());
			}

			std::list<TrackingShot*>::iterator del = it;
			it++;
			delete (*del);
			shots.erase(del);
		}
		else
			it++;
	}
}

void Game::renderShots()
{
	for(std::list<TrackingShot*>::iterator it = shots.begin(); it != shots.end(); it++)
	{
		drawTile(SHOT, (*it)->getTopLeft());
	}
}

void Game::renderWalls()
{
	for(std::list<Wall*>::iterator it = walls.begin(); it != walls.end(); it++)
		drawTile((*it)->getColor(), (*it)->getTopLeft());
}
