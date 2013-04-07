#ifndef _GRID_H
#define _GRID_H

#include "tile.h"
#include "tower.h"
#include "rendering.h"
#include "point.h"

// Container class for holding a 2 dimensional array of tiles
struct Grid 
{
	int width, height;
	Tile** tiles;

	Grid();
	~Grid();

	void releaseTile(int x, int y);
	void buildTower(int x, int y);
	void buildGrass(int x, int y);
	void buildWater(int x, int y);
	void buildSpawn(int x, int y);
	void buildExit(int x, int y);
	
	Tile* Grid::get(int x, int y);
	Tile & Get(int x, int y);
	const Tile & Get(int x, int y) const;
	void Render(int rx, int ry) const;
	void Resize(int newWidth, int newHeight);
	void rollMap(int &spawnX, int &spawnY, int &goalX, int &goalY);
	void notifyTileExit(Point &p, int mobId);
	void notifyTileEnter(Point &p, int mobId);

};

inline void Grid::releaseTile(int x, int y)
{
	delete tiles[x + y*width];
}

inline void Grid::buildTower(int x, int y)
{
	tiles[x + y*width] = new Tower(x, y);
}

inline void Grid::buildGrass(int x, int y)
{
	tiles[x + y*width] = new Grass();
}

inline void Grid::buildWater(int x, int y)
{
	tiles[x + y*width] = new Tile(WATER);
}

inline void Grid::buildSpawn(int x, int y)
{
	tiles[x + y*width] = new Tile(SPAWN);
}

inline void Grid::buildExit(int x, int y)
{
	tiles[x + y*width] = new Tile(EXIT);
}

#endif _GRID_H
