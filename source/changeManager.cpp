#include "changeManager.h"
using namespace changeType;

void ChangeManager::clear()
{
	while(!queue.empty())
		queue.pop();
}

bool ChangeManager::empty() const
{
	return queue.empty();
}

void ChangeManager::addTower()
{
	queue.push(TOWER);
}

void ChangeManager::addIncome()
{
	queue.push(INCOME);
}

UndoType ChangeManager::undoLast()
{
	//assert queue not empty?
	UndoType retVal = queue.front();
	queue.pop();
	return retVal;
}

void ChangeManager::changesConfirmed()
{
	clear();
}
