
#ifndef _GAME_H
#define _GAME_H

#include "monster.h"
#include "resources.h"

/*
enum GameMode
{
MODE_TITLE,
MODE_GAMEPLAY,
};

extern GameMode g_GameMode;
*/

// Width and height of the playing area
#define GAME_WIDTH  22
#define GAME_HEIGHT 13
#define HORIZONTAL_BORDER 30
#define VERTICAL_BORDER 20
#define NUM_TILE_TYPES 4

// Class representing a single square in the game.
struct Tile 
{
	int color;
	Tile() : color(0) {}

	void SetCol(int c)
	{
		color = c;
	}

};

// Container class for holding a 2 dimensional array of tiles
struct Grid 
{
	int width, height;
	Tile* tiles;

	Grid() : width(0), height(0), tiles(0) {}

	~Grid() 
	{
		delete[] tiles;
	}

	Tile & Get(int x, int y);
	const Tile & Get(int x, int y) const;
	void Render(int rx, int ry) const;
	void Resize(int newWidth, int newHeight);
	void rollMap(int &spawnX, int &spawnY, int &goalX, int &goalY);
};

struct Game 
{
	enum UpdateMode
	{
		MODE_ACTIVE  
		//MODE_PAUSED,
		//MODE_GAME_OVER,
	};

	Monster* monsters[SIMUL_WAVES_ALLOWED];
	int waveNumber[SIMUL_WAVES_ALLOWED];
	Grid grid;
	int timer;
	UpdateMode mode;
	bool spawnNextWave;
	int spawnX, spawnY;
	int goalX, goalY;
	int currWave;
	int mobHp;
	int mobMoveSpeed;

	Game();
	~Game();
	void Reset();
	void Render();
	void renderMonsters();
	void spawnWave();
	void Update(int deltaTimeMs);
};

#endif /* !_GAME_H */
