#ifndef _GAME_H
#define _GAME_H

#include <string>
#include <list>
#include <string>
#include <utility>

#include "tileGrid.h"
#include "trackingShot.h"
#include "wall.h"
#include "pathingVertex.h"
#include "input.h"
#include "pathGrid.h"
#include "io.h"

class Game 
{
public:
	Game(int _tileSize);

	~Game();

	Mode Update();

	void render();

	void reloadUI();

	Mode manangePausedMode();

	Mode manageTitleMode();

	Mode manageGameEnded();

	void cleanUp();

	void reset();

private:
	typedef std::pair<int, int> Point;

	Io			*io;
	TileGrid	*tileGrid;
	PathGrid	*pathGrid;
	std::string *mobPath;

	std::vector<Monster*> monsters;
	std::vector<Tower*> towers;
	std::vector<Tower*> newTowers;
	std::vector<Wall*> walls;
	std::vector<Point> mobGridPos;
	std::list<TrackingShot*> shots;

	SpeedMode speedMode;
	SpeedMode rememberSpeedMode;

	bool spawnNextWave;
	bool undoChange;
	bool updatePath;

	const int *wallPos;

	int gridColumns;
	int gridRows;
	int towerAsCounter;
	int towerDmgCounter;
	int towerRangeCounter;
	int spawnX;
	int spawnY;
	int exitX;
	int exitY;
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
	int shotDiam;
	int topScore;
	int score;

	unsigned int tileSize;
	unsigned int verticalBorder;
	unsigned int verticalOffset;

	//Methods

	// Will construct a string of instructions to get from exit to spawnpoint
	// given that pathGrid contains one found by findShortestPath()
	void backtrack(pvPtr iter, std::string &path) const;

	void buildTower(int _x, int _y);

	// Will build walls between (x,y) and adjacent towers
	void buildWalls(int x, int y);

	void buildWater(int x, int y);

	void decreaseScore();

	void undoLastTower();

	//Returns game mode, play or paused
	Mode handleInput();

	// Returns true if a shortest path was found and g_mobPath was Updated
	bool findShortestPath();

	// Generates spawn, exit and Water using randomization. Return true if
	// a proper map was generated and tileGrid Updated with it.
	bool generateMap();

	void increaseScore();

	// Will add contents of newTowers to towers and add associated walls
	void lockTowers();

	// Will check for collisions and handle consequences
	void manageCollisions();

	void moveMobs();

	void moveShots();

	void onNewWave();

	// Towers will shoot if they can
	void shoot();

	// Will spawn a monster if allowed
	void spawnMonster();

	// If a monster has moved into a new grid element, will Update new and previous
	// grid elements
	void UpdateMobGrid();

	// Updated wave number, credits and income on new round
	void UpdateStats();

	// Will check if wave is over and if so, if a new wave should be initiated
	void waveOverCheck();

	void invokeUndoBtn();

	void invokeIncomeBtn();

	void placeTowerTouch(CTouch *touch);//TODO rename

	void invokeDmgBtn();

	void invokeBuySpeedBtn();

	void invokeBuyRangeBtn();

	void UpdatePathGrid();///TODO rename

	void revertPathGridUpdate();

	void monsterDied(Monster *mon);


	void setMonsterSpeed();
	void setShotSpeed();
	void changeGameSpeed();

	bool towerAsUncapped() const;
	bool towerRangeUncapped() const;

	void renderMonsters() const;
	void renderNewTowers() const;
	void renderShots() const;
	void renderWalls() const;
};
#endif /* !_GAME_H */
