#ifndef _OBJECT_H
#define _OBJECT_H

class Object
{
protected:
	int topLeftX, topLeftY;
public:
	Object(int _x, int _y) : topLeftX(_x), topLeftY(_y) {}
	virtual ~Object() = 0;
	int getTopLeftX() const;
	int getTopLeftY() const;
};

inline Object::~Object() {};

inline int Object::getTopLeftX() const
{
	return topLeftX;
}

inline int Object::getTopLeftY() const
{
	return topLeftY;
}

#endif //_OBJECT_H
