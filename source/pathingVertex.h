#ifndef _PATHINGVERTEX_H
#define _PATHINGVERTEX_H

#include<string>
#include <iostream>
#include <queue>
#include "resources.h"

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
	static const int S_POSSIBLE_DIRECTIONS = 4;

	bool connected;
	bool visited;
	pvPtr neighbours[S_POSSIBLE_DIRECTIONS];
public:
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
	void setUnvisited();
	void relaxNode(std::queue<pvPtr> &pq);
	int getCameFrom() const;
	pvPtr getNextToBacktrack() const;
	bool wasVisited() const;
	bool getConnected() const;
	void setConnected();
	void setDisconnected();
private:
	void setCameFrom(int dir);
	Direction cameFrom;
};

inline void PathingVertex::setUnvisited()
{
	visited = false;
}

inline bool PathingVertex::wasVisited() const
{
	return visited == true;
}

inline pvPtr PathingVertex::getNextToBacktrack() const
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
