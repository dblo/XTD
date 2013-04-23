#ifndef _TILE_H
#define _TILE_H

#include "resources.h"
#include "object.h"

class Tile : public Object
{
	int color;
public:
	Tile(int _color, int x, int y) : color(_color), Object(x, y) {}
	virtual ~Tile() {};
	void setColor(int c);	
	int getColor() const;
};

struct Spawn : public Tile
{
	Spawn(int x, int y) : Tile(SPAWN, x, y) {}
};

struct Exit : public Tile
{
	Exit(int x, int y) : Tile(EXIT, x, y) {}
};

inline void Tile::setColor(int c)
{
	color = c;
}

inline int Tile::getColor() const
{
	return color;
}

#endif //_TILE_H
