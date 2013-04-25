#ifndef _CIRCULAR_OBJECT_H
#define _CIRCULAR_OBJECT_H

#include "object.h"
#include "objectWithCenter.h"

class CircularObject : public Object, public ObjectWithCenter
{
protected:
	int radius;
	int ms;
public:
	CircularObject() : radius(0), ms(0), 
		Object(0, 0), ObjectWithCenter(0, 0) {};
	CircularObject(int x, int y, int r, int ms) 
		: Object(x, y), ObjectWithCenter(x+r, y+r), radius(r), ms(ms) {}
	virtual ~CircularObject() = 0;
	int getRadius() const;
	void setMs(int _ms);
};

inline CircularObject::~CircularObject() {};

inline int CircularObject::getRadius() const
{
	return radius;
}

inline void CircularObject::setMs(int _ms)
{
	ms = _ms;
}
#endif //_CIRCULAR_OBJECT_H
