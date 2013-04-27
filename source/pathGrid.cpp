#include "PathGrid.h"

PathGrid::PathGrid(int colsth, int height) : cols(colsth), rows(height)
{
	grid.resize(cols);

	for(int i=0; i < cols; i++)
		grid[i].resize(rows);//xxx
}
//==============================================================================
PathingVertex *PathGrid::at(int x, int y) 
{
	return &grid[x][y];
}
//==============================================================================
bool PathGrid::available(int x, int y) const
{
	return grid[x][y].getConnected();
}
//==============================================================================
/*
Connects all nodes into a grid.
*/
void PathGrid::init()
{
	setAllUnvisited();//do below

	for(int x=1; x < cols-1; x++)
	{
		for(int y=1; y < rows-1; y++)
		{
			grid[x][y].addRight(&grid[x+1][y]);
			grid[x][y].addLeft(&grid[x-1][y]);
			grid[x][y].addBelow(&grid[x][y+1]);
			grid[x][y].addAbove(&grid[x][y-1]);
			grid[x][y].setConnected();
		}
	}

	for(int y=1; y < rows-1; y++)
	{
		grid[0][y].addRight(&grid[1][y]);
		grid[0][y].addBelow(&grid[0][y+1]);
		grid[0][y].addAbove(&grid[0][y-1]);
		grid[0][y].setConnected();
		grid[cols-1][y].addLeft(&grid[cols-2][y]);
		grid[cols-1][y].addBelow(&grid[cols-1][y+1]);
		grid[cols-1][y].addAbove(&grid[cols-1][y-1]);
		grid[cols-1][y].setConnected();
	}

	for(int x=1; x < cols-1; x++)
	{
		grid[x][0].addBelow(&grid[x][1]);
		grid[x][0].addRight(&grid[x+1][0]);
		grid[x][0].addLeft(&grid[x-1][0]);
		grid[x][0].setConnected();
		grid[x][rows-1].addAbove(&grid[x][rows-2]);
		grid[x][rows-1].addLeft(&grid[x-1][rows-1]);
		grid[x][rows-1].addRight(&grid[x+1][rows-1]);
		grid[x][rows-1].setConnected();
	}

	grid[0][0].addRight(&grid[1][0]);
	grid[0][0].addBelow(&grid[0][1]);
	grid[0][0].setConnected();
	grid[0][rows-1].addAbove(&grid[0][rows-2]);
	grid[0][rows-1].addRight(&grid[1][rows-1]);
	grid[0][rows-1].setConnected();
	grid[cols-1][0].addLeft(&grid[cols-2][0]);
	grid[cols-1][0].addBelow(&grid[cols-1][1]);
	grid[cols-1][0].setConnected();
	grid[cols-1][rows-1].addLeft(&grid[cols-2][rows-1]);
	grid[cols-1][rows-1].addAbove(&grid[cols-1][rows-2]);
	grid[cols-1][rows-1].setConnected();
}
//==============================================================================
void PathGrid::add(int x, int y, TileGrid &tileGrid)
{
	pvPtr vert = &grid[x][y];
	vert->setConnected();
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
void PathGrid::remove(int x, int y, TileGrid &tileGrid)
{
	pvPtr vert = &grid[x][y];
	vert->setDisconnected();
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
void PathGrid::setAllUnvisited()
{
	for(int i=0; i < cols; i++)
		for(int j=0; j < rows; j++)
			grid[i][j].setUnvisited();
}
//==============================================================================
void PathGrid::print(pvPtr spawnPtr, pvPtr exitPtr)
{
	std::cout << "====================================================\n";
	for(int i=0; i< rows;i++) 
	{
		for(int j=0; j<cols; j++)
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
