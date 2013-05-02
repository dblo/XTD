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
const int wallPosTile20[16] = {9,  2,  9, 18, 2,  9, 18,  9,  5,  5, 15, 15, 15,  5,  5, 15};
const int wallPosTile40[16] = {18, 4, 18, 36, 4, 18, 36, 18, 11, 11, 29, 29, 29, 10, 10, 29};
const int upgradeCost[3] = {100, 500, 1000};

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
	delete io;
}
//=============================================================================
Mode Game::Update()
{
	if(spawnNextWave) 
		onNewWave();

	spawnMonster();
	moveMobs();
	UpdateMobGrid();
	shoot();
	moveShots();
	manageCollisions();
	waveOverCheck();

	if(lives == 0 || (currWave == MAX_WAVE && mobsAlive == 0))
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

	credits				= BASE_CREDITS;
	mobHp				= MONSTER_BASE_HP;
	spawnTimer			= 0;
	currWave			= 0;
	mobsAlive			= 0;
	lives				= 30;
	towerAsCounter		= 0;
	towerDmgCounter		= 0;
	towerRangeCounter	= 0;
	shotDiam			= (tileSize*2) / 5;
	monsterRadius		= tileSize / 5;

	speedMode			= ImmobileSpeedMode;
	rememberSpeedMode	= FastSpeedMode;

	spawnNextWave		= false;
	rangeUpgraded		= false;
	validPathExists		= true;

	numOfCurrWaveMons	= BASE_MONSTER_COUNT;
	mobMoveSpeed		= tileSize / 6;
	shotMoveSpeed		= (mobMoveSpeed*4)/3;
	spawnNextMobId		= MAX_MONSTER_COUNT;
	verticalBorder		= io->getVerticalBorder();
	horizontalBorder	= io->getHorizontalBorder();

	mobGridPos.reserve(MAX_MONSTER_COUNT);

	tileGrid->buildAllGrass(tileSize, 
		verticalBorder, horizontalBorder);
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

		if(currWave < (MAX_WAVE / 5))
			mobHp += currWave / 2;
		else if(currWave < (MAX_WAVE*2) / 5)
			mobHp += currWave;
		else if(currWave < (MAX_WAVE*3) / 5)
			mobHp += (currWave*3)/2;
		else if(currWave < (MAX_WAVE*4) / 5)
			mobHp += currWave*2;
		else
			mobHp += (currWave*5)/2;

		//std::cout << "HP: " << mobHp << ", Wave: " << currWave << std::endl;
	}

	spawnNextWave = false;
	mobsAlive = numOfCurrWaveMons;

	if(newTowerBuilt || rangeUpgraded)
	{
		rangeUpgraded = false;
		UpdatePathGrid();
		pathGrid->setAllUnvisited();
		if(!findShortestPath())
		{
			revertPathGridUpdate();
			validPathExists = false;
		}
		else
		{
			newTowerBuilt = false;
			validPathExists = true;
			tileGrid->removePathGrassListeners(spawnX, spawnY, *mobPath);
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
		gridTouch();
		break;

	case ChangeSpeedInputEvent:
		changeGameSpeed();
		break;

	case PauseBtnInputEvent:
		return PausedMode;

	case UndoInputEvent:
		if(speedMode == ImmobileSpeedMode)
			invokeUndoTower();
		break;

	case DmgBtnInputEvent:
		invokeDmgBtn();
		break;

	case AsBtnInputEvent:
		if(towerAsUncapped())
			invokeBuySpeedBtn();
		break;

	case RangeBtnInputEvent:
		if(towerRangeUncapped())
		{
			invokeBuyRangeBtn();
			rangeUpgraded = true;
		}
		break;
	}
	return PlayMode;

}
//==============================================================================
void Game::render()
{
	//io->renderBg();
	tileGrid->render(io, tileSize);
	//renderTowers();
	renderWalls();
	renderMonsters();
	renderShots();

	ButtonState dmg, as, ran;

	if(towerDmgCounter < 3)
	{
		if(credits >= upgradeCost[towerDmgCounter])
			dmg = ActiveButtonState;
		else
			dmg = InactiveButtonState;
	}
	else
	{
		if(credits  >= 1000*(towerDmgCounter-1))
			dmg = ActiveButtonState;
		else
			dmg = InactiveButtonState;
	}

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

	io->renderButtons(mobsAlive, newTowerBuilt,
		as, dmg, ran, speedMode);

	io->setTextColor();
	io->renderLivesText(lives);
	io->renderWaveText(currWave);
	io->renderCreditsText(credits);
}
//==============================================================================
void Game::UpdateStats()//TODO rename
{
	currWave++;
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
		if(credits >= TOWER_PRICE && t != 0 && pathGrid->available(x,y))
		{
			Tower *newTower = new Tower(
				x * tileSize + verticalBorder,
				y * tileSize + horizontalBorder,
				tileSize, currWave);

			credits -= TOWER_PRICE;
			buildWalls(x, y);
			towers.push_back(newTower);
			tileGrid->addTower(newTower, x, y);
			newTowerBuilt = true;

			pathGrid->remove(x, y, *tileGrid);
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
	int topLeftY = y * tileSize + horizontalBorder;

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
				spawnY * tileSize + horizontalBorder,
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

		//pathGrid->print(spawnPtr, exitPtr); //std::cout << "====================================================\n";
	}
	//pathGrid->print(spawnPtr, exitPtr); std::cout << "====================================================\n";

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
		spawnY * tileSize + horizontalBorder);

	tileGrid->buildExit(exitX, exitY,
		exitX * tileSize + verticalBorder, 
		exitY * tileSize + horizontalBorder);

	for(int i=(rand() % 3) + 5; i > 0; i--)
	{
		int waterX = rand() % gridColumns,
			waterY = rand() % gridRows;

		for(int j = waterX-1; j <= waterX; j++)
			for(int k = waterY-1; k <= waterY; k++)
				if(tileGrid->validPoint(j,k))
					tileGrid->at(j,k)->setColor(SwampImage);
	}

	for(int i=(rand() % 3) + 5; i > 0; i--)
	{
		int waterX = rand() % gridColumns,
			waterY = rand() % gridRows;

		for(int j = waterX-1; j <= waterX; j++)
			for(int k = waterY-1; k <= waterY; k++)
				if(tileGrid->validPoint(j,k))
					tileGrid->at(j,k)->setColor(DesertImage);
	}

	if(!findShortestPath())
	{
		tileGrid->releaseTile(spawnX, spawnY);
		tileGrid->buildGrass(spawnX, spawnY,
			spawnX * tileSize + verticalBorder,
			spawnY * tileSize + horizontalBorder);

		tileGrid->releaseTile(exitX, exitY);
		tileGrid->buildGrass(exitX, exitY,
			spawnX * tileSize + verticalBorder,
			spawnY * tileSize + horizontalBorder);

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
	for(std::list<Tower*>::const_iterator it = towers.begin(); it != towers.end(); it++)
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
					monsterRadius)); //TODO, handle shot radius
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
			decreaseLifes();
			mobsAlive--;

		}
	}
}
//==============================================================================
void Game::decreaseLifes()
{
	lives--;
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
			Monster *wasShot = (*it)->getTarget();

			if(wasShot->isAlive() && wasShot->wasShot((*it)->getDmg()))
			{
				monsterDied(wasShot);
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
	credits += 1 + mobHp / 10;
	if(credits > MAX_CREDITS)
		credits = MAX_CREDITS;

	mobsAlive--;
	tileGrid->notifyTileExit(mon->getGridPosX(), 
		mon->getGridPosY(), mon->getMobId());
}
//=============================================================================
void Game::invokeUndoTower()
{
	int delX = (io->getLastTouchX() - verticalBorder) / tileSize,
		delY = (io->getLastTouchY() - horizontalBorder) / tileSize;
	Tower* t;

	if(t = dynamic_cast<Tower*> (tileGrid->at(delX, delY)))
	{
		if(t->builtThisWave(currWave))
			credits += TOWER_PRICE;
		else
			credits += TOWER_PRICE/2;

		if(credits > MAX_CREDITS)
			credits = MAX_CREDITS;

		tileGrid->releaseTile(delX, delY);
		tileGrid->buildGrass(delX, delY,
			delX * tileSize + verticalBorder,
			delY * tileSize + horizontalBorder);
		towers.remove(t); //TODO, remove without iterating

		pathGrid->add(delX, delY, *tileGrid);
	}
}
//==============================================================================
void Game::invokeBuySpeedBtn()
{
	if(credits >= upgradeCost[towerAsCounter])
	{
		credits -= upgradeCost[towerAsCounter];
		Tower::buffAs();
		towerAsCounter++;
	}
}
//==============================================================================
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
	if(towerDmgCounter < 3)
	{
		if(credits >= upgradeCost[towerDmgCounter])
		{
			credits -= upgradeCost[towerDmgCounter];
			Tower::buffDmg(2);
			towerDmgCounter++;
		}
	}
	else
	{
		int upgradeCost = 1000*(towerDmgCounter-1);
		if(credits >= upgradeCost)
		{
			credits -= upgradeCost;
			Tower::buffDmg(towerDmgCounter);
			towerDmgCounter++;
		}
	}
}

