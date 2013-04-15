#include "PathGrid.h"
//==============================================================================
PathingVertex *PathGrid::at(int x, int y) 
{
	return &grid[x][y];
}
//==============================================================================
/*
Connects all nodes into a grid.
*/
void PathGrid::init()
{
	reset();//do below

	for(int x=1; x < GRID_COLUMNS-1; x++)
	{
		for(int y=1; y < GRID_ROWS-1; y++)
		{
			grid[x][y].addRight(&grid[x+1][y]);
			grid[x][y].addLeft(&grid[x-1][y]);
			grid[x][y].addBelow(&grid[x][y+1]);
			grid[x][y].addAbove(&grid[x][y-1]);
		}
	}

	for(int y=1; y < GRID_ROWS-1; y++)
	{
		grid[0][y].addRight(&grid[1][y]);
		grid[0][y].addBelow(&grid[0][y+1]);
		grid[0][y].addAbove(&grid[0][y-1]);
		grid[GRID_COLUMNS-1][y].addLeft(&grid[GRID_COLUMNS-2][y]);
		grid[GRID_COLUMNS-1][y].addBelow(&grid[GRID_COLUMNS-1][y+1]);
		grid[GRID_COLUMNS-1][y].addAbove(&grid[GRID_COLUMNS-1][y-1]);
	}

	for(int x=1; x < GRID_COLUMNS-1; x++)
	{
		grid[x][0].addBelow(&grid[x][1]);
		grid[x][0].addRight(&grid[x+1][0]);
		grid[x][0].addLeft(&grid[x-1][0]);

		grid[x][GRID_ROWS-1].addAbove(&grid[x][GRID_ROWS-2]);
		grid[x][GRID_ROWS-1].addLeft(&grid[x-1][GRID_ROWS-1]);
		grid[x][GRID_ROWS-1].addRight(&grid[x+1][GRID_ROWS-1]);
	}

	grid[0][0].addRight(&grid[1][0]);
	grid[0][0].addBelow(&grid[0][1]);
	grid[0][GRID_ROWS-1].addAbove(&grid[0][GRID_ROWS-2]);
	grid[0][GRID_ROWS-1].addRight(&grid[1][GRID_ROWS-1]);
	grid[GRID_COLUMNS-1][GRID_ROWS-1].addLeft(&grid[GRID_COLUMNS-2][GRID_ROWS-1]);
	grid[GRID_COLUMNS-1][0].addLeft(&grid[GRID_COLUMNS-2][0]);
	grid[GRID_COLUMNS-1][0].addBelow(&grid[GRID_COLUMNS-1][1]);
	grid[GRID_COLUMNS-1][GRID_ROWS-1].addAbove(&grid[GRID_COLUMNS-1][GRID_ROWS-2]);
}
//==============================================================================
void PathGrid::add(int x, int y, Grid &tileGrid)
{
	pvPtr vert = &grid[x][y];
	if(tileGrid.validPoint(x-1,y) && tileGrid.isGrass(x-1, y))
	{
		vert->addLeft(&grid[x-1][y]);
		grid[x-1][y].addRight(vert);
	}
	if(tileGrid.validPoint(x+1,y) && tileGrid.isGrass(x+1, y))
	{
		vert->addRight(&grid[x+1][y]);
		grid[x+1][y].addLeft(vert);
	}
	if(tileGrid.validPoint(x,y+1) && tileGrid.isGrass(x, y+1))
	{
		vert->addBelow(&grid[x][y+1]);
		grid[x][y+1].addAbove(vert);
	}
	if(tileGrid.validPoint(x,y-1) && tileGrid.isGrass(x, y-1))
	{
		vert->addAbove(&grid[x][y-1]);
		grid[x][y-1].addBelow(vert);
	}
}
//==============================================================================
void PathGrid::remove(int x, int y, Grid &tileGrid)
{
	pvPtr vert = &grid[x][y];
	if(tileGrid.validPoint(x-1,y) && tileGrid.isGrass(x-1, y))
	{
		vert->removeLeft();
		grid[x-1][y].removeRight();
	}
	if(tileGrid.validPoint(x+1,y) && tileGrid.isGrass(x+1, y))
	{
		vert->removeRight();
		grid[x+1][y].removeLeft();
	}
	if(tileGrid.validPoint(x,y+1) && tileGrid.isGrass(x, y+1))
	{
		vert->removeBelow();
		grid[x][y+1].removeAbove();
	}
	if(tileGrid.validPoint(x,y-1) && tileGrid.isGrass(x, y-1))
	{
		vert->removeAbove();
		grid[x][y-1].removeBelow();
	}
}
//==============================================================================
void PathGrid::reset()
{
	for(int i=0; i < GRID_COLUMNS; i++)
		for(int j=0; j < GRID_ROWS; j++)
			grid[i][j].setUnvisited();
}
//==============================================================================
void PathGrid::print(pvPtr spawnPtr, pvPtr exitPtr)
{
	std::cout << "====================================================\n";
	for(int i=0; i< GRID_ROWS;i++) 
	{
		for(int j=0; j<GRID_COLUMNS; j++)
		{
			if(&grid[j][i] == spawnPtr)
				std::cout << "s";
			else if(&grid[j][i] == exitPtr)
				std::cout << "e";
			else
				std::cout << grid[j][i].wasVisited();
		}
		std::cout << std::endl;
	}
	std::cout << "====================================================\n";
}
//==============================================================================
