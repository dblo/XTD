#ifndef _OBJECT_WITH_CENTER_H
#define _OBJECT_WITH_CENTER_H

class ObjectWithCenter
{
protected:
	int centerX, centerY;
public:
	ObjectWithCenter(int _x, int _y) : centerX(_x), centerY(_y) {}
	virtual ~ObjectWithCenter() = 0{}
	int getCenterX() const;
	int getCenterY() const;
};

inline int ObjectWithCenter::getCenterX() const
{
	return centerX;
}

inline int ObjectWithCenter::getCenterY() const
{
	return centerY;
}

#endif //_OBJECT_WITH_CENTER_H
