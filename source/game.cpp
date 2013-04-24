#include <iostream>
#include <vector>
#include <queue>
#include <time.h>
#include <time.h>
#include <stdio.h>

#include "game.h"
#include "tower.h"

#include "Iw2D.h"
#include "s3eKeyboard.h"
#include "s3ePointer.h"

//One of these arrays gets pointed to by Game::wallPos to position new walls
const int wallPosTile20[16] = {9, 2, 9, 18, 2, 9, 18, 9, 5, 5, 15, 15, 15, 5, 5, 15};
const int wallPosTile40[16] = {18, 4, 18, 36, 4, 18, 36, 18, 11, 11, 29, 29, 29, 10, 10, 29};

//int tempx, tempy, tempz;
//=============================================================================
Game::Game(int _tileSize) : tileSize(_tileSize)
{
	setUpUI();
	init();
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

	towers.clear();

	delete tileGrid;
	delete pathGrid;
	delete mobPath;
}
//=============================================================================
void Game::update()
{
	if(currWave > FINAL_WAVE)
	{
		gameMode = MODE_GAME_ENDED;
		manageGameEnded();
	}
	else
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
}
//=============================================================================
// Initialize the game
void Game::init()
{
	Tower::setAttSpeed(GAMESPEED);
	srand(time(NULL));

	tileGrid = new Grid();
	pathGrid = new PathGrid();

	addIncome		= 0;
	credits			= 70;
	currWave		= 0;
	income			= 5;
	lockedTowers	= 0;
	mobsAlive		= 0;
	mobHp			= 2;
	mobPath			= 0;
	score			= 0;
	spawnTimer		= 5;

	spawnNextWave	= false;
	undoChange		= false;
	buildMode		= true;
	takeTouch		= false;
	contWaves		= false;
	speedMode		= FAST;
	gameMode		= MODE_TITLE;

	numOfCurrWaveMons	= BASE_MONSTER_COUNT;
	mobMoveSpeed		= tileSize / 10;
	shotMoveSpeed		= mobMoveSpeed + tileSize / 20;
	spawnNextMobId		= MAX_MONSTER_COUNT;

	towers.reserve(GRID_COLUMNS*GRID_ROWS);
	mobGridPos.reserve(MAX_MONSTER_COUNT);

	tileGrid->buildAllGrass(tileSize, 
		verticalBorder, verticalOffset);
	pathGrid->init();

	for(int i=0; i < MAX_MONSTER_COUNT; i++)
		monsters[i] = new Monster();

	while(!generateMap())
		pathGrid->init();

	if(tileSize < 40)
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
		mobHp += currWave / 3;
	}

	spawnNextWave = false;
	mobsAlive = numOfCurrWaveMons;

	if(contWaves)
	{
		lockedTowers = newTowers.size();
		updatePathGrid();
	}

	if(lockedTowers > 0)
	{
		pathGrid->setAllUnvisited();
		if(!findShortestPath())
		{
			lockedTowers = 0;
			revertPathGridUpdate();
		}
		else
		{
			tileGrid->removePathGrassListeners(spawnX, spawnY, *mobPath);
			lockTowers();
			tileGrid->setPathGrassListeners(spawnX, spawnY, *mobPath);
		}
	}
	updateStats();
}
//=============================================================================
void Game::handleInput()
{
	if(g_Input.getTouchCount() == 0)
		takeTouch = true;

	if(takeTouch && g_Input.getTouchCount() > 0)
	{
		CTouch *touch = g_Input.getTouch(0);

		if(validTouch(touch))
		{
			if(gridTouch(touch))
			{
				placeTowerTouch(touch);
			}
			else
			{
				if(buttonTouchX(touch))
				{
					if(towerTouch(touch))
					{
						invokeTowerBtn();
					}
					else if(speedTouch(touch))
					{
						invokeSpeedBtn();
					}
					else if(incomeTouch(touch))
					{
						invokeIncomeBtn();
					}
					else if(pauseTouch(touch))
					{
						invokePauseBtn();
					}
					else if(contTouch(touch))
					{
						invokeContBtn();
					}
					else if(undoTouch(touch))
					{
						invokeUndoBtn();
					}
				}
			}
			takeTouch = false;
		}
	}
}
//==============================================================================
void Game::render()
{
	drawBG(tileSize);	
	tileGrid->render(tileSize);
	renderWalls();
	renderNewTowers();

	renderMonsters();
	renderShots();

	renderButtons();
	renderText();

}
void Game::setUpUI()
{
	setBorders();
	setButtonSize();
	setTextAreas();
}
//==============================================================================
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

	if(addIncome > 0) //move this if econ change func
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
		x = (t->getCenterX() - verticalBorder) / tileSize;
		y = (t->getCenterY() - verticalOffset) / tileSize;

		tileGrid->addTower(t, x, y);
		towers.push_back(t);
		buildWalls(x, y);
		newTowers.pop_front();
		lockedTowers--;
	}
}
//==============================================================================
void Game::undoLastTower()
{
	credits += TOWER_PRICE;
	if(credits > MAX_RESOURCE)
		credits = MAX_RESOURCE;

	Tower* t = newTowers.back();
	int x = (t->getCenterX() - verticalBorder) / tileSize;
	int y = (t->getCenterY() - verticalOffset) / tileSize;

	pathGrid->add(x, y, *tileGrid);
	newTowers.pop_back();
	undoChange = false;
	delete t;
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
// Takes grid pos of new tower and adds to newTowers if possible
void Game::buildTower(int x, int y)
{
	if(pathGrid->available(x, y))
	{
		Tile *t = tileGrid->get(x, y);
		if(credits >= TOWER_PRICE && t != 0 && t->getColor() == GRASS)
		{
			Tower *newTower = new Tower(x, y,
				x * tileSize + verticalBorder,
				y * tileSize + verticalOffset,
				tileSize);

			for(unsigned int i=0; i < newTowers.size(); i++)
			{//TODO, make a better solution to prevent the need for this scanning
				if(*newTowers[i] == newTower)
				{
					delete newTower;
					return;
				}
			}
			credits -= TOWER_PRICE;
			newTowers.push_back(newTower);
		}
	}
}
//==============================================================================
void Game::buildWater(int x, int y)
{
	if(tileGrid->get(x, y)->getColor() == GRASS)
	{
		tileGrid->buildWater(x, y);
		pathGrid->remove(x, y, *tileGrid);
	}
}
//==============================================================================
//Takes grid pos coords of tower
void Game::buildWalls(int x, int y)
{
	int topLeftX = x * tileSize + verticalBorder;
	int topLeftY = y * tileSize + verticalOffset;

	if(tileGrid->isTower(x, y-1))
		walls.push_back(new Wall(VERWALL, 
		topLeftX + *wallPos, topLeftY - *(wallPos+1)));

	if(tileGrid->isTower(x, y+1))
		walls.push_back(new Wall(VERWALL, 
		topLeftX + *(wallPos+2), topLeftY + *(wallPos+3)));

	if(tileGrid->isTower(x-1, y))
		walls.push_back(new Wall(HORWALL, 
		topLeftX - *(wallPos+4), topLeftY + *(wallPos+5)));

	if(tileGrid->isTower(x+1, y))
		walls.push_back(new Wall(HORWALL, 
		topLeftX + *(wallPos+6), topLeftY + *(wallPos+7)));

	if(tileGrid->isTower(x-1, y-1))
		walls.push_back(new Wall(WALL14, 
		topLeftX - *(wallPos+8), topLeftY - *(wallPos+9)));

	if(tileGrid->isTower(x+1, y+1))
		walls.push_back(new Wall(WALL14, 
		topLeftX + *(wallPos+10), topLeftY + *(wallPos+11)));

	if(tileGrid->isTower(x+1, y-1))
		walls.push_back(new Wall(WALL23, 
		topLeftX + *(wallPos+12), topLeftY - *(wallPos+13)));

	if(tileGrid->isTower(x-1, y+1))
		walls.push_back(new Wall(WALL23, 
		topLeftX - *(wallPos+14), topLeftY + *(wallPos+15)));
}
//==============================================================================
void Game::spawnMonster() 
{
	if(spawnTimer == 0)
	{
		if(spawnNextMobId < numOfCurrWaveMons )
		{
			monsters[spawnNextMobId]->init(spawnX, spawnY, 
				spawnX * tileSize + verticalBorder,
				spawnY * tileSize + verticalOffset,
				mobHp, mobMoveSpeed, spawnNextMobId,
				tileSize / 2 - tileSize / 10, //TODO, handle monster radius
				tileSize);

			mobGridPos[spawnNextMobId].first = spawnX;
			mobGridPos[spawnNextMobId].second = spawnY;
			spawnNextMobId++;
			spawnTimer = MONSTER_SPAWN_INTERVAL;
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
			tileGrid->notifyTileExit(mobGridPos[i].first, mobGridPos[i].second, i);
			mobGridPos[i].first = monsters[i]->getGridPosX();
			mobGridPos[i].second = monsters[i]->getGridPosY();
			tileGrid->notifyTileEnter(mobGridPos[i].first, mobGridPos[i].second, i);
			monsters[i]->gridPosUpdated();
		}
	}
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
	pvPtr spawnPtr = pathGrid->at(spawnX, spawnY);
	pvPtr exitPtr = pathGrid->at(exitX, exitY);

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

	}
	//pathGrid.print(spawnPtr, exitPtr);

	if(foundPath == true)
	{
		backtrack(exitPtr, shortestPath);
		delete mobPath;
		mobPath = new std::string(shortestPath.rbegin(), shortestPath.rend());
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
	tileGrid->buildSpawn(spawnX, spawnY,
		spawnX * tileSize + verticalBorder, 
		spawnY * tileSize + verticalOffset);

	tileGrid->buildExit(exitX, exitY,
		exitX * tileSize + verticalBorder, 
		exitY * tileSize + verticalOffset);

	for(int i=(rand() % 10) + 3; i > 0; i--)
		buildWater(rand() % GRID_COLUMNS, rand() % GRID_ROWS);

	if(!findShortestPath())
	{
		tileGrid->releaseTile(spawnX, spawnY);
		tileGrid->buildGrass(spawnX, spawnY,
			spawnX * tileSize + verticalBorder,
			spawnY * tileSize + verticalOffset);

		tileGrid->releaseTile(exitX, exitY);
		tileGrid->buildGrass(exitX, exitY,
			spawnX * tileSize + verticalBorder,
			spawnY * tileSize + verticalOffset);

		tileGrid->setAllGrass();
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
					monsters[target], (*it)->getDmg(), shotMoveSpeed,
					tileSize / 5)); //TODO, handle radius
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
		{
			if(!monsters[i]->move(*mobPath, tileSize))
			{
				decreaseScore();
				mobsAlive--;

			}
		}
		/*else if(monsters[i]->despawned())
		mobsAlive--;*/
	}
}
//==============================================================================
void Game::decreaseScore()
{
	score -= mobHp*2;
	if(score < 0)
		score = 0;
}
//==============================================================================
void Game::increaseScore()
{
	score += mobHp;
	if(score > MAX_SCORE)
		score = MAX_SCORE;
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
				monsterDied(getsShot);
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
void Game::monsterDied(Monster *mon)
{
	if(credits < MAX_RESOURCE)
		credits++;

	increaseScore();
	mobsAlive--;

	tileGrid->notifyTileExit(mon->getGridPosX(), 
		mon->getGridPosY(), mon->getMobId());
}
//==============================================================================
void Game::renderShots() const
{
	Iw2DSetColour(0xffffffff);
	for(std::list<TrackingShot*>::const_iterator it = shots.begin(); 
		it != shots.end(); it++)
	{
		drawTile(SHOT, (*it)->getTopLeftX(), (*it)->getTopLeftY(), 
			(*it)->getRadius()*2, (*it)->getRadius()*2);
	}
}
//==============================================================================
void Game::renderWalls() const
{
	Iw2DSetColour(0xffffffff);
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
		tileSize, tileSize);

	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
}
//==============================================================================
void Game::renderMonsters() const
{
	Iw2DSetColour(0xffffffff);
	for(int j=0; j < numOfCurrWaveMons; j++)
	{
		if(monsters[j]->monsterIsAlive()) 
		{
			drawTile(MONSTER, monsters[j]->getTopLeftX(), 
				monsters[j]->getTopLeftY(), tileSize, tileSize);
		}
	}
}
//=============================================================================
void Game::renderAlphaButton(int color, int yIndex) const
{
	Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
	Iw2DSetColour(0xFF0C5907);
	drawTile(color, buttonX, buttonY[yIndex], buttonWid, buttonHi);
	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
	Iw2DSetColour(0xffffffff);

}
//=============================================================================
void Game::renderButtons() const
{
	Iw2DSetColour(0xffffffff);

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

	/*if(addIncome > 0)
	{
	renderAlphaButton(INCOME, INCOMEBUTTON);
	}
	else*/
	drawTile(INCOME, buttonX, buttonY[INCOMEBUTTON], buttonWid, buttonHi);

	if(newTowers.size() - lockedTowers > 0)
	{
		renderAlphaButton(UNDO, UNDOBUTTON);
	}
	else
		drawTile(UNDO, buttonX, buttonY[UNDOBUTTON], buttonWid, buttonHi);

	if(contWaves)
	{
		renderAlphaButton(CONTWAVES, CONTWAVESBUTTON);
	}
	else
		drawTile(CONTWAVES, buttonX, buttonY[CONTWAVESBUTTON], buttonWid, buttonHi);


	drawTile(PAUSE, buttonX, buttonY[PAUSEBUTTON], buttonWid, buttonHi);
}
//==============================================================================
void Game::renderPaused(int qx, int cx, int y) const
{
	Iw2DSetColour(0xFF0C5907);
	Iw2DFillRect(CIwSVec2(qx, y), 
		CIwSVec2(tileSize*5, tileSize*2));
	Iw2DFillRect(CIwSVec2(cx, y), 
		CIwSVec2(tileSize*5, tileSize*2));

	Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
	Iw2DSetColour(0xFF12AB09); 

	Iw2DDrawString("QUIT", CIwSVec2(qx, y), CIwSVec2(tileSize*4, tileSize*2), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);

	Iw2DDrawString("CONTINIUE", CIwSVec2(cx, y), CIwSVec2(tileSize*4, tileSize*2), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);

	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
}
//==============================================================================
void Game::renderTitleScren(int newX, int newY) const
{
	Iw2DSetColour(0xFF0C5907);
	Iw2DFillRect(CIwSVec2(newX, newY), 
		CIwSVec2(largeButtonWid, 2*tileSize));

	Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
	Iw2DSetColour(0xFF12AB09); 

	Iw2DDrawString("NEW GAME", CIwSVec2(newX, newY), CIwSVec2(largeButtonWid, tileSize*2), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);

	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
}
//==============================================================================
void Game::renderGameEnded(int x, int y) const
{
	Iw2DSetColour(0xFF0C5907);
	Iw2DFillRect(CIwSVec2(x, y), 
		CIwSVec2(largeButtonWid*3, tileSize*2));

	Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
	Iw2DSetColour(0xFF12AB09); 

	char str[20];
	sprintf(str, "Your score was %d", score);
	Iw2DDrawString(str, CIwSVec2(x, y), CIwSVec2(largeButtonWid*3, tileSize*2), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);

	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
}
//==============================================================================
void Game::renderText() const
{
	Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
	Iw2DSetColour(0xFF12AB09); //0xFF18860D);

	drawText(CREDITSTEXT, 'C', credits);
	drawText(INCOMETEXT, 'I', income);
	drawText(WAVETEXT, 'W', currWave);

	char str[9]; //TODO build string
	if(score > 99999)
		sprintf(str, "S %d", score);
	else if(score> 9999)
		sprintf(str, "S 0%d", score);
	else if(score > 999)
		sprintf(str, "S 00%d", score);
	else if(score> 99)
		sprintf(str, "S 000%d", score);
	else if(score> 9)
		sprintf(str, "S 0000%d", score);
	else
		sprintf(str, "S 00000%d", score);

	Iw2DDrawString(str, CIwSVec2(textX[SCORETEXT], textY), 
		CIwSVec2(textWid, textHi), 
		IW_2D_FONT_ALIGN_LEFT, IW_2D_FONT_ALIGN_TOP);

	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
}
//==============================================================================
void Game::drawText(Texts x, char c, int text) const
{
	char str[6];

	//for(int i=MAX_RESOURCE; i > text; i /= 10)

	if(text > 99)
		sprintf(str, "%c %d", c, text);
	else if(text > 9)
		sprintf(str, "%c 0%d", c, text);
	else
		sprintf(str, "%c 00%d", c, text);

	Iw2DDrawString(str, CIwSVec2(textX[x], textY), CIwSVec2(textWid, textHi), 
		IW_2D_FONT_ALIGN_LEFT, IW_2D_FONT_ALIGN_TOP);

	//char cc[30];
	//sprintf(cc, "%d\n%d\n%d", tempx,
	//tempy, tempz);

	//Iw2DDrawString(cc, CIwSVec2(100, 100), CIwSVec2(100, 100), 
	//IW_2D_FONT_ALIGN_LEFT, IW_2D_FONT_ALIGN_CENTRE);
}
//==============================================================================
void Game::setBorders()
{
	int wid = Iw2DGetSurfaceWidth();
	int hi = Iw2DGetSurfaceHeight();
	largeButtonWid	= tileSize * 4;

	if(wid < hi)//TODO LOL
	{
		int temp = wid;
		wid = hi;
		hi = temp;
	}

	if(tileSize < 40) //TODO
	{
		buttonWid = 50;
		horizontalBorder = (hi - GRID_ROWS*tileSize - tileSize) / 2; //tileSize = textHi
		verticalBorder = (wid - GRID_COLUMNS*tileSize - buttonWid) / 3;	
	}
	else
	{
		buttonWid = 130;

		horizontalBorder = (hi - GRID_ROWS*tileSize - tileSize) / 2;
		verticalBorder = (wid - GRID_COLUMNS*tileSize - buttonWid) / 3;	
	}
	verticalOffset = horizontalBorder + tileSize; //tilesize = texthi
}
//==============================================================================
void Game::setButtonSize()
{
	buttonX = GRID_COLUMNS * tileSize + 2*verticalBorder;
	buttonHi = (tileSize*3)/2;
	int verticalSpace = buttonHi + horizontalBorder;

	buttonY[BUYTOWERBUTTON]		= verticalOffset;
	buttonY[SPEEDBUTTON]		= buttonY[BUYTOWERBUTTON] + verticalSpace;
	buttonY[INCOMEBUTTON]		= buttonY[SPEEDBUTTON] + verticalSpace;
	buttonY[PAUSEBUTTON]		= buttonY[INCOMEBUTTON] + verticalSpace;
	buttonY[CONTWAVESBUTTON]	= buttonY[PAUSEBUTTON] + verticalSpace;
	buttonY[UNDOBUTTON]			= buttonY[CONTWAVESBUTTON] + verticalSpace;

	buttonY[BUYTOWERBUTTONBOTTOM]	= buttonY[BUYTOWERBUTTON] + buttonHi;
	buttonY[SPEEDBUTTONBOTTOM]		= buttonY[SPEEDBUTTON] + buttonHi;
	buttonY[INCOMEBUTTONBOTTOM]		= buttonY[INCOMEBUTTON] + buttonHi;
	buttonY[PAUSEBUTTONBOTTOM]		= buttonY[PAUSEBUTTON] + buttonHi;
	buttonY[CONTWAVESBUTTONBOTTOM]	= buttonY[CONTWAVESBUTTON] + buttonHi;
	buttonY[UNDOBUTTONBOTTOM]		= buttonY[UNDOBUTTON] + buttonHi;
}
//=============================================================================
void Game::setTextAreas()
{
	textY = horizontalBorder/2;
	textWid = 5 * tileSize;
	textHi = tileSize;

	textX[0] = horizontalBorder;
	textX[1] = textX[0] + textWid;
	textX[2] = textX[1] + textWid; 
	textX[3] = textX[2] + textWid; 

	/*for(int i=0; i < sizeof(textY) / sizeof(int); i++)
	textY[i] += g_horizontalBorder;*/
}
//=============================================================================
bool Game::validTouch(CTouch *touch)
{
	return touch->x >= verticalBorder			
		&& touch->x < Iw2DGetSurfaceWidth() - verticalBorder
		&& touch->y >= verticalOffset	
		&& touch->y < Iw2DGetSurfaceHeight() - horizontalBorder;
}
bool Game::gridTouch(CTouch *touch)
{
	return touch->x < tileSize * GRID_COLUMNS + verticalBorder;
}
void Game::placeTowerTouch(CTouch *touch)
{
	if(buildMode)
		buildTower((touch->x - verticalBorder) / tileSize, 
		(touch->y - verticalOffset) / tileSize);
}
bool Game::buttonTouchX(CTouch *touch)
{
	return touch->x >= GRID_COLUMNS * tileSize + 2*verticalBorder;
}
bool Game::towerTouch(CTouch *touch)
{
	return touch->y < buttonY[BUYTOWERBUTTONBOTTOM];
}
void Game::invokeTowerBtn()
{
	if(buildMode)
	{
		buildMode = false;
		lockedTowers = newTowers.size();
		updatePathGrid();
	}
	else
	{
		buildMode = true;
	}
}
bool Game::speedTouch(CTouch *touch)
{
	return touch->y < buttonY[SPEEDBUTTONBOTTOM] 
	&& touch->y >= buttonY[SPEEDBUTTON];
}
void Game::invokeSpeedBtn()
{
	if(speedMode == NEWWAVE)
	{
		speedMode = FAST;
	}
	else
		if(mobsAlive == 0)
			speedMode = NEWWAVE;
}
bool Game::incomeTouch(CTouch *touch)
{
	return touch->y < buttonY[INCOMEBUTTONBOTTOM]
	&& touch->y >= buttonY[INCOMEBUTTON];
}
void Game::invokeIncomeBtn()
{
	if(credits >= INCOME_PRICE && income + addIncome < MAX_RESOURCE)
	{
		credits -= INCOME_PRICE;
		addIncome += INCOME_PRICE / 10;
	}
}
bool Game::pauseTouch(CTouch *touch)
{
	return touch->y < buttonY[PAUSEBUTTONBOTTOM]
	&& touch->y >= buttonY[PAUSEBUTTON];
}
void Game::invokePauseBtn()
{
	gameMode = MODE_PAUSED;
}

