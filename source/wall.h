
#ifndef _WALL_H
#define _WALL_H

#include "tile.h"

class Wall : public Tile
{
	int wid, hi;
public:
	Wall(Image i, int x,  int y, int wi, int hei) : Tile(i, x, y),
	wid(wi), hi(hei) {}
	~Wall() {}

	int getWid() const
	{
		return wid;
	}
	int getHi() const
	{
		return hi;
	}
	void setFullHi(int decreaseY)
	{
		hi *= 2;
		topLeftY -= decreaseY;
	}
	void setFullWid(int decreaseX)
	{
		wid *= 2;
		topLeftX -= decreaseX;
	}
};
#endif //_WALL_H
