#ifndef _POINT_H
#define _POINT_H

class Point
{
	int x, y;

public:
	Point() : x(0), y(0) {}
	Point(int _x, int _y) : x(_x), y(_y) {}

	void setPoint(int _x, int _y)
	{
		x = _x;
		y = _y;
	}

	void addToX(int _x)
	{
		x += _x;
	}

	void addToY(int _y)
	{
		y += _y;
	}

	int getX() const
	{
		return x;
	}
	int getY() const
	{
		return y;
	}

	void incX()
	{
		x++;
	}

	void decX()
	{
		x--;
	}

	void incY()
	{
		y++;
	}

	void decY()
	{
		y--;
	}
};
#endif //_POINT_H
