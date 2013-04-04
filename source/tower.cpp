
#include "tower.h"

void Tower::incDmg(int _dmg)
{
	dmg += _dmg;
}

void Tower::incRange(int _range)
{
	range += range;
}

void Tower::intAttSpeed(int _attSpeed)
{
	attSpeed += _attSpeed;
}
void Tower::setTowerStatics(int _dmg, int _range, int _attSpeed, int _sellVal) 
{
	dmg =_dmg; 
	range = _range;
	attSpeed = attSpeed;
	sellVal = sellVal;
}

Tower::~Tower()
{
	for(int i=0; i < SIMUL_WAVES_ALLOWED; i++)
		delete[] rangeTables[i];
	delete[] rangeTables;
}
