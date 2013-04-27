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
	io			= new Io(_tileSize);
	tileGrid	= 0;
	pathGrid	= 0;
	mobPath		= 0;

	io->setUpUI(gridColumns, gridRows);
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
	io->reset();

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

	speedMode			= ImmobileSpeedMode;
	rememberSpeedMode	= NormalSpeedMode;

	spawnNextWave	= false;
	undoChange		= false;
	updatePath		= false;

	numOfCurrWaveMons	= BASE_MONSTER_COUNT;
	mobMoveSpeed		= tileSize / 20;
	shotMoveSpeed		= (mobMoveSpeed*4)/3;
	spawnNextMobId		= MAX_MONSTER_COUNT;
	verticalBorder		= io->getVerticalBorder();
	verticalOffset		= io->getVerticalOffset();

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
	CTouch *touch = 0;

	switch(io->handleInput(&touch))
	{
	case DoNothingInputEvent:
		return PlayMode;

	case PlaceTowerInputEvent:
		placeTowerTouch(touch);
		break;

	case ChangeSpeedInputEvent:
		changeSpeedMode();
		break;

	case PauseBtnInputEvent:
		return PausedMode;

	case UndoBtnInputEvent:
		invokeUndoBtn();
		break;

	case DmgBtnInputEvent:
		if(towerDmgUncapped())
			invokeDmgBtn();
		break;

	case AsBtnInputEvent:
		if(towerAsUncapped())
			invokeBuySpeedBtn();
		break;

	case RangeBtnInputEvent:
		invokeBuyRangeBtn();
		updatePath = true;
		break;

	case IncomeBtnInputEvent:
		invokeIncomeBtn();
		break;
	}
	return PlayMode;
}
//==============================================================================
void Game::render()
{
	io->renderBg();
	tileGrid->render(io, tileSize);
	renderWalls();
	renderNewTowers();
	renderMonsters();
	renderShots();

	ButtonState dmg, as, ran;

	if(towerDmgUncapped())
	{
		if(credits >= upgradeCost[towerDmgCounter])
			dmg = ActiveButtonState;
		else
			dmg = InactiveButtonState;
	}
	else
		dmg = InvisButtonState;

	if(towerAsUncapped())
	{
		if(credits >= upgradeCost[towerAsCounter])
			as = ActiveButtonState;
		else
			as = InactiveButtonState;
	}
	else
		as = InvisButtonState;

	if(towerRangeUncapped())
	{
		if(credits >= upgradeCost[towerRangeCounter])
			ran = ActiveButtonState;
		else
			ran = InactiveButtonState;
	}
	else
		ran = InvisButtonState;

	io->renderButtons(mobsAlive, newTowers.empty(),
		as, dmg, ran,
		credits >= INCOME_PRICE, speedMode);

	io->renderScoreText(score);
	io->renderIncomeText(income);
	io->renderWaveText(currWave);
	io->renderCreditsText(credits);
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
		Tower *t = newTowers.back();
		x = (t->getCenterX() - verticalBorder) / tileSize;
		y = (t->getCenterY() - verticalOffset) / tileSize;

		tileGrid->addTower(t, x, y);
		towers.push_back(t);
		buildWalls(x, y);
		newTowers.pop_back();
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

		if(io->contWavesActive())
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
		if(monsters[i]->monsterIsAlive() && 
			!monsters[i]->move(*mobPath, tileSize))
		{
			decreaseScore();
			mobsAlive--;

		}
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
//=============================================================================
void Game::monsterDied(Monster *mon)
{
	if(credits < MAX_CREDITS)
		credits++;

	increaseScore();
	mobsAlive--;

	tileGrid->notifyTileExit(mon->getGridPosX(), 
		mon->getGridPosY(), mon->getMobId());
}
//=============================================================================
void Game::invokeIncomeBtn()
{
	if(credits >= INCOME_PRICE && income + addIncome < MAX_RESOURCE)
	{
		credits -= INCOME_PRICE;
		addIncome += INCOME_PRICE / 10;
	}
}

void Game::invokeUndoBtn()
{
	if(!newTowers.empty())
		undoChange = true;
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

void Game::UpdatePathGrid()
{
	for(std::vector<Tower*>::const_iterator it = newTowers.begin();
		it != newTowers.end(); it++)
		pathGrid->remove(((*it)->getTopLeftX() - verticalBorder) / tileSize, 
		((*it)->getTopLeftY() - verticalOffset) / tileSize,
		*tileGrid);
}

void Game::revertPathGridUpdate()
{
	for(std::vector<Tower*>::const_iterator it = newTowers.begin();
		it != newTowers.end(); it++)
		pathGrid->add(((*it)->getTopLeftX() - verticalBorder) / tileSize, 
		((*it)->getTopLeftY() - verticalOffset) / tileSize,
		*tileGrid);
}

Mode Game::manangePausedMode()
{
	Iw2DSurfaceClear(0xFF0C5907);
	render();
	return io->manangePausedMode();
}

Mode Game::manageTitleMode()
{
	return io->manageTitleMode();
}

Mode Game::manageGameEnded()
{
	Iw2DSurfaceClear(0xFF0C5907);
	render();
	return io->manageGameEnded(topScore);
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

	for(std::vector<Tower*>::iterator it = newTowers.begin(); it != newTowers.end(); it++)
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
void Game::placeTowerTouch(CTouch *touch)
{
	buildTower((touch->x - verticalBorder) / tileSize, 
		(touch->y - verticalOffset) / tileSize);
}
//==============================================================================
void Game::renderShots() const
{
	Iw2DSetColour(0xffffffff);
	for(std::list<TrackingShot*>::const_iterator it = shots.begin(); 
		it != shots.end(); it++)
	{
		io->drawTile(ShotImage, (*it)->getTopLeftX(), (*it)->getTopLeftY(), 
			(*it)->getRadius()*2, (*it)->getRadius()*2);
	}
}
//==============================================================================
void Game::renderWalls() const
{
	Iw2DSetColour(0xffffffff);
	for(std::vector<Wall*>::const_iterator it = walls.begin(); it != walls.end(); it++)
		io->drawTile((*it)->getColor(), (*it)->getTopLeftX(), (*it)->getTopLeftY());
}
//==============================================================================
void Game::renderNewTowers() const
{
	Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
	Iw2DSetColour(0xFF0C5907);

	for(std::vector<Tower*>::const_iterator it = newTowers.begin(); it != newTowers.end(); it++)
		io->drawTile((*it)->getColor(), (*it)->getTopLeftX(), (*it)->getTopLeftY(),
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
			io->drawTile(MonsterImage, monsters[j]->getTopLeftX(), 
				monsters[j]->getTopLeftY(), tileSize, tileSize);
		}
	}
}
