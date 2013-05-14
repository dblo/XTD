#ifndef _TOWERLISTENER_H
#define _TOWERLISTENER_H

struct TowerListener
{
	virtual void mobLeft(int mobId) = 0;
	virtual void mobEntered(int mobId) = 0;
	virtual ~TowerListener() {};
};

#endif //_TOWERLISTENER_H
