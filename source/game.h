#ifndef _GAME_H
#define _GAME_H

#include <string>
#include <list>
#include <string>
#include <utility>

#include "grid.h"
#include "trackingShot.h"
#include "wall.h"
#include "pathingVertex.h"
#include "input.h"
#include "pathGrid.h"

class Game 
{
	typedef std::pair<int, int> Point;

	//enum UpdateMode
	//{
	//	MODE_ACTIVE  
	//	//MODE_PAUSED,
	//	//MODE_GAME_FINISHED,
	//};

	Grid tileGrid;
	PathGrid pathGrid;
	Monster monsters[NUM_MAX_MOBS];
	std::list<TrackingShot*> shots;
	std::vector<Tower*> towers;
	std::vector<Tower*> newTowers;
	std::vector<Wall*> walls;
	std::vector<Point> mobGridPos;
	//UpgradeLevel towerRange;
	GameSpeedMode speedMode;
	//UpdateMode mode;
	bool spawnNextWave;
	int spawnX, spawnY;
	int exitX, exitY;
	int spawnNextMobId;
	int mobHp;
	int mobMoveSpeed;
	int shotMoveSpeed;
	int currWave;
	int credits;
	int income;
	int numOfCurrWaveMons;
	int mobsAlive;
	int spawnTimer;
	int addIncome;
	bool changesConfirmed;
	bool buildMode;
	bool takeTouch;
	bool contWave;
	bool discardChanges;
	void onNewWave();
	//incomeCounter

	//============================================================================
	//Methods
	//============================================================================
	//Will construct a string of instructions to get from exit to spawnpoint
	//given that pathGrid contains one found by findShortestPath()
	void backtrack(pvPtr iter, std::string &path) const;
	//============================================================================
	void buildTower(int _x, int _y);
	//============================================================================
	//Will build wall between (x,y) and adjacent towers
	void buildWalls(int x, int y);
	//============================================================================
	void buildWater(int x, int y);
	//============================================================================
	//Will undo all pending changes like purchase of towers, upgares and income
	void discard();
	//=============================================================================
	//Returns true if a shortest path was found and g_mobPath was updated
	bool findShortestPath();
	//============================================================================
	//Generates spawn, exit and water using randomization. Return true if
	//a proper map was generated and tileGrid updated with it.
	bool generateMap();
	//============================================================================
	void handleInput();
	//============================================================================
	//Will add contents of newTowers to towers and add associated walls
	void lockNewTowers();
	//============================================================================
	//Will check for collisions and handle consequences
	void manageCollisions();
	//============================================================================
	void moveMobs();
	//============================================================================
	void moveShots();
	//============================================================================
	void renderButtons() const;
	//============================================================================
	void renderMonsters();
	//============================================================================
	void renderNewTowers();
	//============================================================================
	void renderText();
	//============================================================================
	void renderShots();
	//============================================================================
	void renderWalls();
	//============================================================================
	//Set up game for a new game
	void reset();
	//============================================================================
	//Towers will shoot if they can
	void shoot();
	//============================================================================
	//Will spawn a monster if allowed
	void spawnMonster();
	//============================================================================
	//If a monster has moved into a new grid element, will update new and previous
	//grid elements
	void updateMobGrid();
	//=============================================================================
	//Will check if wave is over and if so, if a new wave should be initiated
	void waveOverCheck();
	//=============================================================================
public:
	//=============================================================================
	Game();
	//=============================================================================
	~Game();
	//=============================================================================
	void Update();
	//=============================================================================
	void Render();
};
//=============================================================================
#endif /* !_GAME_H */
