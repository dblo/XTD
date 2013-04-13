#include <iostream>

#include "game.h"
#include "tower.h"

#include "Iw2D.h"
#include "s3eKeyboard.h"
#include "s3ePointer.h"

#include <vector>
#include <queue>
#include <time.h>
#include <time.h>
#include <stdio.h>

Game::Game()
{
	reset();
}

Game::~Game() 
{
	shots.clear();

	for(std::vector<Wall*>::iterator it = walls.begin(); it != walls.end(); it++)
		delete (*it);
	walls.clear();

	delete g_mobPath; //Deleting global in resource.h, CHANGE THIS SHIT
}

void Game::Update()
{
	if(spawnNextWave) 
	{
		spawnNextMobId = 0;

		if(numOfCurrWaveMons < NUM_MAX_MOBS)
			numOfCurrWaveMons += 3;
		else 
		{
			numOfCurrWaveMons = 7;
			mobHp +=2;
		}

		spawnNextWave = false;
		mobsAlive = numOfCurrWaveMons;

		resetPathGridVisits();
		if(!findShortestPath())
		{
			changesConfirmed = false;
		}
		else
		{
			if(!newTowers.empty() && changesConfirmed)
			{
				removePathGrassListeners();
				buildNewTowers();
				setPathGrassListeners();
			}
		}

		if(credits + income < 1000)
			credits += income;
		else
			credits = 999;

		if(currWave < 999)//Needless? not game that long
			currWave++;
		else
			currWave = 999;

		if(addIncome > 0)
		{
			if(income + addIncome < 1000)
			{
				income += addIncome;
				addIncome = 0;
			}
			else
				income = 999;
		}

		
	}

	checkDiscard();
	spawnMonster();
	moveMobs();
	updateMobGrid();

	shoot();
	moveShots();

	handleInput();

	checkCollisions();
	waveOverCheck();
}

void Game::buildNewTowers()
{
	int x, y;
	for(std::vector<Tower*>::const_iterator it = newTowers.begin(); it != newTowers.end(); it++)
	{
		x = (*it)->getCenter().getX() / g_tileSize;
		y = (*it)->getCenter().getY() / g_tileSize;

		grid.addTower(*it, x, y);
		towers.push_back(*it);
		buildWalls(x, y);
	}
	newTowers.clear();
}

void Game::reset()
{
	towers.clear();
	towers.reserve(GRID_COLUMNS*GRID_ROWS);
	addIncome = 0;
	srand (time(NULL));
	changesConfirmed = true;
	discardChanges = false;
	mobsAlive = 0;
	buildMode = true;
	credits = 990;
	income = 999;
	spawnPoint.setPoint(0, 0);
	exitPoint.setPoint(1, 0);
	spawnNextWave = false;
	currWave = 0;
	numOfCurrWaveMons = 7;
	mobMoveSpeed = 2;
	spawnNextMobId = NUM_MAX_MOBS;
	Tower::setAttSpeed(g_gameSpeed);
	towerRange = LEVEL1;
	mobHp = 1;
	spawnTimer = 5;
	takeTouch = true;
	contWave = false;
	speedMode = FAST;

	grid.buildAllGrass();
	while(true)
	{
		initPathGrid();
		generateMap();

		if(!findShortestPath())
		{
			grid.releaseTile(spawnPoint.getX(), spawnPoint.getY());
			grid.buildGrass(spawnPoint.getX(), spawnPoint.getY());
			grid.releaseTile(exitPoint.getX(), exitPoint.getY());
			grid.buildGrass(exitPoint.getX(), exitPoint.getY());
			grid.setAllGrass();
			std::cout << "MAKING NEW MAP in game::reset!\n";
		}
		else
			break;
	}
}

