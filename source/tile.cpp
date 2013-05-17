#include "tile.h"
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
	for(TLConstIter it = listeners.begin(); it != listeners.end(); it++)
	{
		(*it)->mobEntered(mobid);
	}
}
//=============================================================================
void Tile::broadcastExit(int mobId) const
{
	for(TLConstIter it = listeners.begin(); it != listeners.end(); it++)
	{
		(*it)->mobLeft(mobId);
	}
}
//=============================================================================
void Tile::addListener(TowerListener *t)
{
	listeners.push_back(t);
}

void Tile::setPartOfPath( bool isPart )
{
	partOfPath = isPart;
}

bool Tile::isPartofPath()
{
	return partOfPath;
}

void Tile::removeListener( TowerListener *t )
{
	listeners.remove(t);
}
