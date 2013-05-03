#ifndef _TILE_H
#define _TILE_H
#include <vector>
#include "resources.h"
#include "object.h"
#include "path_Grass_listener.h"

class Tile : public Object
{
	Image color;
	std::vector<TowerListener*> listeners;
public:
	Tile(Image _color, int x, int y) : color(_color), Object(x, y) {}
	virtual ~Tile() {};
	Image getColor() const;
	void setColor(Image c);
	void broadcastEnter(int mobid) const;
	void broadcastExit(int mobId) const;
	void addListener(TowerListener *t);
	void clearListeners();
};

#endif //_TILE_H
