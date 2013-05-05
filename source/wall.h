
#ifndef _WALL_H
#define _WALL_H

#include "tile.h"

class Wall : public Tile
{
public:
	Wall(Image i, int topLeftX,  int topLeftY) : 
		Tile(i, topLeftX, topLeftY) {}
	~Wall() {}
};
#endif //_WALL_H
