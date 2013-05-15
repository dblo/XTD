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
	int mobId; // todo dont need to store?
	//int armor;
	monster::Direction movingDir;
	int nextInstr;
	bool update;
	int moveCounter;
	int baseSpeed;
	int savedMove;
public:
	Monster();
	~Monster() {}
	void spawn(int _gridPosX, int _gridPosY, 
		int _topLeftX, int _topLeftY, int _hp, 
		int _mobId, int rad, int tileSize,
		const std::string &path);
	int getMs() const;
	int getHp() const;
	int getWaveId() const;
	int getMobId() const;

	// Return false if monster reached exit
	bool move(const std::string &path, int tileSize);

	void updateGridPos();

	void updateDirection(const std::string &path);
	void positionUpdated();

	// Returns true if monster died
	bool takeDamage(int dmg);
	void updateCenter();

	// Returns true when monster has moved into a new grid pos
	bool updatePosition() const;
	bool isAlive() const;

	// If monster is known to be dead/despawned, return 
	// weather it despawned this frame
	bool despawned();
};

inline bool Monster::isAlive() const
{
	return hp > 0;
}

inline bool Monster::updatePosition() const
{
	return update;
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

inline void Monster::positionUpdated()
{
	update = false;
}

#endif // _MONSTER_H
