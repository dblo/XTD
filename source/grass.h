#ifndef _GRASS_H
#define _GRASS_H

#include <vector>
#include <typeinfo>
#include <iostream>
#include "tile.h"
#include "path_grass_listener.h"
//=============================================================================
class Grass : public Tile
{
	std::vector<TowerListener*> listeners;
public:
	Grass(int x, int y) : Tile(GRASS, x, y) {}
	~Grass() {}

	void broadcastEnter(int mobid) const;
	void broadcastExit(int mobId) const;
	void addListener(TowerListener *t);
	void clearListeners();
};
//=============================================================================
#endif // _GRASS_H
