
#ifndef _WALL_H
#define _WALL_H

#include "tile.h"

class Wall : public Tile, public Object
{
public:
	Wall(Image i, int x,  int y) : Tile(i), Object(x, y) {}
};
#endif //_WALL_H