void Game::handleInput()
{
	if(g_Input.getTouchCount() == 0)
		takeTouch = true;

	if(takeTouch && g_Input.getTouchCount() > 0)
	{
		CTouch *touch = g_Input.getTouch(0);

		if(touch->x >= 10 && touch->x <= Iw2DGetSurfaceWidth() - 10
			&& touch->y >= 10 && touch->y < Iw2DGetSurfaceHeight() - 10)
		{
			if(touch->x < g_tileSize * (GRID_COLUMNS + 1))
			{
				if(buildMode)
					buildTower((touch->x - 10 ) / g_tileSize, (touch->y - 10) / g_tileSize);
			}
			else
			{
				if(touch->x > (GRID_COLUMNS + 1) * g_tileSize && touch->x < Iw2DGetSurfaceWidth() - 10)
				{
					if(touch->y < 10)
						;//do nothing
					else if(touch->y <40)
					{
						if(buildMode)
						{
							buildMode = false;
							changesConfirmed = true;
						}
						else
						{
							buildMode = true;

							if(!contWave)
								changesConfirmed = false;
						}
					}
					else if(touch->y < 50)
						;//do nothing
					else if(touch->y < 80)
					{
						if(speedMode == NEWWAVE)
						{
							speedMode = FAST;
						}
						else
							if(mobsAlive == 0)
								speedMode = NEWWAVE;
					}
					else if(touch->y < 90)
						;//do nothing
					else if(touch->y < 120)
					{
						if(credits >= BUY_INCOME && income + addIncome < 999)
						{
							credits -= BUY_INCOME;
							addIncome++;
							changesConfirmed = false;
						}
					}
					else if(touch->y < 240)
						;//do nothing
					else if(touch->y < 270)
					{
						discardChanges = true;
					}
					else if(touch->y < 280)
						;//do nothing
					else if(touch->y < 310)
					{
						contWave = !contWave;
					}
				}
			}
			takeTouch = false;
		}
	}
}

void Game::checkDiscard()
{
	if(discardChanges)
	{
		credits += addIncome * 5;
		addIncome = 0;

		credits += newTowers.size() * 10;
		if(credits > 999)
			credits = 999;

		int x, y;
		for(std::vector<Tower*>::const_iterator it = newTowers.begin(); it != newTowers.end(); it++)
		{
			x = (*it)->getCenter().getX() / g_tileSize;
			y = (*it)->getCenter().getY() / g_tileSize;

			addToPathGrid(x, y);
		}
		newTowers.clear();
		//		newWalls.clear();
		discardChanges = false;
		changesConfirmed = true;
	}
}

void Game::waveOverCheck()
{
	if(mobsAlive == 0)
		if(contWave)
			spawnNextWave = true;
		else if(speedMode == NEWWAVE)
		{
			spawnNextWave = true;
			speedMode = FAST;
		}
}

void Game::initPathGrid()
{
	for(int x=0; x < GRID_COLUMNS-1; x++)
		for(int y=0; y < GRID_ROWS; y++)
			pathGrid[x][y].addRight(&pathGrid[x+1][y]);

	for(int x=1; x < GRID_COLUMNS; x++)
		for(int y=0; y < GRID_ROWS; y++)
			pathGrid[x][y].addLeft(&pathGrid[x-1][y]);

	for(int x=0; x < GRID_COLUMNS; x++)
		for(int y=0; y < GRID_ROWS-1; y++)
			pathGrid[x][y].addBelow(&pathGrid[x][y+1]);

	for(int x=0; x < GRID_COLUMNS; x++)
		for(int y=1; y < GRID_ROWS; y++)
			pathGrid[x][y].addAbove(&pathGrid[x][y-1]);
	/*
	for(int x=0; x < GRID_COLUMNS; x++)
	for(int y=0; y < GRID_ROWS; y++)
	std::cout << "check: " << ((pathGrid[x][y].neighbours[0] != 0) ? 1:0) << std::endl;*/
}

//Add tower and walls to newXXX structures
void Game::buildTower(int x, int y)
{
	if(credits >= 10)
	{
		credits -= 10;
		Tile *t = grid.get(x, y);
		if(t != 0 && t->getColor() == GRASS )
		{
			newTowers.push_back(new Tower(x, y));
			removeFromPathGrid(x, y);			

			//buildWalls(x, y);
		}
	}
}

void Game::buildWater(int x, int y)
{
	if(grid.get(x, y)->getColor() == GRASS)
	{
		grid.buildWater(x, y);
		removeFromPathGrid(x, y);
	}
}

void Game::addToPathGrid(int x, int y)
{
	pvPtr vert = &pathGrid[x][y];
	if(validPoint(x-1,y) && grid.isGrassAt(x-1, y))
	{
		vert->addLeft(&pathGrid[x-1][y]);
		pathGrid[x-1][y].addRight(vert);
	}
	if(validPoint(x+1,y) && grid.isGrassAt(x+1, y))
	{
		vert->addRight(&pathGrid[x+1][y]);
		pathGrid[x+1][y].addLeft(vert);
	}
	if(validPoint(x,y+1) && grid.isGrassAt(x, y+1))
	{
		vert->addBelow(&pathGrid[x][y+1]);
		pathGrid[x][y+1].addAbove(vert);
	}
	if(validPoint(x,y-1) && grid.isGrassAt(x, y-1))
	{
		vert->addAbove(&pathGrid[x][y-1]);
		pathGrid[x][y-1].addBelow(vert);
	}
}

