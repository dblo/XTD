#include "pathingVertex.h"
#include <iostream>
//==============================================================================
PathingVertex::PathingVertex() : visited(false), cameFrom(UNDEF)
{
	for(int i=0; i < S_POSSIBLE_DIRECTIONS; i++)
		neighbours[i] = 0;
}
//==============================================================================
void PathingVertex::relaxNode(std::queue<pvPtr> &pq)
{
	pvPtr p = neighbours[RIGHT];
	if(p != 0 && !p->wasVisited())
	{
		p->setVisited();
		p->setCameFrom(LEFT);
		pq.push(p);
	}
	p = neighbours[LEFT];
	if(p != 0 && !p->wasVisited())
	{
		p->setVisited();
		p->setCameFrom(RIGHT);
		pq.push(p);
	}
	p = neighbours[UP];
	if(p != 0 && !p->wasVisited())
	{
		p->setVisited();
		p->setCameFrom(DOWN);
		pq.push(p);
	}
	p = neighbours[DOWN];
	if(p != 0 && !p->wasVisited())
	{
		p->setVisited();
		p->setCameFrom(UP);
		pq.push(p);
	}
}
//==============================================================================
void PathingVertex::setCameFrom(int dir)
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
//==============================================================================
