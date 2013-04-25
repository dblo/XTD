#ifndef _MONSTER_H
#define _MONSTER_H

#include <string>

#include "circularObject.h"
#include "resources.h"
#include "gridPosObject.h"

namespace monster
{
	enum Direction
	{
		RightDirection,
		UpDirection,
		LeftDirection,
		DownDirection,
		StillDirection
	};
}

class Monster : public CircularObject, public GridPosObject
{
	int hp;
	int mobId; // dont need to store?
	//int armor;
	monster::Direction movingDir;
	bool alive;
	int nextInstr;
	bool UpdateGridPos;
	int moveCounter;
	bool inNewSquare;

public:
	Monster();
	~Monster() {}
	void init(int _gridPosX, int _gridPosY, 
		int _topLeftX, int _topLeftY, int _hp, 
		int _ms, int _mobId, int rad, int tileSize);
	int getMs() const;
	int getHp() const;
	int getWaveId() const;
	int getMobId() const;
	bool monsterIsAlive() const;
	bool move(const std::string &path, int tileSize);
	void UpdateDirection(const std::string &path);
	void gridPosUpdated();
	bool wasShot(int dmg);
	void UpdateCenter();
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
	return UpdateGridPos;
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
	UpdateGridPos = false;
}

#endif // _MONSTER_H
