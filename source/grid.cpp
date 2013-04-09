#include "grid.h"
#include <iostream>

Grid::Grid()
{
	tiles = new Tile*[GRID_COLUMNS * GRID_ROWS];

	for(int x=0; x < GRID_COLUMNS; x++)
		for(int y=0; y < GRID_ROWS; y++) 
		{
			buildGrass(x,y);
		}
}

Grid::~Grid()
{
	for(int i=0; i < GRID_COLUMNS * GRID_ROWS; i++)
	{
		delete tiles[i];
	}
	delete[] tiles;
}

bool Grid::isGrassAt(int x, int y) const
{
	return at(x, y)->getColor() == GRASS;
}

const Tile & Grid::get(int x, int y) const
{
	//IwAssertMsg(APP, Valid(x,y), ("Coordinate out of range for Grid (%d,%d)", x, y));
	return *tiles[x + y*GRID_COLUMNS];
}

Tile* Grid::get(int x, int y)
{
	return tiles[x + y*GRID_COLUMNS];
}

Tile* Grid::get(Point &p)
{
	return tiles[p.getX() + p.getY()*GRID_COLUMNS];
}

//(rx, ry) => (0, 0)
void Grid::render() const
{
	for (int x=0; x<GRID_COLUMNS; x++)
	{
		for (int y=0; y<GRID_ROWS; y++)
		{
			drawTile(
				get(x,y).getColor(),
				x*g_tileSize,
				y*g_tileSize
				);
		}
	}
}

void Grid::notifyTileExit(const Point &p, int mobId)
{
	Grass *tile = 0;
	
	if(tile = dynamic_cast<Grass*>(get(p.getX(), p.getY())))
		tile->broadcastExit(mobId);
}

void Grid::notifyTileEnter(const Point &p, int mobId)
{
	Grass *tile = 0;
	
	if(tile = dynamic_cast<Grass*>(get(p.getX(), p.getY())))
		tile->broadcastEnter(mobId);
}
