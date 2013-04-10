#ifndef _PATHINGVERTEX_H
#define _PATHINGVERTEX_H

#include<string>
#include <iostream>
#include <queue>
#include "resources.h"

#define POSSIBLE_DIRECTIONS 4

class PathingVertex;
typedef PathingVertex* pvPtr;

enum Direction
{
	RIGHT,
	UP,
	LEFT,
	DOWN,
	UNDEF
};

class PathingVertex
{
	bool visited;
public:
	pvPtr neighbours[POSSIBLE_DIRECTIONS];	//MVE P
		
	PathingVertex();
	void removeAbove();
	void removeBelow();
	void removeLeft();
	void removeRight();
	void addAbove(pvPtr p);
	void addBelow(pvPtr p);
	void addLeft(pvPtr p);
	void addRight(pvPtr p);
	void setVisited();
	void relaxNode(std::queue<pvPtr> &pq);
	int getCameFrom() const;
	pvPtr getNExtToBacktrack() const;
	bool wasVisited() const;
private:
	void setCameFrom(int dir);
	Direction cameFrom;
};

inline bool PathingVertex::wasVisited() const
{
	return visited == true;
}

inline pvPtr PathingVertex::getNExtToBacktrack() const
{
	return neighbours[cameFrom];
}

inline int PathingVertex::getCameFrom() const
{
	return cameFrom;
}

inline void PathingVertex::setVisited()
{
	visited = true;
}

inline void PathingVertex::setCameFrom(int dir)
{
	switch(dir)
	{
	case RIGHT:
		cameFrom = RIGHT;
		break;
	case LEFT:
		cameFrom = LEFT;
		break;
	case DOWN:
		cameFrom = DOWN;
		break;
	case UP:
		cameFrom = UP;
		break;
	}
}

inline void PathingVertex::removeAbove()
{
	neighbours[UP] = 0;
}

inline void PathingVertex::removeBelow()
{
	neighbours[DOWN] = 0;
}

inline void PathingVertex::removeLeft()
{
	neighbours[LEFT] = 0;
}

inline void PathingVertex::removeRight()
{
	neighbours[RIGHT] = 0;
}

inline void PathingVertex::addAbove(pvPtr p)
{
	neighbours[UP] = p;
}

inline void PathingVertex::addBelow(pvPtr p)
{
	neighbours[DOWN] = p;
}

inline void PathingVertex::addLeft(pvPtr p)
{
	neighbours[LEFT] = p;
}

inline void PathingVertex::addRight(pvPtr p)
{
	neighbours[RIGHT] = p;
}
#endif //_PATHINGVERTEX_H
