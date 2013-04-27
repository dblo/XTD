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
const int upgradeCost[3] = {100, 300, 1000};

//int tempx, tempy, tempz;
//=============================================================================
Game::Game(int _tileSize) : tileSize(_tileSize)
{
	setUpUI();
	tileGrid = 0;
	pathGrid = 0;
	mobPath = 0;
}
//=============================================================================
Game::~Game() 
{
	cleanUp();

	delete pathGrid;
	delete tileGrid;
	delete mobPath;
}
//=============================================================================
Mode Game::Update()
{
	if(undoChange)
		undoLastTower();

	if(spawnNextWave) 
		onNewWave();

	spawnMonster();
	moveMobs();
	UpdateMobGrid();
	shoot();
	moveShots();
	manageCollisions();
	waveOverCheck();

	if(score > topScore)
		topScore = score;

	if(currWave == FINAL_WAVE)
	{
		manageGameEnded();
		return EndedMode;
	}
	return handleInput();
}
//=============================================================================
// Initialize the game
void Game::reset()
{
	Tower::resetTowers(tileSize);
	srand(time(NULL));

	tileGrid = new TileGrid(gridColumns, gridRows);
	pathGrid = new PathGrid(gridColumns, gridRows);

	credits			= 700;
	income			= 500;
	addIncome		= 0;
	currWave		= 0;
	mobsAlive		= 0;
	mobHp			= 1;
	mobPath			= 0;
	score			= 0;
	spawnTimer		= 5;
	topScore		= 0;
	towerAsCounter	= 0;
	towerDmgCounter	= 0;
	towerRangeCounter = 0;
	takeNextInputAt	= INT_MAX;
	holdingPlayCounter = 0;

	showBuildMenu	= false;
	spawnNextWave	= false;
	undoChange		= false;
	contWaves		= false;
	updatePath		= false;
	speedMode		= ImmobileSpeedMode;
	rememberSpeedMode = NormalSpeedMode;

	numOfCurrWaveMons	= BASE_MONSTER_COUNT;
	mobMoveSpeed		= tileSize / 20;
	shotMoveSpeed		= (mobMoveSpeed*4)/3;
	spawnNextMobId		= MAX_MONSTER_COUNT;

	towers.reserve(gridColumns*gridRows/2);
	mobGridPos.reserve(MAX_MONSTER_COUNT);

	tileGrid->buildAllGrass(tileSize, 
		verticalBorder, verticalOffset);
	pathGrid->init();

	for(int i=0; i < MAX_MONSTER_COUNT; i++)
		monsters.push_back(new Monster());

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

	if(newTowers.size() > 0 || updatePath)
	{
		updatePath = false;
		UpdatePathGrid();
		pathGrid->setAllUnvisited();
		if(!findShortestPath())
		{
			revertPathGridUpdate();
		}
		else
		{
			tileGrid->removePathGrassListeners(spawnX, spawnY, *mobPath);
			lockTowers();
			tileGrid->setPathGrassListeners(spawnX, spawnY, *mobPath);
		}
	}
	UpdateStats();
}
//=============================================================================
Mode Game::handleInput()
{
	if(g_Input.getTouchCount() == 0)
	{
		if(holdingPlayCounter == 1)
		{
			changeSpeedMode();
		}
		holdingPlayCounter = 0;
		takeNextInputAt = 0;
	}
	else if((uint32)s3eTimerGetMs() > takeNextInputAt)
	{
		CTouch *touch = g_Input.getTouch(0);
		takeNextInputAt = (uint32)s3eTimerGetMs() + TOUCH_INTERVAL;

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
					if(speedTouch(touch))
					{
						invokeSpeedBtn();
					}
					else if(buyTouch(touch))
					{
						invokeBuyBtn();
					}
					else if(pauseTouch(touch))
					{
						return PausedMode;
					}
					else if(undoTouch(touch))
					{
						invokeUndoBtn();
					}
					else if(showBuildMenu)
					{
						if(damageTouch(touch) && towerDmgUncapped())
						{
							invokeDmgBtn();
						}
						else if(buySpeedTouch(touch) && towerAsUncapped())
						{
							invokeBuySpeedBtn();
						}
						else if(buyRangeTouch(touch) && towerRangeUncapped())
						{
							invokeBuyRangeBtn();
							updatePath = true;
						}
					}
					else if(incomeTouch(touch))
					{
						invokeIncomeBtn();
					}
				}
			}
		}
	}
	return PlayMode;
}
//==============================================================================
void Game::render()
{
	drawBG(buttonX - verticalBorder/2);
	tileGrid->render(tileSize);
	renderWalls();
	renderNewTowers();

	renderMonsters();
	renderShots();

	renderButtons();
	renderText();

}
//==============================================================================
void Game::setUpUI()
{	
	if(tileSize < 40)
	{
		buttonWid = 50;
	}
	else
		buttonWid = 130;

	textHi			= tileSize;
	textcols		= tileSize * 5;
	largeButtonWid	= tileSize * 5;
	largeButtonHi	= tileSize * 2;


	int cols = (Iw2DGetSurfaceWidth() - buttonWid) / tileSize;
	gridColumns = ((Iw2DGetSurfaceWidth() - buttonWid) % tileSize >= 30) ? cols : cols-1;

	int rows = (Iw2DGetSurfaceHeight() - textHi) / tileSize;
	gridRows = (Iw2DGetSurfaceHeight() % tileSize >= 20) ? rows : rows-1;


	setBorders();

	buttonX			= gridColumns * tileSize + 2*verticalBorder + verticalBorder/2;
	buttonHi		= (Iw2DGetSurfaceHeight() - 8*horizontalBorder) / 7;
	textY			= horizontalBorder/2;
	
	setButtonSize();
	setTextAreas();
}
//==============================================================================
void Game::setBorders()
{
	int cols = Iw2DGetSurfaceWidth();
	int hi = Iw2DGetSurfaceHeight();

	if(cols < hi)
	{
		int temp = cols;
		cols = hi;
		hi = temp;
	}

	horizontalBorder = (hi - gridRows*tileSize - textHi) / 2;
	verticalBorder = (cols - gridColumns*tileSize - buttonWid) / 3;	
	verticalOffset = horizontalBorder + tileSize;
}
//==============================================================================
void Game::UpdateStats()
{
	if(credits + income <= MAX_CREDITS)
		credits += income;
	else
		credits = MAX_CREDITS;

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
	while(newTowers.size() > 0)
	{
		Tower *t = newTowers.front();
		x = (t->getCenterX() - verticalBorder) / tileSize;
		y = (t->getCenterY() - verticalOffset) / tileSize;

		tileGrid->addTower(t, x, y);
		towers.push_back(t);
		buildWalls(x, y);
		newTowers.pop_front();
	}
}
//==============================================================================
void Game::undoLastTower()
{
	credits += TOWER_PRICE;
	if(credits > MAX_CREDITS)
		credits = MAX_CREDITS;

	Tower* t = newTowers.back();	
	newTowers.pop_back();

	pathGrid->add((t->getCenterX() - verticalBorder) / tileSize, 
		(t->getCenterY() - verticalOffset) / tileSize, 
		*tileGrid);
	undoChange = false;
	delete t;
}
//==============================================================================
void Game::waveOverCheck()
{
	if(mobsAlive == 0 && !spawnNextWave)
	{
		if(speedMode == rememberSpeedMode)
			speedMode = ImmobileSpeedMode;

		if(contWaves)
		{
			speedMode = rememberSpeedMode;
			spawnNextWave = true;
		}
	}
}
//==============================================================================
// Takes grid pos of new tower and adds to newTowers if possible
void Game::buildTower(int x, int y)
{
	if(pathGrid->available(x, y))
	{
		Tile *t = tileGrid->get(x, y);
		if(credits >= TOWER_PRICE && t != 0 && t->getColor() == GrassImage)
		{
			Tower *newTower = new Tower(
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
	if(tileGrid->get(x, y)->getColor() == GrassImage)
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
		walls.push_back(new Wall(VerWallImage, 
		topLeftX + *wallPos, topLeftY - *(wallPos+1)));

	if(tileGrid->isTower(x, y+1))
		walls.push_back(new Wall(VerWallImage, 
		topLeftX + *(wallPos+2), topLeftY + *(wallPos+3)));

	if(tileGrid->isTower(x-1, y))
		walls.push_back(new Wall(HorWallImage, 
		topLeftX - *(wallPos+4), topLeftY + *(wallPos+5)));

	if(tileGrid->isTower(x+1, y))
		walls.push_back(new Wall(HorWallImage, 
		topLeftX + *(wallPos+6), topLeftY + *(wallPos+7)));

	if(tileGrid->isTower(x-1, y-1))
		walls.push_back(new Wall(Wall14Image, 
		topLeftX - *(wallPos+8), topLeftY - *(wallPos+9)));

	if(tileGrid->isTower(x+1, y+1))
		walls.push_back(new Wall(Wall14Image, 
		topLeftX + *(wallPos+10), topLeftY + *(wallPos+11)));

	if(tileGrid->isTower(x+1, y-1))
		walls.push_back(new Wall(Wall23Image, 
		topLeftX + *(wallPos+12), topLeftY - *(wallPos+13)));

	if(tileGrid->isTower(x-1, y+1))
		walls.push_back(new Wall(Wall23Image, 
		topLeftX - *(wallPos+14), topLeftY + *(wallPos+15)));
}
//==============================================================================
void Game::spawnMonster() 
{
	if(spawnTimer == 0)
	{
		if(spawnNextMobId < numOfCurrWaveMons)
		{
			monsters[spawnNextMobId]->init(spawnX, spawnY, 
				spawnX * tileSize + verticalBorder,
				spawnY * tileSize + verticalOffset,
				mobHp, mobMoveSpeed, spawnNextMobId,
				tileSize / 2, //TODO, handle monster radius
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
void Game::UpdateMobGrid()
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
	while(iter->getCameFrom() != UndefDirection) 
	{
		switch (iter->getCameFrom())
		{
		case RightDirection:
			path += 'l';
			break;
		case LeftDirection:
			path += 'r';
			break;
		case DownDirection:
			path += 'u';
			break;
		case UpDirection:
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
found, the previous path will not be Updated. */
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
	//pathGrid->print(spawnPtr, exitPtr);

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
	int x = rand() % gridColumns;
	int y = rand() % gridRows;
	spawnX = x;
	spawnY = y;

	do {
		x = rand() % gridColumns;
		y = rand() % gridRows;
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
		buildWater(rand() % gridColumns, rand() % gridRows);

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
	int target;
	Monster *tarMon;
	for(std::vector<Tower*>::const_iterator it = towers.begin(); it != towers.end(); it++)
	{
		if((*it)->armed())
		{
			target = (*it)->aquireTarget(numOfCurrWaveMons);

			while(target < numOfCurrWaveMons)
			{
				tarMon = monsters[target];
				if((*it)->targetInRange(tarMon->getCenterX(), tarMon->getCenterY(),
					tarMon->getRadius()))
					break;
				else
					target = (*it)->aquireTarget(numOfCurrWaveMons, ++target);
			}

			if(target < numOfCurrWaveMons)
			{
				shots.push_back(new TrackingShot((*it)->getCenterX(),
					(*it)->getCenterY(),
					tarMon, (*it)->shoot(), 
					shotMoveSpeed,
					tileSize / 5)); //TODO, handle shot radius
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
	if(credits < MAX_CREDITS)
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
		drawTile(ShotImage, (*it)->getTopLeftX(), (*it)->getTopLeftY(), 
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
	for(std::vector<Monster*>::const_iterator it = monsters.begin();
		it != monsters.end(); it++)
		if((*it)->monsterIsAlive())
			drawTile(MonsterImage, (*it)->getTopLeftX(), 
			(*it)->getTopLeftY(), tileSize, tileSize);
}
//=============================================================================
void Game::renderAlphaButton(int color, int yIndex) const
{
	Iw2DSetAlphaMode(IW_2D_ALPHA_HALF);
	Iw2DSetColour(0xFF40C020);
	drawTile(color, buttonX, buttonY[yIndex], buttonWid, buttonHi);

	//Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
	Iw2DSetColour(0xffffffff);
	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
}
//=============================================================================
void Game::renderButtons() const
{
	Iw2DSetColour(0xffffffff);

	if(contWaves)
		drawTile(ContWavesImage, buttonX, buttonY[SpeedButton], buttonWid, buttonHi);
	else if(speedMode == ImmobileSpeedMode && mobsAlive == 0)
	{
		drawTile(SpeedImage, buttonX, buttonY[SpeedButton], buttonWid, buttonHi);
	}
	else if(speedMode == NormalSpeedMode)
		drawTile(NormalSpeedImage, buttonX, buttonY[SpeedButton], buttonWid, buttonHi); 
	else
		drawTile(FastSpeedImage, buttonX, buttonY[SpeedButton], buttonWid, buttonHi); 

	/*if(showBuildMenu)
	renderAlphaButton(BuyImage, BuyButton);
	else*/
	drawTile(BuyImage, buttonX, buttonY[BuyButton], buttonWid, buttonHi);

	if(newTowers.empty())
	{
		renderAlphaButton(UndoImage, UndoButton);
	}
	else
		drawTile(UndoImage, buttonX, buttonY[UndoButton], buttonWid, buttonHi);

	drawTile(PauseImage, buttonX, buttonY[PauseButton], buttonWid, buttonHi);

	if(showBuildMenu)
	{
		if(credits >= 100)
		{
			if(towerDmgUncapped())
				drawTile(BuyDamageImage, buttonX, buttonY[BuyDamageButton], buttonWid, buttonHi);

			if(towerAsUncapped())
				drawTile(BuySpeedImage, buttonX, buttonY[BuySpeedButton], buttonWid, buttonHi);

			if(towerRangeUncapped())
				drawTile(BuyRangeImage, buttonX, buttonY[BuyRangeButton], buttonWid, buttonHi);
		}
		else
		{
			if(towerDmgUncapped())
				renderAlphaButton(BuyDamageImage, BuyDamageButton);

			if(towerAsUncapped())
				renderAlphaButton(BuySpeedImage, BuySpeedButton);

			if(towerRangeUncapped())
				renderAlphaButton(BuyRangeImage, BuyRangeButton);
		}
	}
	else
	{
		if(credits >= INCOME_PRICE)
		{
			drawTile(IncomeImage, buttonX, buttonY[IncomeButton], buttonWid, buttonHi);
			
		}
		else
			renderAlphaButton(IncomeImage, IncomeButton);
	}
}
//==============================================================================
void Game::renderPaused(int qx, int cx, int y) const
{
	Iw2DSetColour(0xFF0C5907);
	Iw2DFillRect(CIwSVec2(qx, y), 
		CIwSVec2(largeButtonWid, tileSize*2));
	Iw2DFillRect(CIwSVec2(cx, y), 
		CIwSVec2(largeButtonWid, tileSize*2));

	Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
	Iw2DSetColour(0xFF12AB09); 

	Iw2DDrawString("QUIT", CIwSVec2(qx, y), CIwSVec2(largeButtonWid, tileSize*2), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);

	Iw2DDrawString("CONTINIUE", CIwSVec2(cx, y), CIwSVec2(largeButtonWid, tileSize*2), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);

	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
}
//==============================================================================
void Game::renderTitleScren(int newX, int newY) const
{
	Iw2DSetColour(0xFF40C020);
	Iw2DFillRect(CIwSVec2(newX, newY), 
		CIwSVec2(largeButtonWid, 2*tileSize));

	Iw2DSetColour(0xFF12AB09); 

	Iw2DDrawString("NEW GAME", CIwSVec2(newX, newY), CIwSVec2(largeButtonWid, tileSize*2), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
}
//==============================================================================
void Game::renderGameEnded(int x, int y) const
{
	Iw2DSetColour(0xFF0C5907);
	Iw2DFillRect(CIwSVec2(x, y), 
		CIwSVec2(largeButtonWid*3, tileSize*2));

	Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
	Iw2DSetColour(0xFF12AB09); 

	char str[30];
	sprintf(str, "Your highest score was %d", topScore);
	Iw2DDrawString(str, CIwSVec2(x, y), CIwSVec2(largeButtonWid*3, tileSize*2), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);

	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
}
//==============================================================================
void Game::renderText() const
{
	//Iw2DSetAlphaMode(IW_2D_ALPHA_HALF);
	Iw2DSetColour(0x770C5907);

	drawText(IncomeText, 'I', income);
	drawText(WaveText, 'W', currWave);
	renderScore();
	renderCredits();

	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
}
//==============================================================================
void Game::renderCredits() const
{
	char str[7];
	if(credits > 999)
		sprintf(str, "C %d", credits);
	else if(credits > 99)
		sprintf(str, "C 0%d", credits);
	else if(credits > 9)
		sprintf(str, "C 00%d", credits);
	else
		sprintf(str, "C 000%d", credits);

	Iw2DDrawString(str, CIwSVec2(textX[CreditsText], textY), CIwSVec2(textcols, textHi), 
		IW_2D_FONT_ALIGN_LEFT, IW_2D_FONT_ALIGN_TOP);
}
//==============================================================================
void Game::renderScore() const
{
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

	Iw2DDrawString(str, CIwSVec2(textX[ScoreText], textY), 
		CIwSVec2(textcols, textHi), 
		IW_2D_FONT_ALIGN_LEFT, IW_2D_FONT_ALIGN_TOP);
}
//==============================================================================
void Game::drawText(Text x, char c, int text) const
{
	char str[6];
	if(text > 99)
		sprintf(str, "%c %d", c, text);
	else if(text > 9)
		sprintf(str, "%c 0%d", c, text);
	else
		sprintf(str, "%c 00%d", c, text);

	Iw2DDrawString(str, CIwSVec2(textX[x], textY), CIwSVec2(textcols, textHi), 
		IW_2D_FONT_ALIGN_LEFT, IW_2D_FONT_ALIGN_TOP);
}
//==============================================================================
void Game::setButtonSize()
{
	int verticalSpace = buttonHi + horizontalBorder;

	buttonY[SpeedButton]		= horizontalBorder;
	buttonY[BuyButton]			= buttonY[SpeedButton] + verticalSpace;
	buttonY[IncomeButton]		= buttonY[BuyButton] + verticalSpace;
	buttonY[BuyDamageButton]	= buttonY[BuyButton] + verticalSpace;
	buttonY[BuySpeedButton]		= buttonY[BuyDamageButton] + verticalSpace;
	buttonY[BuyRangeButton]		= buttonY[BuySpeedButton] + verticalSpace;
	buttonY[UndoButton]			= buttonY[BuyRangeButton] + verticalSpace;
	buttonY[PauseButton]		= buttonY[UndoButton] + verticalSpace;

	buttonY[BuyBottomButton]		= buttonY[BuyButton] + buttonHi;
	buttonY[SpeedBottomButton]		= buttonY[SpeedButton] + buttonHi;
	buttonY[IncomeBottomButton]		= buttonY[IncomeButton] + buttonHi;
	buttonY[PauseBottomButton]		= buttonY[PauseButton] + buttonHi;
	buttonY[BuyDamageBottomButton]	= buttonY[BuyDamageButton] + buttonHi;
	buttonY[UndoBottomButton]		= buttonY[UndoButton] + buttonHi;
	buttonY[BuySpeedBottomButton]	= buttonY[BuySpeedButton] + buttonHi;
	buttonY[BuyRangeBottomButton]	= buttonY[BuyRangeButton] + buttonHi;
}
//=============================================================================
void Game::setTextAreas()
{
	textX[ScoreText] = horizontalBorder;
	textX[IncomeText] = textX[ScoreText] + textcols;
	textX[WaveText] = textX[IncomeText] + textcols; 
	textX[CreditsText] = textX[WaveText] + textcols; 
}
//=============================================================================
bool Game::validTouch(CTouch *touch) const
{
	return touch->x >= verticalBorder			
		&& touch->x < Iw2DGetSurfaceWidth() - verticalBorder
		&& touch->y >= buttonY[SpeedButton]
	&& touch->y < Iw2DGetSurfaceHeight() - horizontalBorder;
}
bool Game::gridTouch(CTouch *touch) const
{
	return touch->x < tileSize * gridColumns + verticalBorder
		&& touch->y > verticalOffset;
}
void Game::placeTowerTouch(CTouch *touch)
{
	buildTower((touch->x - verticalBorder) / tileSize, 
		(touch->y - verticalOffset) / tileSize);
}
bool Game::buttonTouchX(CTouch *touch) const
{
	return touch->x >= gridColumns * tileSize + 2*verticalBorder;
}
bool Game::buyTouch(CTouch *touch) const
{
	return touch->y > buttonY[BuyButton] &&
		touch->y < buttonY[BuyBottomButton];
}
void Game::invokeBuyBtn()
{
	showBuildMenu = !showBuildMenu;
}
bool Game::speedTouch(CTouch *touch) const
{
	return touch->y < buttonY[SpeedBottomButton];
}
void Game::invokeSpeedBtn()
{
	holdingPlayCounter++;
	if(holdingPlayCounter > 2)
	{
		contWaves = true;
	}
	else if(contWaves)
	{
		contWaves = false;

		//Prevent speed change in next input-handling cycle
		holdingPlayCounter = 0;
	}
}

bool Game::incomeTouch(CTouch *touch) const
{
	return touch->y < buttonY[IncomeBottomButton]
	&& touch->y >= buttonY[IncomeButton];
}
void Game::invokeIncomeBtn()
{
	if(credits >= INCOME_PRICE && income + addIncome < MAX_RESOURCE)
	{
		credits -= INCOME_PRICE;
		addIncome += INCOME_PRICE / 10;
	}
}
bool Game::pauseTouch(CTouch *touch) const
{
	return touch->y < buttonY[PauseBottomButton]
	&& touch->y >= buttonY[PauseButton];
}

bool Game::undoTouch(CTouch *touch) const
{
	return touch->y < buttonY[UndoBottomButton]
	&& touch->y >= buttonY[UndoButton];
}

void Game::invokeUndoBtn()
{
	if(!newTowers.empty())
		undoChange = true;
}

bool Game::buySpeedTouch(CTouch *touch) const
{
	return touch->y < buttonY[BuySpeedBottomButton]
	&& touch->y >= buttonY[BuySpeedButton];
}

void Game::invokeBuySpeedBtn()
{
	if(credits >= upgradeCost[towerAsCounter])
	{
		credits -= upgradeCost[towerAsCounter];
		Tower::buffAs();
		towerAsCounter++;
	}
}

void Game::invokeBuyRangeBtn()
{
	if(credits >= upgradeCost[towerRangeCounter])
	{
		credits -= upgradeCost[towerRangeCounter];
		Tower::buffRange();
		towerRangeCounter++;
	}
}

void Game::invokeDmgBtn()
{
	if(credits >= upgradeCost[towerDmgCounter])
	{
		credits -= upgradeCost[towerDmgCounter];
		Tower::buffDmg(2);
		towerDmgCounter++;
	}
}

bool Game::buyRangeTouch(CTouch *touch) const
{
	return touch->y < buttonY[BuyRangeBottomButton]
	&& touch->y >= buttonY[BuyRangeButton];
}

bool Game::damageTouch(CTouch *touch) const
{
	return touch->y < buttonY[BuyDamageBottomButton]
	&& touch->y >= buttonY[BuyDamageButton];
}

void Game::UpdatePathGrid()
{
	for(std::deque<Tower*>::const_iterator it = newTowers.begin();
		it != newTowers.end(); it++)
		pathGrid->remove(((*it)->getTopLeftX() - verticalBorder) / tileSize, 
		((*it)->getTopLeftY() - verticalOffset) / tileSize,
		*tileGrid);
}

void Game::revertPathGridUpdate()
{
	for(std::deque<Tower*>::const_iterator it = newTowers.begin();
		it != newTowers.end(); it++)
		pathGrid->add(((*it)->getTopLeftX() - verticalBorder) / tileSize, 
		((*it)->getTopLeftY() - verticalOffset) / tileSize,
		*tileGrid);
}

Mode Game::manangePausedMode()
{
	if(g_Input.getTouchCount() == 0)
	{
		takeNextInputAt = 0;
	}
	int quitLeftX		= (gridColumns / 2 - 4) * tileSize,
		continiueLeftX	= (gridColumns / 2 + 3) * tileSize,
		y				= (gridRows / 2 - 1) * tileSize;

	Iw2DSurfaceClear(0xFF0C5907);
	render();
	renderPaused(quitLeftX, continiueLeftX, y);
	Iw2DSurfaceShow();

	if(g_Input.getTouchCount() > 0 &&
		(uint32)s3eTimerGetMs() > takeNextInputAt)
	{
		CTouch *touch = g_Input.getTouch(0);

		if(isTouchingLargeBtn(touch, quitLeftX, y))
		{
			takeNextInputAt = (uint32)s3eTimerGetMs() + TOUCH_INTERVAL;
			return TitleMode;
		}
		else if(isTouchingLargeBtn(touch, continiueLeftX, y))
		{
			takeNextInputAt = (uint32)s3eTimerGetMs() + TOUCH_INTERVAL;
			return PlayMode;
		}
	}
	return PausedMode;
}

bool Game::isTouchingLargeBtn(CTouch *touch, unsigned int x, unsigned int y) const
{
	return touch->x > x && touch->x <= x + largeButtonWid
		&& touch->y >= y && touch->y < y + tileSize*2;
}

Mode Game::manageTitleMode()
{
	if(g_Input.getTouchCount() == 0)
	{
		takeNextInputAt = 0;
	}
	int newGameX = (Iw2DGetSurfaceWidth() - largeButtonWid) / 2;
	int newGameY = 6*tileSize;

	Iw2DSurfaceClear(0xFF0C5907);
	renderTitleScren(newGameX, newGameY);
	Iw2DSurfaceShow();

	if(g_Input.getTouchCount() > 0 &&
		(uint32)s3eTimerGetMs() > takeNextInputAt)
	{
		CTouch *touch = g_Input.getTouch(0);

		if(isTouchingLargeBtn(touch, newGameX, newGameY))
		{
			takeNextInputAt = (uint32)s3eTimerGetMs() + TOUCH_INTERVAL;
			return PlayMode;
		}
	}
	return TitleMode;
}

Mode Game::manageGameEnded()
{
	if(g_Input.getTouchCount() == 0)
	{
		takeNextInputAt = 0;
	}
	int x = (Iw2DGetSurfaceWidth() - 3*largeButtonWid) / 2,
		y = (gridRows / 2 - 1) * tileSize;

	Iw2DSurfaceClear(0xFF0C5907);
	render();
	renderGameEnded(x, y);
	Iw2DSurfaceShow();

	if(g_Input.getTouchCount() > 0 && 
		(uint32)s3eTimerGetMs() > takeNextInputAt)
	{
		takeNextInputAt = (uint32)s3eTimerGetMs() + TOUCH_INTERVAL;
		return TitleMode;
	}
	return EndedMode;
}

void Game::setMonsterSpeed()
{
	for(int i=0; i < numOfCurrWaveMons; i++)
		monsters[i]->setMs(mobMoveSpeed);
}

void Game::setShotSpeed()
{
	for(std::list<TrackingShot*>::const_iterator it = shots.begin();
		it != shots.end(); it++)
		(*it)->setMs(shotMoveSpeed);
}

void Game::changeGameSpeed()
{
	if(speedMode == NormalSpeedMode)
	{
		shotMoveSpeed *= 2;
		mobMoveSpeed *= 2;
		speedMode = rememberSpeedMode = FastSpeedMode;
		Tower::fastAs();
	}
	else
	{
		shotMoveSpeed /= 2;
		mobMoveSpeed /= 2;
		speedMode = rememberSpeedMode = NormalSpeedMode;
		Tower::slowAs();
	}
	setMonsterSpeed();
	setShotSpeed();
}

void Game::cleanUp()
{
	for(std::vector<Monster*>::iterator it = monsters.begin(); it != monsters.end(); it++)
		delete (*it);
	monsters.clear();

	for(std::vector<Wall*>::iterator it = walls.begin(); it != walls.end(); it++)
		delete (*it);
	walls.clear();

	for(std::list<TrackingShot*>::iterator it = shots.begin(); it != shots.end(); it++)
		delete (*it);
	shots.clear();

	for(std::deque<Tower*>::iterator it = newTowers.begin(); it != newTowers.end(); it++)
		delete (*it);
	newTowers.clear();

	towers.clear();
}

void Game::changeSpeedMode()
{
	if(speedMode == ImmobileSpeedMode)
	{
		speedMode = rememberSpeedMode;
		spawnNextWave = true;
	} 
	else if(speedMode == NormalSpeedMode)
	{
		changeGameSpeed();
	}
	else if(speedMode == FastSpeedMode)
	{
		changeGameSpeed();
	}
}

bool Game::towerDmgUncapped() const
{
	return towerDmgCounter < NUM_OF_UPGRADE_LVLS;
}

bool Game::towerAsUncapped() const
{
	return towerAsCounter < NUM_OF_UPGRADE_LVLS;
}

bool Game::towerRangeUncapped() const
{
	return towerRangeCounter < NUM_OF_UPGRADE_LVLS;
}
