
#ifndef _WALL_H
#define _WALL_H

#include "point.h"
#include "tile.h"

class Wall : public Tile
{
	Point topLeft;
public:
	Wall(Image i, int x,  int y) : Tile(i) 
	{
		topLeft.setPoint(x, y);
	}

	const Point& getTopLeft() const
	{
		return topLeft;
	}
};
#endif //_WALL_H
