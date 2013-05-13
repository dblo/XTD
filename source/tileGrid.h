#ifndef _GRID_H
#define _GRID_H

#include<vector>

#include "Iw2D.h"

#include "tile.h"
#include "tower.h"
#include "io.h"
#include "tile.h"
//==============================================================================
class TileGrid 
{
	typedef std::vector<Tile*> Row;
	std::vector<Row> tiles;
	int rows, cols;

	Tile* at(int x, int y) const;
public:
	TileGrid(int c, int r, int tileSize);
	~TileGrid();
	void buildGrass(int posX, int posY, int LeftX, int LeftY);
	void setSpawn(int posX, int posY);
	//void buildWater(int x, int y);
	void setExit(int posX, int posY);
	void setMud(int x, int y);
	void setIce(int x, int y);
	Tile* get(int x, int y) const;
	void releaseTile(int x, int y);
	void render(const Io *io, int size) const;
	void notifyTileExit(int x, int y, int mobId);
	void notifyTileEnter(int x, int y, int mobId);
	//bool isGrass(int x, int y) const;
	bool validPoint(int x, int y) const;
	//void setAllGrass();
	//void buildAllGrass(int tileSize, int verBorder, int horBorder);
	void removeListener(int x, int y);
	void setListener(int x, int y, Tower* t);
	Image getImage(int x, int y) const;
};
#endif //_GRID_H
