#ifndef _GRASS_H
#define _GRASS_H

#include <vector>
#include <typeinfo>
#include <iostream>
#include "tile.h"
#include "path_grass_listener.h"

class Grass : public Tile
{
	std::vector<TowerListener*> listeners;
public:
	Grass() : Tile(GRASS) {}
	~Grass() {}

	void broadcastEnter(int mobid) const;
	void broadcastExit(int mobId) const;
	void addListener(TowerListener *t);
	void clearListeners();
};

inline void Grass::clearListeners()
{
	listeners.clear();
}

inline void Grass::broadcastEnter(int mobid) const
{
	for(unsigned int i=0; i < listeners.size(); i++)
		listeners[i]->mobEntered(mobid);
}

inline void Grass::broadcastExit(int mobId) const
{
	for(unsigned int i=0; i < listeners.size(); i++)
		listeners[i]->mobLeft(mobId);
}

inline void Grass::addListener(TowerListener *t)
{
	listeners.push_back(t);
}

#endif // _GRASS_H
