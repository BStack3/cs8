#ifndef MYQUEUE_H
#define MYQUEUE_H
#include "mylist.h"

template <class T>
class Queue{
public:
    Queue():_head(nullptr), _tail(nullptr), _size(0){}
    ~Queue();
    Queue(const Queue<T>& other);
    Queue<T> &operator =(const Queue& rhs);
    void push(T item);
    T pop();
    bool empty();
    T front();
    size_t size() {return _size;}
    void clear();

    friend ostream& operator <<(ostream& outs, const Queue& q){
        return print_list(q._head, outs);
    }

private:
    node<T>* _head;
    node<T>* _tail;
    size_t _size;
};

//preconditions: none
//postconditons: all nodes deleted
template<typename T>
Queue<T>::~Queue()
{
    delete_all(_head);
}

//preconditions: none
//postconditons: all nodes deleted
template<typename T>
void Queue<T>::clear()
{
    delete_all(_head);
    _size = 0;
}

//preconditions: none
//postconditons: a new myQueue object is constructed with the contents of 'other'.
template<typename T>
Queue<T>::Queue(const Queue<T>& other)
{
    init_head(_head);
    _size = other._size;

    _tail = copy_list(other._head,this->_head);
}

//preconditions: none
//postconditons: delete any contents of this stack and copy from 'rhs'.
template<typename T>
Queue<T>& Queue<T>::operator =(const Queue& rhs)
{
     //check for self-assignment.
    assert(&rhs != this);
    _size = rhs._size;

    //clear the calling object before assignment.
    delete_all(this->_head);
    _tail = copy_list(rhs._head,this->_head);

    return *this;
}

//preconditions: none
//postconditions: insert a new node at the back of the myQueue with _item = 'item'
template<typename T>
void Queue<T>::push(T item)
{
    _size++;

    //insert after tail and set the tail to point to the new item.
    _tail = insert_after(_head,_tail,item);
}

//preconditions: the myQueue must not be empty
//postconditions: delete the node at the front of the myQueue, returning its value.
template<typename T>
T Queue<T>::pop()
{
    _size--;

    return delete_head(_head);
}

//preconditions: none
//postconditions: returns true if the myQueue is empty, otherwise false.
template<typename T>
bool Queue<T>::empty()
{
     return (_head == nullptr) ? true : false;
}

//preconditions: the myQueue must not be empty.
//postconditions: returns the value of the node at the head of the myQueue.
template<typename T>
T Queue<T>::front()
{
    return _head->_item;
}

#endif // MYQUEUE_H
