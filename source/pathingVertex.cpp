#include "pathingVertex.h"
#include <iostream>
//==============================================================================
PathingVertex::PathingVertex() : visited(false), cameFrom(UndefDirection), connected(false)
{
	for(int i=0; i < S_POSSIBLE_DIRECTIONS; i++)
		neighbours[i] = 0;
}
//==============================================================================
bool PathingVertex::getConnected() const
{
	return connected;
}
//==============================================================================
void PathingVertex::setDisconnected()
{
	connected = false;
}
//==============================================================================
void PathingVertex::setConnected() 
{
	connected = true;
}
//==============================================================================
void PathingVertex::relaxNode(std::queue<pvPtr> &pq)
{
	pvPtr p = neighbours[RightDirection];
	if(p != 0 && !p->wasVisited() && p->getConnected())
	{
		p->setVisited();
		p->setCameFrom(LeftDirection);
		pq.push(p);
	}
	p = neighbours[LeftDirection];
	if(p != 0 && !p->wasVisited() && p->getConnected())
	{
		p->setVisited();
		p->setCameFrom(RightDirection);
		pq.push(p);
	}
	p = neighbours[UpDirection];
	if(p != 0 && !p->wasVisited() && p->getConnected())
	{
		p->setVisited();
		p->setCameFrom(DownDirection);
		pq.push(p);
	}
	p = neighbours[DownDirection];
	if(p != 0 && !p->wasVisited() && p->getConnected())
	{
		p->setVisited();
		p->setCameFrom(UpDirection);
		pq.push(p);
	}
}
//==============================================================================
void PathingVertex::setCameFrom(int dir)
{
	switch(dir)
	{
	case RightDirection:
		cameFrom = RightDirection;
		break;
	case LeftDirection:
		cameFrom = LeftDirection;
		break;
	case DownDirection:
		cameFrom = DownDirection;
		break;
	case UpDirection:
		cameFrom = UpDirection;
		break;
	}
}
//==============================================================================
