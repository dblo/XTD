#ifndef _GRID_H
#define _GRID_H

#include<vector>

#include "Iw2D.h"

#include "tile.h"
#include "tower.h"
#include "io.h"
#include "Grass.h"
//==============================================================================
class TileGrid 
{
	typedef std::vector<Tile*> Row;
	std::vector<Row> tiles;
	int rows, cols;
public:
	TileGrid(int c, int r);
	~TileGrid();
	void buildGrass(int posX, int posY, int LeftX, int LeftY);
	void buildSpawn(int posX, int posY, int LeftX, int LeftY);
	void addTower(Tower* t, int x, int y);
	void buildWater(int x, int y);
	void buildExit(int posX, int posY, int LeftX, int LeftY);
	Tile* get(int x, int y) const;
	void releaseTile(int x, int y);
	void render(const Io *io, int size) const;
	void notifyTileExit(int x, int y, int mobId);
	void notifyTileEnter(int x, int y, int mobId);
	bool isGrass(int x, int y) const;
	Tile* at(int x, int y) const;
	bool validPoint(int x, int y) const;
	void setAllGrass();
	void buildAllGrass(int tileSize, int verBorder, int horBorder);
	void removeListener(int x, int y);
	void setListener(int x, int y, Tower* t);
	void removePathGrassListeners(int pathTravX, int pathTravY, const std::string &path);
	void setPathGrassListeners(int pathTravX, int pathTravY, const std::string &path);
	bool isTower(int x, int y);
};
//==============================================================================
inline void TileGrid::addTower(Tower* t, int x, int y)
{
	releaseTile(x, y);
	tiles[x][y] = t;
}
//==============================================================================
inline bool TileGrid::validPoint(int x, int y) const
{
	return x >= 0 && x < cols && 
		y >= 0 && y < rows;
}
//==============================================================================
inline Tile* TileGrid::at(int x, int y) const
{
	return tiles[x][y];
}
//==============================================================================
inline void TileGrid::releaseTile(int x, int y)
{
	delete tiles[x][y];
}
//==============================================================================
inline void TileGrid::buildGrass(int posX, int posY, int LeftX, int LeftY)
{
	tiles[posX][posY] = new Grass(LeftX, LeftY);
}
//==============================================================================
inline void TileGrid::buildWater(int x, int y)
{
	at(x, y)->setColor(WaterImage);
}
//==============================================================================
inline void TileGrid::buildSpawn(int posX, int posY, int LeftX, int LeftY)
{
	releaseTile(posX, posY);
	tiles[posX][posY] = new Spawn(LeftX, LeftY);
}
//==============================================================================
inline void TileGrid::buildExit(int posX, int posY, int LeftX, int LeftY)
{
	releaseTile(posX, posY);
	tiles[posX][posY] = new Exit(LeftX, LeftY);
}
//==============================================================================
#endif //_GRID_H
