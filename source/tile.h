#ifndef _TILE_H
#define _TILE_H

#include <vector>
#include "resources.h"

struct TowerListener
{
	virtual void mobLeft(int mobId) = 0;
	virtual void mobEntered(int mobId) = 0;
	virtual ~TowerListener() {};
};

// Class representing a single square in the game.
struct Tile 
{
	int color;
	Tile(int _color) : color(_color) {}
	Tile() : color(GRASS) {}
	virtual ~Tile() {};

	void SetCol(int c)
	{
		color = c;
	}

};

struct Grass : public Tile
{
	std::vector<TowerListener*> listeners;

	Grass() : Tile(GRASS) {}
	~Grass() {}

	void broadcastEnter(int mobid)
	{
		for(unsigned int i=0; i < listeners.size(); i++)
			listeners[i]->mobEntered(mobid);
	}

	void broadcastExit(int mobId)
	{
		for(unsigned int i=0; i < listeners.size(); i++)
			listeners[i]->mobLeft(mobId);
	}
};

#endif _TILE_H
