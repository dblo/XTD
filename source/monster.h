
#ifndef _MONSTER_H
#define _MONSTER_H

struct Monster
{
	int hp;
	int ms;
	int waveId;
	int mobId;
	int x, y; //center coords

	Monster();
	void initMonster(int _x, int _y, int _hp, int _ms, int _waveId, int _mobId);

	int getX() const;
	int getY() const;
	int getMs() const;
	int getHp() const;
	int getWaveId() const;
	int getMobId() const;
};

#endif _MONSTER_H
