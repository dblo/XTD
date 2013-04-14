#include "PathGrid.h"
//==============================================================================
PathingVertex *PathGrid::at(int x, int y) 
{
	return &grid[x][y];
}
//==============================================================================
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
	for(int y=0; y < GRID_ROWS; y++)
		grid[0][y].addRight(&grid[1][y]);

	for(int y=0; y < GRID_ROWS; y++)
		grid[GRID_COLUMNS-1][y].addLeft(&grid[GRID_COLUMNS-2][y]);

	for(int x=0; x < GRID_COLUMNS; x++)
		grid[x][0].addBelow(&grid[x][1]);

	for(int x=0; x < GRID_COLUMNS; x++)
		grid[x][GRID_ROWS-1].addAbove(&grid[x][GRID_ROWS-2]);
}
//==============================================================================
void PathGrid::add(int x, int y, Grid &tileGrid)
{
	pvPtr vert = &grid[x][y];
	if(tileGrid.validPoint(x-1,y) && tileGrid.isGrassAt(x-1, y))
	{
		vert->addLeft(&grid[x-1][y]);
		grid[x-1][y].addRight(vert);
	}
	if(tileGrid.validPoint(x+1,y) && tileGrid.isGrassAt(x+1, y))
	{
		vert->addRight(&grid[x+1][y]);
		grid[x+1][y].addLeft(vert);
	}
	if(tileGrid.validPoint(x,y+1) && tileGrid.isGrassAt(x, y+1))
	{
		vert->addBelow(&grid[x][y+1]);
		grid[x][y+1].addAbove(vert);
	}
	if(tileGrid.validPoint(x,y-1) && tileGrid.isGrassAt(x, y-1))
	{
		vert->addAbove(&grid[x][y-1]);
		grid[x][y-1].addBelow(vert);
	}
}
//==============================================================================
void PathGrid::remove(int x, int y, Grid &tileGrid)
{
	pvPtr vert = &grid[x][y];
	if(tileGrid.validPoint(x-1,y) && tileGrid.isGrassAt(x-1, y))
	{
		vert->removeLeft();
		grid[x-1][y].removeRight();
	}
	if(tileGrid.validPoint(x+1,y) && tileGrid.isGrassAt(x+1, y))
	{
		vert->removeRight();
		grid[x+1][y].removeLeft();
	}
	if(tileGrid.validPoint(x,y+1) && tileGrid.isGrassAt(x, y+1))
	{
		vert->removeBelow();
		grid[x][y+1].removeAbove();
	}
	if(tileGrid.validPoint(x,y-1) && tileGrid.isGrassAt(x, y-1))
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
