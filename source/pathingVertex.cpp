#include "pathingVertex.h"
#include <iostream>

PathingVertex::PathingVertex() : visited(false), cameFrom(UNDEF)
{
	for(int i=0; i < POSSIBLE_DIRECTIONS; i++)
		neighbours[i] = 0;
}


void PathingVertex::relaxNode(std::queue<pvPtr> &pq)
{
	pvPtr p = neighbours[RIGHT];
	if(p != 0 && !p->wasVisited())
	{
		//std::cout << "Right\n";
		p->setVisited();
		p->setCameFrom(LEFT);
		pq.push(p);
	}
	p = neighbours[LEFT];
	if(p != 0 && !p->wasVisited())
	{
		//std::cout << "Left\n";
		p->setVisited();
		p->setCameFrom(RIGHT);
		pq.push(p);
	}
	p = neighbours[UP];
	if(p != 0 && !p->wasVisited())
	{
		//std::cout << "Up\n";
		p->setVisited();
		p->setCameFrom(DOWN);
		pq.push(p);
	}
	p = neighbours[DOWN];
	if(p != 0 && !p->wasVisited())
	{
		//std::cout << "Down\n";
		p->setVisited();
		p->setCameFrom(UP);
		pq.push(p);
	}
}

