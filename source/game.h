
#ifndef _GAME_H
#define _GAME_H

#include <string>
#include <list>
#include <string>

#include "grid.h"
#include "point.h"
#include "trackingShot.h"
#include "wall.h"
#include "pathingVertex.h"

class Game 
{
	//enum UpdateMode
	//{
	//	MODE_ACTIVE  
	//	//MODE_PAUSED,
	//	//MODE_GAME_FINISHED,
	//};

	Monster monsters[NUM_MAX_MOBS];
	Grid grid;
	PathingVertex pathGrid[GRID_COLUMNS][GRID_ROWS];
	//UpdateMode mode;
	bool spawnNextWave;
	Point spawnPoint;
	Point exitPoint;
	int currWave;
	int spawnNextMobId;
	int mobHp;
	int mobMoveSpeed;
	UpgradeLevel towerRange;
	Point mobGridPos[NUM_MAX_MOBS];
	int numOfCurrWaveMons;
	int mobsAlive;
	int spawnTimer;
	std::list<TrackingShot*> shots;
	std::list<Tower*> towers;
	std::list<Wall*> walls;

	void reset();
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
	void setListener(Point &pathGrass, Tower* t);
	void buildWalls(int x, int y);
	bool isWallSpace(int x, int y);
	void renderWalls();
	void setPathGrassListeners();
	void findShortestPath();
	void addToPathGrid(int x, int y);
	void removeFromPathGrid(int x, int y);
	bool validPoint(int x, int y) const;
	void initPathGrid();
	void buildWater(int x, int y);
	void backtrack(pvPtr iter, std::string &path) const;
	void waveOverCheck();
	void renderButtons() const;
public:
	Game();
	~Game();
	void Update(int deltaTimeMs);
	void Render();
};

#endif /* !_GAME_H */
