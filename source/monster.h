
#ifndef _MONSTER_H
#define _MONSTER_H

#include "circularObject.h"
#include "resources.h"
#include <string>

#include "gridPosObject.h"

class Monster : public CircularObject, public GridPosObject
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
	int mobId; // dont need to store?
	//int armor;
	Direction movingDir;
	bool alive;
	int nextInstr;
	bool updateGridPos;
	int moveCounter;
	bool inNewSquare;

public:
	Monster();
	//~Monster() {}
	void init(int _x, int _y, int _hp, int _ms, int _mobId);
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
	bool despawned();
};

//called only when monster is known to be dead
inline bool Monster::despawned()
{
	if(hp > 0)
	{
		hp = 0;
		return true;
	}
	return false;
}

inline bool Monster::isAlive() const
{
	return alive;
}

inline bool Monster::getUpdateGridPos() const
{
	return updateGridPos;
}

inline int Monster::getMs() const
{
	return ms;
}

inline int Monster::getHp() const
{
	return hp;
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
