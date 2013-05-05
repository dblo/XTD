#ifndef _GAME_H
#define _GAME_H

#include <string>
#include <list>
#include <string>
#include <utility>
#include <map>

#include "tileGrid.h"
#include "trackingShot.h"
#include "pathingVertex.h"
#include "input.h"
#include "pathGrid.h"
#include "io.h"
#include "wall.h"

typedef std::pair<int, Tower*> TowerElement;
typedef std::pair<int, Wall*> WallElement;

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

	enum Pos
	{
		TopPos,
		BottomPos,
		LeftPos,
		RightPos,
		FullPos
	};

	Io			*io;
	TileGrid	*tileGrid;
	PathGrid	*pathGrid;
	std::string *mobPath;
	
	std::vector<Monster*> monsters;
	std::map<int, Tower*> towers;
	std::map<int, Wall*> walls;
	std::vector<Point> mobGridPos;
	std::list<TrackingShot*> shots;

	SpeedMode speedMode;
	SpeedMode rememberSpeedMode;

	bool spawnNextWave;
	bool updatePath;
	bool validPathExists;
	bool newTowerBuilt;

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
	int numOfCurrWaveMons;
	int mobsAlive;
	int spawnTimer;
	int shotDiam;
	int lives;
	int monsterRadius;

	unsigned int tileSize;
	unsigned int verticalBorder;
	unsigned int horizontalBorder;

	//Methods

	// Will construct a string of instructions to get from exit to spawnpoint
	// given that pathGrid contains one found by findShortestPath()
	void backtrack(pvPtr iter, std::string &path) const;

	void buildTower(int _x, int _y);

	// Will build wall on (x,y) and extend neighbouring walls if necessery
	void buildWalls(int x, int y);

	//void buildWater(int x, int y);

	void decreaseLives();

	// Updates the vertical wall in tile x,y when an adjoining wall is built
	// Pos is location of potential new wall in x,y
	void updateVerWall(int x, int y, Pos pos);

	// Updates the horizaontal wall in tile x,y when an adjoining wall is built
	// Pos is location of potential new wall in x,y
	void updateHorWall(int x, int y, Pos pos);

	//Returns game mode, play or paused
	Mode handleInput();

	// Returns true if a shortest path was found and g_mobPath was Updated
	bool findShortestPath();

	// Set up the map terrain using rng
	void generateMap();
	
	// Will check for collisions and handle consequences
	void manageCollisions();
	void moveMobs();
	void moveShots();
	TowerElement makeTowerElement(int x, int y, Tower *t);
	WallElement makeWallElement(int x, int y, Wall *w);

	// Generate keys for grid positions, except horizontal wall
	int getAlphaKey(int x, int y) const;

	// Generate keys for horizontall wall elements
	int getBetaKey(int x, int y) const;
	void setPathGrassListeners(int pathTravX, int pathTravY);
	void removePathGrassListeners(int pathTravX, int pathTravY);
	void wallTouch(int x, int y);
	void buildWall(int x, int y);

	Wall* makeVertWall(int x, int y, Pos pos);
	Wall* makeHorWall(int x, int y, Pos pos);

	// Manages the events that accur when a new wave begins
	void onNewWave();

	// Towers will shoot if they can
	void shoot();

	// Will spawn a monster if allowed
	void spawnMonster();

	// If a monster has moved into a new grid element, will Update new and previous
	// grid elements
	void UpdateMobGrid();

	// Updated wave number
	void UpdateStats();

	// Will check if wave is over and if so, if a new wave should be initiated
	void waveOverCheck();
	void gridTouch();
	void invokeDeleteTowerBtn();
	void invokeDmgBtn();
	void invokeBuySpeedBtn();
	void invokeBuyRangeBtn();

	// Handles the consequences of a monster dyeing
	void monsterDied(Monster *mon);
	void setMonsterSpeed();
	void setShotSpeed();

	// Updates the speed of moveable objects
	void changeGameSpeed();
	void renderMonsters() const;
	void renderShots() const;
	void renderWalls() const;
	void renderTowers() const;

	// Returns true if tower attack speed can be upgraded further
	bool towerAsUncapped() const;

	// Returns true if tower attack range can be upgraded further
	bool towerRangeUncapped() const;

	// Retuns true if (x, y) is not the spawn or exit point
	bool validIceMud(int x, int y) const;
};
#endif /* !_GAME_H */
