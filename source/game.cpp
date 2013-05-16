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
const int waveMonsterCount[MAX_WAVE] = {100, 150, 200, 250, 300, 300, 300,};
const int upgradeTimes[3]		= {5, 15, 35};
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
void Game::reset()
{
	srand(time(NULL));
	Tower::resetTowers(tileSize);
	io->reset();

	tileGrid = new TileGrid(gridColumns, gridRows, tileSize);
	pathGrid = new PathGrid(gridColumns, gridRows);
	pathGrid->init();
	mobGridPos.reserve(MAX_MONSTER_COUNT);
	mobPath = 0;

	movementSpeeds[0] = tileSize / 27;
	movementSpeeds[1] = movementSpeeds[0] * 2;
	movementSpeeds[2] = movementSpeeds[0] * 4;
	movementSpeeds[3] = movementSpeeds[0] * 8;
	movementSpeeds[4] = movementSpeeds[0] * 5;
	movementSpeeds[5] = movementSpeeds[0] * 10;

	spawnNextWave		= false;
	showMenu			= false;
	credits				= BASE_CREDITS;
	monsterHP			= MONSTER_BASE_HP;
	spawnTimer			= 0;
	currWave			= 0;
	monstersAlive		= 0;
	lives				= 123;
	towerAsCounter		= 0;
	towerDmgCounter		= 0;
	towerRangeCounter	= 0;
	wallCap				= 14;
	wallInc				= 10;
	monsterDiam			= (tileSize*2)/3;
	shotDiameter		= (tileSize*2) / 5;
	monsterRadius		= (tileSize*2)/6;
	numOfCurrWaveMons	= BASE_MONSTER_COUNT;
	spawnNextMobId		= MAX_MONSTER_COUNT;
	border				= io->getBorder();
	gridOffset			= io->getOffset();
	speedMode			= ImmobileSpeedMode;
	rememberSpeedMode	= NormalSpeedMode;
	gridSelection		= NothingSelected;
	btnSelection		= NothingSelected;

	for(int i=0; i < MAX_MONSTER_COUNT; i++)
		monsters.push_back(new Monster());

	tileGrid->init(gridOffset, border, tileSize);
	resetProgBars();
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
		changeGameSpeed();
		break;

	case PauseBtnInputEvent:
		return PausedMode;

	case Btn1Event:
		if(btnSelection == Button1Selected)
		{
			if(gridSelection == TowerSelected)
			{
				//invoke structs btn
			}	
			else if(gridSelection == WallSelected)
			{
				buildRedTower(selectedX, selectedY);
			}
			else if(towerDmgUncapped())
			{
				upgradeTowerDamage();
			}
			clearSelect();
			showMenu = false;
		}
		else 
			btnSelection = Button1Selected;
		break;

	case Btn2Event:
		if(btnSelection == Button2Selected)
		{
			if(gridSelection == TowerSelected)
			{
				//invoke structs btn
			}	
			else if(gridSelection == WallSelected)
			{
				buildTealTower(selectedX, selectedY);
			}	
			else if(towerAsUncapped())
			{
				upgradeTowerSpeed();
			}
			clearSelect();
			showMenu = false;
		}
		else 
			btnSelection = Button2Selected;
		break;

	case Btn3Event:
		if(btnSelection == Button3Selected)
		{
			if(gridSelection == TowerSelected)
			{
				//invoke structs btn
			}	
			else if(gridSelection == WallSelected)
			{
				buildYellowTower(selectedX, selectedY);
			}
			else if((towerRangeUncapped()))
			{
				upgradeTowerRange();
			}
			clearSelect();
			showMenu = false;
		}
		else 
			btnSelection = Button3Selected;
		break;

	case SellInputEvent:
		if(btnSelection == SellBtnSelected)
		{
			if(gridSelection == WallSelected)
				removeWall(selectedX, selectedY);
			else if(gridSelection == TowerSelected)
				removeTower(selectedX, selectedY);
			clearSelect();
			showMenu = false;
		}
		else
			btnSelection = SellBtnSelected;
		break;

	case ClearEvent:
		clearSelect();
		showMenu = false;
		break;
	}
	return PlayMode;
}
void Game::render()
{
	Iw2DSurfaceClear(0);
	io->renderBg();
	tileGrid->render(io, tileSize);

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
	io->renderMenuBtn();
	renderStructSelection();
}
void Game::renderButtons() const
{
	if(showMenu)
	{
		if(speedMode == ImmobileSpeedMode)
			io->renderPlayButton();
		else if(speedMode == NormalSpeedMode)
			io->renderNormalSpeedButton();
		else
			io->renderFastSpeedButton();
		if(gridSelection == NothingSelected)
		{
			renderUpgradeButton(upgradeCost[towerDmgCounter],
				towerDmgUncapped(), 
				dmgProgressBar->isActive(), 
				BuyDamageImage,
				Btn1Button);

			renderUpgradeButton(upgradeCost[towerAsCounter],
				towerAsUncapped(), 
				asProgressBar->isActive(), 
				BuySpeedImage,
				Btn2Button);

			renderUpgradeButton(upgradeCost[towerRangeCounter],
				towerRangeUncapped(), 
				ranProgressBar->isActive(), 
				BuyRangeImage,
				Btn3Button);
		}
		else if(gridSelection == WallSelected)
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
		else
		{
			// Render tower upgrades
		}

		io->renderSellBtn(gridSelection != NothingSelected);
		io->renderPauseButton();
	}
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

	sprintf(str, "R %d", currWave);
	io->renderText(str, WaveText);

	if(gridSelection == NothingSelected)
	{
		if(btnSelection != NothingSelected)
		{
			RenderBasicUpgText(str);
		}
	}
	else if(gridSelection == WallSelected)
	{
		renderUpgWallTxt(str);
	}
	else // Tower selected
	{
		/*
		switch (btnSelection)
		{
		case Button1Selected:
		sprintf(str, "$ %d  Increase damage", upgradeCost[towerDmgCounter]);
		break;
		case Button2Selected:
		sprintf(str, "$ %d  Increase speed", upgradeCost[towerAsCounter]);
		break;
		case Button3Selected:
		sprintf(str, "$ %d  Increase range", upgradeCost[towerRangeCounter]);
		break;
		}
		io->renderText(str, InfoText);*/
	}
	io->setTextColor(false);
}
void Game::waveOverCheck()
{
	if(monstersAlive == 0 && speedMode == rememberSpeedMode)
	{
		speedMode = ImmobileSpeedMode;
		monsterHP *= 2;
		wallCap += wallInc;
		wallInc -= 1;
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
		tileSize, currWave, TIER1_TOWER_PRICE,
		tileSize);

	addTower(makeTowerElement(x, y, newTower), TIER1_TOWER_PRICE);
}
void Game::buildTealTower(int x, int y)
{
	Tower *newTower = new TealTowerBase(
		x * tileSize + border,
		y * tileSize + gridOffset,
		tileSize, currWave, TIER1_TOWER_PRICE,
		tileSize);

	addTower(makeTowerElement(x, y, newTower), TIER1_TOWER_PRICE);
}
void Game::buildYellowTower(int x, int y)
{
	Tower *newTower = new YellowTowerBase(
		x * tileSize + border,
		y * tileSize + gridOffset,
		tileSize, currWave, TIER1_TOWER_PRICE,
		tileSize);

	addTower(makeTowerElement(x, y, newTower), TIER1_TOWER_PRICE);
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
				spawnTimer = MONSTER_SPAWN_INTERVAL;
			else
				spawnTimer = 3*MONSTER_SPAWN_INTERVAL;

			if(speedMode == FastSpeedMode) // todo change at time of speedchange only
				spawnTimer /= 2;
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

		// Adding path end char
		mobPath->append("0");
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
	if(asProgressBar->isActive())
	{
		towerAsCounter--;
		addCredits(upgradeCost[towerAsCounter]);
		asProgressBar->abort();
	}
	else if(attemptPurchase(upgradeCost[towerAsCounter]))
	{
		asProgressBar->start((int)s3eTimerGetMs(), 
			upgradeTimes[towerAsCounter]);
		towerAsCounter++;
	}
}
void Game::upgradeTowerRange()
{
	if(ranProgressBar->isActive())
	{
		towerRangeCounter--;
		addCredits(upgradeCost[towerRangeCounter]);
		ranProgressBar->abort();
	}
	else if(attemptPurchase(upgradeCost[towerRangeCounter]))
	{
		ranProgressBar->start((int)s3eTimerGetMs(), 
			upgradeTimes[towerRangeCounter]);
		towerRangeCounter++;
	}
}
void Game::upgradeTowerDamage()
{
	if(dmgProgressBar->isActive())
	{
		towerDmgCounter--;
		addCredits(upgradeCost[towerDmgCounter]);
		dmgProgressBar->abort();
	}
	else if(attemptPurchase(upgradeCost[towerDmgCounter]))
	{
		dmgProgressBar->start((int)s3eTimerGetMs(), 
			upgradeTimes[towerDmgCounter]);
		towerDmgCounter++;
	}
}
Mode Game::manangePausedMode()
{
	render();
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
		if(isBuilt(gridPosX, gridPosY))
		{
			selectStruct(gridPosX, gridPosY);
			btnSelection = NothingSelected;
			showMenu = true;
		}
		else
		{
			if(speedMode == ImmobileSpeedMode && canAddWall())
				buildWall(gridPosX, gridPosY);

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
	io->initProgBars(&dmgProgressBar,
		&asProgressBar, &ranProgressBar);
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
	clearSelect();
}
void Game::removeTower(int x, int y)
{
	Tower *t = towers.find(getKey(x,y))->second;

	credits += t->getSellValue();
	if(credits > MAX_CREDITS)
		credits = MAX_CREDITS;

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
	if(showMenu && gridSelection == NothingSelected)
	{
		if(dmgProgressBar->isActive())
			io->renderProgressBar(dmgProgressBar);

		if(asProgressBar->isActive())
			io->renderProgressBar(asProgressBar);

		if(ranProgressBar->isActive())
			io->renderProgressBar(ranProgressBar);
	}
}
void Game::updateUpgrades()
{
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
		io->renderTileSelected(selectedX*tileSize + border,
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
		if(!towerDmgUncapped())
			return;

		sprintf(str, "$ %d  Increase damage", upgradeCost[towerDmgCounter]);
		break;
	case Button2Selected:
		if(!towerAsUncapped())
			return;

		sprintf(str, "$ %d  Increase speed", upgradeCost[towerAsCounter]);
		break;
	case Button3Selected:
		if(!towerRangeUncapped())
			return;

		sprintf(str, "$ %d  Increase range", upgradeCost[towerRangeCounter]);
		break;
	}
	io->renderText(str, InfoText);
}
void Game::renderUpgWallTxt( char * str ) const
{
	switch (btnSelection)
	{
	case Button1Selected:
		sprintf(str, "$ %d  Build red tower", TIER1_TOWER_PRICE);
		break;
	case Game::Button2Selected:
		sprintf(str, "$ %d  Build teal tower", TIER1_TOWER_PRICE);
		break;
	case Game::Button3Selected:
		sprintf(str, "$ %d  Build yellow tower", TIER1_TOWER_PRICE);
		break;
	case Game::SellBtnSelected:
		sprintf(str, "Remove wall");
		break;
	default:
		return;
	}
	io->renderText(str, InfoText);
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
	dmgProgressBar->abort();
	asProgressBar->abort();
	ranProgressBar->abort();
}

void Game::resetUI()
{
	delete dmgProgressBar;
	delete asProgressBar;
	delete ranProgressBar;
	setUI();
}
