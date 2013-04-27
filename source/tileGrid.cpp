#include "tileGrid.h"
#include <iostream>
//==============================================================================
TileGrid::TileGrid() {}
//==============================================================================
TileGrid::~TileGrid()
{
	for(int i=0; i < GRID_COLUMNS ; i++)
		for(int j=0; j < GRID_ROWS; j++)
	{
		delete tiles[i][j];
	}
}
//==============================================================================
bool TileGrid::isGrass(int x, int y) const
{
	return at(x, y)->getColor() == GrassImage;
}
//==============================================================================
//const Tile & Grid::get(int x, int y) const
//{
//	//IwAssertMsg(APP, Valid(x,y), ("Coordinate out of range for Grid (%d,%d)", x, y));
//	return *tiles[x][y];
//}
//==============================================================================
Tile* TileGrid::get(int x, int y) const
{
	if(validPoint(x, y))
		return tiles[x][y];
	return 0;
}
//==============================================================================
void TileGrid::render(int size) const
{
	Iw2DSetColour(0xffffffff);
	Tile *tile;	
	for (int x=0; x<GRID_COLUMNS; x++)
	{
		for (int y=0; y<GRID_ROWS; y++)
		{
			tile = get(x,y);
			drawTile(
				tile->getColor(),
				tile->getTopLeftX(),
				tile->getTopLeftY(),
				size, size
				);
		}
	}
}
//==============================================================================
void TileGrid::notifyTileExit(int x, int y, int mobId)
{
	Grass *tile = 0;
	
	if(tile = dynamic_cast<Grass*>(get(x, y)))
		tile->broadcastExit(mobId);
}
//==============================================================================
void TileGrid::notifyTileEnter(int x, int y, int mobId)
{
	Grass *tile = 0;
	
	if(tile = dynamic_cast<Grass*>(get(x, y)))
		tile->broadcastEnter(mobId);
}
//==============================================================================
void TileGrid::setPathGrassListeners(int pathTravX, int pathTravY, const std::string &path)
{
	unsigned int nxtInstr = 0;
	Tower *newListener;

	while(nxtInstr < path.length())
	{
		switch((path)[nxtInstr])
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

		int xLowLim, yLowLim, xHiLim, yHiLim;
		
		//if(rangeUpgraded())
		//{
			xHiLim = pathTravX+2;
			yHiLim = pathTravY+2;
			xLowLim = pathTravX-2;
			yLowLim = pathTravY-2;
		/*}
		else
		{
			xHiLim = pathTravX+1;
			yHiLim = pathTravY+1;
			xLowLim = pathTravX-1;
			yLowLim = pathTravY-1;
		}
*/
		for(int x=xLowLim; x <= xHiLim; x++)
			for(int y=yLowLim; y <= yHiLim; y++)
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
void TileGrid::removePathGrassListeners(int pathTravX, int pathTravY, const std::string &path)
{
	unsigned int nxtInstr = 0;

	while(nxtInstr < path.length())
	{
		switch((path)[nxtInstr])
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
void TileGrid::setListener(int x, int y, Tower *t)
{
	Grass* g;

	if(g = dynamic_cast<Grass*>(tiles[x][y]))
	{
		g->addListener(t);
	}
}
//==============================================================================
void TileGrid::removeListener(int x, int y)
{
	Grass* g;

	if(g = dynamic_cast<Grass*>(tiles[x][y]))
	{
		g->clearListeners();
	}
}
//==============================================================================
bool TileGrid::isTower(int x, int y)
{
	if(validPoint(x, y))
		if((tiles[x][y]->getColor() == TowerImage))
			return true;
	return false;
}
//==============================================================================
void TileGrid::setAllGrass()
{
	for(int x=0; x < GRID_COLUMNS; x++)
		for(int y=0; y < GRID_ROWS; y++) 
		{
			tiles[x][y]->setColor(GrassImage);
		}
}
//==============================================================================
void TileGrid::buildAllGrass(int tileSize, int verBorder, int horBorder)
{
	for(int x=0; x < GRID_COLUMNS; x++)
		for(int y=0; y < GRID_ROWS; y++) 
		{
			buildGrass(x, y, 
				x * tileSize + verBorder,
				y * tileSize + horBorder);
		}
}
