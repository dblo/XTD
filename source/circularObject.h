#ifndef _CIRCULAR_OBJECT
#define _CIRCULAR_OBJECT

#include "point.h"

class CircularObject
{
protected:
	int radius;
	Point center;
	Point topLeft;
	int ms;
public:
	virtual ~CircularObject() {};
	virtual void move() = 0;
	const Point& getTopLeft() const;
	const Point& getCenter() const;
	int getRadius() const;
};

inline const Point& CircularObject::getTopLeft() const
{
	return topLeft;
}

inline const Point& CircularObject::getCenter() const
{
	return center;
}

inline int CircularObject::getRadius() const
{
	return radius;
}
#endif //_CIRCULAR_OBJECT
