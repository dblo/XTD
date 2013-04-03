
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
#define GAME_WIDTH  6
#define GAME_HEIGHT 6

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
	void rollMap();
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
    void Reset();
    void Render();
	void Update(int deltaTimeMs);
};

#endif /* !_GAME_H */
