
#ifndef _MONSTER_H
#define _MONSTER_H

#include "circularObject.h"
#include "point.h"
#include "resources.h"
#include <string>

class Monster : public CircularObject
{
	enum Direction
	{
		RIGHT,
		UP,
		LEFT,
		DOWN,
		STILL
	};

	int hp;
	int waveId;
	int mobId; //remove?
	//int armor;
	Direction movingDir;
	bool alive;
	int nextInstr;
	bool updateGridPos;
	int moveCounter;
	bool inNewSquare;
	Point currentGridPos;

public:
	Monster();
	void init(int _x, int _y, int _hp, int _ms, int _waveId, int _mobId);
	const Point& getGridPos() const;
	const Point& getTopLeft() const;
	int getMs() const;
	int getHp() const;
	int getWaveId() const;
	int getMobId() const;
	bool monsterIsAlive() const;
	void move();
	void updateDirection();
	void gridPosUpdated();
	bool wasShot(int dmg);
	void updateCenter();
	bool getUpdateGridPos() const;
	bool isAlive() const;
};

inline bool Monster::isAlive() const
{
	return alive;
}

inline bool Monster::getUpdateGridPos() const
{
	return updateGridPos;
}

inline const Point& Monster::getGridPos() const
{
	return currentGridPos;
}

inline const Point& Monster::getTopLeft() const
{
	return topLeft;
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

#endif // _MONSTER_H
