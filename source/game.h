
#ifndef _GAME_H
#define _GAME_H

#include <string>

#include "grid.h"
#include "point.h"

/*
enum GameMode
{
MODE_TITLE,
MODE_GAMEPLAY,
};

extern GameMode g_GameMode;
*/

struct Game 
{
	//enum UpdateMode
	//{
	//	MODE_ACTIVE  
	//	//MODE_PAUSED,
	//	//MODE_GAME_FINISHED,
	//};

	Monster monsters[NUM_MAX_MOBS];
	int waveNumber;
	Grid grid;
	int timer;
	//UpdateMode mode;
	bool spawnNextWave;
	int spawnX, spawnY;
	int goalX, goalY;
	int currWave;
	int numOfCurrWaveMons;
	int spawnNextMobId;
	int mobHp;
	int mobMoveSpeed;
	Point mobGridPos[NUM_MAX_MOBS];

	Game();
	~Game();
	void buildTower(int _x, int _y);
	void Reset();
	void Render();
	void renderMonsters();
	void spawnMonster();
	void Update(int deltaTimeMs);
	void moveMobs();
	void findShortestPath();
	void updateMobGrid();
};

#endif /* !_GAME_H */
