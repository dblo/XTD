#ifndef _GAME_H
#define _GAME_H

#include <string>
#include <list>
#include <string>
#include <utility>
#include <map>

#include "tileGrid.h"
#include "baseShot.h"
#include "pathingVertex.h"
#include "input.h"
#include "pathGrid.h"
#include "io.h"
#include "wall.h"
#include "progBar.h"

typedef std::pair<int, Tower*> TowerElement;
typedef std::pair<int, Wall*> WallElement;

class Game 
{
public:
	Game(int _tileSize, Io *_io);
	~Game();

	void cleanUp();

	// Returns the current game state
	Mode manageGameEnded();

	// Returns the current game state
	Mode manangePausedMode();

	void render();

	// Set up the game for a new session
	void reset();

	void resetProgBars();

	// Determines the size and position of the ui components
	void setUI();

	// Calls the vaious main methods that constitute the game logic
	// Returns the game state current game state
	Mode update();

private:
	// The gamespeed states
	enum SpeedMode
	{
		ImmobileSpeedMode,
		NormalSpeedMode,
		FastSpeedMode
	};

	enum Selected
	{
		NothingSelected,
		WallSelected,
		TowerSelected,
		Button1Selected,
		Button2Selected,
		Button3Selected,
		SellBtnSelected
	};

	typedef std::pair<int, int> Point;
	typedef std::vector<Monster*>::const_iterator MonsterConstIter;
	typedef std::map<int, Tower*>::const_iterator TowerMapConstIter;
	typedef std::map<int, Wall*>::const_iterator WallMapConstIter;
	typedef std::list<BaseShot*>::const_iterator ShotsConstIter;
	
	Io	* const io;
	TileGrid	*tileGrid;
	PathGrid	*pathGrid;
	std::string *mobPath;

	std::vector<Monster*>	monsters;
	std::map<int, Tower*>	towers;
	std::map<int, Wall*>	walls;
	std::vector<Point>		mobGridPos;
	std::list<BaseShot*>	shots;

	ProgBar *dmgProgressBar;
	ProgBar *asProgressBar;
	ProgBar *ranProgressBar;

	Selected gridSelection;
	Selected btnSelection;

	SpeedMode speedMode;
	SpeedMode rememberSpeedMode;

	bool spawnNextWave;
	bool showMenu;

	int movementSpeeds[6]; //todo const
	int monsterDiam;
	int gridColumns;
	int gridRows;
	int towerAsCounter;
	int towerDmgCounter;
	int towerRangeCounter;
	int spawnX;
	int spawnY;
	int exitX;
	int exitY;
	int selectedX;
	int selectedY;
	int spawnNextMobId;
	int monsterHP;
	int monsterRadius;
	int monstersAlive;
	int currWave;
	int credits;
	int numOfCurrWaveMons;
	int spawnTimer;
	int shotDiameter;
	int lives;

	unsigned int wallCap;
	unsigned int tileSize;

	// The width between the left edge of the screen and the left edge of 
	// the grid
	unsigned int gridOffset;
	
	// Size of the border at the edges of the screen
	unsigned int border;

	// Add newTower and decrease credits by price
	void addTower( TowerElement newTower, int price);

	// Will construct a string of instructions to get from exit to spawnpoint
	// given that pathGrid contains one found by Game::findShortestPath()
	void backtrack(pvPtr iter, std::string &path) const;

	// Adds new base tower of below type and decreases credits by base tower cost
	void buildRedTower(int x, int y);
	void buildTealTower(int x, int y);
	void buildYellowTower(int x, int y);

	// Will build wall on (x,y)
	void buildWall(int x, int y);

	// Updates the speed of moveable objects
	void changeGameSpeed();

	// Return true if a new wall can be added
	bool canAddWall();

	// Retuns key for a grid position
	int getKey(int x, int y) const;

	// Returns a grid coordiante, takes a screen value
	int getGridCoordX(int xVal) const;
	int getGridCoordY(int yVal) const;

	// Depending on current speedMode will build or remove a wall
	void gridTouch();

	//Returns game mode, play or paused
	Mode handleInput();

	void upgradeTowerDamage();
	void upgradeTowerSpeed();
	void upgradeTowerRange();

	// Returns true if a tower is built on x,y
	bool isTower(int x, int y) const;

	// Returns true if a wall is built on x,y
	bool isBuilt(int x, int y) const;

	// Retunvalue indicates which neighbouring tiles contain towers.
	// Neighbouring walls can be updated.
	unsigned int findNeighbours(int x, int y, bool updateNeighbours);

	// Returns true if a shortest path was found and g_mobPath was Updated
	bool findShortestPath();

	// Check if game over and if so, manage that
	bool gameEnded();

	// Set up the map terrain using rng
	void generateMap();

	// Uses the any neighbouring walls to determine wall type (shape)
	Image getWallImage(unsigned int neighbours) const;

	// Returns a <int, Tower*> pair for tile (x,y)
	TowerElement makeTowerElement(int x, int y, Tower *t);

	// Returns a <int, Wall*> pair for tile (x,y)
	WallElement makeWallElement(int x, int y, Wall *w);

	// Will check for collisions and handle consequences
	void manageCollisions();

	// Handles the consequences of a monster dyeing
	void monsterDied(Monster *mon);

	void moveMobs();
	void moveShots();

	// Manages the events that accur when a new wave begins
	void onNewWave();

	// Removes all listeners (towers that listen to monsters entering/exiting 
	// a tile)
	void removePathGrassListeners();

	void removeTower(int x, int y);
	void removeWall(int x, int y);
	void renderButtons() const;

//	void RenderBasicButtons() const;

	void renderText() const;

	void renderUpgWallTxt( char * str ) const;

	void RenderBasicUpgText( char * str ) const;

	void renderMonsters() const;
	void renderShots() const;
	void renderWalls() const;
	void renderTowers() const;
	void setMonsterSpeed(Monster *mon, int gridPosX, int gridPosY);
	void setShotSpeed(BaseShot *shot);

	// Travels the monster path and sets all towers sufficently close to a tile
	// to listen to monsters entering/exiting it
	void setPathGrassListeners();

	// Towers will shoot if they can
	void shoot();

	// Will spawn a monster if allowed
	void spawnMonster();

	// Returns true if tower damage can be upgraded further
	bool towerDmgUncapped() const;

	// Returns true if tower attack speed can be upgraded further
	bool towerAsUncapped() const;

	// Returns true if tower attack range can be upgraded further
	bool towerRangeUncapped() const;

	// If a monster has moved into a new grid element, will Update new and 
	// previous grid elements
	void updateMobGrid();

	// Updates the wall in tile x,y when an adjoining wall is built
	void updateWall(int x, int y);

	// Returns true if (x, y) is not the spawn or exit point
	bool notSpawnOrExit(int x, int y) const;

	// Handle building and removing walls
	void wallTouch(int x, int y);

	// Will check if wave is over and if so, if a new wave should be initiated
	void waveOverCheck();

	// Manage state of upgrades
	void updateUpgrades();

	void renderProgressBars() const;
	void addCredits(int addAmount);

	// Returns true if purchase was successful and credits were decreased
	bool attemptPurchase(int amount);

	// Tags a tile/button as selected after it's been touched
	void selectStruct(int x, int y);

	void renderStructSelection() const;
	void invokeMenuBtn();

	// Set the current selection to nothing
	void clearSelect();

	void renderUpgradeButton(int cost, bool uncapped, 
		bool inProgress, Image img, Button btn) const;

	// Returns the tower known to exists at (x,y)
	Tower* getTower(int x, int y) const;
};
#endif /* !_GAME_H */
