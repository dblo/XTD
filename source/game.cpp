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
	updateMobGrid();
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
// 
void Game::reset()
{
	Tower::resetTowers(tileSize);
	srand(time(NULL));

	tileGrid = new TileGrid(gridColumns, gridRows, tileSize);
	pathGrid = new PathGrid(gridColumns, gridRows);
	io->reset();

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
	shotDiam			= (tileSize*2) / 5;
	monsterRadius		= tileSize / 5;

	speedMode			= ImmobileSpeedMode;
	rememberSpeedMode	= FastSpeedMode;

	spawnNextWave		= false;
	updatePath			= false;
	validPathExists		= true;

	numOfCurrWaveMons	= BASE_MONSTER_COUNT;
	mobMoveSpeed		= tileSize / 12;
	shotMoveSpeed		= (mobMoveSpeed*4)/3;
	spawnNextMobId		= MAX_MONSTER_COUNT;
	horizontalBorder	= io->getHorizontalBorder();
	verticalBorder		= horizontalBorder;

	mobGridPos.reserve(MAX_MONSTER_COUNT);
	pathGrid->init();

	for(int i=0; i < MAX_MONSTER_COUNT; i++)
		monsters.push_back(new Monster());

	// Allocate tiles as grass
	for(int i=0; i < gridColumns; i++)
		for(int j=0; j < gridRows; j++)
			tileGrid->buildGrass(i, j, 
			i*tileSize + verticalBorder, 
			j*tileSize + horizontalBorder);

	pathGrid->init();
	generateMap();

	// Set up initial path on
	findShortestPath();

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
		removePathGrassListeners(spawnX, spawnY);
		setPathGrassListeners(spawnX, spawnY);
	}
	updateStats();
}
//=============================================================================
Mode Game::handleInput()
{
	CTouch *touch = 0; //No longer need this here

	switch(io->handleInput(&touch))
	{
	case DoNothingInputEvent:
		return PlayMode;

	case GridInputEvent: //grid touch rename
		gridTouch();
		break;

	case ChangeSpeedInputEvent:
		changeGameSpeed();
		break;

	case PauseBtnInputEvent:
		return PausedMode;

		//case UndoInputEvent:
		//if(speedMode == ImmobileSpeedMode)
		//invokeDeleteTowerBtn();
		//break;

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
			updatePath = true;
		}
		break;
	}
	return PlayMode;

}
//==============================================================================
void Game::render()
{
	io->renderBg();
	tileGrid->render(io, tileSize);
	renderTowers();
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
void Game::updateStats()//TODO rename
{
	currWave++;
}
//==============================================================================
void Game::waveOverCheck()
{
	if(mobsAlive == 0 && speedMode == rememberSpeedMode)
	{
		speedMode = ImmobileSpeedMode;
		wallCap += 10;
	}
}
//==============================================================================
void Game::wallTouch(int x, int y)
{
	if(pathGrid->available(x,y))
	{
		if(walls.size() < wallCap)
			buildWall(x,y);
	}
	else
		removeWall(x, y);
}
//==============================================================================
void Game::updateWall(int x, int y)
{
	Image wallImage = getWallType(findNeighbours(x, y, false));

	// Update the wall image of tile at x,y
	walls.find(getKey(x, y))->second->setColor(wallImage);
}
//==============================================================================
unsigned int Game::findNeighbours(int x, int y, bool updateNeighbours)
{
	unsigned int neighbours = 0;
	if(y > 0 && !pathGrid->available(x, y-1))
	{
		if(y < gridRows-1 && !pathGrid->available(x, y+1))
		{
			neighbours |= 2; // Neighbour below

			if(updateNeighbours)
				updateWall(x, y+1);
		}

		neighbours |= 1; // Neighbour above
		if(updateNeighbours)
			updateWall(x, y-1);
	}
	else if(y < gridRows-1 && !pathGrid->available(x, y+1))
	{
		neighbours |= 2; // Neighbour below
		if(updateNeighbours)
			updateWall(x, y+1);
	}

	if(x > 0 && !pathGrid->available(x-1, y))
	{
		if(x < gridColumns - 1 && !pathGrid->available(x+1, y))
		{
			neighbours |= 4; // Neighbour on the right
			if(updateNeighbours)
				updateWall(x+1, y);
		}

		neighbours |= 8; // Neighbour on the left
		if(updateNeighbours)
			updateWall(x-1, y);
	}
	else if(x < gridColumns - 1 && !pathGrid->available(x+1, y))
	{
		neighbours |= 4;// Neighbour on the right
		if(updateNeighbours)
			updateWall(x+1, y);
	}
	//std::cout << ">>: " << neighbours << "\n";
	return neighbours;
}
//==============================================================================
void Game::buildWall(int x, int y)
{
	pathGrid->remove(x,y);
	Image wallImage = getWallType(findNeighbours(x, y, true));
	walls.insert(makeWallElement(x, y, new Wall(wallImage,
		x * tileSize + verticalBorder,
		y * tileSize + horizontalBorder,
		tileSize, tileSize)));
}
//==============================================================================
Image Game::getWallType(unsigned int neighbours) const
{
	Image wallType;
	switch(neighbours)
	{
	case 0:
		wallType = CrossWallImage;
		break;
	case 1:
		wallType = UdWallImage;
		break;
	case 2:
		wallType = UdWallImage;
		break;
	case 3:
		wallType = UdWallImage;
		break;
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
//==============================================================================
void Game::buildTower(int x, int y)
{
	//if(credits >= TOWER_PRICE && pathGrid->available(x,y))
	//{
	//	Tower *newTower = new Tower(
	//		x * tileSize + verticalBorder,
	//		y * tileSize + horizontalBorder,
	//		tileSize, currWave);

	//	credits -= TOWER_PRICE;
	//	towers.insert(makeTowerElement(x, y, newTower));
	//	newTowerBuilt = true;
	//	pathGrid->remove(x, y);
	//}
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
			tileGrid->notifyTileEnter(mobGridPos[i].first, mobGridPos[i].second, i);
			monsters[i]->gridPosUpdated();
			monsters[i]->updateMs(tileGrid->getTileType(newX, newY), mobMoveSpeed);
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
int Game::getKey(int x, int y) const
{
	return x*100+y;
}
//==============================================================================
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
//==============================================================================
void Game::shoot()
{
	int target;
	Monster *tarMon;
	Tower *t;
	for(std::map<int, Tower*>::const_iterator it = towers.begin(); it != towers.end(); it++)
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
				shots.push_back(new TrackingShot(t->getCenterX(),
					t->getCenterY(),
					tarMon, t->shoot(), 
					shotMoveSpeed,
					monsterRadius)); //TODO, handle shot radius
			}
		}
		else
		{
			t->reloadTick();
		}
	}
}
//==============================================================================
void Game::moveMobs()
{
	for(int i=0; i < numOfCurrWaveMons; i++)
	{
		if(monsters[i]->isAlive() && 
			!monsters[i]->move(*mobPath, tileSize))
		{
			decreaseLives();
			mobsAlive--;

		}
	}
}
//==============================================================================
void Game::decreaseLives()
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
void Game::invokeDeleteTowerBtn()
{
	//int delX = (io->getLastTouchX() - verticalBorder) / tileSize,
	//	delY = (io->getLastTouchY() - horizontalBorder) / tileSize;
	//std::map<int, Tower*>::iterator it;

	//if((it = towers.find(getKey(delX, delY))) != towers.end())
	//{
	//	if((*it).second->builtThisWave(currWave))
	//		credits += TOWER_PRICE;
	//	else
	//		credits += TOWER_PRICE/2;

	//	if(credits > MAX_CREDITS)
	//		credits = MAX_CREDITS;

	//	tileGrid->releaseTile(delX, delY);
	//	towers.erase(getKey(delX, delY));
	//	updatePath = true;
	//	pathGrid->add(delX, delY, *tileGrid);
	//}
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

	for(std::map<int, Wall*>::const_iterator it = walls.begin(); it != walls.end(); it++)
		delete (*it).second;
	walls.clear();

	for(std::map<int, Tower*>::const_iterator it = towers.begin(); it != towers.end(); it++)
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
	if(speedMode == ImmobileSpeedMode)
		wallTouch((io->getLastTouchX() - verticalBorder) / tileSize, 
		(io->getLastTouchY() - horizontalBorder) / tileSize);





	/*buildTower((io->getLastTouchX() - verticalBorder) / tileSize, 
	(io->getLastTouchY() - horizontalBorder) / tileSize);*/
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

	Wall *w;
	for(std::map<int, Wall*>::const_iterator it = walls.begin(); it != walls.end(); it++)
	{
		w = (*it).second; //need this? TODO
		io->drawTile(w->getColor(), w->getTopLeftX(), w->getTopLeftY(),
			tileSize, tileSize);
	}
}
//==============================================================================
void Game::renderTowers() const
{
	Iw2DSetColour(0xffffffff);

	for(std::map<int, Tower*>::const_iterator it = towers.begin(); it != towers.end(); it++)
	{
		io->drawTile((*it).second->getColor(), (*it).second->getTopLeftX(), 
			(*it).second->getTopLeftY(), tileSize, tileSize);
	}
}
//==============================================================================
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
//==============================================================================
void Game::reloadUI()
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
//==============================================================================
void Game::setPathGrassListeners(int pathTravX, int pathTravY)
{
	unsigned int nxtInstr = 0;
	std::map<int, Tower*>::iterator it;

	while(nxtInstr < mobPath->length())
	{
		switch((*mobPath)[nxtInstr])
		{
		case 'r':
			pathTravX++;
			break;
		case 'u':
			pathTravY--;
			break;
		case 'l':
			pathTravX--;
			break;
		case 'd':
			pathTravY++;
			break;
		}
		nxtInstr++;

		int xLowLim, yLowLim, xHiLim, yHiLim;

		xHiLim = pathTravX+2;
		yHiLim = pathTravY+2;
		xLowLim = pathTravX-2;
		yLowLim = pathTravY-2;

		for(int x=xLowLim; x <= xHiLim; x++)
			for(int y=yLowLim; y <= yHiLim; y++)
			{
				if(tileGrid->validPoint(x, y))
				{
					if((it = towers.find(getKey(x, y))) != towers.end())
					{
						tileGrid->setListener(pathTravX, pathTravY, (*it).second);
					}
				}
			}
	}
}
//==============================================================================
void Game::removePathGrassListeners(int pathTravX, int pathTravY)
{
	unsigned int nxtInstr = 0;

	while(nxtInstr < mobPath->length())
	{
		switch((*mobPath)[nxtInstr])
		{
		case 'r':
			pathTravX++;
			break;
		case 'u':
			pathTravY--;
			break;
		case 'l':
			pathTravX--;
			break;
		case 'd':
			pathTravY++;
			break;
		}
		nxtInstr++;

		tileGrid->removeListener(pathTravX, pathTravY);
	}	
}
//==============================================================================
void Game::removeWall(int x, int y)
{
	pathGrid->add(x,y);
	std::map<int, Wall*>::iterator it = walls.find(getKey(x, y));
	delete it->second;
	walls.erase(it);
}
//==============================================================================
Image Game::getTileType(int x, int y) const
{
	return tileGrid->getTileType(x, y);
}