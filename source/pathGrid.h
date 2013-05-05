#ifndef _PATHGRID_H
#define _PATHGRID_H

#include <vector>
#include "pathingVertex.h"

class PathGrid
{
	typedef std::vector<PathingVertex> Row;

	std::vector<Row> grid;
	int rows, cols;
public:
	PathGrid(int _cols, int _rows);
	void init();
	void add(int x, int y);
	void remove(int x, int y);
	void setAllUnvisited();
	PathingVertex *at(int x, int y);
	void print(pvPtr spawnPtr, pvPtr exitPtr);
	bool validPoint(int x, int y) const;

	// Returns true if tile is not disconnected, it can be built upon.
	bool available(int x, int y) const;
};
#endif //_PATHGRID_H
