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
//=============================================================================
Game::Game()
{
	reset();
}
//=============================================================================
Game::~Game() 
{
	shots.clear();

	for(std::vector<Wall*>::iterator it = walls.begin(); it != walls.end(); it++)
		delete (*it);
	walls.clear();

	for(std::list<TrackingShot*>::iterator it = shots.begin(); it != shots.end(); it++)
		delete (*it);
	shots.clear();

	for(std::vector<Tower*>::iterator it = newTowers.begin(); it != newTowers.end(); it++)
		delete (*it);
	newTowers.clear();

	for(int i=0; i < NUM_MAX_MOBS; i++)
		delete monsters[i];
}
//=============================================================================
void Game::Update()
{
	if(discardChanges)
		discard();

	if(spawnNextWave) 
		onNewWave();

	spawnMonster();
	moveMobs();
	updateMobGrid();
	shoot();
	moveShots();
	handleInput();
	manageCollisions();
	waveOverCheck();
}
//=============================================================================
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
	credits = 500;
	income = 100;
	spawnNextWave = false;
	numOfCurrWaveMons = 7;
	mobMoveSpeed = g_tileSize / 10;
	shotMoveSpeed = mobMoveSpeed + g_tileSize / 20;
	spawnNextMobId = NUM_MAX_MOBS;
	Tower::setAttSpeed(GAMESPEED*2);
	currWave = 0;
	//towerRange = LEVEL1;
	mobGridPos.reserve(NUM_MAX_MOBS);
	mobHp = 1;
	spawnTimer = 5;
	takeTouch = true;
	contWaves = false;
	speedMode = FAST;

	for(int i=0; i < NUM_MAX_MOBS; i++)
		monsters[i] = new Monster();

	tileGrid.buildAllGrass();
	pathGrid.init();

	while(!generateMap())
		pathGrid.init();

	setButtonSize();
}
//=============================================================================
void Game::onNewWave()
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

	if(changesConfirmed)
	{
		if(!newTowers.empty())
		{
			pathGrid.reset();
			if(!findShortestPath())
			{
				changesConfirmed = false;
			}
			else
			{
				tileGrid.removePathGrassListeners(spawnX, spawnY);
				lockNewTowers();
				tileGrid.setPathGrassListeners(spawnX, spawnY);
			}
		}

		//upgarde?	
	}
	updateStats();
}
//=============================================================================
void Game::updateStats()
{
	if(credits + income < 1000)
		credits += income;
	else
		credits = 999;

	if(currWave < 999)
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
//=============================================================================
void Game::lockNewTowers()
{
	int x, y;
	for(std::vector<Tower*>::const_iterator it = newTowers.begin(); it != newTowers.end(); it++)
	{
		x = (*it)->getCenterX() / g_tileSize;
		y = (*it)->getCenterY() / g_tileSize;

		tileGrid.addTower(*it, x, y);
		towers.push_back(*it);
		buildWalls(x, y);
	}
	newTowers.clear();
}
//==============================================================================
void Game::handleInput()
{
	int buttonHi = (g_tileSize*3)/2;
	int verticalSpace = buttonHi + g_horizontalBar;

	if(g_Input.getTouchCount() == 0)
		takeTouch = true;

	if(takeTouch && g_Input.getTouchCount() > 0)
	{
		CTouch *touch = g_Input.getTouch(0);

		if(touch->x >= g_verticalBar && touch->x <= Iw2DGetSurfaceWidth() - g_verticalBar
			&& touch->y >= g_horizontalBar && touch->y < Iw2DGetSurfaceHeight() - g_horizontalBar)
		{
			if(touch->x < g_tileSize * GRID_COLUMNS + g_verticalBar)
			{
				if(buildMode)
					buildTower((touch->x - 10 ) / g_tileSize, (touch->y - 10) / g_tileSize);
			}
			else
			{
				if(touch->x >= GRID_COLUMNS * g_tileSize + 2*g_verticalBar && 
					touch->x <= Iw2DGetSurfaceWidth() - g_verticalBar)
				{
					if(touch->y < buttonY[BUYTOWERBUTTONBOT] + g_horizontalBar) //Tower button
					{
						if(buildMode)
						{
							buildMode = false;
							changesConfirmed = true;
						}
						else
						{
							buildMode = true;
							changesConfirmed = false;
						}
					}
					else if(touch->y < buttonY[SPEEDBUTTONBOT]  + g_horizontalBar 
						&& touch->y >= buttonY[SPEEDBUTTON] + g_horizontalBar) //Speed button
					{
						if(speedMode == NEWWAVE)
						{
							speedMode = FAST;
						}
						else
							if(mobsAlive == 0)
								speedMode = NEWWAVE;
					}
					else if(touch->y < buttonY[INCOMEBUTTONBOT] + g_horizontalBar
						&& touch->y >= buttonY[INCOMEBUTTON] + g_horizontalBar) //Income button
					{
						if(credits >= BUY_INCOME && income + addIncome < 999)
						{
							credits -= BUY_INCOME;
							addIncome++;
							changesConfirmed = false;
						}
					}
					else if(touch->y < buttonY[PAUSEBUTTONBOT] + g_horizontalBar
						&& touch->y >= buttonY[PAUSEBUTTON] + g_horizontalBar) //Pause button
					{
						discardChanges = true;
					}
					else if(touch->y < buttonY[CONTWAVESBUTTONBOT] + g_horizontalBar
						&& touch->y >= buttonY[CONTWAVESBUTTON] + g_horizontalBar) //Continiouos button
					{
						contWaves = !contWaves;
					}
				}
			}
			takeTouch = false;
		}
	}
}
//==============================================================================
void Game::discard()
{
	credits += addIncome * BUY_INCOME;
	addIncome = 0;

	credits += newTowers.size() * 10;
	if(credits > 999)
		credits = 999;

	int x, y;
	for(std::vector<Tower*>::const_iterator it = newTowers.begin(); it != newTowers.end(); it++)
	{
		x = (*it)->getCenterX() / g_tileSize;
		y = (*it)->getCenterY() / g_tileSize;

		pathGrid.add(x, y, tileGrid);
	}
	newTowers.clear();
	discardChanges = false;
}
//==============================================================================
void Game::waveOverCheck()
{
	if(mobsAlive == 0)
		if(contWaves)
			spawnNextWave = true;
		else if(speedMode == NEWWAVE)
		{
			spawnNextWave = true;
			speedMode = FAST;
		}
}
//==============================================================================
void Game::buildTower(int x, int y)
{
	if(credits >= 10)
	{
		credits -= 10;
		Tile *t = tileGrid.get(x, y);
		if(t != 0 && t->getColor() == GRASS )
		{
			newTowers.push_back(new Tower(x, y));
			pathGrid.remove(x, y, tileGrid);			
		}
		if(!contWaves)
			changesConfirmed = false;
	}
}
//==============================================================================
void Game::buildWater(int x, int y)
{
	if(tileGrid.get(x, y)->getColor() == GRASS)
	{
		tileGrid.buildWater(x, y);
		pathGrid.remove(x, y, tileGrid);
	}
}
//==============================================================================
void Game::buildWalls(int x, int y)
{
	int topLeftX = x * g_tileSize;
	int topLeftY = y * g_tileSize;

	if(tileGrid.isTower(x, y-1))
		walls.push_back(new Wall(VERWALL, topLeftX + 8, topLeftY - 3));

	if(tileGrid.isTower(x, y+1))
		walls.push_back(new Wall(VERWALL, topLeftX + 8, topLeftY + 17));

	if(tileGrid.isTower(x-1, y))
		walls.push_back(new Wall(HORWALL, topLeftX - 3, topLeftY + 8));

	if(tileGrid.isTower(x+1, y))
		walls.push_back(new Wall(HORWALL, topLeftX + 17, topLeftY + 8));

	if(tileGrid.isTower(x-1, y-1))
		walls.push_back(new Wall(WALL14, topLeftX - 5, topLeftY - 5));

	if(tileGrid.isTower(x+1, y+1))
		walls.push_back(new Wall(WALL14, topLeftX + 15, topLeftY + 15));

	if(tileGrid.isTower(x+1, y-1))
		walls.push_back(new Wall(WALL23, topLeftX + 15, topLeftY - 5));

	if(tileGrid.isTower(x-1, y+1))
		walls.push_back(new Wall(WALL23, topLeftX - 5, topLeftY + 15));
}
//==============================================================================
void Game::spawnMonster() 
{
	if(spawnTimer == 0)
	{
		if(spawnNextMobId < numOfCurrWaveMons )
		{
			monsters[spawnNextMobId]->init(spawnX, spawnY, mobHp,
				mobMoveSpeed, spawnNextMobId);

			mobGridPos[spawnNextMobId].first = spawnX;
			mobGridPos[spawnNextMobId].second = spawnY;
			spawnNextMobId++;
			spawnTimer = 10;
		}
	}
	else
		spawnTimer--;
}
//==============================================================================
void Game::updateMobGrid()
{
	for(int i=0; i < numOfCurrWaveMons; i++)
	{
		if(monsters[i]->getUpdateGridPos())
		{
			tileGrid.notifyTileExit(mobGridPos[i].first, mobGridPos[i].second, i);
			mobGridPos[i].first = monsters[i]->getGridPosX();
			mobGridPos[i].second = monsters[i]->getGridPosY();
			tileGrid.notifyTileEnter(mobGridPos[i].first, mobGridPos[i].second, i);
			monsters[i]->gridPosUpdated();
		}
	}
}
//==============================================================================
void Game::Render()
{
	//	drawBG(); not used, only clear bg
	tileGrid.render();
	renderWalls();
	renderNewTowers();

	renderMonsters();
	renderShots();

	renderButtons();
	renderText();

}
//==============================================================================
void Game::backtrack(pvPtr iter, std::string &path) const
{
	while(iter->getCameFrom() != UNDEF) 
	{
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
		iter = iter->getNextToBacktrack();
	}
}
//=============================================================================
/* Attempts to find a shortest path from spawn to every node until exit has 
been found. If path to exit was found, calls backtrack to create a string 
containing the path from exit to spawn and then reverse it. If no path was 
found, the previous path will not be updated. */
//=============================================================================
bool Game::findShortestPath()
{
	std::string shortestPath = "";
	std::queue<pvPtr> pq;
	bool foundPath = false;
	pvPtr spawnPtr = pathGrid.at(spawnX, spawnY);
	pvPtr exitPtr = pathGrid.at(exitX, exitY);

	pq.push(spawnPtr);

	while(!pq.empty())
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

		//pathGrid.print(spawnPtr, exitPtr);
	}

	if(foundPath == true)
	{
		backtrack(exitPtr, shortestPath);
		delete g_mobPath;
		g_mobPath = new std::string(shortestPath.rbegin(), shortestPath.rend());
		return true;
	}
	return false;
}
//=============================================================================
bool Game::generateMap()
{
	int x = rand() % GRID_COLUMNS;
	int y = rand() % GRID_ROWS;
	spawnX = x;
	spawnY = y;

	do {
		x = rand() % GRID_COLUMNS;
		y = rand() % GRID_ROWS;
	} while(x == spawnX || y == spawnY);

	exitX = x;
	exitY = y;

	tileGrid.buildSpawn(spawnX, spawnY);
	tileGrid.buildExit(exitX, exitY);

	for(int i=(rand() % 10) + 3; i > 0; i--)
		buildWater(rand() % GRID_COLUMNS, rand() % GRID_ROWS);

	if(!findShortestPath())
	{
		tileGrid.releaseTile(spawnX, spawnY);
		tileGrid.buildGrass(spawnX, spawnY);
		tileGrid.releaseTile(exitX, exitY);
		tileGrid.buildGrass(exitX, exitY);
		tileGrid.setAllGrass();
		std::cout << "Remaking map in game::generateMap()\n";
		return false;
	}
	return true;
	/*for(int i=0; i < 20; i++)
	for(int j=1; j<15; j++)
	buildTower(i,j);*/
}
//==============================================================================
void Game::shoot()
{
	for(std::vector<Tower*>::const_iterator it = towers.begin(); it != towers.end(); it++)
	{
		if((*it)->armed())
		{
			int target = (*it)->aquireTarget(numOfCurrWaveMons);
			if(target < numOfCurrWaveMons)
			{
				shots.push_back(new TrackingShot((*it)->getCenterX(),
					(*it)->getCenterY(),
					monsters[target], (*it)->getDmg(), shotMoveSpeed));
				(*it)->initiateReload();
			}
		}
		else
		{
			(*it)->reloadTick();
		}
	}
}
//==============================================================================
void Game::moveMobs()
{
	for(int i=0; i < numOfCurrWaveMons; i++)
	{
		if(monsters[i]->monsterIsAlive())
			monsters[i]->move();
		else if(monsters[i]->despawned())
			mobsAlive--;
	}
}
//==============================================================================
void Game::moveShots()
{
	for(std::list<TrackingShot*>::const_iterator it = shots.begin(); it != shots.end(); it++)
	{
		(*it)->move();
	}
}
//==============================================================================
void Game::manageCollisions()
{
	std::list<TrackingShot*>::iterator del ;
	for(std::list<TrackingShot*>::iterator it = shots.begin(); it != shots.end();)
	{
		if((*it)->colliding())
		{
			Monster *getsShot = (*it)->getTarget();

			if(getsShot->isAlive() && getsShot->wasShot((*it)->getDmg()))
			{
				mobsAlive--;

				if(credits < 999)
					credits++;

				tileGrid.notifyTileExit(getsShot->getGridPosX(), getsShot->getGridPosY(), getsShot->getMobId());
			}

			del = it;
			it++;
			delete (*del);
			shots.erase(del);
		}
		else
			it++;
	}
}
//==============================================================================
void Game::renderShots()
{
	for(std::list<TrackingShot*>::const_iterator it = shots.begin(); 
		it != shots.end(); it++)
	{
		drawTile(SHOT, (*it)->getTopLeftX(), (*it)->getTopLeftY(), 
			(*it)->getRadius()*2, (*it)->getRadius()*2);		//opti?
	}
}
//==============================================================================
void Game::renderWalls()
{
	for(std::vector<Wall*>::const_iterator it = walls.begin(); it != walls.end(); it++)
		drawTile((*it)->getColor(), (*it)->getTopLeftX(), (*it)->getTopLeftY());
}
//==============================================================================
void Game::renderNewTowers()
{
	Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
	Iw2DSetColour(0xFF0C5907);

	for(std::vector<Tower*>::const_iterator it = newTowers.begin(); it != newTowers.end(); it++)
		drawTile((*it)->getColor(), (*it)->getTopLeftX(), (*it)->getTopLeftY(),
		g_tileSize, g_tileSize);

	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
	Iw2DSetColour(0xffffffff);
}
//==============================================================================
void Game::renderMonsters()
{
	for(int j=0; j < numOfCurrWaveMons; j++)
	{
		if(monsters[j]->monsterIsAlive()) 
		{
			drawTile(MONSTER, monsters[j]->getTopLeftX(), 
				monsters[j]->getTopLeftY(), g_tileSize, g_tileSize);
		}
	}
}
//=============================================================================
void Game::renderButtons() const
{
	if(buildMode)
	{
		Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
		Iw2DSetColour(0xFF0C5907);
		drawTile(BUYTOWER, buttonX, buttonY[BUYTOWERBUTTON], buttonWid, buttonHi);
		Iw2DSetColour(0xffffffff);

		Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
	}
	else
		drawTile(BUYTOWER, buttonX, buttonY[BUYTOWERBUTTON], buttonWid, buttonHi);

	if(speedMode == FAST && mobsAlive == 0)
	{
		Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
		Iw2DSetColour(0xFF0C5907);
		drawTile(SPEED, buttonX, buttonY[SPEEDBUTTON], buttonWid, buttonHi);
		Iw2DSetColour(0xffffffff);

		Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
	}
	else
		drawTile(SPEED, buttonX, buttonY[SPEEDBUTTON], buttonWid, buttonHi); 

	if(addIncome > 0)
	{
		Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
		Iw2DSetColour(0xFF0C5907);
		drawTile(INCOME, buttonX, buttonY[INCOMEBUTTON], buttonWid, buttonHi);
		Iw2DSetColour(0xffffffff);

		Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
	}
	else
		drawTile(INCOME, buttonX, buttonY[INCOMEBUTTON], buttonWid, buttonHi);

	if(newTowers.size() > 0 && !discardChanges)
	{
		Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
		Iw2DSetColour(0xFF0C5907);
		drawTile(PAUSE, buttonX, buttonY[PAUSEBUTTON], buttonWid, buttonHi);
		Iw2DSetColour(0xffffffff);
		Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
	}
	else
		drawTile(PAUSE, buttonX, buttonY[PAUSEBUTTON], buttonWid, buttonHi);

	if(contWaves)
	{
		Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
		Iw2DSetColour(0xFF0C5907);
		drawTile(CONTWAVES, buttonX, buttonY[CONTWAVESBUTTON], buttonWid, buttonHi);
		Iw2DSetColour(0xffffffff);
		Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
	}
	else
		drawTile(CONTWAVES, buttonX, buttonY[CONTWAVESBUTTON], buttonWid, buttonHi);
}
//==============================================================================
void Game::renderText()
{
	char str[6];

	Iw2DSetColour(0xFF0C5907);

	if(credits > 99)
		sprintf(str, "c %d", credits);
	else if(credits > 9)
		sprintf(str, "c 0%d", credits);
	else
		sprintf(str, "c 00%d", credits);

	Iw2DDrawString(str, CIwSVec2(textX, textY[CREDITSTEXT]), CIwSVec2(textWid, textHi), 
		IW_2D_FONT_ALIGN_LEFT, IW_2D_FONT_ALIGN_CENTRE);

	if(income > 99)
		sprintf(str, "i %d", income);
	else if(income > 9)
		sprintf(str, "i 0%d", income);
	else
		sprintf(str, "i 00%d", income);
	Iw2DDrawString(str, CIwSVec2(textX, textY[INCOMETEXT]), CIwSVec2(textWid, textHi), 
		IW_2D_FONT_ALIGN_LEFT, IW_2D_FONT_ALIGN_CENTRE);

	if(currWave > 99)
		sprintf(str, "w %d", currWave);
	else if(currWave > 9)
		sprintf(str, "w 0%d", currWave);
	else
		sprintf(str, "w 00%d", currWave);
	Iw2DDrawString(str, CIwSVec2(textX, textY[WAVETEXT]), CIwSVec2(textWid, textHi), 
		IW_2D_FONT_ALIGN_LEFT, IW_2D_FONT_ALIGN_CENTRE);

	Iw2DSetColour(0xffffffff);
}
//==============================================================================
void Game::setButtonSize()
{
	buttonX = GRID_COLUMNS * g_tileSize + g_verticalBar;
	buttonWid = Iw2DGetSurfaceWidth() - buttonX - g_verticalBar*2;
	buttonHi = (g_tileSize*3)/2;
	int verticalSpace = buttonHi + g_horizontalBar;

	buttonY[BUYTOWERBUTTON] = 0;
	buttonY[SPEEDBUTTON] = verticalSpace;
	buttonY[INCOMEBUTTON] = verticalSpace * 2;
	buttonY[PAUSEBUTTON] = Iw2DGetSurfaceHeight() - 2*verticalSpace - g_horizontalBar;
	buttonY[CONTWAVESBUTTON] = Iw2DGetSurfaceHeight() - verticalSpace - g_horizontalBar;
	buttonY[BUYTOWERBUTTONBOT] = buttonY[BUYTOWERBUTTON] + buttonHi;
	buttonY[SPEEDBUTTONBOT] = buttonY[SPEEDBUTTON] + buttonHi;
	buttonY[INCOMEBUTTONBOT] = buttonY[INCOMEBUTTON] + buttonHi;
	buttonY[PAUSEBUTTONBOT] = buttonY[PAUSEBUTTON] + buttonHi;
	buttonY[CONTWAVESBUTTONBOT] = buttonY[CONTWAVESBUTTON] + buttonHi;

	textX = GRID_COLUMNS * g_tileSize + g_verticalBar*3;
	textWid = buttonWid - g_verticalBar;
	textHi = g_tileSize;
	verticalSpace = textHi + g_horizontalBar;

	textY[0] = buttonY[INCOMEBUTTONBOT] + textHi - g_horizontalBar;
	textY[1] = textY[0] + textHi;
	textY[2] = textY[1] + textHi; 
}
//=============================================================================
