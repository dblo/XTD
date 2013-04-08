#ifndef _TILE_H
#define _TILE_H

#include "resources.h"

class Tile 
{
	int color;
public:
	Tile(int _color) : color(_color) {}
	virtual ~Tile() {};
	void setColor(int c);	
	int getColor() const;
};

struct Spawn : public Tile
{
	Spawn() : Tile(SPAWN) {}
};

struct Exit : public Tile
{
	Exit() : Tile(EXIT) {}
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
