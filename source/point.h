#ifndef _POINT_H
#define _POINT_H

struct Point
{
	int x, y;

	Point() : x(0), y(0) {}
	Point(int _x, int _y) : x(_x), y(_y) {}
	void setX(int _x) 
	{
		x = _x;
	}

	void setY(int _y)
	{
		y = _y;
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

#endif _POINT_H
