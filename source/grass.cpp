#include "Grass.h"
//=============================================================================

void Grass::clearListeners()
{
	listeners.clear();
}
//=============================================================================

void Grass::broadcastEnter(int mobid) const
{
	for(unsigned int i=0; i < listeners.size(); i++)
		listeners[i]->mobEntered(mobid);
}
//=============================================================================
void Grass::broadcastExit(int mobId) const
{
	for(unsigned int i=0; i < listeners.size(); i++)
		listeners[i]->mobLeft(mobId);
}
//=============================================================================
void Grass::addListener(TowerListener *t)
{
	listeners.push_back(t);
}
//=============================================================================
