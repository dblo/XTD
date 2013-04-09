#ifndef _PATHINGVERTEX_H
#define _PATHINGVERTEX_H

#include<string>
#include <iostream>
#include <queue>
#include "resources.h"


#define MAX_PATH_LENGTH 99
#define POSSIBLE_DIRECTIONS 4

class PathingVertex;
class PathingVertexComp;
typedef PathingVertex* pvPtr;

class PathingVertex
{
	enum Direction
	{
		RIGHT,
		UP,
		LEFT,
		DOWN,
		UNDEF
	};

	Direction cameFrom;

	int getPathLength() const;
	void setCameFrom(int dir);
public:
	pvPtr neighbours[POSSIBLE_DIRECTIONS];	//MVE P
	int pathLength; //size of path from source to here


	PathingVertex();
	void removeAbove();
	void removeBelow();
	void removeLeft();
	void removeRight();
	void addAbove(pvPtr p);
	void addBelow(pvPtr p);
	void addLeft(pvPtr p);
	void addRight(pvPtr p);
	void setPathLength(int l);
	void setSpawnPoint();
	void relaxNode(std::priority_queue<PathingVertex*, std::vector<PathingVertex*>, PathingVertexComp> &pq);
	void backtrack(std::string &path) const;
};

class PathingVertexComp
{
public:
	bool operator()(PathingVertex *p1, PathingVertex *p2)
	{
		return p1->pathLength > p2->pathLength;
	}
};

inline void PathingVertex::setSpawnPoint()
{
	pathLength = 0;
}

inline int PathingVertex::getPathLength() const
{
	return pathLength;
}

inline void PathingVertex::setPathLength(int l)
{
	pathLength = l;
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
