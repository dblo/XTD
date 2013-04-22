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

//One of these arrays gets pointed to by Game::wallPos to position new walls
const int wallPosTile20[16] = {9, 2, 9, 18, 2, 9, 18, 9, 5, 5, 15, 15, 15, 5, 5, 15};
const int wallPosTile40[16] = {18, 4, 18, 36, 4, 18, 36, 18, 11, 11, 29, 29, 29, 10, 10, 29};

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

	for(std::deque<Tower*>::iterator it = newTowers.begin(); it != newTowers.end(); it++)
		delete (*it);
	newTowers.clear();

	for(int i=0; i < MAX_MONSTER_COUNT; i++)
		delete monsters[i];
}
////=============================================================================
//bool Game::changeMade() const
//{
//	return undoQueue.empty();
//}
//=============================================================================
void Game::Update()
{
	if(undoChange)
		undoLastTower();

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
	undoChange = false;
	mobsAlive = 0;
	buildMode = true;
	credits = 500;
	income = 10;
	spawnNextWave = false;
	numOfCurrWaveMons = BASE_MONSTER_COUNT;
	mobMoveSpeed = g_tileSize / 10;
	shotMoveSpeed = mobMoveSpeed + g_tileSize / 20;
	spawnNextMobId = MAX_MONSTER_COUNT;
	Tower::setAttSpeed(GAMESPEED);
	currWave = 0;
	//towerRange = LEVEL1;
	mobGridPos.reserve(MAX_MONSTER_COUNT);
	mobHp = 1;
	spawnTimer = 5;
	takeTouch = true;
	contWaves = false;
	speedMode = FAST;
	lockedTowers = 0;

	for(int i=0; i < MAX_MONSTER_COUNT; i++)
		monsters[i] = new Monster();

	tileGrid.buildAllGrass();
	pathGrid.init();

	while(!generateMap())
		pathGrid.init();

	setButtonSize();

	if(g_tileSize < 40)
		wallPos = wallPosTile20;
	else
		wallPos = wallPosTile40;
}
//=============================================================================
void Game::onNewWave()
{
	spawnNextMobId = 0;

	if(numOfCurrWaveMons < MAX_MONSTER_COUNT)
		numOfCurrWaveMons += 3;
	else 
	{
		numOfCurrWaveMons = BASE_MONSTER_COUNT;
		mobHp +=2;
	}

	spawnNextWave = false;
	mobsAlive = numOfCurrWaveMons;

	if(contWaves)
		lockedTowers = newTowers.size();

	if(newTowers.size() > 0)
	{
		pathGrid.reset();
		if(!findShortestPath())
		{
			lockedTowers = 0;
		}
		else
		{
			tileGrid.removePathGrassListeners(spawnX, spawnY);
			lockTowers();
			tileGrid.setPathGrassListeners(spawnX, spawnY);
		}
	}
	updateStats();
}
//=============================================================================
void Game::updateStats()
{
	if(credits + income <= MAX_RESOURCE)
		credits += income;
	else
		credits = MAX_RESOURCE;

	if(currWave < MAX_RESOURCE)
		currWave++;
	else
		currWave = MAX_RESOURCE;

	if(addIncome > 0) //move this if keep econ change
	{
		if(income + addIncome <= MAX_RESOURCE)
		{
			income += addIncome;
			addIncome = 0;
		}
		else
			income = MAX_RESOURCE;
	}
}
//=============================================================================
void Game::lockTowers()
{
	int x, y;
	while(lockedTowers > 0)
	{
		Tower *t = newTowers.front();
		x = t->getCenterX() / g_tileSize;
		y = t->getCenterY() / g_tileSize;

		tileGrid.addTower(t, x, y);
		towers.push_back(t);
		buildWalls(x, y);
		newTowers.pop_front();
		lockedTowers--;
	}
}
//==============================================================================
void Game::handleInput()
{
	int buttonHi = (g_tileSize*3)/2;
	int verticalSpace = buttonHi + g_horizontalBorder;

	if(g_Input.getTouchCount() == 0)
		takeTouch = true;

	if(takeTouch && g_Input.getTouchCount() > 0)
	{
		CTouch *touch = g_Input.getTouch(0);

		if(touch->x >= g_verticalBorder && touch->x <= Iw2DGetSurfaceWidth() - g_verticalBorder
			&& touch->y >= g_horizontalBorder && touch->y < Iw2DGetSurfaceHeight() - g_horizontalBorder)
		{
			if(touch->x < g_tileSize * GRID_COLUMNS + g_verticalBorder)
			{
				if(buildMode)
					buildTower((touch->x - g_verticalBorder) / g_tileSize, 
					(touch->y - g_horizontalBorder) / g_tileSize);
			}
			else
			{
				if(touch->x >= GRID_COLUMNS * g_tileSize + 2*g_verticalBorder && 
					touch->x <= Iw2DGetSurfaceWidth() - g_verticalBorder)
				{
					if(touch->y < buttonY[BUYTOWERBUTTONBOTTOM] + g_horizontalBorder) //Tower button
					{
						if(buildMode)
						{
							buildMode = false;
							lockedTowers = newTowers.size();
						}
						else
						{
							buildMode = true;
						}
					}
					else if(touch->y < buttonY[SPEEDBUTTONBOTTOM]  + g_horizontalBorder 
						&& touch->y >= buttonY[SPEEDBUTTON] + g_horizontalBorder) //Speed button
					{
						if(speedMode == NEWWAVE)
						{
							speedMode = FAST;
						}
						else
							if(mobsAlive == 0)
								speedMode = NEWWAVE;
					}
					else if(touch->y < buttonY[INCOMEBUTTONBOTTOM] + g_horizontalBorder
						&& touch->y >= buttonY[INCOMEBUTTON] + g_horizontalBorder) //Income button
					{
						if(credits >= INCOME_PRICE && income + addIncome < 999)
						{
							credits -= INCOME_PRICE;
							addIncome++;
						}
					}
					else if(touch->y < buttonY[PAUSEBUTTONBOTTOM] + g_horizontalBorder
						&& touch->y >= buttonY[PAUSEBUTTON] + g_horizontalBorder) //Pause button
					{
						if(newTowers.size() > lockedTowers)
							undoChange = true;
					}
					else if(touch->y < buttonY[CONTWAVESBUTTONBOTTOM] + g_horizontalBorder
						&& touch->y >= buttonY[CONTWAVESBUTTON] + g_horizontalBorder) //Continiouos button
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
void Game::undoLastTower()
{
	credits += 10; //change mn
	if(credits > MAX_RESOURCE)
		credits = MAX_RESOURCE;

	Tower* t = newTowers.back();
	int x = t->getCenterX() / g_tileSize;
	int y = t->getCenterY() / g_tileSize;

	pathGrid.add(x, y, tileGrid);
	newTowers.pop_back();
	undoChange = false;
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
	if(pathGrid.available(x, y))
	{
		if(credits >= TOWER_PRICE)
		{
			credits -= TOWER_PRICE;
			Tile *t = tileGrid.get(x, y);
			if(t != 0 && t->getColor() == GRASS )
			{
				newTowers.push_back(new Tower(x, y));
				pathGrid.remove(x, y, tileGrid);
			}
		}
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
		walls.push_back(new Wall(VERWALL, topLeftX + *wallPos, topLeftY - *(wallPos+1)));

	if(tileGrid.isTower(x, y+1))
		walls.push_back(new Wall(VERWALL, topLeftX + *(wallPos+2), topLeftY + *(wallPos+3)));

	if(tileGrid.isTower(x-1, y))
		walls.push_back(new Wall(HORWALL, topLeftX - *(wallPos+4), topLeftY + *(wallPos+5)));

	if(tileGrid.isTower(x+1, y))
		walls.push_back(new Wall(HORWALL, topLeftX + *(wallPos+6), topLeftY + *(wallPos+7)));

	if(tileGrid.isTower(x-1, y-1))
		walls.push_back(new Wall(WALL14, topLeftX - *(wallPos+8), topLeftY - *(wallPos+9)));

	if(tileGrid.isTower(x+1, y+1))
		walls.push_back(new Wall(WALL14, topLeftX + *(wallPos+10), topLeftY + *(wallPos+11)));

	if(tileGrid.isTower(x+1, y-1))
		walls.push_back(new Wall(WALL23, topLeftX + *(wallPos+12), topLeftY - *(wallPos+13)));

	if(tileGrid.isTower(x-1, y+1))
		walls.push_back(new Wall(WALL23, topLeftX - *(wallPos+14), topLeftY + *(wallPos+15)));
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
	/*for(int i=0; i < 20; i++)
	for(int j=2; j<15; j++)
	buildTower(i,j);*/
	return true;
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
void Game::renderShots() const
{
	for(std::list<TrackingShot*>::const_iterator it = shots.begin(); 
		it != shots.end(); it++)
	{
		drawTile(SHOT, (*it)->getTopLeftX(), (*it)->getTopLeftY(), 
			((*it)->getRadius()*3)/2, ((*it)->getRadius()*3)/2);
	}
}
//==============================================================================
void Game::renderWalls() const
{
	for(std::vector<Wall*>::const_iterator it = walls.begin(); it != walls.end(); it++)
		drawTile((*it)->getColor(), (*it)->getTopLeftX(), (*it)->getTopLeftY());
}
//==============================================================================
void Game::renderNewTowers() const
{
	Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
	Iw2DSetColour(0xFF0C5907);

	for(std::deque<Tower*>::const_iterator it = newTowers.begin(); it != newTowers.end(); it++)
		drawTile((*it)->getColor(), (*it)->getTopLeftX(), (*it)->getTopLeftY(),
		g_tileSize, g_tileSize);

	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
	Iw2DSetColour(0xffffffff);
}
//==============================================================================
void Game::renderMonsters() const
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
void Game::renderAlphaButton(int color, int yIndex) const
{
	Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
	Iw2DSetColour(0xFF0C5907);
	drawTile(color, buttonX, buttonY[yIndex], buttonWid, buttonHi);
	Iw2DSetColour(0xffffffff);
	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
}
//=============================================================================
void Game::renderButtons() const
{
	if(buildMode)
	{
		renderAlphaButton(BUYTOWER, BUYTOWERBUTTON);
	}
	else
		drawTile(BUYTOWER, buttonX, buttonY[BUYTOWERBUTTON], buttonWid, buttonHi);

	if(speedMode == FAST && mobsAlive == 0) //speemode fast == game on atm
	{
		renderAlphaButton(SPEED, SPEEDBUTTON);
	}
	else
		drawTile(SPEED, buttonX, buttonY[SPEEDBUTTON], buttonWid, buttonHi); 

	if(addIncome > 0)
	{
		renderAlphaButton(INCOME, INCOMEBUTTON);
	}
	else
		drawTile(INCOME, buttonX, buttonY[INCOMEBUTTON], buttonWid, buttonHi);

	if(newTowers.size() - lockedTowers > 0)
	{
		renderAlphaButton(PAUSE, PAUSEBUTTON);
	}
	else
		drawTile(PAUSE, buttonX, buttonY[PAUSEBUTTON], buttonWid, buttonHi);

	if(contWaves)
	{
		renderAlphaButton(CONTWAVES, CONTWAVESBUTTON);
	}
	else
		drawTile(CONTWAVES, buttonX, buttonY[CONTWAVESBUTTON], buttonWid, buttonHi);
}
//==============================================================================
void Game::renderText() const
{
	char str[6];

	Iw2DSetColour(0xFF18860D);

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

void Game::setBorders()
{
	g_horizontalBorder = (Iw2DGetSurfaceHeight() - GRID_ROWS*g_tileSize) / 2;
	g_verticalBorder = 10;
}
//==============================================================================
void Game::setButtonSize()
{
	buttonX = GRID_COLUMNS * g_tileSize + g_verticalBorder;
	buttonHi = (g_tileSize*3)/2;
	int verticalSpace = buttonHi + g_horizontalBorder;

	//buttonWid = Iw2DGetSurfaceWidth() - buttonX - g_verticalBorder*2;
	if(g_tileSize < 40) //fix
		buttonWid = 50;
	else
		buttonWid = 130;

	buttonY[BUYTOWERBUTTON] = 0;
	buttonY[SPEEDBUTTON] = verticalSpace;
	buttonY[INCOMEBUTTON] = verticalSpace * 2;
	buttonY[PAUSEBUTTON] = Iw2DGetSurfaceHeight() - 2*verticalSpace - g_horizontalBorder;
	buttonY[CONTWAVESBUTTON] = Iw2DGetSurfaceHeight() - verticalSpace - g_horizontalBorder;
	buttonY[BUYTOWERBUTTONBOTTOM] = buttonY[BUYTOWERBUTTON] + buttonHi;
	buttonY[SPEEDBUTTONBOTTOM] = buttonY[SPEEDBUTTON] + buttonHi;
	buttonY[INCOMEBUTTONBOTTOM] = buttonY[INCOMEBUTTON] + buttonHi;
	buttonY[PAUSEBUTTONBOTTOM] = buttonY[PAUSEBUTTON] + buttonHi;
	buttonY[CONTWAVESBUTTONBOTTOM] = buttonY[CONTWAVESBUTTON] + buttonHi;

	textX = GRID_COLUMNS * g_tileSize + g_verticalBorder*3;
	textWid = buttonWid - g_verticalBorder/2;
	textHi = g_tileSize;
	verticalSpace = textHi + g_horizontalBorder;

	textY[0] = buttonY[INCOMEBUTTONBOTTOM] + textHi - g_horizontalBorder;
	textY[1] = textY[0] + textHi;
	textY[2] = textY[1] + textHi; 
}
//=============================================================================
