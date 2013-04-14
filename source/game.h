
#ifndef _GAME_H
#define _GAME_H

#include <string>
#include <list>
#include <string>

#include "grid.h"
#include "trackingShot.h"
#include "wall.h"
#include "pathingVertex.h"
#include "input.h"
#include <utility>
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

	PathGrid pathGrid;
	Monster monsters[NUM_MAX_MOBS];
	Grid grid;
	std::list<TrackingShot*> shots;
	std::vector<Tower*> towers;
	std::vector<Tower*> newTowers;
	std::vector<Wall*> walls;
	std::vector<Point> mobGridPos;
	UpgradeLevel towerRange;
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
	//bool changesMade;
	int addIncome;
	bool changesConfirmed;
	bool buildMode;
	bool takeTouch;
	bool contWave;
	bool discardChanges;

	void checkDiscard();
	void reset();
	void handleInput();
	void renderText();
	void buildTower(int _x, int _y);
	void renderMonsters();
	void spawnMonster();
	void moveMobs();
	void updateMobGrid();
	void generateMap();
	void shoot();
	void moveShots();
	void checkCollisions();
	void renderShots();
	void buildWalls(int x, int y);
	bool isWallSpace(int x, int y);
	void renderWalls();
	bool findShortestPath();
	void buildWater(int x, int y);
	void backtrack(pvPtr iter, std::string &path) const;
	void waveOverCheck();
	void renderButtons() const;
	void renderNewTowers();
	void buildNewTowers();
public:
	Game();
	~Game();
	void Update();
	void Render();
};

#endif /* !_GAME_H */