void Game::UpdatePathGrid()
{
	/*for(std::vector<Tower*>::const_iterator it = newTowers.begin();
	it != newTowers.end(); it++)
	*/
}

void Game::revertPathGridUpdate()
{
	/*for(std::vector<Tower*>::const_iterator it = newTowers.begin();
	it != newTowers.end(); it++)
	*/
}

Mode Game::manangePausedMode()
{
	Iw2DSurfaceClear(0xffff9900);
	render();
	return io->manangePausedMode();
}

Mode Game::manageTitleMode()
{
	return io->manageTitleMode();
}

Mode Game::manageGameEnded()
{
	Iw2DSurfaceClear(0xffff9900);
	render();
	return io->manageGameEnded(lives);
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
	if(speedMode == ImmobileSpeedMode)
	{
		speedMode = rememberSpeedMode;
		spawnNextWave = true;
	} 
	else
	{
		if(speedMode == NormalSpeedMode)
		{
			shotMoveSpeed	*= FAST_TO_SLOW_FACTOR;
			mobMoveSpeed	*= FAST_TO_SLOW_FACTOR;
			speedMode = rememberSpeedMode = FastSpeedMode;
			Tower::fastAs();
		}
		else
		{
			shotMoveSpeed	/= FAST_TO_SLOW_FACTOR;
			mobMoveSpeed	/= FAST_TO_SLOW_FACTOR;
			speedMode = rememberSpeedMode = NormalSpeedMode;
			Tower::slowAs();
		}
		setMonsterSpeed();
		setShotSpeed();
	}
}

