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
void TileGrid::releaseTile(int x, int y)
{
	delete tiles[x][y];
}
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
void TileGrid::setSpawn(int posX, int posY)
{
	at(posX,posY)->setColor(SpawnImage);
}
//==============================================================================
void TileGrid::setExit(int posX, int posY)
{
	at(posX, posY)->setColor(ExitImage);
}
//==============================================================================
void TileGrid::setMud(int x, int y)
{
	at(x,y)->setColor(MudImage);
}
//==============================================================================
void TileGrid::setIce(int x, int y)
{
	at(x,y)->setColor(IceImage);
}
//==============================================================================
bool TileGrid::isGrass(int x, int y) const
{
	return at(x, y)->getColor() == GrassImage;
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
	Iw2DSetColour(0xffffffff);
	Tile *tile;
	for (int x=0; x<cols; x++)
	{
		for (int y=0; y<rows; y++)
		{
			tile = get(x,y);

			io->drawTile(
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
	at(x,y)->broadcastExit(mobId);
}
//==============================================================================
void TileGrid::notifyTileEnter(int x, int y, int mobId)
{
	at(x,y)->broadcastEnter(mobId);
}
//==============================================================================
void TileGrid::setListener(int x, int y, Tower *t)
{
	at(x,y)->addListener(t);
}
//==============================================================================
void TileGrid::removeListener(int x, int y)
{
	at(x,y)->clearListeners();
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
//void TileGrid::setAllGrass()
//{
//	for(int x=0; x < cols; x++)
//		for(int y=0; y < rows; y++) 
//		{
//			tiles[x][y]->setColor(GrassImage);
//		}
//}
//==============================================================================
//void TileGrid::buildAllGrass(int tileSize, int verBorder, int horBorder)
//{
//	for(int x=0; x < cols; x++)
//		for(int y=0; y < rows; y++) 
//		{
//			makeTi(x, y, 
//				x * tileSize + verBorder,
//				y * tileSize + horBorder);
//		}
//}
////==============================================================================
