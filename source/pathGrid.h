#ifndef _PATHGRID_H
#define _PATHGRID_H

#include <vector>
#include "pathingVertex.h"
#include "grid.h"

class PathGrid
{
	PathingVertex grid[GRID_COLUMNS][GRID_ROWS];
public:
	PathGrid() {};
	void init();
	void add(int x, int y, Grid &tileGrid);
	void remove(int x, int y, Grid &tileGrid);
	void reset();
	PathingVertex *at(int x, int y);
};
#endif //_PATHGRID_H
