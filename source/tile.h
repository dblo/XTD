#ifndef _TILE_H
#define _TILE_H
#include <vector>
#include "resources.h"
#include "object.h"
#include "pathGrassListener.h"

class Tile : public Object
{
	Image img;
	std::vector<TowerListener*> listeners;
public:
	Tile(Image _img, int x, int y) : img(_img), Object(x, y) {}
	virtual ~Tile() {};
	Image getImage() const;
	void setImage(Image c);
	void broadcastEnter(int mobid) const;
	void broadcastExit(int mobId) const;
	void addListener(TowerListener *t);
	void clearListeners();
};
#endif //_TILE_H