void Game::removeFromPathGrid(int x, int y)
{
	pvPtr vert = &pathGrid[x][y];
	if(validPoint(x-1,y) && grid.isGrassAt(x-1, y))
	{
		vert->removeLeft();
		pathGrid[x-1][y].removeRight();
	}
	if(validPoint(x+1,y) && grid.isGrassAt(x+1, y))
	{
		vert->removeRight();
		pathGrid[x+1][y].removeLeft();
	}
	if(validPoint(x,y+1) && grid.isGrassAt(x, y+1))
	{
		vert->removeBelow();
		pathGrid[x][y+1].removeAbove();
	}
	if(validPoint(x,y-1) && grid.isGrassAt(x, y-1))
	{
		vert->removeAbove();
		pathGrid[x][y-1].removeBelow();
	}
}

bool Game::validPoint(int x, int y) const
{
	return (x >= 0 && x < GRID_COLUMNS && y >= 0 && y < GRID_ROWS);
}

void Game::buildWalls(int x, int y)
{
	int topLeftX = x * g_tileSize;
	int topLeftY = y * g_tileSize;

	if(isWallSpace(x, y-1))
		walls.push_back(new Wall(VERWALL, topLeftX + 8, topLeftY - 3));

	if(isWallSpace(x, y+1))
		walls.push_back(new Wall(VERWALL, topLeftX + 8, topLeftY + 17));

	if(isWallSpace(x-1, y))
		walls.push_back(new Wall(HORWALL, topLeftX - 3, topLeftY + 8));

	if(isWallSpace(x+1, y))
		walls.push_back(new Wall(HORWALL, topLeftX + 17, topLeftY + 8));

	if(isWallSpace(x-1, y-1))
		walls.push_back(new Wall(WALL14, topLeftX - 5, topLeftY - 5));

	if(isWallSpace(x+1, y+1))
		walls.push_back(new Wall(WALL14, topLeftX + 15, topLeftY + 15));

	if(isWallSpace(x+1, y-1))
		walls.push_back(new Wall(WALL23, topLeftX + 15, topLeftY - 5));

	if(isWallSpace(x-1, y+1))
		walls.push_back(new Wall(WALL23, topLeftX - 5, topLeftY + 15));
}

bool Game::isWallSpace(int x, int y)
{
	if(x>=0 && x < GRID_COLUMNS && y >= 0 && y < GRID_ROWS)
		if((grid.get(x,y)->getColor() == TOWER))
			return true;
	return false;
}

void Game::spawnMonster() 
{
	if(spawnTimer == 0)
	{
		if(spawnNextMobId < numOfCurrWaveMons )
		{
			monsters[spawnNextMobId].init(spawnPoint.getX(), spawnPoint.getY(), mobHp
				, mobMoveSpeed, currWave, spawnNextMobId);

			mobGridPos[spawnNextMobId].setPoint(spawnPoint.getX(), spawnPoint.getY());
			spawnNextMobId++;
			spawnTimer = 5;
		}
	}
	else
		spawnTimer--;
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
	//	drawBG(); not used, only clear bg
	grid.render();
	renderWalls();
	renderNewTowers();

	renderMonsters();
	renderShots();

	renderButtons();
	renderText();

}

void Game::renderMonsters()
{
	for(int j=0; j < numOfCurrWaveMons; j++)
	{
		if(monsters[j].monsterIsAlive()) 
		{
			drawTile(MONSTER, monsters[j].getTopLeft(), g_tileSize);
		}
	}
}

void Game::backtrack(pvPtr iter, std::string &path) const
{
	while(iter->getCameFrom() != UNDEF) 
	{
		//std::cout << "camefrom: " << iter->getCameFrom() << "\n";

		switch (iter->getCameFrom())
		{
		case RIGHT:
			path += 'l';
			break;
		case LEFT:
			path += 'r';
			break;
		case DOWN:
			path += 'u';
			break;
		case UP:
			path += 'd';
			break;
		default:
			std::cout << "ERROR: Game::backtrack()\n";
			break;
		}
		iter = iter->getNExtToBacktrack();
	}
}

void Game::resetPathGridVisits()
{
	for(int i=0; i < GRID_COLUMNS; i++)
		for(int j=0; j < GRID_ROWS; j++)
			pathGrid[i][j].setUnvisited();
}

