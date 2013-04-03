/*
Made by Olle Olsson
*/

#ifndef _GAME_H
#define _GAME_H

/*
enum GameMode
{
    MODE_TITLE,
    MODE_GAMEPLAY,
};

extern GameMode g_GameMode;
*/

// Width and height of the playing area
#define GAME_WIDTH  10
#define GAME_HEIGHT 16

// Class representing a single square in the game.
struct Tile 
{
	int color;

	Tile() : color(0) {}

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
	const Tile & Get(int x, int y) const;
	void Grid::Resize(int newWidth, int newHeight);
};

struct Game 
{
	 enum UpdateMode
    {
        MODE_ACTIVE  
        //MODE_PAUSED,
        //MODE_GAME_OVER,
    };

	Grid grid;
	int timer;
	UpdateMode mode;
	Game();
    //void Reset();
    void Render();
	void Update(int deltaTimeMs);
};

#endif /* !_GAME_H */
