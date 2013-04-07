
#ifndef _MONSTER_H
#define _MONSTER_H

#include "point.h"
#include "resources.h"
#include <string>

enum DIRECTION
{
	STILL,
	RIGHT,
	UP,
	LEFT,
	DOWN
};

struct Monster
{
	int hp;
	int ms;
	int waveId;
	int mobId; //remove?
	//int armor;
	int x, y; //upper left corner
	DIRECTION movingDir;
	bool alive;
	int nextInstr;
	bool updateGridPos;
	int moveCounter;
	bool inNewSquare;
	Point currentGridPos;

	static std::string mobPath;

	Monster();
	void initMonster(int _x, int _y, int _hp, int _ms, int _waveId, int _mobId);

	Point& getGridPos();
	int getX() const;
	int getY() const;
	int getMs() const;
	int getHp() const;
	int getWaveId() const;
	int getMobId() const;
	bool monsterIsAlive() const;
	void move();
	void updateDirection();
	void gridPosUpdated();
};

inline Point& Monster::getGridPos() 
{
	return currentGridPos;
}

inline int Monster::getX() const
{
	return x;
}

inline int Monster::getY() const
{
	return y;
}

inline int Monster::getMs() const
{
	return ms;
}

inline int Monster::getHp() const
{
	return hp;
}

inline int Monster::getWaveId() const
{
	return waveId;
}

inline int Monster::getMobId() const
{
	return mobId;
}

inline bool Monster::monsterIsAlive() const
{
	return alive;
}

inline void Monster::gridPosUpdated()
{
	updateGridPos = false;
}

#endif _MONSTER_H