bool Game::findShortestPath()
{
	std::string shortestPath = "";
	std::queue<pvPtr> pq;
	bool foundPath = false;

	pvPtr exitPtr = &pathGrid[exitPoint.getX()][exitPoint.getY()];
	pvPtr spawnPtr = &pathGrid[spawnPoint.getX()][spawnPoint.getY()];

	pq.push(spawnPtr);

	while(pq.empty() == false)
	{
		PathingVertex *p = pq.front();

		if(p == exitPtr)
		{
			foundPath = true;
			break;
		}
		pq.pop();
		p->setVisited();
		p->relaxNode(pq);

		/*	for(int i=0; i< GRID_ROWS;i++) 
		{
		for(int j=0; j<GRID_COLUMNS; j++)
		{

		if(&pathGrid[j][i] == spawnPtr)
		std::cout << "S";
		else if(&pathGrid[j][i] == exitPtr)
		std::cout << "E";
		else
		std::cout << pathGrid[j][i].wasVisited();
		}
		std::cout << std::endl;
		}
		std::cout << "====================================================\n\n";*/
	}

	if(foundPath == true)
	{
		backtrack(exitPtr, shortestPath);
		delete g_mobPath;
		g_mobPath = new std::string(shortestPath.rbegin(), shortestPath.rend());
		return true;
	}
	/*std::cout << "local PATH IS:"; 
	std::cout << shortestPath << "!\n";*/

	//	IwAssertMsg(APP, shortestPath != "", ("Shortest path not found! In Game::findShortestPath()\n\n"));

	//std::cout << "Global path is-" << *g_mobPath << "-\tGame::findClosestPath()\n";
	return false;
}

void Game::generateMap()
{
	int x = rand() % GRID_COLUMNS;
	int y = rand() % GRID_ROWS;
	spawnPoint.setPoint(x, y);
	//spawnPoint.setPoint(0,0);

	do {
	x = rand() % GRID_COLUMNS;
	y = rand() % GRID_ROWS;
	} while(x == spawnPoint.getX() && y == spawnPoint.getY());
	exitPoint.setPoint(x, y);
	//exitPoint.setPoint(19,0);

	grid.buildSpawn(spawnPoint.getX(), spawnPoint.getY());
	grid.buildExit(exitPoint.getX(), exitPoint.getY());

	for(int i=rand() % 15; i > 0; i--)
		buildWater(rand() % GRID_COLUMNS, rand() % GRID_ROWS);
	/*
	for(int i=0; i < 20; i++)
		for(int j=1; j<15; j++)
			buildTower(i,j);*/
}

void Game::shoot()
{
	for(std::vector<Tower*>::const_iterator it = towers.begin(); it != towers.end(); it++)
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
	{
		//std::cout << "IN :" << i << "\t";
		if(monsters[i].monsterIsAlive())
			monsters[i].move();
		else if(monsters[i].despawned())
			mobsAlive--;
		//std::cout << "OUT :" << i << std::endl;
	}
}

void Game::setListener(Point &pathGrass, Tower *t)
{
	Grass* g;

	if(g = dynamic_cast<Grass*>(grid.get(pathGrass)))
	{
		g->addListener(t);
	}
}

void Game::removeListener(Point &pathGrass)
{
	Grass* g;

	if(g = dynamic_cast<Grass*>(grid.get(pathGrass)))
	{
		g->clearListeners();
	}
}

void Game::setPathGrassListeners()
{
	Point pathTrav(spawnPoint.getX(), spawnPoint.getY());
	unsigned int nxtInstr = 0;
	Tower *newListener;

	while(nxtInstr < (*g_mobPath).length())
	{
		switch((*g_mobPath)[nxtInstr])
		{
		case 'r':
			pathTrav.addToX(1);
			break;
		case 'u':
			pathTrav.addToY(-1);
			break;
		case 'l':
			pathTrav.addToX(-1);
			break;
		case 'd':
			pathTrav.addToY(1);
			break;
		}
		nxtInstr++;

		int xLim = pathTrav.getX()+1;
		int yLim = pathTrav.getY()+1;

		for(int x=pathTrav.getX()-1; x <= xLim; x++)
			for(int y=pathTrav.getY()-1; y <= yLim; y++)
			{
				if(x>=0 && x < GRID_COLUMNS && y >= 0 && y < GRID_ROWS)//valid method make
				{
					if(newListener = dynamic_cast<Tower*>(grid.get(x,y)))
					{
						setListener(pathTrav, newListener);
					}
				}
			}
	}
}

