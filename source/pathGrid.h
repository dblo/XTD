#ifndef _PATHGRID_H
#define _PATHGRID_H

#include <vector>
#include "pathingVertex.h"
#include "tileGrid.h"

class PathGrid
{
	typedef std::vector<PathingVertex> Row;

	std::vector<Row> grid;
	int rows, cols;
public:
	PathGrid(int _cols, int _rows);
	void init();
	void add(int x, int y, TileGrid &tileGrid);
	void remove(int x, int y, TileGrid &tileGrid);
	void setAllUnvisited();
	PathingVertex *at(int x, int y);
	void print(pvPtr spawnPtr, pvPtr exitPtr);
	bool available(int x, int y) const;
};
#endif //_PATHGRID_H
