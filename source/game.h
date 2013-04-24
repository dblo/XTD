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

enum GameMode //namespace ist för mode prefix?
	{
		MODE_TITLE,
		MODE_GAMEPLAY,
		MODE_PAUSED,
		MODE_GAME_ENDED
	};

//
class Game 
{
public:
	Game(int _tileSize);

	~Game();

	void update();

	void render();

	void setUpUI();

	void setTileSize(int _tileSize);

	GameMode getGameMode() const;

	void manangePausedMode();

	bool manageTitleMode();
	
	void newGame();

private:
	typedef std::pair<int, int> Point;
	
	//enum UpgradeLevel
	//{
	//	LEVEL1,
	//	LEVEL2,
	//	LEVEL3
	//};

	enum Buttons {
		BUYTOWERBUTTON,
		SPEEDBUTTON,
		PAUSEBUTTON,
		INCOMEBUTTON,
		CONTWAVESBUTTON,
		UNDOBUTTON,
		UNDOBUTTONBOTTOM,
		BUYTOWERBUTTONBOTTOM,
		SPEEDBUTTONBOTTOM,
		PAUSEBUTTONBOTTOM,
		INCOMEBUTTONBOTTOM,
		CONTWAVESBUTTONBOTTOM,
		QUITBUTTON,
		CONTINIUEBUTTON
	};

	enum GameSpeedMode
	{
		NEWWAVE,
		NORMAL,
		FAST
	};

	enum Texts {
		CREDITSTEXT,
		INCOMETEXT,
		WAVETEXT,
		SCORETEXT
	};

	GameMode gameMode;
	std::string *mobPath;
	const int *wallPos;
	Grid *tileGrid;
	PathGrid *pathGrid;
	Monster *monsters[MAX_MONSTER_COUNT];
	std::list<TrackingShot*> shots;
	std::vector<Tower*> towers;
	std::deque<Tower*> newTowers;
	std::vector<Wall*> walls;
	std::vector<Point> mobGridPos;
	//UpgradeLevel towerRange;
	GameSpeedMode speedMode;
	//UpdateMode mode;
	//ChangeManager undoQueue;
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
	signed int score;
	int numOfCurrWaveMons;
	int mobsAlive;
	int spawnTimer;
	int addIncome;
	bool buildMode;
	bool takeTouch;
	bool contWaves;
	bool undoChange;
	void onNewWave();
	unsigned int buttonY[12];
	int buttonX;
	int buttonWid;
	int buttonHi;
	unsigned int textX[4];
	int textY;
	int textWid;
	int textHi;
	unsigned int lockedTowers;
	unsigned int verticalBorder;
	unsigned int horizontalBorder;
	unsigned int tileSize;
	unsigned int verticalOffset;
	int largeButtonWid;
	//Methods

	// Will construct a string of instructions to get from exit to spawnpoint
	// given that pathGrid contains one found by findShortestPath()
	void backtrack(pvPtr iter, std::string &path) const;

	void buildTower(int _x, int _y);

	// Will build walls between (x,y) and adjacent towers
	void buildWalls(int x, int y);

	void buildWater(int x, int y);

	void decreaseScore();

	void drawText(Texts x, char c, int text) const; //move to rendering?

	void undoLastTower();

	// Returns true if a shortest path was found and g_mobPath was updated
	bool findShortestPath();

	// Generates spawn, exit and water using randomization. Return true if
	// a proper map was generated and tileGrid updated with it.
	bool generateMap();

	void handleInput();

	void increaseScore();

	void init();

	// Will add contents of newTowers to towers and add associated walls
	void lockTowers();

	// Will check for collisions and handle consequences
	void manageCollisions();

	void moveMobs();

	void moveShots();

	void renderAlphaButton(int color, int yIndex) const;

	void renderButtons() const;

	void renderMonsters() const;

	void renderNewTowers() const;

	void renderText() const;

	void renderShots() const;

	void renderWalls() const;

	// Towers will shoot if they can
	void shoot();

	// Will spawn a monster if allowed
	void spawnMonster();

	// If a monster has moved into a new grid element, will update new and previous
	// grid elements
	void updateMobGrid();

	// Updated wave number, credits and income on new round
	void updateStats();

	// Will check if wave is over and if so, if a new wave should be initiated
	void waveOverCheck();

	bool validTouch(CTouch *touch);

	bool buttonTouchX(CTouch *touch);

	bool gridTouch(CTouch *touch);

	void placeTowerTouch(CTouch *touch);

	bool towerTouch(CTouch *touch);

	void invokeTowerBtn();

	bool speedTouch(CTouch *touch);

	void invokeSpeedBtn();

	bool incomeTouch(CTouch *touch);

	void invokeIncomeBtn();

	bool pauseTouch(CTouch *touch);

	void invokePauseBtn();

	bool undoTouch(CTouch *touch);

	void invokeUndoBtn();

	bool contTouch(CTouch *touch);

	void invokeContBtn();

	void monsterDied(Monster *mon);

	void updatePathGrid();

	void revertPathGridUpdate();
	void setButtonSize();

	void setTextAreas();
	void setBorders();

	bool isTouchingLargeBtn(CTouch *touch, int x, int y) const;
	void renderPaused(int qx, int cx, int y) const;
	void renderTitleScren(int newX, int newY) const;
	void manageGameEnded();
	void renderGameEnded( int x, int y) const;
};
#endif /* !_GAME_H */
