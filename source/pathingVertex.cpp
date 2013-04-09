#include "pathingVertex.h"
#include <iostream>

PathingVertex::PathingVertex() : pathLength(MAX_PATH_LENGTH), cameFrom(UNDEF)
{
	for(int i=0; i < POSSIBLE_DIRECTIONS; i++)
		neighbours[i] = 0;
}

void PathingVertex::backtrack(std::string &path) const
{
	switch (cameFrom)
	{
	case RIGHT:
		path += 'r';
		break;
	case LEFT:
		path += 'l';
		break;
	case DOWN:
		path += 'd';
		break;
	case UP:
		path += 'u';
		break;
	default: //UNDEF
		return; //Path traversal complete, return to prevent further recursion
	}
	std::cout << "camefrom: " << cameFrom << "\n";

	if(neighbours[cameFrom] == 0)
		std::cout << "Error\n";
	else
	neighbours[cameFrom]->backtrack(path);
}

void PathingVertex::relaxNode(std::priority_queue<PathingVertex*, std::vector<PathingVertex*>, PathingVertexComp> &pq)
{
	pvPtr p = neighbours[RIGHT];

	std::cout << "pathlenght: " << pathLength << "\n";

	if(p != 0 && p->getPathLength() > pathLength + 1)
	{
		std::cout << "Right\n";
		p->setPathLength(pathLength + 1);
		p->setCameFrom(LEFT);
		pq.push(p);
	}
	p = neighbours[LEFT];
	if(p != 0 && p->getPathLength() > pathLength + 1)
	{
		std::cout << "Left\n";
		p->setPathLength(pathLength + 1);
		p->setCameFrom(RIGHT);
		pq.push(p);
	}
	p = neighbours[UP];
	if(p != 0 && p->getPathLength() > pathLength + 1)
	{
		p->setPathLength(pathLength + 1);
		p->setCameFrom(DOWN);
		pq.push(p);
	}
	p = neighbours[DOWN];
	if(p != 0 && p->getPathLength() > pathLength + 1)
	{
		std::cout << "Down\n";
		p->setPathLength(pathLength + 1);
		p->setCameFrom(UP);
		pq.push(p);
	}
}


