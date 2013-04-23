#ifndef _GRID_H
#define _GRID_H

#include "tile.h"
#include "tower.h"
#include "rendering.h"
#include "grass.h"
#include "Iw2D.h"
//==============================================================================
class Grid 
{
	Tile* tiles[GRID_COLUMNS][GRID_ROWS];
public:
	Grid();
	~Grid();
	void buildGrass(int posX, int posY, int leftX, int leftY);
	void buildSpawn(int posX, int posY, int leftX, int leftY);
	//Tower* buildTowerMan(int x, int y);
	void addTower(Tower* t, int x, int y);
	void buildWater(int x, int y);
	void buildExit(int posX, int posY, int leftX, int leftY);
	Tile* get(int x, int y) const;
	//const Tile & get(int x, int y) const;
	void releaseTile(int x, int y);
	void render() const;
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
inline void Grid::setAllGrass()
{
	for(int x=0; x < GRID_COLUMNS; x++)
		for(int y=0; y < GRID_ROWS; y++) 
		{
			tiles[x][y]->setColor(GRASS);
		}
}
//==============================================================================
inline void Grid::buildAllGrass(int tileSize, int verBorder, int horBorder)
{
	for(int x=0; x < GRID_COLUMNS; x++)
		for(int y=0; y < GRID_ROWS; y++) 
		{
			buildGrass(x, y, 
				x * tileSize + verBorder,
				y * tileSize + horBorder);
		}
}
//==============================================================================
inline void Grid::addTower(Tower* t, int x, int y)
{
	releaseTile(x, y);
	tiles[x][y] = t;
}
//==============================================================================
inline bool Grid::validPoint(int x, int y) const
{
	return x >= 0 && x < GRID_COLUMNS && y >= 0 && y < GRID_ROWS;
}
//==============================================================================
inline Tile* Grid::at(int x, int y) const
{
	return tiles[x][y];
}
//==============================================================================
inline void Grid::releaseTile(int x, int y)
{
	delete tiles[x][y];
}
//==============================================================================
//inline Tower* Grid::buildTowerMan(int x, int y)
//{
//	releaseTile(x, y);
//	Tower *t = new Tower(x, y);
//	tiles[x][y] = t;
//	return t;
//}
//==============================================================================
inline void Grid::buildGrass(int posX, int posY, int leftX, int leftY)
{
	tiles[posX][posY] = new Grass(leftX, leftY);
}
//==============================================================================
inline void Grid::buildWater(int x, int y)
{
	at(x, y)->setColor(WATER);
}
//==============================================================================
inline void Grid::buildSpawn(int posX, int posY, int leftX, int leftY)
{
	releaseTile(posX, posY);
	tiles[posX][posY] = new Spawn(leftX, leftY);
}
//==============================================================================
inline void Grid::buildExit(int posX, int posY, int leftX, int leftY)
{
	releaseTile(posX, posY);
	tiles[posX][posY] = new Exit(leftX, leftY);
}
//==============================================================================
#endif //_GRID_H
