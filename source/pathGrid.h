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
	PathingVertex *at(int x, int y);
	void connect(int x, int y);
	void disconnect(int x, int y);
	void init();
	void print(pvPtr spawnPtr, pvPtr exitPtr);
	void setAllUnvisited();
	bool validPoint(int x, int y) const;

	// Returns true if tile is not disconnected, it can be built upon.
	bool isConnected(int x, int y) const;
};
#endif //_PATHGRID_H