void Game::removePathGrassListeners()
{
	Point pathTrav(spawnPoint.getX(), spawnPoint.getY());
	unsigned int nxtInstr = 0;

	while(nxtInstr < (*g_mobPath).length())
	{
		switch((*g_mobPath)[nxtInstr])
		{
		case 'r':
			pathTrav.addToX(1);
			break;
		case 'u':
			pathTrav.addToY(-1);
			break;
		case 'l':
			pathTrav.addToX(-1);
			break;
		case 'd':
			pathTrav.addToY(1);
			break;
		}
		nxtInstr++;

		removeListener(pathTrav);
	}	
}

void Game::moveShots()
{
	for(std::list<TrackingShot*>::const_iterator it = shots.begin(); it != shots.end(); it++)
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

			if(getsShot.isAlive() && getsShot.wasShot((*it)->getDmg()))
			{
				//monster died
				mobsAlive--;

				if(credits < 999)
					credits++;
				
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
	for(std::list<TrackingShot*>::const_iterator it = shots.begin(); it != shots.end(); it++)
	{
		drawTile(SHOT, (*it)->getTopLeft(), (*it)->getRadius()*4); //MAGIC NUMBER
	}
}

void Game::renderWalls()
{
	for(std::vector<Wall*>::const_iterator it = walls.begin(); it != walls.end(); it++)
		drawTile((*it)->getColor(), (*it)->getTopLeft());
}

void Game::renderNewTowers()
{
	Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
	Iw2DSetColour(0xFF0C5907);

	for(std::vector<Tower*>::const_iterator it = newTowers.begin(); it != newTowers.end(); it++)
		drawTile((*it)->getColor(), (*it)->getCenter().getX() - g_tileSize/2, 
		(*it)->getCenter().getY() - g_tileSize / 2);

	//no longer drawing walls during shadow mode
	/*for(std::vector<Wall*>::const_iterator it = newWalls.begin(); it != newWalls.end(); it++)
	drawTile((*it)->getColor(), (*it)->getTopLeft());*/

	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
	Iw2DSetColour(0xffffffff);
}

void Game::renderButtons() const
{
	if(buildMode)
	{
		Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
		Iw2DSetColour(0xFF0C5907);
		drawTile(BUYTOWER, 410, 0);
		Iw2DSetColour(0xffffffff);

		Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
	}
	else
		drawTile(BUYTOWER, 410, 0);

	if(speedMode == FAST && mobsAlive == 0)
	{
		Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
		Iw2DSetColour(0xFF0C5907);
		drawTile(SPEED, 410, 40); 
		Iw2DSetColour(0xffffffff);

		Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
	}
	else
		drawTile(SPEED, 410, 40); 

	if(addIncome > 0)
	{
		Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
		Iw2DSetColour(0xFF0C5907);
		drawTile(INCOME, 410, 80);
		Iw2DSetColour(0xffffffff);

		Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
	}
	else
		drawTile(INCOME, 410, 80);


	if(newTowers.size() > 0 && !discardChanges)
	{
		Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
		Iw2DSetColour(0xFF0C5907);
		drawTile(PAUSE, 410, 230);
		Iw2DSetColour(0xffffffff);

		Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
	}
	else
		drawTile(PAUSE, 410, 230);

	if(contWave)
	{
		Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
		Iw2DSetColour(0xFF0C5907);
		drawTile(CONTWAVES, 410, 270);
		Iw2DSetColour(0xffffffff);

		Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
	}
	else
		drawTile(CONTWAVES, 410, 270);
}

void Game::renderText()
{
	char str[6];;

	Iw2DSetColour(0xFF0C5907);

	if(credits > 99)
		sprintf(str, "c %d", credits);
	else if(credits > 9)
		sprintf(str, "c 0%d", credits);
	else
		sprintf(str, "c 00%d", credits);

	Iw2DDrawString(str, CIwSVec2(426, 125), CIwSVec2(50, 10), IW_2D_FONT_ALIGN_LEFT, IW_2D_FONT_ALIGN_TOP);

	if(income > 99)
		sprintf(str, "i %d", income);
	else if(income > 9)
		sprintf(str, "i 0%d", income);
	else
		sprintf(str, "i 00%d", income);
	Iw2DDrawString(str, CIwSVec2(430, 140), CIwSVec2(50, 10), IW_2D_FONT_ALIGN_LEFT, IW_2D_FONT_ALIGN_TOP);

	if(currWave > 99)
		sprintf(str, "w %d", currWave);
	else if(currWave > 9)
		sprintf(str, "w 0%d", currWave);
	else
		sprintf(str, "w 00%d", currWave);
	Iw2DDrawString(str, CIwSVec2(424, 155), CIwSVec2(50, 10), IW_2D_FONT_ALIGN_LEFT, IW_2D_FONT_ALIGN_TOP);

	Iw2DSetColour(0xffffffff);
}
