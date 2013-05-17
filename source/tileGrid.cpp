#include "tileGrid.h"
#include <iostream>
//==============================================================================
TileGrid::TileGrid(int c, int r, int tileSize) : rows(r), cols(c)
{
	tiles.resize(cols);
	for(int i=0; i < cols; i++)
		tiles[i].resize(rows);
}
//==============================================================================
TileGrid::~TileGrid()
{
	for(int i=0; i < cols; i++)
		for(int j=0; j < rows; j++)
			delete tiles[i][j];
}
//==============================================================================
bool TileGrid::validPoint(int x, int y) const
{
	return x >= 0 && x < cols && 
		y >= 0 && y < rows;
}
//==============================================================================
Tile* TileGrid::at(int x, int y) const
{
	return tiles[x][y];
}
//==============================================================================
//void TileGrid::releaseTile(int x, int y)
//{
//	delete tiles[x][y];
//}
//==============================================================================
void TileGrid::buildGrass( int posX, int posY, int LeftX, int LeftY)
{
	tiles[posX][posY] = new Tile(GrassImage, LeftX, LeftY);
}
////==============================================================================
// void TileGrid::buildWater(int x, int y)
//{
//	at(x, y)->setColor(WaterImage);
//}
//==============================================================================
void TileGrid::setMud(int x, int y)
{
	at(x,y)->setImage(MudImage);
}
//==============================================================================
void TileGrid::setIce(int x, int y)
{
	at(x,y)->setImage(IceImage);
}
//==============================================================================
Tile* TileGrid::get(int x, int y) const
{
	if(validPoint(x, y))
		return tiles[x][y];
	return 0;
}
//==============================================================================
void TileGrid::render(const Io *io, int size) const
{
	Tile *tile;
	for (int x=0; x<cols; x++)
	{
		for (int y=0; y<rows; y++)
		{
			tile = get(x,y);

			io->drawTile(
				tile->getImage(),
				tile->getTopLeftX(),
				tile->getTopLeftY(),
				size, size
				);
		}
	}
}
//==============================================================================
void TileGrid::notifyTileExit(int x, int y, int mobId) const
{
	at(x,y)->broadcastExit(mobId);
}
//==============================================================================
void TileGrid::notifyTileEnter(int x, int y, int mobId) const
{
	at(x,y)->broadcastEnter(mobId);
}
//==============================================================================
void TileGrid::setListener(int x, int y, Tower *t) const
{
	at(x,y)->addListener(t);
}
//==============================================================================
void TileGrid::clearListeners(int x, int y) const
{
	at(x,y)->clearListeners();
}
Image TileGrid::getImage(int x, int y) const
{
	return at(x, y)->getImage();
}

void TileGrid::init( int verOffset, int horOffset, int tileSize )
{
	// Allocate tiles as grass
	for(int i=0; i < cols; i++)
		for(int j=0; j < rows; j++)
		{
			buildGrass(i, j, 
				i*tileSize + horOffset, 
				j*tileSize + verOffset);
		}
}

void TileGrid::setTowerAsListener( int x, int y, Tower *t , int tileSize)
{
	int towerGridRange = t->getRange() / tileSize;
	int lowLimX = x - towerGridRange;
	int hiLimX = x + towerGridRange;
	int lowLimY = y - towerGridRange;
	int hiLimY = y + towerGridRange;

	for(int travX = lowLimX; travX <= hiLimX; travX++)
		for(int travY = lowLimY; travY <= hiLimY; travY++)
		{
			if(validPoint(travX, travY) && at(travX, travY)->isPartofPath())
			{
				setListener(travX, travY, t);
			}
		}
}

void TileGrid::removeTowerAsListener( int x, int y, Tower *t , int tileSize)
{
	int towerGridRange = t->getRange() / tileSize;
	int lowLimX = x - towerGridRange;
	int hiLimX = x + towerGridRange;
	int lowLimY = y - towerGridRange;
	int hiLimY = y + towerGridRange;
	TowerListener *tl;

	for(int travX = lowLimX; travX <= hiLimX; travX++)
		for(int travY = lowLimY; travY <= hiLimY; travY++)
			if(validPoint(travX, travY) && at(travX, travY)->isPartofPath())
			{
				tl = t;
				at(travX, travY)->removeListener(tl);
			}
}

void TileGrid::setPathPart( int x, int y, bool isPart)
{
	at(x,y)->setPartOfPath(isPart);
}

bool TileGrid::isPartOfPath( int x, int y ) const
{
	return at(x,y)->isPartofPath();
}
