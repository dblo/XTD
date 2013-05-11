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

const int N_MUDSPEED_INDEX		= 0;
const int N_GRASSSPEED_INDEX	= 1;
const int N_ICESPEED_INDEX		= 2;
const int F_MUDSPEED_INDEX		= 1;
const int F_GRASSSPEED_INDEX	= 3;
const int F_ICESPEED_INDEX		= 4;
const int N_SHOTSPEED_INDEX		= 3;
const int F_SHOTSPEED_INDEX		= 5;

const int upgradeCost[3]		= {100, 1000, 5000};
const int damageUpgrades[3]		= {2, 4, 8};
const int waveMonsterCount[3]	= {100, 200, 300};
const int upgradeTimes[3]		= {5, 15, 35};
//const int monsterHPs[MAX_WAVE];
int movementSpeeds[6];

Game::Game(int _tileSize) : tileSize(_tileSize)
{
	// Allocate and set up io object here since it is needed to render
	// all states of the game i.e. title screen.
	io = new Io(_tileSize);
	io->setUpUI(gridColumns, gridRows); 

	tileGrid	= 0;
	pathGrid	= 0;
	mobPath		= 0;
	roundProgressBar = 0;
	dmgProgressBar = 0;
	asProgressBar = 0;
	ranProgressBar = 0;
}
Game::~Game() 
{
	cleanUp();

	delete pathGrid;
	delete tileGrid;
	delete mobPath;
	delete io;
	delete roundProgressBar;
	delete dmgProgressBar;
	delete asProgressBar;
	delete ranProgressBar;
}
Mode Game::update()
{
	if(spawnNextWave) 
		onNewWave();

	spawnMonster();
	moveMobs();
	updateMobGrid();
	shoot();
	moveShots();
	manageCollisions();
	waveOverCheck();
	updateUpgrades();

	if(gameEnded())
		return EndedMode;
	return handleInput();
}
bool Game::gameEnded()
{
	if(lives == 0 || (currWave == MAX_WAVE && monstersAlive == 0))
	{
		manageGameEnded();
		return true;
	}
	return false;
}
void Game::reset()
{
	srand(time(NULL));
	Tower::resetTowers(tileSize);
	io->reset();
	io->initProgBars(&roundProgressBar, &dmgProgressBar,
		&asProgressBar, &ranProgressBar);

	tileGrid = new TileGrid(gridColumns, gridRows, tileSize);
	pathGrid = new PathGrid(gridColumns, gridRows);
	pathGrid->init();
	mobGridPos.reserve(MAX_MONSTER_COUNT);

	movementSpeeds[0] = tileSize / 24;
	movementSpeeds[1] = tileSize / 12;
	movementSpeeds[2] = tileSize / 8;
	movementSpeeds[3] = tileSize / 6;
	movementSpeeds[4] = tileSize / 4;
	movementSpeeds[5] = tileSize / 3;

	credits				= BASE_CREDITS+9000;
	monsterHP			= MONSTER_BASE_HP;
	spawnTimer			= 0;
	currWave			= 0;
	monstersAlive		= 0;
	lives				= 99;
	towerAsCounter		= 0;
	towerDmgCounter		= 0;
	towerRangeCounter	= 0;
	wallCap				= 10;
	shotDiameter		= (tileSize*2) / 5;
	monsterRadius		= tileSize / 3;
	spawnNextWave		= false;
	numOfCurrWaveMons	= BASE_MONSTER_COUNT;
	spawnNextMobId		= MAX_MONSTER_COUNT;
	border				= io->getBorder();
	gridOffset			= io->getOffset();
	speedMode			= ImmobileSpeedMode;
	rememberSpeedMode	= NormalSpeedMode;
	currSelected		= NothingSelected;

	for(int i=0; i < MAX_MONSTER_COUNT; i++)
		monsters.push_back(new Monster());

	// Allocate tiles as grass
	for(int i=0; i < gridColumns; i++)
		for(int j=0; j < gridRows; j++)
			tileGrid->buildGrass(i, j, 
			i*tileSize + border, 
			j*tileSize + gridOffset);

	generateMap();

	// Set initial path
	findShortestPath();
}
void Game::onNewWave()
{
	spawnNextMobId = 0;
	numOfCurrWaveMons = waveMonsterCount[currWave];
	spawnNextWave = false;
	monstersAlive = numOfCurrWaveMons;

	// Increase monsters hp depending on wavenumber
	if(currWave < (MAX_WAVE / 5))
		monsterHP += currWave / 2;
	else if(currWave < (MAX_WAVE * 2) / 5)
		monsterHP += currWave;
	else if(currWave < (MAX_WAVE * 3) / 5)
		monsterHP += (currWave * 3) / 2;
	else if(currWave < (MAX_WAVE * 4) / 5)
		monsterHP += currWave * 2;
	else
		monsterHP += (currWave * 5) / 2;

	pathGrid->setAllUnvisited();
	if(findShortestPath())
	{
		removePathGrassListeners();
		setPathGrassListeners();
	}
	currWave++;
}
Mode Game::handleInput()
{
	switch(io->handleInput(currSelected == StructureSelected))
	{
	case DoNothingInputEvent:
		break;

	case GridInputEvent:
		gridTouch();
		break;

	case MenuEvent:
		currSelected = NothingSelected;
		break;

	case ChangeSpeedInputEvent:
		changeGameSpeed();
		break;

	case PauseBtnInputEvent:
		return PausedMode;

	case Btn2Event:
		if(towerDmgUncapped())
			invokeUpgradeDmgBtn();
		break;

	case Btn3Event:
		if(towerAsUncapped())
			invokeUpgradeSpeedBtn();
		break;

	case Btn4Event:
		if(towerRangeUncapped())
			invokeUpgradeRangeBtn();
		break;

	case ClearEvent:
		currSelected = NothingSelected;
		break;
	}
	return PlayMode;
}
void Game::render()
{
	io->renderBg();
	tileGrid->render(io, tileSize);

	renderWalls();
	renderTowers();
	renderSpawnExit();
	renderMonsters();
	renderShots();
	renderText();

	if(io->menuShowing() || currSelected == StructureSelected)
		io->renderMenuBG();

	renderButtons();
	renderProgressBars();
	renderSelected();
	io->renderMenuBtn();
}
void Game::renderButtons() const
{
	if(io->menuShowing() || currSelected == StructureSelected)
	{
		if(speedMode == ImmobileSpeedMode)
			io->renderPlayButton();
		else if(speedMode == NormalSpeedMode)
			io->renderNormalSpeedButton();
		else
			io->renderFastSpeedButton();

		if(towerDmgUncapped())
		{
			if(credits >= upgradeCost[towerDmgCounter] && !dmgProgressBar->isActive())
				io->renderUpgDmgButton(true);
			else
				io->renderUpgDmgButton(false);
		} else if(dmgProgressBar->isActive())
			io->renderUpgDmgButton(false);

		if(towerAsUncapped())
		{
			if(credits >= upgradeCost[towerAsCounter]&& !asProgressBar->isActive())
				io->renderUpgSpdButton(true);
			else
				io->renderUpgSpdButton(false);
		} else if(asProgressBar->isActive())
			io->renderUpgSpdButton(false);

		if(towerRangeUncapped())
		{
			if(credits >= upgradeCost[towerRangeCounter] && !ranProgressBar->isActive())
				io->renderUpgRangeButton(true);
			else
				io->renderUpgRangeButton(false);
		} else if(ranProgressBar->isActive())
			io->renderUpgRangeButton(false);

		io->renderPauseButton();
	}
}
void Game::renderText() const
{
	io->setTextColor(true);

	char str[8];
	sprintf(str, "C %d", credits);
	io->renderText(str, CreditsText);

	sprintf(str, "L %d", lives);
	io->renderText(str, LivesText);

	sprintf(str, "R %d", currWave);
	io->renderText(str, WaveText);

	/*sprintf(str, "W %d", wallCap - walls.size());
	io->renderText(str, WallsText);*/

	sprintf(str, "$ %d", 1234);
	io->renderText(str, InfoText);

	io->setTextColor(false);
}
void Game::waveOverCheck()
{
	if(monstersAlive == 0 && speedMode == rememberSpeedMode)
	{
		speedMode = ImmobileSpeedMode;
		wallCap += 10;
	}
}
bool Game::canAddWall()
{
	return walls.size() < wallCap;
}
void Game::wallTouch(int x, int y)
{

	//else if(!isTower(x, y))
	//removeWall(x, y);
}
void Game::updateWall(int x, int y)
{
	Image wallImage = getWallImage(findNeighbours(x, y, false));
	Wall * wall = walls.find(getKey(x, y))->second;
	wall->setImage(wallImage);
}
unsigned int Game::findNeighbours(int x, int y, bool updateNeighbours)
{
	unsigned int neighbours = 0;
	if(y > 0 && isWall(x, y-1))
	{
		neighbours |= 1; // Neighbour above
		if(updateNeighbours)
			updateWall(x, y-1);
	}

	if(y < gridRows-1 && isWall(x, y+1))
	{
		neighbours |= 2; // Neighbour below
		if(updateNeighbours)
			updateWall(x, y+1);
	}

	if(x > 0 && isWall(x-1, y))
	{
		neighbours |= 8; // Neighbour on the left
		if(updateNeighbours)
			updateWall(x-1, y);
	}

	if(x < gridColumns - 1 && isWall(x+1,y))
	{
		neighbours |= 4;// Neighbour on the right
		if(updateNeighbours)
			updateWall(x+1, y);
	}
	return neighbours;
}
void Game::buildWall(int x, int y)
{
	pathGrid->disconnect(x,y);
	Image wallImage = getWallImage(findNeighbours(x, y, true));
	walls.insert(makeWallElement(x, y, new Wall(wallImage,
		x * tileSize + border,
		y * tileSize + gridOffset)));
}
Image Game::getWallImage(unsigned int neighbours) const
{
	// Uses most common type as init val
	Image wallType = UdWallImage; 
	switch(neighbours)
	{
	case 0:
		wallType = CrossWallImage;
		break;
		/*case 1:
		wallType = UdWallImage;
		break;
		case 2:
		wallType = UdWallImage;
		break;
		case 3:
		wallType = UdWallImage;
		break;*/
	case 4:
		wallType = RlWallImage;
		break;
	case 5:
		wallType = RuWallImage;
		break;
	case 6:
		wallType = RdWallImage;
		break;
	case 7:
		wallType = UdrWallImage;
		break;
	case 8:
		wallType = RlWallImage;
		break;
	case 9:
		wallType = LuWallImage;
		break;
	case 10:
		wallType = LdWallImage;
		break;
	case 11:
		wallType = UdlWallImage;
		break;
	case 12:
		wallType = RlWallImage;
		break;
	case 13:
		wallType = RluWallImage;
		break;
	case 14:
		wallType = RldWallImage;
		break;
	case 15:
		wallType = CrossWallImage;
		break;
	}
	return wallType;
}
void Game::buildTower(int x, int y)
{
	Tower *newTower = new Tower(
		x * tileSize + border,
		y * tileSize + gridOffset,
		tileSize, currWave);

	credits -= TOWER_PRICE;
	towers.insert(makeTowerElement(x, y, newTower));
}
//==============================================================================
//void Game::buildWater(int x, int y)
//{
//	if(tileGrid->get(x, y)->getColor() == GrassImage)
//	{
//		tileGrid->buildWater(x, y);
//		pathGrid->remove(x, y, *tileGrid);
//	}
//}
//==============================================================================
void Game::spawnMonster()
{
	if(spawnTimer == 0)
	{
		if(spawnNextMobId < numOfCurrWaveMons)
		{
			monsters[spawnNextMobId]->spawn(
				spawnX, 
				spawnY,
				spawnX * tileSize + border,
				spawnY * tileSize + gridOffset,
				monsterHP, 
				spawnNextMobId,
				monsterRadius, 
				tileSize);

			setMonsterSpeed(monsters[spawnNextMobId], spawnX, spawnY);

			mobGridPos[spawnNextMobId].first = spawnX;
			mobGridPos[spawnNextMobId].second = spawnY;
			spawnNextMobId++;

			if(spawnNextMobId % 25)
				spawnTimer = MONSTER_SPAWN_INTERVAL;
			else
				spawnTimer = 20*MONSTER_SPAWN_INTERVAL;
		}
	}
	else
		spawnTimer--;
}
void Game::updateMobGrid()
{
	int newX, newY;
	for(int i=0; i < numOfCurrWaveMons; i++)
	{
		if(monsters[i]->updatePosition())
		{
			tileGrid->notifyTileExit(mobGridPos[i].first, mobGridPos[i].second, i);
			newX = monsters[i]->getGridPosX();
			newY = monsters[i]->getGridPosY();
			mobGridPos[i].first = newX;
			mobGridPos[i].second = newY;
			tileGrid->notifyTileEnter(newX, newY, i);

			// Update monster speed if entering different terraint type
			setMonsterSpeed(monsters[i], newX, newY);
			monsters[i]->positionUpdated();
		}
	}
}
// Traverses the path between exit and spawn, using that every node store
// the direction to move in. Builds up a string that represent the path.
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
// Uses a BFS algorithm in attempting to find a shortest path from spawn to 
// every node until exit has been found. If a path to exit was found, calls 
// backtrack to create a string containing the path from exit to spawn and 
// then reverse it. If no path was found, the previous path will not be updated.
// Returns true if a shortest path was found.
bool Game::findShortestPath()
{
	std::string shortestPath = "";
	std::queue<pvPtr> pq;
	bool foundPath = false;
	const pvPtr spawnPtr = pathGrid->at(spawnX, spawnY);
	const pvPtr exitPtr = pathGrid->at(exitX, exitY);
	PathingVertex *p;

	pq.push(spawnPtr);
	while(!pq.empty())
	{
		p = pq.front();

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
int Game::getKey(int x, int y) const
{
	return x*100+y;
}
void Game::generateMap()
{
	spawnX = 0;
	spawnY = 0;//rand() % gridRows;
	exitX = gridColumns-1;
	exitY = 0;//rand() % gridRows;

	for(int i = 0; i < gridColumns; i++)
	{
		for(int j = 0; j < gridRows; j++)
			if(!(rand() % 3))
				tileGrid->setIce(i,j);
	}
	for(int i = 0; i < gridColumns; i++)
	{
		for(int j = 0; j < gridRows; j++)
			if(!(rand() % 3))
				tileGrid->setMud(i,j);
	}
}
void Game::shoot()
{
	int target;
	Monster *tarMon;
	Tower *tower;
	for(TowerMapConstIter it = towers.begin(); it != towers.end(); it++)
	{
		tower = (*it).second;
		if(tower->armed())
		{
			target = tower->acquireTarget(numOfCurrWaveMons);
			while(target < numOfCurrWaveMons)
			{
				tarMon = monsters[target];
				if(tower->targetInRange(tarMon->getCenterX(), 
					tarMon->getCenterY(),
					tarMon->getRadius()))
					break; // Locked onto target
				else
					// Out of range, aquire new target
					target = tower->acquireTarget(numOfCurrWaveMons, ++target);
			}

			if(target < numOfCurrWaveMons)
			{
				shots.push_back(new TrackingShot(
					tower->getCenterX(),
					tower->getCenterY(),
					tarMon, 
					tower->shoot(),
					shotDiameter / 2));

				setShotSpeed(shots.back());
			}
		}
		else
		{
			tower->reloadTick();
		}
	}
}
void Game::moveMobs()
{
	for(int i=0; i < numOfCurrWaveMons; i++)
	{
		if(monsters[i]->isAlive() && 
			!monsters[i]->move(*mobPath, tileSize))
		{
			lives--;
			monstersAlive--;
			tileGrid->notifyTileExit(monsters[i]->getGridPosX(), 
				monsters[i]->getGridPosY(), monsters[i]->getMobId());
		}
	}
}
void Game::moveShots()
{
	for(ShotsConstIter it = shots.begin(); it != shots.end(); it++)
	{
		(*it)->move();
	}
}
void Game::manageCollisions()
{
	std::list<TrackingShot*>::iterator del ;
	for(std::list<TrackingShot*>::iterator it = shots.begin(); it != shots.end();)
	{
		if((*it)->colliding())
		{
			Monster *collidingMonster = (*it)->getTarget();

			if(collidingMonster->isAlive() && 
				collidingMonster->takeDamage((*it)->getDmg()))
			{
				monsterDied(collidingMonster);
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
	credits += 1 + monsterHP / 10;
	if(credits > MAX_CREDITS)
		credits = MAX_CREDITS;

	tileGrid->notifyTileExit(mon->getGridPosX(), 
		mon->getGridPosY(), mon->getMobId());

	monstersAlive--;
}
void Game::invokeUpgradeSpeedBtn()
{
	if(asProgressBar->isActive())
	{
		towerAsCounter--;
		addCredits(upgradeCost[towerAsCounter]);
		asProgressBar->abort();
	}
	else if(purchase(upgradeCost[towerAsCounter]))
	{
		asProgressBar->start((int)s3eTimerGetMs(), 
			upgradeTimes[towerAsCounter]);
		towerAsCounter++;
	}
}
void Game::invokeUpgradeRangeBtn()
{
	if(ranProgressBar->isActive())
	{
		towerRangeCounter--;
		addCredits(upgradeCost[towerRangeCounter]);
		ranProgressBar->abort();
	}
	else if(purchase(upgradeCost[towerRangeCounter]))
	{
		ranProgressBar->start((int)s3eTimerGetMs(), 
			upgradeTimes[towerRangeCounter]);
		towerRangeCounter++;
	}
}
void Game::invokeUpgradeDmgBtn()
{
	if(currSelected != StructureSelected)
	{
		if(dmgProgressBar->isActive())
		{
			towerDmgCounter--;
			addCredits(upgradeCost[towerDmgCounter]);
			dmgProgressBar->abort();
		}
		else if(purchase(upgradeCost[towerDmgCounter]))
		{
			dmgProgressBar->start((int)s3eTimerGetMs(), 
				upgradeTimes[towerDmgCounter]);
			towerDmgCounter++;
		}
		currSelected = Button2Selected;
	}
	else
	{
		buildTower(selectedX, selectedY);
		select(NothingSelected);
	}
}
Mode Game::manangePausedMode()
{
	Iw2DSurfaceClear(0xFF4E4949);
	render();
	return io->manangePausedMode();
}
Mode Game::manageTitleMode()
{
	Iw2DSurfaceClear(0xFF4E4949);
	return io->manageTitleMode();
}
Mode Game::manageGameEnded()
{
	Iw2DSurfaceClear(0xFF4E4949);
	render();
	return io->manageGameEnded(lives);
}
void Game::setMonsterSpeed(Monster *mon, int gridPosX, int gridPosY)
{
	switch(tileGrid->getTileType(gridPosX, gridPosY))
	{
	case MudImage:
		if(speedMode == NormalSpeedMode)
			mon->setMs(movementSpeeds[N_MUDSPEED_INDEX]);
		else
			mon->setMs(movementSpeeds[F_MUDSPEED_INDEX]);
		break;
	case IceImage:
		if(speedMode == NormalSpeedMode)
			mon->setMs(movementSpeeds[N_ICESPEED_INDEX]);
		else
			mon->setMs(movementSpeeds[F_ICESPEED_INDEX]);
		break;
	default: // grass, spawn, exit
		if(speedMode == NormalSpeedMode)
			mon->setMs(movementSpeeds[N_GRASSSPEED_INDEX]);
		else if(speedMode == FastSpeedMode)
			mon->setMs(movementSpeeds[F_GRASSSPEED_INDEX]);
		else // ImmobileSpeed
		{
			if(rememberSpeedMode == NormalSpeedMode)
				mon->setMs(movementSpeeds[N_GRASSSPEED_INDEX]);
			else
				mon->setMs(movementSpeeds[F_GRASSSPEED_INDEX]);
		}
		break;
	}
}
void Game::setShotSpeed(TrackingShot *shot)
{
	if(speedMode == NormalSpeedMode)
		shot->setMs(movementSpeeds[N_SHOTSPEED_INDEX]);
	else
		shot->setMs(movementSpeeds[F_SHOTSPEED_INDEX]);
}
void Game::changeGameSpeed()
{
	if(speedMode == ImmobileSpeedMode)
	{
		//Start new round
		speedMode = rememberSpeedMode;
		spawnNextWave = true;
	} 
	else 
	{
		if(speedMode == NormalSpeedMode)
		{
			speedMode = rememberSpeedMode = FastSpeedMode;
			Tower::fastAs();
		}
		else
		{
			speedMode = rememberSpeedMode = NormalSpeedMode;
			Tower::slowAs();
		}

		for(int i=0; i < numOfCurrWaveMons; i++)
			setMonsterSpeed(monsters[i], 
			monsters[i]->getGridPosX(), 
			monsters[i]->getGridPosY());

		for(ShotsConstIter it = shots.begin(); it != shots.end(); it++)
			setShotSpeed((*it));
	}
}
void Game::cleanUp()
{
	for(MonsterConstIter it = monsters.begin(); it != monsters.end(); it++)
		delete (*it);
	monsters.clear();

	for(ShotsConstIter it = shots.begin(); it != shots.end(); it++)
		delete (*it);
	shots.clear();

	for(WallMapConstIter it = walls.begin(); it != walls.end(); it++)
		delete (*it).second;
	walls.clear();

	for(TowerMapConstIter it = towers.begin(); it != towers.end(); it++)
		delete (*it).second;
	towers.clear();
}
bool Game::towerDmgUncapped() const
{
	return towerDmgCounter < MAX_DAMAGE_LEVEL;
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
	int gridPosX = getGridCoordX(io->getLastTouchX());
	int gridPosY = getGridCoordY(io->getLastTouchY());

	if(notSpawnOrExit(gridPosX, gridPosY))
	{
		if(speedMode != ImmobileSpeedMode)
		{
			if(isWall(gridPosX, gridPosY))
				select(StructureSelected, gridPosX, gridPosY);
			else 
				currSelected = NothingSelected;
		}
		else
		{
			if(isWall(gridPosX, gridPosY))
				select(StructureSelected, gridPosX, gridPosY);
			else
			{
				if(canAddWall())
					buildWall(gridPosX, gridPosY);

				currSelected = NothingSelected;
			}
		}
	}
	else
		currSelected = NothingSelected;
}
int Game::getGridCoordX(int xVal) const
{
	return (xVal - border) / tileSize;
}
int Game::getGridCoordY(int yVal) const
{
	return (yVal- gridOffset) / tileSize;
}
void Game::renderShots() const
{
	for(ShotsConstIter it = shots.begin(); it != shots.end(); it++)
	{
		io->drawTile(ShotImage, 
			(*it)->getTopLeftX(), 
			(*it)->getTopLeftY(),
			shotDiameter, shotDiameter);
	}
}
void Game::renderWalls() const
{
	for(WallMapConstIter it = walls.begin(); it != walls.end(); it++)
	{
		io->drawTile((*it).second->getImage(), 
			(*it).second->getTopLeftX(), 
			(*it).second->getTopLeftY(),
			tileSize, tileSize);
	}
}
void Game::renderTowers() const
{
	for(TowerMapConstIter it = towers.begin(); it != towers.end(); it++)
	{
		io->drawTile(TowerImage,// (*it).second->getImage(), 
			(*it).second->getTopLeftX(), 
			(*it).second->getTopLeftY(), 
			tileSize, tileSize);
	}
}
void Game::renderMonsters() const
{
	for(MonsterConstIter it = monsters.begin(); it != monsters.end(); it++)
	{
		if((*it)->isAlive()) 
		{
			io->drawTile(MonsterImage, (*it)->getTopLeftX(), 
				(*it)->getTopLeftY(), (tileSize*4)/5, (tileSize*4)/5); //TODO new monster images
		}
	}
}
void Game::setUI()
{
	io->setUpUI(gridColumns, gridRows);
}
bool Game::notSpawnOrExit(int x, int y) const
{
	return (x != spawnX || y != spawnY) && (x != exitX || y != exitY);
}
TowerElement Game::makeTowerElement(int x, int y, Tower *t)
{
	return TowerElement(getKey(x, y), t);
}
WallElement Game::makeWallElement(int x, int y, Wall *w)
{
	return WallElement(getKey(x, y), w);
}
void Game::setPathGrassListeners()
{
	int xTrav = spawnX,
		yTrav = spawnY;
	unsigned int nxtInstr = 0;

	while(nxtInstr < mobPath->length())
	{
		switch((*mobPath)[nxtInstr])
		{
		case 'r':
			xTrav++;
			break;
		case 'u':
			yTrav--;
			break;
		case 'l':
			xTrav--;
			break;
		case 'd':
			yTrav++;
			break;
		}
		nxtInstr++;

		int xHiLim = xTrav+2;
		int yHiLim = yTrav+2;
		TowerMapConstIter it;

		// Traverse the 5*5 grid centered at xTrav, yTrav and add all towers in
		// that grid as listeners to the path-grass at xTrav, yTrav
		for(int x = xTrav-2; x <= xHiLim; x++)
			for(int y = yTrav-2; y <= yHiLim; y++)
			{
				if(tileGrid->validPoint(x, y))
				{
					if((it = towers.find(getKey(x, y))) != towers.end())
					{
						tileGrid->setListener(xTrav, yTrav, (*it).second);
					}
				}
			}
	}
}
void Game::removePathGrassListeners()
{
	int x = spawnX,
		y = spawnY;
	unsigned int nxtInstr = 0;

	while(nxtInstr < mobPath->length())
	{
		switch((*mobPath)[nxtInstr])
		{
		case 'r':
			x++;
			break;
		case 'u':
			y--;
			break;
		case 'l':
			x--;
			break;
		case 'd':
			y++;
			break;
		}
		nxtInstr++;

		tileGrid->removeListener(x, y);
	}	
}
void Game::removeWall(int x, int y)
{
	pathGrid->connect(x,y);
	std::map<int, Wall*>::iterator it = walls.find(getKey(x, y));
	delete it->second;
	walls.erase(it);

	// Update adjacent walls
	if((it = walls.find(getKey(x, y-1))) != walls.end())
		updateWall(x, y-1);
	if((it = walls.find(getKey(x, y+1))) != walls.end())
		updateWall(x, y+1);
	if((it = walls.find(getKey(x-1, y))) != walls.end())
		updateWall(x-1, y);
	if((it = walls.find(getKey(x+1, y))) != walls.end())
		updateWall(x+1, y);
}
Image Game::getTileType(int x, int y) const
{
	return tileGrid->getTileType(x, y);
}
void Game::towerTouch(int x, int y)
{
	if(isWall(x, y))
	{
		if(!isTower(x, y))
		{
			if(credits >= TOWER_PRICE)
				buildTower(x, y);
		}
		else
			removeTower(x, y);
	}
}
void Game::removeTower(int x, int y)
{
	towers.erase(getKey(x, y));
	credits += TOWER_PRICE;
	if(credits > MAX_CREDITS)
		credits = MAX_CREDITS;
}
bool Game::isTower(int x, int y) const
{
	return towers.find(getKey(x,y)) != towers.end();
}
bool Game::isWall(int x, int y) const
{
	return !pathGrid->isConnected(x,y);
}
void Game::renderProgressBars() const
{
	if(dmgProgressBar->isActive())
		io->renderProgressBar(dmgProgressBar);

	if(asProgressBar->isActive())
		io->renderProgressBar(asProgressBar);

	if(ranProgressBar->isActive())
		io->renderProgressBar(ranProgressBar);
}
void Game::updateUpgrades()
{
	//	roundProgressBar->tick();

	if(dmgProgressBar->tick((int)s3eTimerGetMs()))
	{
		Tower::buffDmg(damageUpgrades[towerDmgCounter]);
	}

	if(asProgressBar->tick((int)s3eTimerGetMs()))
	{
		Tower::buffAs();
	}

	if (ranProgressBar->tick((int)s3eTimerGetMs()))
	{
		Tower::buffRange();
	}
}
void Game::renderSpawnExit() const
{
	io->drawTile(SpawnImage, spawnX*tileSize + border, 
		spawnY*tileSize + gridOffset, tileSize, tileSize);
	io->drawTile(ExitImage, exitX*tileSize + border,
		exitY*tileSize + gridOffset, tileSize, tileSize);
}
void Game::addCredits( int addAmount )
{
	credits += addAmount;
	if(credits > MAX_CREDITS)
		credits = MAX_CREDITS;
}
bool Game::purchase( int amount )
{
	if(credits >= amount)
	{
		credits -= amount;
		return true;
	}
	return false;
}
void Game::select( Selected selected, int x, int y )
{
	currSelected = selected;
	selectedX = x;
	selectedY = y;
}
void Game::renderSelected() const
{
	switch(currSelected)
	{
	case NothingSelected:
		break;
	case StructureSelected:
		io->renderTileSelected(selectedX*tileSize + border,
			selectedY*tileSize + gridOffset);
		break;
		//case Button2Selected:
		//	io->renderButtonSelected(BuyDamageButton);
		//	break;
		//case Button3Selected:
		//	io->renderButtonSelected(BuySpeedButton);
		//	break;
		//case Button4Selected:
		//	io->renderButtonSelected(BuyRangeButton);
		//	break;
	}
}

void Game::renderSelectedText() const
{

}
