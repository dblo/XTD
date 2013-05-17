#ifndef _GRID_H
#define _GRID_H

#include<vector>

#include "Iw2D.h"

#include "tile.h"
#include "tower.h"
#include "io.h"
#include "tile.h"
//=============================================================
class TileGrid 
{
	typedef std::vector<Tile*> Row;
	std::vector<Row> tiles;
	int rows, cols;

	Tile* at(int x, int y) const;
	void buildGrass(int posX, int posY, int LeftX, int LeftY);
	Tile* get(int x, int y) const;
public:
	TileGrid(int c, int r, int tileSize);
	~TileGrid();
	void init(int verOffset, int horOffset, int tileSize);
	//void buildWater(int x, int y);
	void setMud(int x, int y);
	void setIce(int x, int y);
	//void releaseTile(int x, int y);
	void render(const Io *io, int size) const;
	void notifyTileExit(int x, int y, int mobId) const;
	void notifyTileEnter(int x, int y, int mobId) const;
	bool validPoint(int x, int y) const;
	void clearListeners(int x, int y) const;
	void setListener(int x, int y, Tower* t) const;
	Image getImage(int x, int y) const;
	void setTowerAsListener(int x, int y, Tower *t, int tileSize);
	void removeTowerAsListener(int x, int y, Tower *t, int tileSize);
	void setPathPart(int x, int y, bool isPart);
	bool isPartOfPath(int x, int y) const;
};
#endif //_GRID_H
