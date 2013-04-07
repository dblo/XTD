#include "grid.h"
#include <iostream>

Grid::Grid() : width(GAME_WIDTH), height(GAME_HEIGHT)
{
	tiles = new Tile*[width * height];

	for(int x=0; x < width; x++)
		for(int y=0; y < height; y++) 
		{
			buildGrass(x,y);
		}
}

Grid::~Grid()
{
	for(int i=0; i < width * height; i++)
	{
		delete tiles[i];
	}
	delete[] tiles;
}

Tile & Grid::Get(int x, int y)
{
	//IwAssertMsg(APP, Valid(x,y), ("Coordinate out of range for Grid (%d,%d)", x, y));
	return *tiles[x + y*width];
}

const Tile & Grid::Get(int x, int y) const
{
	//IwAssertMsg(APP, Valid(x,y), ("Coordinate out of range for Grid (%d,%d)", x, y));
	return *tiles[x + y*width];
}

Tile* Grid::get(int x, int y)
{
	return tiles[x + y*width];
}

//(rx, ry) => (0, 0)
void Grid::Render(int rx, int ry) const
{
	for (int x=0; x<width; x++)
	{
		for (int y=0; y<height; y++)
		{
			DrawTile(
				Get(x,y).color,
				x*g_TileSize + rx,
				y*g_TileSize + ry,
				g_TileSize
				);
		}
	}
}

void Grid::notifyTileExit(Point &p, int mobId)
{
	dynamic_cast<Grass*>(get(p.getX(), p.getY()))->broadcastExit(mobId);
}

void Grid::notifyTileEnter(Point &p, int mobId)
{
	dynamic_cast<Grass*>(get(p.getX(), p.getY()))->broadcastEnter(mobId);
}

void Grid::Resize(int newWidth, int newHeight)
{

	//if (width!=newWidth || height!=newHeight)
	//{
	//	width = newWidth;
	//	height = newHeight;
	//	delete [] tiles;
	//	tiles = new Tile*[width*height];
	//}

}

void Grid::rollMap(int &spawnX, int &spawnY, int &goalX, int &goalY)
{


	spawnX = spawnY = 0;
	goalX = GAME_WIDTH-1;
	goalY = GAME_HEIGHT-1;

	releaseTile(spawnX, spawnY);
	buildSpawn(spawnX, spawnY);

	releaseTile(goalX, goalY);
	buildExit(goalX, goalY);

	releaseTile(0,1);
	buildTower(0,1);

	releaseTile(1, 1);
	releaseTile(2, 1);
	releaseTile(3, 1);
	releaseTile(3, 2);
	buildTower(1,1);
	buildTower(2,1);
	buildTower(3,1);
	buildTower(3,2);

	releaseTile(5, 0);
	releaseTile(5, 1);
	releaseTile(5, 2);
	releaseTile(6, 0);
	releaseTile(6, 1);
	releaseTile(8, 0);
	releaseTile(8, 1);
	buildWater(5, 0);
	buildWater(5, 1);
	buildWater(5, 2);
	buildWater(6, 0);
	buildWater(6, 1);
	buildWater(8, 0);
	buildWater(8, 1);
}
