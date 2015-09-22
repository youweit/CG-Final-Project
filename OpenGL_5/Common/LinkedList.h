/*
* DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
* Version 2, December 2004
*
* Copyright(C) 2013 You-wei Teng
* 
* Everyone is permitted to copy and distribute verbatim or modified
* copies of this license document, and changing it is allowed as long
* as the name is changed.
*
* DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
* TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
*
* 0. You just DO WHAT THE FUCK YOU WANT TO.
*
* My Github: https://github.com/Tengag
*/

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

template<class T>
struct ListNode{
	T data;
	ListNode<T> *next;
};

template<class T>
class LinkedList{
private:
	int total_size;
	ListNode<T> *head;
	ListNode<T> *tail;
	ListNode<T> *getNode(int index);

public:
	LinkedList();
	~LinkedList();
	int size();
	bool add(T);
	bool remove(int index);
	bool remove_tail();
	bool remove_head();
	bool set(int index, T);
	T get(int index);
	void clear();
};

template<typename T>
LinkedList<T>::LinkedList(){
	head = tail = NULL;
	total_size = 0;
}

template<typename T>
LinkedList<T>::~LinkedList(){
	ListNode *tmp;
	while (head != NULL){
		tmp = head;
		head = head->next;
		delete tmp;
	}
	tail = NULL;
	total_size = 0;
}

template<typename T>
ListNode<T>* LinkedList<T>::getNode(int index){
	int position = 0;
	ListNode<T> *current = head;

	while (position < index){
		current = current->next;
		position++;
	}
	return current;
}

template<typename T>
int LinkedList<T>::size(){
	return total_size;
}

template<typename T>
bool LinkedList<T>::add(T _t){
	if (_t != NULL){
		ListNode<T> *tmp = new ListNode<T>();
		tmp->data = _t;
		tmp->next = NULL;

		if (head){
			tail->next = tmp;
			tail = tmp;
		}
		else{
			head = tmp;
			tail = tmp;
		}
		total_size++;
		return true;
	}
}

template<typename T>
bool LinkedList<T>::remove(int index){
	//reduce getNode
	if (index == 0)
		return remove_head();
	if (index + 1 == total_size)
		return remove_tail();
	
	ListNode<T> *tmp = getNode(index - 1);
	ListNode<T> *toDelete = tmp->next;

	tmp->next = tmp->next->next;
	delete toDelete;
	total_size--;

	return true;
}

template<typename T>
T LinkedList<T>::get(int index){
	ListNode<T> *tmp = getNode(index);
	return tmp->data;
}

template<typename T>
bool LinkedList<T>::set(int index, T _t){
	// Check if index position is in bounds
	if (index < 0 || index >= total_size)
		return false;

	getNode(index)->data = _t;
	return true;
}


template<typename T>
void LinkedList<T>::clear(){
	while (size() > 0)
		remove_head();
}

template<typename T>
bool LinkedList<T>::remove_head(){
	if (total_size <= 0)
		return false;
	if (total_size > 1){
		ListNode<T> *_next = head->next;
		delete head;
		head = _next;
		total_size--;
		return true;
	}
	else{
		return remove_tail();
	}
}

template<typename T>
bool LinkedList<T>::remove_tail(){
	if (total_size <= 0)
		return false;
	if (total_size >= 2){
		ListNode<T> *tmp = getNode(total_size - 2);
		delete tmp->next;
		tmp->next = NULL;
		tail = tmp;
		total_size--;
		return true;
	}
	else{
		T ret = head->data;
		delete head;
		head = tail = NULL;
		total_size = 0;
		return true;
	}
}
#endif