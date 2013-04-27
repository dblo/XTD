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
//#include "controller.h"

enum Mode
{
	TitleMode,
	PlayMode,
	PausedMode,
	EndedMode
};

class Game 
{
public:
	Game(int _tileSize);

	~Game();

	Mode Update();

	void render();

	void setUpUI();

	Mode manangePausedMode();

	Mode manageTitleMode();

	Mode manageGameEnded();

	void cleanUp();

	void reset();

private:
	typedef std::pair<int, int> Point;

	enum Button {
		BuyButton,
		SpeedButton,
		PauseButton,
		IncomeButton,
		BuyDamageButton,
		UndoButton,
		UndoBottomButton,
		BuyBottomButton,
		SpeedBottomButton,
		PauseBottomButton,
		IncomeBottomButton,
		BuyDamageBottomButton,
		QuitButton,
		ContiniueButton,
		BuySpeedButton,
		BuyRangeButton,
		BuySpeedBottomButton,
		BuyRangeBottomButton,
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
	std::string *mobPath;

	std::vector<Monster*> monsters;
	std::list<TrackingShot*> shots;
	std::vector<Tower*> towers;
	std::deque<Tower*> newTowers;
	std::vector<Wall*> walls;
	std::vector<Point> mobGridPos;

	SpeedMode speedMode;
	SpeedMode rememberSpeedMode;

	bool showBuildMenu;
	bool spawnNextWave;
	bool contWaves;
	bool undoChange;
	bool updatePath;

	const int *wallPos;

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
	int buttonX;
	int buttonWid;
	int buttonHi;
	int textY;
	int textWid;
	int textHi;
	int topScore;
	int holdingPlayCounter;
	int score;

	unsigned int takeNextInputAt;
	unsigned int buttonY[NUM_BUTTON_YPOS];
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

	bool validTouch(CTouch *touch) const;

	bool buttonTouchX(CTouch *touch) const;

	bool gridTouch(CTouch *touch) const;

	void placeTowerTouch(CTouch *touch);

	bool buyTouch(CTouch *touch) const;

	void invokeBuyBtn();

	bool speedTouch(CTouch *touch) const;

	void invokeSpeedBtn();

	bool incomeTouch(CTouch *touch) const;

	void invokeIncomeBtn();

	bool pauseTouch(CTouch *touch) const;

	bool undoTouch(CTouch *touch) const;

	void invokeUndoBtn();

	bool damageTouch(CTouch *touch) const;

	void invokeDmgBtn();

	void monsterDied(Monster *mon);

	bool buySpeedTouch(CTouch *touch) const;

	void invokeBuySpeedBtn();

	bool buyRangeTouch(CTouch *touch) const;

	void invokeBuyRangeBtn();

	void UpdatePathGrid();

	void revertPathGridUpdate();
	void setButtonSize();

	void setTextAreas();
	void setBorders();

	bool isTouchingLargeBtn(CTouch *touch, unsigned int x, unsigned int y) const;
	void renderPaused(int qx, int cx, int y) const;
	void renderTitleScren(int newX, int newY) const;
	void renderGameEnded( int x, int y) const;

	void changeGameSpeed();
	void setMonsterSpeed();
	void setShotSpeed();
	void changeSpeedMode();
	void renderScore() const;
	void renderCredits() const;
	bool towerAsUncapped() const;
	bool towerDmgUncapped() const;
	bool towerRangeUncapped() const;


};
#endif /* !_GAME_H */
