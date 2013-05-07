#include "tile.h"
#include <iostream>
//=============================================================================
void Tile::setColor(Image c)
{
	color = c;
}
//=============================================================================
Image Tile::getColor() const
{
	return color;
}
//=============================================================================
void Tile::clearListeners()
{
	listeners.clear();
}
//=============================================================================

void Tile::broadcastEnter(int mobid) const
{
	for(unsigned int i=0; i < listeners.size(); i++)
	{
		listeners[i]->mobEntered(mobid);
	}
}
//=============================================================================
void Tile::broadcastExit(int mobId) const
{
	for(unsigned int i=0; i < listeners.size(); i++)
	{
		listeners[i]->mobLeft(mobId);
	}
}
//=============================================================================
void Tile::addListener(TowerListener *t)
{
	listeners.push_back(t);
}
//=============================================================================
