
#ifndef _GAME_H
#define _GAME_H

#include <string>
#include <list>

#include "grid.h"
#include "point.h"
#include "trackingShot.h"
#include "wall.h"

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
	std::list<TrackingShot*> shots;
	std::list<Tower*> towers;
	std::list<Wall*> walls;

	void Reset();
	void buildTower(int _x, int _y);
	void renderMonsters();
	void spawnMonster();
	void moveMobs();
	void findShortestPath();
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
public:
	Game();
	~Game();
	void Update(int deltaTimeMs);
	void Render();
};

#endif /* !_GAME_H */