bool Game::undoTouch(CTouch *touch)
{
	return touch->y < buttonY[UNDOBUTTONBOTTOM]
	&& touch->y >= buttonY[UNDOBUTTON];
}

void Game::invokeUndoBtn()
{
	if(newTowers.size() > lockedTowers)
		undoChange = true;
}
bool Game::contTouch(CTouch *touch)
{
	return touch->y < buttonY[CONTWAVESBUTTONBOTTOM]
	&& touch->y >= buttonY[CONTWAVESBUTTON];
}
void Game::invokeContBtn()
{
	contWaves = !contWaves;
}

void Game::setTileSize(int _tileSize)
{
	tileSize = _tileSize;
}
void Game::updatePathGrid()
{
	for(unsigned int i=0; i < lockedTowers; i++)
	{
		Tower* it = newTowers.at(i);
		pathGrid->remove((it->getTopLeftX() - verticalBorder) / tileSize, 
			(it->getTopLeftY() - verticalOffset) / tileSize,
			*tileGrid);
	}
}

void Game::revertPathGridUpdate()
{
	for(unsigned int i=0; i < lockedTowers; i++)
	{
		Tower* it = newTowers.at(i);
		pathGrid->add((it->getTopLeftX() - verticalBorder) / tileSize, 
			(it->getTopLeftY() - verticalOffset) / tileSize,
			*tileGrid);
	}
}

