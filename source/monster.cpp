#include "monster.h"

Monster::Monster() : x(0), y(0), hp(0), ms(0), waveId(0), mobId(0) {}

void Monster::initMonster(int _x, int _y, int _hp, int _ms, int _waveId, int _mobId)
{
	x = _x;
	y = _y;
	hp = _hp;
	ms = _ms;
	waveId = _waveId;
	mobId = _mobId;
}

int Monster::getX() const
{
	return x;
}
int Monster::getY() const
{
	return y;
}
int Monster::getMs() const
{
	return ms;
}
int Monster::getHp() const
{
	return hp;
}
int Monster::getWaveId() const
{
	return waveId;
}
int Monster::getMobId() const
{
	return mobId;
}
