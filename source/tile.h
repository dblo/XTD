#ifndef _TILE_H
#define _TILE_H
#include <list>
#include "resources.h"
#include "object.h"
#include "pathGrassListener.h"

class Tile : public Object
{
	typedef std::list<TowerListener*>::const_iterator TLConstIter;

	bool partOfPath;
	Image img;
	std::list<TowerListener*> listeners;
public:
	Tile(Image _img, int x, int y) : img(_img), Object(x, y) {partOfPath = false;}
	virtual ~Tile() {};
	Image getImage() const;
	void setImage(Image c);
	void broadcastEnter(int mobid) const;
	void broadcastExit(int mobId) const;
	void addListener(TowerListener *t);
	void removeListener(TowerListener *t);
	void clearListeners();
	void setPartOfPath(bool isPart);
	bool isPartofPath();
};
#endif //_TILE_H