void Game::cleanUp()
{
	for(std::vector<Monster*>::const_iterator it = monsters.begin(); it != monsters.end(); it++)
		delete (*it);
	monsters.clear();

	for(std::list<TrackingShot*>::const_iterator it = shots.begin(); it != shots.end(); it++)
		delete (*it);
	shots.clear();

	for(std::list<Wall*>::const_iterator it = walls.begin(); it != walls.end(); it++)
		delete (*it);
	walls.clear();

	towers.clear();
}

bool Game::towerAsUncapped() const
{
	return towerAsCounter < MAX_SPEED_LEVEL;
}

bool Game::towerRangeUncapped() const
{
	return towerRangeCounter < MAX_RANGE_LEVEL;
}
void Game::gridTouch()
{
	buildTower((io->getLastTouchX() - verticalBorder) / tileSize, 
		(io->getLastTouchY() - horizontalBorder) / tileSize);
}
//==============================================================================
void Game::renderShots() const
{
	Iw2DSetColour(0xffffffff);
	for(std::list<TrackingShot*>::const_iterator it = shots.begin(); 
		it != shots.end(); it++)
	{
		io->drawTile(ShotImage, (*it)->getTopLeftX(), (*it)->getTopLeftY(),
			shotDiam, shotDiam);
	}
}
//==============================================================================
void Game::renderWalls() const
{
	Iw2DSetColour(0xffffffff);

	for(std::list<Wall*>::const_iterator it = walls.begin(); it != walls.end(); it++)
		io->drawTile((*it)->getColor(), (*it)->getTopLeftX(), (*it)->getTopLeftY());
}
//==============================================================================
void Game::renderTowers() const
{
	//Iw2DSetColour(0xffffffff);

	//for(std::list<Tower*>::const_iterator it = towers.begin(); it != towers.end(); it++)
	//{
	//	io->drawTile((*it)->getColor(), (*it)->getTopLeftX(), (*it)->getTopLeftY(),
	//		tileSize, tileSize);
	//	renderWalls((*it));
	//	}
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
//==============================================================================
void Game::reloadUI()
{
	io->setUpUI(gridColumns, gridRows);
}
