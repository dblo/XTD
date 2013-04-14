#include "grid.h"
#include <iostream>
//==============================================================================
Grid::Grid() {}
//==============================================================================
Grid::~Grid()
{
	for(int i=0; i < GRID_COLUMNS ; i++)
		for(int j=0; j < GRID_ROWS; j++)
	{
		delete tiles[i][j];
	}
}
//==============================================================================
bool Grid::isGrassAt(int x, int y) const
{
	return at(x, y)->getColor() == GRASS;
}
//==============================================================================
const Tile & Grid::get(int x, int y) const
{
	//IwAssertMsg(APP, Valid(x,y), ("Coordinate out of range for Grid (%d,%d)", x, y));
	return *tiles[x][y];
}
//==============================================================================
Tile* Grid::get(int x, int y)
{
	if(validPoint(x, y))
		return tiles[x][y];
	return 0;
}
//==============================================================================
void Grid::render() const
{
	for (int x=0; x<GRID_COLUMNS; x++)
	{
		for (int y=0; y<GRID_ROWS; y++)
		{
			drawTile(
				get(x,y).getColor(),
				x*g_tileSize,
				y*g_tileSize
				);
		}
	}
}
//==============================================================================
void Grid::notifyTileExit(int x, int y, int mobId)
{
	Grass *tile = 0;
	
	if(tile = dynamic_cast<Grass*>(get(x, y)))
		tile->broadcastExit(mobId);
}
//==============================================================================
void Grid::notifyTileEnter(int x, int y, int mobId)
{
	Grass *tile = 0;
	
	if(tile = dynamic_cast<Grass*>(get(x, y)))
		tile->broadcastEnter(mobId);
}
//==============================================================================
void Grid::setPathGrassListeners(int pathTravX, int pathTravY )
{
	unsigned int nxtInstr = 0;
	Tower *newListener;

	while(nxtInstr < g_mobPath->length())
	{
		switch((*g_mobPath)[nxtInstr])
		{
		case 'r':
			pathTravX++;
			break;
		case 'u':
			pathTravY--;
			break;
		case 'l':
			pathTravX--;
			break;
		case 'd':
			pathTravY++;
			break;
		}
		nxtInstr++;

		int xLim = pathTravX+1;
		int yLim = pathTravY+1;

		for(int x=pathTravX-1; x <= xLim; x++)
			for(int y=pathTravY-1; y <= yLim; y++)
			{
				if(validPoint(x, y))
				{
					if(newListener = dynamic_cast<Tower*>(tiles[x][y]))
					{
						setListener(pathTravX, pathTravY, newListener);
					}
				}
			}
	}
}
//==============================================================================
void Grid::removePathGrassListeners(int pathTravX, int pathTravY )
{
	unsigned int nxtInstr = 0;

	while(nxtInstr < g_mobPath->length())
	{
		switch((*g_mobPath)[nxtInstr])
		{
		case 'r':
			pathTravX++;
			break;
		case 'u':
			pathTravY--;
			break;
		case 'l':
			pathTravX--;
			break;
		case 'd':
			pathTravY++;
			break;
		}
		nxtInstr++;

		removeListener(pathTravX, pathTravY);
	}	
}
//==============================================================================
void Grid::setListener(int x, int y, Tower *t)
{
	Grass* g;

	if(g = dynamic_cast<Grass*>(tiles[x][y]))
	{
		g->addListener(t);
	}
}
//==============================================================================
void Grid::removeListener(int x, int y)
{
	Grass* g;

	if(g = dynamic_cast<Grass*>(tiles[x][y]))
	{
		g->clearListeners();
	}
}
//==============================================================================
