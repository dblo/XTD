#ifndef _UNDOQUEUE_H
#define _UNDOQUEUE_H

#include <queue>

namespace changeType
{
	enum UndoType
	{
		TOWER,
		INCOME
	};
}

class ChangeManager
{
public:
	void clear();
	bool empty() const;
	void addTower();
	void addIncome();
	changeType::UndoType undoLast();
	void changesConfirmed();
private:
	std::queue<changeType::UndoType> queue;
};
#endif //_UNDOQUEUE_H
