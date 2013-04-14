#ifndef _GRID_POS_OBJECT_H
#define _GRID_POS_OBJECT_H

class GridPosObject
{
protected:
	int gridPosX, gridPosY;
public:
	GridPosObject(int x, int y) : gridPosX(x), gridPosY(y) {}
	virtual ~GridPosObject() {};
	int getGridPosX() const;
	int getGridPosY() const;
};

inline int GridPosObject::getGridPosX() const
{
	return gridPosX;
}

inline int GridPosObject::getGridPosY() const
{
	return gridPosY;
}
#endif //_GRID_POS_OBJECT_H
