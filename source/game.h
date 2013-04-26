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

enum Mode
{
	TitleMode,
	PlayMode,
	PausedMode,
	EndedMode
};

//
class Game 
{
public:
	Game(int _tileSize);

	~Game();

	Mode Update();

	void render();

	void setUpUI();

	//void setTileSize(int _tileSize);

	Mode manangePausedMode();

	Mode manageTitleMode();

	Mode manageGameEnded();

	void cleanUp();

	void reset();

private:
	typedef std::pair<int, int> Point;

	//enum UpgradeLevel change format
	//{
	//	LEVEL1,
	//	LEVEL2,
	//	LEVEL3
	//};

	enum Button {
		TowerButton,
		SpeedButton,
		PauseButton,
		IncomeButton,
		ContWavesButton,
		UndoButton,
		UndoBottomButton,
		TowerBottomButton,
		SpeedBottomButton,
		PauseBottomButton,
		IncomeBottomButton,
		ContWavesBottomButton,
		QuitButton,
		ContiniueButton
	};

	enum SpeedMode
	{
		ImmobileSpeedMode,
		NormalSpeedMode,
		FastSpeedMode
	};

	enum Text {
		CreditsText,
		IncomeText,
		WaveText,
		ScoreText
	};

	TileGrid *tileGrid;
	PathGrid *pathGrid;
	Monster *monsters[MAX_MONSTER_COUNT];
	std::string *mobPath;

	std::list<TrackingShot*> shots;
	std::vector<Tower*> towers;
	std::deque<Tower*> newTowers;
	std::vector<Wall*> walls;
	std::vector<Point> mobGridPos;

	//UpgradeLevel towerRange;
	SpeedMode speedMode;
	SpeedMode rememberSpeedMode;

	int takeNextInputAt;

	bool showBuildMenu;
	bool spawnNextWave;
	bool contWaves;
	bool undoChange;

	const int *wallPos;

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
	int buttonX;
	int buttonWid;
	int buttonHi;
	int textY;
	int textWid;
	int textHi;
	int topScore;
	int holdingPlayCounter;

	signed int score;

	unsigned int buttonY[12];
	unsigned int textX[4];	

	unsigned int verticalBorder;
	unsigned int horizontalBorder;
	unsigned int tileSize;
	unsigned int verticalOffset;
	unsigned int largeButtonWid;
	unsigned int largeButtonHi;

	//Methods

	// Will construct a string of instructions to get from exit to spawnpoint
	// given that pathGrid contains one found by findShortestPath()
	void backtrack(pvPtr iter, std::string &path) const;

	void buildTower(int _x, int _y);

	// Will build walls between (x,y) and adjacent towers
	void buildWalls(int x, int y);

	void buildWater(int x, int y);

	void decreaseScore();

	void drawText(Text x, char c, int text) const; //move to rendering?

	void undoLastTower();

	// Returns true if a shortest path was found and g_mobPath was Updated
	bool findShortestPath();

	// Generates spawn, exit and Water using randomization. Return true if
	// a proper map was generated and tileGrid Updated with it.
	bool generateMap();

	//Returns game mode, play or paused
	Mode handleInput();

	void increaseScore();

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

	// If a monster has moved into a new grid element, will Update new and previous
	// grid elements
	void UpdateMobGrid();

	// Updated wave number, credits and income on new round
	void UpdateStats();

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

	void UpdatePathGrid();

	void revertPathGridUpdate();
	void setButtonSize();

	void setTextAreas();
	void setBorders();

	bool isTouchingLargeBtn(CTouch *touch, unsigned int x, unsigned int y) const;
	void renderPaused(int qx, int cx, int y) const;
	void renderTitleScren(int newX, int newY) const;
	void renderGameEnded( int x, int y) const;
	void onNewWave();

	void changeGameSpeed();
	void setMonsterSpeed();
	void setShotSpeed();
	void changeSpeedMode();
};
#endif /* !_GAME_H */
