#include "tile.h"
#include <iostream>
//=============================================================================
void Tile::setImage(Image _img)
{
	img = _img;
}
//=============================================================================
Image Tile::getImage() const
{
	return img;
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
