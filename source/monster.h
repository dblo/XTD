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
	int nextInstr;
	bool updateGridPos;
	int moveCounter;
	int baseSpeed;
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
	bool move(const std::string &path, int tileSize);
	void UpdateDirection(const std::string &path);
	void gridPosUpdated();
	bool wasShot(int dmg);
	void UpdateCenter();

	// Returns true when monster has moved into a new grid pos
	bool getUpdateGridPos() const;
	bool isAlive() const;

	// If monster is known to be dead/despawned, return 
	// weather it despawned this frame
	bool despawned();
};

inline bool Monster::isAlive() const
{
	return hp > 0;
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

inline void Monster::gridPosUpdated()
{
	updateGridPos = false;
}

#endif // _MONSTER_H
