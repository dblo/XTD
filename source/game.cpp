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

const int upgradeCost[3] = {100, 500, 1000};
int movementSpeeds[6];

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
Game::~Game() 
{
	cleanUp();

	delete pathGrid;
	delete tileGrid;
	delete mobPath;
	delete io;
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

	if(gameEnded())
		return EndedMode;
	return handleInput();
}
bool Game::gameEnded()
{
	if(lives == 0 || (currWave == MAX_WAVE && mobsAlive == 0))
	{
		manageGameEnded();
		return true;
	}
	return false;
}
void Game::reset()
{
	Tower::resetTowers(tileSize);
	srand(time(NULL));

	tileGrid = new TileGrid(gridColumns, gridRows, tileSize);
	pathGrid = new PathGrid(gridColumns, gridRows);
	io->reset();
	
	movementSpeeds[0] = tileSize / 24;
	movementSpeeds[1] = tileSize / 12;
	movementSpeeds[2] = tileSize / 8;
	movementSpeeds[3] = tileSize / 6;
	movementSpeeds[4] = tileSize / 4;
	movementSpeeds[5] = tileSize / 3;

	credits				= BASE_CREDITS;
	mobHp				= MONSTER_BASE_HP;
	spawnTimer			= 0;
	currWave			= 0;
	mobsAlive			= 0;
	lives				= 99;
	towerAsCounter		= 0;
	towerDmgCounter		= 0;
	towerRangeCounter	= 0;
	wallCap				= 10;

	// Shot diameter is 40% of tilesize
	shotDiam			= (tileSize*2) / 5;

	// Monster radius is 33% of tilesize
	monsterRadius		= tileSize / 3;

	speedMode			= ImmobileSpeedMode;
	rememberSpeedMode	= NormalSpeedMode;

	spawnNextWave		= false;
	updatePath			= false;
	validPathExists		= true;

	numOfCurrWaveMons	= BASE_MONSTER_COUNT;
	spawnNextMobId		= MAX_MONSTER_COUNT;
	horizontalBorder	= io->getHorizontalBorder();
	horizontalOffset	= io->gethorizontalOffset();

	mobGridPos.reserve(MAX_MONSTER_COUNT);
	pathGrid->init();

	for(int i=0; i < MAX_MONSTER_COUNT; i++)
		monsters.push_back(new Monster());

	// Allocate tiles as grass
	for(int i=0; i < gridColumns; i++)
		for(int j=0; j < gridRows; j++)
			tileGrid->buildGrass(i, j, 
			i*tileSize + horizontalOffset, 
			j*tileSize + horizontalBorder);

	pathGrid->init();
	generateMap();

	// Set initial path
	findShortestPath();
}
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

	updatePath = false;
	pathGrid->setAllUnvisited();
	if(!findShortestPath())
	{
		validPathExists = false;
	}
	else
	{
		newTowerBuilt = false;
		validPathExists = true;
		removePathGrassListeners();
		setPathGrassListeners();

	}
	updateStats();
}
Mode Game::handleInput()
{
	switch(io->handleInput())
	{
	case DoNothingInputEvent:
		return PlayMode;

	case GridInputEvent:
		gridTouch();
		break;

	case ChangeSpeedInputEvent:
		changeGameSpeed();
		break;

	case PauseBtnInputEvent:
		return PausedMode;

	case DmgBtnInputEvent:
		invokeUpgradeDmgBtn();
		break;

	case AsBtnInputEvent:
		if(towerAsUncapped())
			invokeUpgradeSpeedBtn();
		break;

	case RangeBtnInputEvent:
		if(towerRangeUncapped())
		{
			invokeUpgradeRangeBtn();
			updatePath = true;
		}
		break;
	}
	return PlayMode;

}
void Game::render()
{
	//io->renderBg();
	tileGrid->render(io, tileSize);
	renderWalls();
	renderTowers();
	renderMonsters();
	renderShots();
	renderText();
	renderButtons();
}
void Game::renderButtons() const
{
	if(speedMode == ImmobileSpeedMode)
		io->renderPlayButton();
	else if(speedMode == NormalSpeedMode)
		io->renderNormalSpeedButton();
	else
		io->renderFastSpeedButton();

	if(towerDmgCounter < 3)
	{
		if(credits >= upgradeCost[towerDmgCounter])
			io->renderUpgDmgButton(true);
		else
			io->renderUpgDmgButton(false);
	}
	else
	{
		if(credits  >= 1000*(towerDmgCounter-1))
			io->renderUpgDmgButton(true);
		else
			io->renderUpgDmgButton(false);
	}

	if(towerAsUncapped())
	{
		if(credits >= upgradeCost[towerAsCounter])
			io->renderUpgSpdButton(true);
		else
			io->renderUpgSpdButton(false);
	}

	if(towerRangeUncapped())
	{
		if(credits >= upgradeCost[towerRangeCounter])
			io->renderUpgRangeButton(true);
		else
			io->renderUpgRangeButton(false);
	}
	io->renderPauseButton();
}
void Game::renderText() const
{
	io->setTextColor();
	io->renderLivesText(lives);
	io->renderWaveText(currWave);
	io->renderCreditsText(credits);
}
void Game::updateStats()//TODO rename
{
	currWave++;
}
void Game::waveOverCheck()
{
	if(mobsAlive == 0 && speedMode == rememberSpeedMode)
	{
		speedMode = ImmobileSpeedMode;
		wallCap += 10;
	}
}
void Game::wallTouch(int x, int y)
{
	if(!isWall(x, y))
	{
		if(walls.size() < wallCap)
			buildWall(x,y);
	}
	else if(!isTower(x, y))
		removeWall(x, y);
}
void Game::updateWall(int x, int y)
{
	Image wallImage = getWallType(findNeighbours(x, y, false));

	// Update the wall image of tile at x,y
	walls.find(getKey(x, y))->second->setColor(wallImage);
}
unsigned int Game::findNeighbours(int x, int y, bool updateNeighbours)
{
	unsigned int neighbours = 0;
	if(y > 0 && isWall(x, y-1))
	{
		if(y < gridRows-1 && isWall(x, y+1))
		{
			neighbours |= 2; // Neighbour below

			if(updateNeighbours)
				updateWall(x, y+1);
		}

		neighbours |= 1; // Neighbour above
		if(updateNeighbours)
			updateWall(x, y-1);
	}
	else if(y < gridRows-1 && isWall(x, y+1))
	{
		neighbours |= 2; // Neighbour below
		if(updateNeighbours)
			updateWall(x, y+1);
	}

	if(x > 0 && isWall(x-1, y))
	{
		if(x < gridColumns - 1 && isWall(x+1, y))
		{
			neighbours |= 4; // Neighbour on the right
			if(updateNeighbours)
				updateWall(x+1, y);
		}

		neighbours |= 8; // Neighbour on the left
		if(updateNeighbours)
			updateWall(x-1, y);
	}
	else if(x < gridColumns - 1 && isWall(x+1,y))
	{
		neighbours |= 4;// Neighbour on the right
		if(updateNeighbours)
			updateWall(x+1, y);
	}
	return neighbours;
}
void Game::buildWall(int x, int y)
{
	pathGrid->remove(x,y);
	Image wallImage = getWallType(findNeighbours(x, y, true));
	walls.insert(makeWallElement(x, y, new Wall(wallImage,
		x * tileSize + horizontalOffset,
		y * tileSize + horizontalBorder)));
}
Image Game::getWallType(unsigned int neighbours) const
{
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
		x * tileSize + horizontalOffset,
		y * tileSize + horizontalBorder,
		tileSize, currWave);

	credits -= TOWER_PRICE;
	towers.insert(makeTowerElement(x, y, newTower));
	newTowerBuilt = true;
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
			monsters[spawnNextMobId]->init(
				spawnX, 
				spawnY,
				spawnX * tileSize + horizontalOffset,
				spawnY * tileSize + horizontalBorder,
				mobHp, 
				0,
				spawnNextMobId,
				monsterRadius, 
				tileSize);

			setMonsterSpeed(monsters[spawnNextMobId], spawnX, spawnY);

			mobGridPos[spawnNextMobId].first = spawnX;
			mobGridPos[spawnNextMobId].second = spawnY;
			spawnNextMobId++;
			spawnTimer = MONSTER_SPAWN_INTERVAL;
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
		if(monsters[i]->getUpdateGridPos())
		{
			tileGrid->notifyTileExit(mobGridPos[i].first, mobGridPos[i].second, i);
			newX = monsters[i]->getGridPosX();
			newY = monsters[i]->getGridPosY();
			mobGridPos[i].first = newX;
			mobGridPos[i].second = newY;
			tileGrid->notifyTileEnter(newX, newY, i);

			setMonsterSpeed(monsters[i], newX, newY);
			monsters[i]->gridPosUpdated();
		}
	}
}
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
/* Attempts to find a shortest path from spawn to every node until exit has 
been found. If path to exit was found, calls backtrack to create a string 
containing the path from exit to spawn and then reverse it. If no path was 
found, the previous path will not be Updated. */
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
int Game::getKey(int x, int y) const
{
	return x*100+y;
}
void Game::generateMap()
{
	spawnX = gridColumns-1;
	spawnY = rand() % gridRows;
	exitX = 0;
	exitY = rand() % gridRows;

	tileGrid->setSpawn(spawnX, spawnY);
	tileGrid->setExit(exitX, exitY);

	for(int i=(rand() % 3) + 5; i > 0; i--)
	{
		int newTerrainX = rand() % gridColumns,
			newTerrainY = rand() % gridRows;

		for(int j = newTerrainX-1; j <= newTerrainX; j++)
			for(int k = newTerrainY-1; k <= newTerrainY; k++)
				if(tileGrid->validPoint(j,k) && validIceMud(j,k))
					tileGrid->setIce(j,k);
	}

	for(int i=(rand() % 3) + 5; i > 0; i--)
	{
		int newTerrainX = rand() % gridColumns,
			newTerrainY = rand() % gridRows;

		for(int j = newTerrainX-1; j <= newTerrainX; j++)
			for(int k = newTerrainY-1; k <= newTerrainY; k++)
				if(tileGrid->validPoint(j,k) && validIceMud(j,k))
					tileGrid->setMud(j,k);
	}
}
void Game::shoot()
{
	int target;
	Monster *tarMon;
	Tower *t;
	for(TowerMapConstIter it = towers.begin(); it != towers.end(); it++)
	{
		t = (*it).second;
		if(t->armed())
		{
			target = t->aquireTarget(numOfCurrWaveMons);
			while(target < numOfCurrWaveMons)
			{
				tarMon = monsters[target];
				if(t->targetInRange(tarMon->getCenterX(), tarMon->getCenterY(),
					tarMon->getRadius()))
					break;
				else
					target = t->aquireTarget(numOfCurrWaveMons, ++target);
			}

			if(target < numOfCurrWaveMons)
			{
				shots.push_back(new TrackingShot(
					t->getCenterX(),
					t->getCenterY(),
					tarMon, 
					t->shoot(), 
					movementSpeeds[N_SHOTSPEED_INDEX],
					shotDiam / 2));

				setShotSpeed(shots.back());
			}
		}
		else
		{
			t->reloadTick();
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
			decreaseLives();
			mobsAlive--;
			tileGrid->notifyTileExit(monsters[i]->getGridPosX(), 
				monsters[i]->getGridPosY(), monsters[i]->getMobId());
		}
	}
}
void Game::decreaseLives()
{
	lives--;
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
void Game::monsterDied(Monster *mon)
{
	credits += 1 + mobHp / 10;
	if(credits > MAX_CREDITS)
		credits = MAX_CREDITS;

	tileGrid->notifyTileExit(mon->getGridPosX(), 
				mon->getGridPosY(), mon->getMobId());

	mobsAlive--;
}
void Game::invokeUpgradeSpeedBtn()
{
	if(credits >= upgradeCost[towerAsCounter])
	{
		credits -= upgradeCost[towerAsCounter];
		Tower::buffAs();
		towerAsCounter++;
	}
}
void Game::invokeUpgradeRangeBtn()
{
	if(credits >= upgradeCost[towerRangeCounter])
	{
		credits -= upgradeCost[towerRangeCounter];
		Tower::buffRange();
		towerRangeCounter++;
	}
}
void Game::invokeUpgradeDmgBtn()
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
Mode Game::manangePausedMode()
{
	Iw2DSurfaceClear(0xFF4E4949);
	render();
	return io->manangePausedMode();
}
Mode Game::manageTitleMode()
{
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
	for(std::vector<Monster*>::const_iterator it = monsters.begin(); it != monsters.end(); it++)
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
	if(speedMode != ImmobileSpeedMode || wallCap == walls.size()) //spawnNextWave)
		towerTouch(getGridCoordX(io->getLastTouchX()),
		getGridCoordY(io->getLastTouchY()));
	else
		wallTouch(getGridCoordX(io->getLastTouchX()),
		getGridCoordY(io->getLastTouchY()));
}
int Game::getGridCoordX(int xVal) const
{
	return (xVal - horizontalOffset) / tileSize;
}
int Game::getGridCoordY(int yVal) const
{
	return (yVal- horizontalBorder) / tileSize;
}
void Game::renderShots() const
{
	Iw2DSetColour(0xffffffff);
	for(ShotsConstIter it = shots.begin(); 
		it != shots.end(); it++)
	{
		io->drawTile(ShotImage, (*it)->getTopLeftX(), (*it)->getTopLeftY(),
			shotDiam, shotDiam);
	}
}
void Game::renderWalls() const
{
	Wall *w;
	Iw2DSetColour(0xffffffff);
	for(WallMapConstIter it = walls.begin(); it != walls.end(); it++)
	{
		w = (*it).second;
		io->drawTile(w->getColor(), w->getTopLeftX(), w->getTopLeftY(),
			tileSize, tileSize);
	}
}
void Game::renderTowers() const
{
	Iw2DSetColour(0xffffffff);

	for(TowerMapConstIter it = towers.begin(); it != towers.end(); it++)
	{
		io->drawTile((*it).second->getColor(), (*it).second->getTopLeftX(), 
			(*it).second->getTopLeftY(), tileSize, tileSize);
	}
}
void Game::renderMonsters() const
{
	Iw2DSetColour(0xffffffff);
	for(int j=0; j < numOfCurrWaveMons; j++)
	{
		if(monsters[j]->isAlive()) 
		{
			io->drawTile(MonsterImage, monsters[j]->getTopLeftX(), 
				monsters[j]->getTopLeftY(), tileSize, tileSize);
		}
	}
}
void Game::setUI()
{
	io->setUpUI(gridColumns, gridRows);
}
bool Game::validIceMud(int x, int y) const
{
	return !((x == spawnX && y == spawnY) ||
		(x == exitX && y == exitY));
}
TowerElement Game::makeTowerElement(int x, int y, Tower *t)
{
	return TowerElement(getKey(x, y), t);
}
// Packs coords and w into key,value pair
WallElement Game::makeWallElement(int x, int y, Wall *w)
{
	return WallElement(getKey(x, y), w);
}
void Game::setPathGrassListeners()
{
	int xTrav = spawnX,
		yTrav = spawnY;
	unsigned int nxtInstr = 0;
	TowerMapConstIter it;

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

		int xLowLim, yLowLim, xHiLim, yHiLim;

		xHiLim = xTrav+2;
		yHiLim = yTrav+2;
		xLowLim = xTrav-2;
		yLowLim = yTrav-2;

		for(int x=xLowLim; x <= xHiLim; x++)
			for(int y=yLowLim; y <= yHiLim; y++)
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
	pathGrid->add(x,y);
	std::map<int, Wall*>::iterator it = walls.find(getKey(x, y));
	delete it->second;
	walls.erase(it);
	pathGrid->add(x,y);

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