void Game::manangePausedMode()
{
	int quitLeftX		= (GRID_COLUMNS / 2 - 6) * tileSize,
		continiueLeftX	= (GRID_COLUMNS / 2 + 1) * tileSize,
		y				= (GRID_ROWS / 2 - 1) * tileSize;

	Iw2DSurfaceClear(0xffff9900);
	render();
	renderPaused(quitLeftX, continiueLeftX, y);
	Iw2DSurfaceShow();

	if(g_Input.getTouchCount() == 0)
		takeTouch = true;

	if(takeTouch && g_Input.getTouchCount() > 0)
	{
		CTouch *touch = g_Input.getTouch(0);

		if(isTouchingLargeBtn(touch, quitLeftX, y))
			gameMode = MODE_TITLE;

		else if(isTouchingLargeBtn(touch, continiueLeftX, y))
		{
			gameMode = MODE_GAMEPLAY;
			takeTouch = false;
		}
	}
}

bool Game::isTouchingLargeBtn(CTouch *touch, int x, int y) const
{
	return touch->x > x && touch->x <= x + largeButtonWid
		&& touch->y >= y && touch->y < y + tileSize*2;
}

GameMode Game::getGameMode() const
{
	return gameMode;
}

bool Game::manageTitleMode()
{
	int newGameX = (Iw2DGetSurfaceWidth() - largeButtonWid) / 2;
	int newGameY = 6*tileSize;

	if(g_Input.getTouchCount() == 0)
		takeTouch = true;

	Iw2DSurfaceClear(0xffff9900);
	renderTitleScren(newGameX, newGameY);
	Iw2DSurfaceShow();

	if(takeTouch && g_Input.getTouchCount() > 0)
	{
		CTouch *touch = g_Input.getTouch(0);

		if(isTouchingLargeBtn(touch, newGameX, newGameY))
		{
			takeTouch = false;
			return true;
		}
	}
	return false;
}

void Game::newGame()
{
	gameMode = MODE_GAMEPLAY;
}

void Game::manageGameEnded()
{
	int x		= (Iw2DGetSurfaceWidth() - 3*largeButtonWid) / 2,
		y		= (GRID_ROWS / 2 - 1) * tileSize;

	Iw2DSurfaceClear(0xffff9900);
	render();
	renderGameEnded(x, y);
	Iw2DSurfaceShow();

	if(g_Input.getTouchCount() == 0)
		takeTouch = true;

	if(takeTouch && g_Input.getTouchCount() > 0)
	{
		//CTouch *touch = g_Input.getTouch(0);

		/*if(touch->x > x && touch->x <= x + largeButtonWid*2
			&& touch->y >= y && touch->y < y + tileSize*2)*/
			gameMode = MODE_TITLE;

		takeTouch = false;
	}
}
