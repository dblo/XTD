#ifndef _Grass_H
#define _Grass_H

#include <vector>
#include <typeinfo>
#include <iostream>
#include "tile.h"
#include "path_Grass_listener.h"
//=============================================================================
class Grass : public Tile
{
	std::vector<TowerListener*> listeners;
public:
	Grass(int x, int y) : Tile(GrassImage, x, y) {}
	~Grass() {}

	void broadcastEnter(int mobid) const;
	void broadcastExit(int mobId) const;
	void addListener(TowerListener *t);
	void clearListeners();
};
//=============================================================================
#endif // _Grass_H
