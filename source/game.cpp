#include <iostream> //debug
#include <vector>
#include <queue>
#include <time.h>
#include <time.h>
#include <stdio.h>

#include "game.h"
#include "redTowerBase.h"
#include "tealTowerBase.h"
#include "yellowTowerBase.h"
#include "Iw2D.h"
#include "s3eKeyboard.h"
#include "s3ePointer.h"


const int N_MUDSPEED_INDEX		= 0;
const int N_GRASSSPEED_INDEX	= 1;
const int N_ICESPEED_INDEX		= 2;
const int F_MUDSPEED_INDEX		= 1;
const int F_GRASSSPEED_INDEX	= 2;
const int F_ICESPEED_INDEX		= 3;
const int N_SHOTSPEED_INDEX		= 4;
const int F_SHOTSPEED_INDEX		= 5;
const int upgradeCost[3]		= {100, 1000, 5000};
const int damageUpgrades[3]		= {5, 20, 40};
const int speedUpgrades[3]		= {40, 30, 20};
const int waveMonsterCount[MAX_WAVE] = {100, 150, 200, 250, 300, 300, 300,};
const int upgradeTimes[3]		= {10, 20, 40};
//const int monsterHPs[MAX_WAVE];

Game::Game(int _tileSize, Io * _io) : tileSize(_tileSize), io(_io)
{
	tileSize = _tileSize;
	setUI();

	tileGrid		= 0;
	pathGrid		= 0;
	mobPath			= 0;
}
Game::~Game() 
{
	cleanUp();
	delete damageProgressBar;
	delete speedProgressBar;
	delete rangeProgressBar;
}
void Game::reset()
{
	srand(time(NULL));
	io->reset();

	tileGrid = new TileGrid(gridColumns, gridRows, tileSize);
	pathGrid = new PathGrid(gridColumns, gridRows);
	pathGrid->init();
	mobGridPos.reserve(MAX_MONSTER_COUNT);

	mobPath = new std::string("");

	movementSpeeds[0] = tileSize / 27;
	movementSpeeds[1] = movementSpeeds[0] * 3;
	movementSpeeds[2] = movementSpeeds[0] * 6;
	//movementSpeeds[3] = movementSpeeds[0] * 8;
	movementSpeeds[4] = movementSpeeds[0] * 7;
	//movementSpeeds[5] = movementSpeeds[0] * 10;

	spawnNextWave		= false;
	showMenu			= false;
	pathFound			= true;	
	spawnInterval		= BASE_SPAWN_INTERVAL;
	towerBaseRange		= tileSize;
	credits				= BASE_CREDITS+9000;
	monsterHP			= MONSTER_BASE_HP;
	holdingGridCounter	= 0;
	holdingTileX		= 0;
	holdingTileY		= 0;
	spawnTimer			= 0;
	towerDamage			= TOWER_BASE_DMG;
	towerSpeed			= TOWER_BASE_SPEED;
	towerRange			= tileSize;
	currWave			= 0;
	monstersAlive		= 0;
	lives				= 123;
	towerAsCounter		= 0;
	towerDamageCounter	= 0;
	towerRangeCounter	= 0;
	wallCap				= 8;
	wallInc				= 7;
	monsterDiam			= (tileSize*2) / 3;
	monsterRadius		= (tileSize*2) / 6;
	shotDiameter		= (tileSize*2) / 5;
	pathMark			= (shotDiameter*2) / 3;
	numOfCurrWaveMons	= BASE_MONSTER_COUNT;
	spawnNextMobId		= MAX_MONSTER_COUNT;
	border				= io->getBorder();
	gridOffset			= io->getOffset();
	speedMode			= ImmobileSpeedMode;
	rememberSpeedMode	= NormalSpeedMode;
	gridSelection		= NothingSelected;
	btnSelection		= NothingSelected;

	damageProgBarRemainder = 0;
	speedProgBarRemainder = 0;
	rangeProgBarRemainder = 0;

	for(int i=0; i < MAX_MONSTER_COUNT; i++)
		monsters.push_back(new Monster());

	tileGrid->init(gridOffset, border, tileSize);
	resetProgBars();
	generateMap();

	// Set initial path
	findShortestPath();

	// Set up the isPartOfPath att in tiles
	setPathGrassListeners();
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
void Game::onNewWave()
{
	spawnNextMobId = 0;
	numOfCurrWaveMons = waveMonsterCount[currWave];
	monstersAlive = numOfCurrWaveMons;
	spawnNextWave = false;
	currWave++;

	if(damageProgressBar->upgrading())
	{
		damageProgressBar->setActive(true);

		if(damageProgBarRemainder > 0)
			damageProgressBar->compensatePause((int)s3eTimerGetMs() + 
			damageProgBarRemainder);
		damageProgBarRemainder = 0;
	}

	if(speedProgressBar->upgrading())
	{
		speedProgressBar->setActive(true);

		if(speedProgBarRemainder > 0)
			speedProgressBar->compensatePause((int)s3eTimerGetMs() + 
			speedProgBarRemainder);
		speedProgBarRemainder = 0;
	}

	if(rangeProgressBar->upgrading())
	{
		rangeProgressBar->setActive(true);

		if(rangeProgBarRemainder > 0)
			rangeProgressBar->compensatePause((int)s3eTimerGetMs() + 
			rangeProgBarRemainder);
		rangeProgBarRemainder = 0;
	}
}
Mode Game::handleInput()
{
	switch(io->handleInput(showMenu))
	{
	case DoNothingInputEvent:
		break;

	case GridInputEvent:
		gridTouch();
		break;

	case MenuEvent:
		invokeMenuBtn();
		break;

	case PlayInputEvent:
		if(speedMode == ImmobileSpeedMode)
			changeGameSpeed();
		break;

	case PauseBtnInputEvent:
		if(speedMode != ImmobileSpeedMode)
			pauseProgBars();
		return PausedMode;

	case Btn1Event:
		handleButtonEvent(Button1Selected);
		break;

	case Btn2Event:
		handleButtonEvent(Button2Selected);
		break;

	case Btn3Event:
		handleButtonEvent(Button3Selected);
		break;

	case SellInputEvent:
		if(gridSelection == TowerSelected ||
			gridSelection == WallSelected)
			sellStructure();
		else if(btnSelection != NothingSelected)
			cancelUpgrade();
		break;

	case ClearEvent:
		clearSelect();
		showMenu = false;
		break;

	case HoldingGridEvent:
		updateGridHold();
		break;

	case ReleasedGridEvent:
		releaseGridHold();
		break;
	}
	return PlayMode;
}
void Game::render()
{
	Iw2DSurfaceClear(0);
	io->renderBg();
	tileGrid->render(io, tileSize); 
	if(speedMode == ImmobileSpeedMode)
		renderPath();
	renderWalls();
	renderTowers();
	io->renderSpawn(spawnX*tileSize + border, 
		spawnY*tileSize + gridOffset, 
		tileSize);
	io->renderExit(exitX*tileSize + border, 
		exitY*tileSize + gridOffset, 
		tileSize);
	renderMonsters();
	renderShots();
	renderText();

	if(showMenu)
		io->renderMenuBG();

	renderButtons();
	renderProgressBars();
	io->renderUpgradeButton();
	renderStructSelection();
}
void Game::renderButtons() const
{
	if(showMenu)
	{
		if(gridSelection == NothingSelected)
			renderGlobalUpgradeButtons();
		else if(gridSelection == WallSelected)
			renderWallUpgradeButtons();
		else
			renderTowerUpgradeButtons();

		bool sellActive = false;
		if(gridSelection == TowerSelected || // TODO better imp
			(speedMode == ImmobileSpeedMode &&
			gridSelection == WallSelected) ||
			(gridSelection == NothingSelected && 
			((btnSelection == Button1Selected && 
			damageProgressBar->upgrading()) ||
			(btnSelection == Button2Selected &&
			speedProgressBar->upgrading()) ||
			(btnSelection == Button3Selected) &&
			rangeProgressBar->upgrading())))
			sellActive = true;
		io->renderSellBtn(sellActive);
	}
	if(speedMode == ImmobileSpeedMode)
	{
		if(speedMode == ImmobileSpeedMode)
			io->renderPlayButton();
		else if(speedMode == NormalSpeedMode)
			io->renderNormalSpeedButton();
		else
			io->renderFastSpeedButton();
	}
	io->renderPauseButton();
	io->renderSendButton();
}
void Game::renderText() const
{
	io->setTextColor(true);

	char str[30];
	sprintf(str, "C %d", credits);
	io->renderText(str, CreditsText);

	sprintf(str, "W %d", wallCap - walls.size());
	io->renderText(str, WallText);

	sprintf(str, "L %d", lives);
	io->renderText(str, LivesText);

	sprintf(str, "Round %d", currWave);
	io->renderText(str, WaveText);

	if(gridSelection == NothingSelected && 
		btnSelection != NothingSelected)
		RenderBasicUpgText(str);
	else if(gridSelection == WallSelected)
		renderUpgWallTxt(str);
	else
		renderUpgTowerTxt(str);

	io->setTextColor(false);

}
void Game::waveOverCheck()
{
	if(monstersAlive == 0 && speedMode == rememberSpeedMode)
	{
		speedMode = ImmobileSpeedMode;
		monsterHP *= 2;
		wallCap += wallInc;

		if(wallCap >1)
			wallInc -= 1;

		if(damageProgressBar->isActive())
		{
			damageProgressBar->setActive(false);
			damageProgBarRemainder = 
				damageProgressBar->getRemaining((int)s3eTimerGetMs());
		}

		if(speedProgressBar->isActive())
		{
			speedProgressBar->setActive(false);
			speedProgBarRemainder = 
				speedProgressBar->getRemaining((int)s3eTimerGetMs());
		}

		if(rangeProgressBar->isActive())
		{
			rangeProgressBar->setActive(false);
			rangeProgBarRemainder = 
				rangeProgressBar->getRemaining((int)s3eTimerGetMs());
		}
	}
}
bool Game::canAddWall()
{
	return walls.size() < wallCap;
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
	if(y > 0 && isBuilt(x, y-1))
	{
		neighbours |= 1; // Neighbour above
		if(updateNeighbours)
			updateWall(x, y-1);
	}

	if(y < gridRows-1 && isBuilt(x, y+1))
	{
		neighbours |= 2; // Neighbour below
		if(updateNeighbours)
			updateWall(x, y+1);
	}

	if(x > 0 && isBuilt(x-1, y))
	{
		neighbours |= 8; // Neighbour on the left
		if(updateNeighbours)
			updateWall(x-1, y);
	}

	if(x < gridColumns - 1 && isBuilt(x+1,y))
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
void Game::buildRedTower(int x, int y)
{
	Tower *newTower = new RedTowerBase(
		x * tileSize + border,
		y * tileSize + gridOffset,
		tileSize, TIER1_TOWER_PRICE,
		towerDamage, towerSpeed, towerRange);

	addTower(makeTowerElement(x, y, newTower), TIER1_TOWER_PRICE);
	tileGrid->setTowerAsListener(x, y, newTower, tileSize);
}
void Game::buildTealTower(int x, int y)
{
	Tower *newTower = new TealTowerBase(
		x * tileSize + border,
		y * tileSize + gridOffset,
		tileSize, TIER1_TOWER_PRICE,
		towerDamage, towerSpeed, towerRange);

	addTower(makeTowerElement(x, y, newTower), TIER1_TOWER_PRICE);
	tileGrid->setTowerAsListener(x, y, newTower, tileSize);
}
void Game::buildYellowTower(int x, int y)
{
	Tower *newTower = new YellowTowerBase(
		x * tileSize + border,
		y * tileSize + gridOffset,
		tileSize, TIER1_TOWER_PRICE,
		towerDamage, towerSpeed, towerRange);

	addTower(makeTowerElement(x, y, newTower), TIER1_TOWER_PRICE);
	tileGrid->setTowerAsListener(x, y, newTower, tileSize);
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
				tileSize,
				*mobPath);

			setMonsterSpeed(monsters[spawnNextMobId], spawnX, spawnY);

			mobGridPos[spawnNextMobId].first = spawnX;
			mobGridPos[spawnNextMobId].second = spawnY;
			spawnNextMobId++;

			if(spawnNextMobId % 5 > 0)
				spawnTimer = spawnInterval;
			else
				spawnTimer = 3*spawnInterval;
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
	int xTrav = exitX,
		yTrav = exitY;
	while(iter->getCameFrom() != UndefDirection) 
	{
		tileGrid->setPathPart(xTrav, yTrav, true);
		switch (iter->getCameFrom())
		{
		case RightDirection:
			path += 'l';
			xTrav++;
			break;
		case LeftDirection:
			path += 'r';
			xTrav--;
			break;
		case DownDirection:
			path += 'u';
			yTrav++;
			break;
		case UpDirection:
			path += 'd';
			yTrav--;
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
	std::queue<pvPtr> pq;
	bool foundPath = false;
	const pvPtr spawnPtr = pathGrid->at(spawnX, spawnY);
	const pvPtr exitPtr = pathGrid->at(exitX, exitY);
	PathingVertex *p;

	pathGrid->setAllUnvisited();

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
		clearPath();
		removePathGrassListeners();
		std::string shortestPath = "";
		backtrack(exitPtr, shortestPath);
		delete mobPath;
		mobPath = new std::string(shortestPath.rbegin(), shortestPath.rend());

		// Adding path end char
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
	spawnY = rand() % gridRows;
	exitX = gridColumns-1;
	exitY = rand() % gridRows;
	int rn;

	for(int i = 0; i < gridColumns; i++)
	{
		for(int j = 0; j < gridRows; j++)
		{
			rn = rand() % 3;

			if(rn == 0)
				tileGrid->setMud(i,j);
			else if(rn == 1)
				tileGrid->setIce(i,j);
		}
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
				tower->shoot(shots, tarMon);
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
	std::list<BaseShot*>::iterator del ;
	for(std::list<BaseShot*>::iterator it = shots.begin(); it != shots.end();)
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
void Game::upgradeTowerSpeed()
{
	if(attemptPurchase(upgradeCost[towerAsCounter]))
	{
		bool delayUpgradeStart = (speedMode == ImmobileSpeedMode);
		speedProgressBar->start((int)s3eTimerGetMs(), 
			upgradeTimes[towerAsCounter],
			delayUpgradeStart);

		if(delayUpgradeStart)
			speedProgBarRemainder =
			speedProgressBar->getRemaining((int)s3eTimerGetMs());
	}
}
void Game::upgradeTowerRange()
{
	if(attemptPurchase(upgradeCost[towerRangeCounter]))
	{
		bool delayUpgradeStart = (speedMode == ImmobileSpeedMode);
		rangeProgressBar->start((int)s3eTimerGetMs(), 
			upgradeTimes[towerRangeCounter],
			delayUpgradeStart);

		if(delayUpgradeStart)
			rangeProgBarRemainder = 
			rangeProgressBar->getRemaining((int)s3eTimerGetMs());
	}
}
void Game::upgradeTowerDamage()
{
	if(attemptPurchase(upgradeCost[towerDamageCounter]))
	{
		bool delayUpgradeStart = (speedMode == ImmobileSpeedMode);
		damageProgressBar->start((int)s3eTimerGetMs(), 
			upgradeTimes[towerDamageCounter],
			delayUpgradeStart);

		if(delayUpgradeStart)
			damageProgBarRemainder = 
			damageProgressBar->getRemaining((int)s3eTimerGetMs());
	}
}
Mode Game::manangePausedMode()
{
	render();
	if(speedMode != ImmobileSpeedMode)
		unpauseProgBars();

	return io->manangePausedMode();
}
Mode Game::manageGameEnded()
{
	render();
	return io->manageGameEnded(lives);
}
void Game::setMonsterSpeed(Monster *mon, int gridPosX, int gridPosY)
{
	switch(tileGrid->getImage(gridPosX, gridPosY))
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
void Game::setShotSpeed(BaseShot *shot)
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
			towerSpeed *= 2;
			setTowerSpeed(false);
			spawnTimer /= 2;
		}
		else
		{
			speedMode = rememberSpeedMode = NormalSpeedMode;
			towerSpeed /= 2;
			setTowerSpeed(true);
			spawnTimer *= 2;
		}

		for(int i=0; i < numOfCurrWaveMons; i++)
			setMonsterSpeed(monsters[i], 
			monsters[i]->getGridPosX(), 
			monsters[i]->getGridPosY());

		for(ShotsConstIter it = shots.begin(); it != shots.end(); it++)
			setShotSpeed((*it));
	}
	showMenu = false;
	clearSelect();
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

	delete pathGrid;
	delete tileGrid;
	delete mobPath;
}
bool Game::towerDamagUncapped() const
{
	return towerDamageCounter < MAX_DAMAGE_LEVEL;
}
bool Game::towerSpeedUncapped() const
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
		if(isBuilt(gridPosX, gridPosY))
		{
			selectStruct(gridPosX, gridPosY);
			btnSelection = NothingSelected;
			showMenu = true;
		}
		else
		{
			if(speedMode == ImmobileSpeedMode && canAddWall())
			{
				buildWall(gridPosX, gridPosY);
				if(tileGrid->isPartOfPath(gridPosX, gridPosY))
				{	
					if(findShortestPath())
					{
						setPathGrassListeners();
						pathFound  = true;	
					}
					else
						pathFound = false;

				}
			}

			clearSelect();
			showMenu = false;
		}
	}
	else
	{
		clearSelect();
		showMenu = false;
	}
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
		io->drawTile((*it)->getImage(),
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
		io->drawTile((*it).second->getImage(), 
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
			io->drawTile(MonsterImage, 
				(*it)->getTopLeftX(), 
				(*it)->getTopLeftY(), 
				monsterDiam, monsterDiam);
		}
	}
}
void Game::setUI()
{
	gridColumns	= Iw2DGetSurfaceWidth() / tileSize;
	gridRows	= GRID_ROWS; 

	io->setUpUI(gridColumns, gridRows, tileSize);
	io->initProgBars(&damageProgressBar,
		&speedProgressBar, &rangeProgressBar, tileSize);
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
	for(TowerMapConstIter it = towers.begin(); it != towers.end(); it++)
		tileGrid->setTowerAsListener(((*it).second)->getTopLeftX() / tileSize,
		((*it).second)->getTopLeftY() / tileSize, 
		(*it).second,
		tileSize);

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

		tileGrid->clearListeners(x, y);
	}	
}
void Game::removeWall(int x, int y)
{
	pathGrid->connect(x,y);
	std::map<int, Wall*>::iterator it = walls.find(getKey(x, y));
	delete it->second;
	walls.erase(it);
	clearSelect();

	if(findShortestPath())
	{
		setPathGrassListeners();
		pathFound  = true;
	}

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
void Game::removeTower(int x, int y)
{
	Tower *t = towers.find(getKey(x,y))->second;
	addCredits(t->getSellValue());
	tileGrid->removeTowerAsListener(x, y, t, tileSize);
	delete t;
	towers.erase(getKey(x, y));
	clearSelect();
}
bool Game::isTower(int x, int y) const
{
	return towers.find(getKey(x,y)) != towers.end();
}
bool Game::isBuilt(int x, int y) const
{
	return !pathGrid->isConnected(x,y);
}
void Game::renderProgressBars() const
{
	if(damageProgressBar->upgrading())
		io->renderProgressBar(damageProgressBar);

	if(speedProgressBar->upgrading())
		io->renderProgressBar(speedProgressBar);

	if(rangeProgressBar->upgrading())
		io->renderProgressBar(rangeProgressBar);
}
void Game::updateUpgrades()
{
	if(damageProgressBar->isActive() && 
		damageProgressBar->tick((int)s3eTimerGetMs()))
	{
		buffTowerDamage(damageUpgrades[towerDamageCounter]);
		towerDamage += damageUpgrades[towerDamageCounter];
		towerDamageCounter++;
	}

	if(speedProgressBar->isActive() && 
		speedProgressBar->tick((int)s3eTimerGetMs()))
	{
		int newSpeed = speedUpgrades[towerAsCounter];
		if(speedMode == FastSpeedMode)
			newSpeed /= 2;

		buffTowerSpeed(newSpeed);
		towerSpeed = newSpeed;
		towerAsCounter++;
	}

	if (rangeProgressBar->isActive() && 
		rangeProgressBar->tick((int)s3eTimerGetMs()))
	{
		towerRangeCounter++;
		towerRange = (towerRangeCounter+1)*tileSize;
		buffTowerRange();
		removePathGrassListeners();
		setPathGrassListeners();
	}
}
void Game::addCredits( int addAmount )
{
	credits += addAmount;
	if(credits > MAX_CREDITS)
		credits = MAX_CREDITS;
}
bool Game::attemptPurchase( int amount )
{
	if(credits >= amount)
	{
		credits -= amount;
		return true;
	}
	return false;
}
void Game::selectStruct(int x, int y )
{
	if(isTower(x,y))
		gridSelection = TowerSelected;
	else
		gridSelection = WallSelected;

	selectedX = x;
	selectedY = y;
}
void Game::renderStructSelection() const
{
	if(gridSelection != NothingSelected)
	{
		TowerMapConstIter it;
		if((it = towers.find(getKey(selectedX, selectedY))) != towers.end())
		{
			int towerRange = (*it).second->getRange();
			io->renderShowRange((*it).second->getCenterX() - towerRange,
				(*it).second->getCenterY() - towerRange, 
				towerRange*2);
		}
		else
			io->renderShowRange(selectedX*tileSize + border,
			selectedY*tileSize + gridOffset, 
			tileSize);
	}
}
void Game::invokeMenuBtn() 
{
	clearSelect();
	showMenu = !showMenu;
}
void Game::clearSelect()
{
	gridSelection = NothingSelected;
	btnSelection = NothingSelected;
}
void Game::addTower( TowerElement newTower, int price)
{
	credits -= price;
	towers.insert(newTower);
}
void Game::renderUpgradeButton(int cost, bool uncapped, bool inProgress, Image img, Button btn) const
{
	if(uncapped)
	{
		if(credits >= cost && !inProgress)
			io->renderButton(true, img, btn);
		else
			io->renderButton(false, img, btn);
	} else if(inProgress)
		io->renderButton(false, img, btn);
}
void Game::RenderBasicUpgText( char * str ) const
{
	switch (btnSelection)
	{
	case Button1Selected:
		if(!towerDamagUncapped())
			return;

		sprintf(str, "$ %d  Increase damage", upgradeCost[towerDamageCounter]);
		break;
	case Button2Selected:
		if(!towerSpeedUncapped())
			return;

		sprintf(str, "$ %d  Increase speed", upgradeCost[towerAsCounter]);
		break;
	case Button3Selected:
		if(!towerRangeUncapped())
			return;

		sprintf(str, "$ %d  Increase range", upgradeCost[towerRangeCounter]);
		break;
	}
}
void Game::renderUpgWallTxt( char * str ) const
{
	switch (btnSelection)
	{
	case Button1Selected:
		sprintf(str, "$ %d  Build red tower", TIER1_TOWER_PRICE);
		break;
	case Button2Selected:
		sprintf(str, "$ %d  Build teal tower", TIER1_TOWER_PRICE);
		break;
	case Button3Selected:
		sprintf(str, "$ %d  Build yellow tower", TIER1_TOWER_PRICE);
		break;
	case SellBtnSelected:
		sprintf(str, "Remove wall");
		break;
	default:
		return;
	}
}
void Game::renderUpgTowerTxt( char * str ) const
{
	Tower *selected = getTower(selectedX, selectedY);
	switch (btnSelection)
	{
	case Button1Selected:
		sprintf(str, "$ %d  %s", selected->getUpg1Price(),
			selected->getDescription(1));
		break;
	case Button2Selected:
		sprintf(str, "$ %d  %s", selected->getUpg2Price(),
			selected->getDescription(2));
		break;
	case Button3Selected:
		sprintf(str, "$ %d  %s", selected->getUpg3Price(),
			selected->getDescription(3));
		break;
	case SellBtnSelected:
		sprintf(str, "Sell tower for %d", selected->getSellValue());
		break;
	default:
		return;
	}
}
Tower *Game::getTower( int x, int y ) const
{
	return towers.find(getKey(x, y))->second;
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
void Game::resetProgBars()
{
	damageProgressBar->abort();
	speedProgressBar->abort();
	rangeProgressBar->abort();
}
void Game::resetUI()
{
	delete damageProgressBar;
	delete speedProgressBar;
	delete rangeProgressBar;
	setUI();
}
void Game::setTowerSpeed( bool setFast ) const
{
	TowerMapConstIter it = towers.begin();
	if(setFast)
		for(; it != towers.end(); it++)
			(*it).second->setFastSpeed();
	else
		for(; it != towers.end(); it++)
			(*it).second->setSlowSpeed();
}
void Game::buffTowerSpeed( int newSpeed) const
{
	for(TowerMapConstIter it = towers.begin(); it != towers.end(); it++)
		(*it).second->buffSpeed(newSpeed);
}
void Game::buffTowerDamage( int buff ) const
{
	for(TowerMapConstIter it = towers.begin(); it != towers.end(); it++)
		(*it).second->buffDamage(buff);
}
void Game::buffTowerRange() const
{
	for(TowerMapConstIter it = towers.begin(); it != towers.end(); it++)
		(*it).second->buffRange(towerRange);
}

void Game::renderGlobalUpgradeButtons() const
{
	renderUpgradeButton(upgradeCost[towerDamageCounter],
		towerDamagUncapped(), 
		damageProgressBar->upgrading(), 
		BuyDamageImage,
		Btn1Button);

	renderUpgradeButton(upgradeCost[towerAsCounter],
		towerSpeedUncapped(), 
		speedProgressBar->upgrading(), 
		BuySpeedImage,
		Btn2Button);

	renderUpgradeButton(upgradeCost[towerRangeCounter],
		towerRangeUncapped(), 
		rangeProgressBar->upgrading(), 
		BuyRangeImage,
		Btn3Button);
}

void Game::renderWallUpgradeButtons() const
{
	renderUpgradeButton(TIER1_TOWER_PRICE,
		true, 
		false, 
		RedTowerImage,
		Btn1Button);

	renderUpgradeButton(TIER1_TOWER_PRICE,
		true,
		false, 
		TealTowerImage,
		Btn2Button);

	renderUpgradeButton(TIER1_TOWER_PRICE,
		true, 
		false, 
		YellowTowerImage,
		Btn3Button);
}

void Game::renderTowerUpgradeButtons() const
{
	Tower *selected = getTower(selectedX, selectedY);

	renderUpgradeButton(selected->getUpg1Price(),
		selected->upgrade1Available(),
		false, 
		selected->getUpg1Image(),
		Btn1Button);

	renderUpgradeButton(selected->getUpg2Price(),
		selected->upgrade2Available(),
		false, 
		selected->getUpg2Image(),
		Btn2Button);

	renderUpgradeButton(selected->getUpg3Price(),
		selected->upgrade3Available(), 
		false, 
		selected->getUpg3Image(),
		Btn3Button);
}

bool Game::isWall( int x, int y ) const
{
	return walls.find(getKey(x,y)) != walls.end();
}

bool Game::removeWallHold()
{
	return holdingGridCounter > 0 && 
		speedMode == ImmobileSpeedMode &&
		isWall(getGridCoordX(holdingTileX), getGridCoordY(holdingTileY)) &&
		!isTower(getGridCoordX(holdingTileX), getGridCoordY(holdingTileY));
}

void Game::sellStructure()
{
	if(gridSelection == WallSelected && 
		speedMode == ImmobileSpeedMode)
		removeWall(selectedX, selectedY);
	else if(gridSelection == TowerSelected)
		removeTower(selectedX, selectedY);
	clearSelect();
	showMenu = false;
}

void Game::updateGridHold()
{
	if(io->getLastTouchX() != holdingTileX ||
		io->getLastTouchY() != holdingTileY)
	{
		holdingTileX = io->getLastTouchX();
		holdingTileY = io->getLastTouchY();
		holdingGridCounter = 0;
	}
	else
		holdingGridCounter++;
}

void Game::releaseGridHold()
{
	if(removeWallHold())
		removeWall(getGridCoordX(holdingTileX), 
		getGridCoordY(holdingTileY));
	holdingGridCounter = 0;
	holdingTileX = holdingTileY = 0;
	clearSelect();
}

void Game::handleButtonEvent(Selected btnSel)
{
	if(btnSelection == btnSel)
	{
		if(gridSelection == TowerSelected)
		{
			//invoke tower btn
		}	
		else if(gridSelection == WallSelected)
		{
			unsigned int towerSize = towers.size();
			attemptBuildTower(btnSel);
			if(towers.size() != towerSize)
			{
				clearSelect();
				showMenu = false;
			}
		}
		else
			attemptUpgradeGlobals(btnSel);
	}
	else 
		btnSelection = btnSel;
}

void Game::attemptBuildTower( Selected btnSel )
{
	switch (btnSel)
	{
	case Game::Button1Selected:
		if(credits >= TIER1_TOWER_PRICE)
			buildRedTower(selectedX, selectedY);
		break;

	case Game::Button2Selected:
		if(credits >= TIER1_TOWER_PRICE)
			buildTealTower(selectedX, selectedY);
		break;

	case Game::Button3Selected:
		if(credits >= TIER1_TOWER_PRICE)
			buildYellowTower(selectedX, selectedY);
		break;
	}
}

void Game::attemptUpgradeGlobals( Selected btnSel )
{
	switch(btnSel)
	{
	case Game::Button1Selected:
		if(towerDamagUncapped())
		{
			upgradeTowerDamage();
			clearSelect();
			showMenu = false;
		}
		break;
	case Game::Button2Selected:
		if(towerSpeedUncapped())
		{
			upgradeTowerSpeed();
			clearSelect();
			showMenu = false;
		}
		break;
	case Game::Button3Selected:
		if((towerRangeUncapped()))
		{
			upgradeTowerRange();
			clearSelect();
			showMenu = false;
		}
		break;
	}
	clearSelect();
	showMenu = false;
}

void Game::pauseProgBars()
{
	if(damageProgressBar->isActive())
		damageProgBarRemainder = damageProgressBar->getRemaining((int)s3eTimerGetMs());

	if(speedProgressBar->isActive())
		speedProgBarRemainder = speedProgressBar->getRemaining((int)s3eTimerGetMs());

	if(rangeProgressBar->isActive())
		rangeProgBarRemainder = rangeProgressBar->getRemaining((int)s3eTimerGetMs());
}

void Game::unpauseProgBars()
{
	if(damageProgressBar->upgrading())
		damageProgressBar->compensatePause((int)s3eTimerGetMs() + damageProgBarRemainder);

	if(speedProgressBar->upgrading())
		speedProgressBar->compensatePause((int)s3eTimerGetMs() + speedProgBarRemainder);

	if(rangeProgressBar->upgrading())
		rangeProgressBar->compensatePause((int)s3eTimerGetMs() + rangeProgBarRemainder);
}

void Game::renderPath() const
{
	//Iw2DSetAlphaMode(IW_2D_ALPHA_HALF);
	if(pathFound)
		//Iw2DSetColour(0x66ffff00);
			Iw2DSetColour(0xFFd03D50);
	else
		Iw2DSetColour(0xff0000ff);
	//Iw2DSetColour(0x660000ff);

	int xTrav = spawnX,
		yTrav = spawnY;
	unsigned int nxtInstr = 0;
	int xOffset = tileSize/2 - pathMark/2 + border;
	int yOffset = tileSize/2 - pathMark/2 + gridOffset;

	while(nxtInstr < mobPath->length()-1)
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
		io->drawTile(ShowPathIamge,
			xTrav*tileSize + xOffset,
			yTrav*tileSize + yOffset,
			pathMark, pathMark);
	}

	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
	Iw2DSetColour(0xffffffff);
}

void Game::clearPath() const
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
		tileGrid->setPathPart(x, y, false);
	}	
}

void Game::cancelUpgrade()
{
	switch (btnSelection)
	{
	case Game::Button1Selected:
		if(damageProgressBar->upgrading())
		{
			addCredits(upgradeCost[towerDamageCounter]);
			damageProgressBar->abort();
		}
		break;
	case Game::Button2Selected:
		if(speedProgressBar->upgrading())
		{
			addCredits(upgradeCost[towerAsCounter]);
			speedProgressBar->abort();
		}
		break;
	case Game::Button3Selected:
		if(rangeProgressBar->upgrading())
		{
			addCredits(upgradeCost[towerRangeCounter]);
			rangeProgressBar->abort();
		}
		break;
	}
}	
