
#ifndef _WALL_H
#define _WALL_H

#include "tile.h"

class Wall : public Tile
{
public:
	Wall(Image i, int x,  int y) : Tile(i, x, y) {}
	~Wall() {}
};
#endif //_WALL_H
