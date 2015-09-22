#include "LinkedList.h"

LinkedList::LinkedList()
{
	//initialise node pointers
	_begin = NULL;
	_end = NULL;
	_current = NULL;
}

void LinkedList::addToList(Bullet *data)
{
	Node *newNodePtr;

	//craete new instance of Node
	newNodePtr = new Node;

	//set data
	newNodePtr->setData(data);

	//check if list is empty
	if (_begin == NULL)
	{
		_begin = newNodePtr;
	}
	else
	{
		newNodePtr->setPrevNode(_end);
		_end->setNextNode(newNodePtr);
	}

	//set current pointer end end pointer
	_end = newNodePtr;
	_current = newNodePtr;
}