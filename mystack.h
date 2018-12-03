#ifndef MYStack_H
#define MYStack_H
#include "mylist.h"

template <class T>
class Stack{
public:
    Stack():_top(nullptr),_size(0){}
    ~Stack();
    Stack(const Stack<T>& other);
    Stack<T>& operator =(const Stack<T>& rhs);

    void push(T item);
    T pop();
    T top();
    bool empty();
    size_t size() {return _size;}
    void clear();

    friend ostream& operator << (ostream& outs, const Stack& s){
        return print_list(s._top, outs);
    }
private:
    node<T>* _top;
    size_t _size;
};

//preconditions: none
//postconditons: all nodes deleted
template<typename T>
Stack<T>::~Stack()
{
    delete_all(_top);
}

//preconditions: none
//postconditons: all nodes deleted
template<typename T>
void Stack<T>::clear()
{
    delete_all(_top);
    _size = 0;
}

//preconditions: none
//postconditons: a new myStack object is constructed with the contents of 'other'.
template<typename T>
Stack<T>::Stack(const Stack<T>& other)
{
    init_head(_top);
    _size = other._size;
    copy_list(other._top,this->_top);
}

//preconditions: none
//postconditons: delete any contents of this myStack and copy from 'rhs'.
template<typename T>
Stack<T>& Stack<T>::operator =(const Stack<T>& rhs)
{
    //must clear the calling object before assignment.
    //check for self-assignment.
    _size = rhs._size;

    assert(rhs._top != this->_top);
    delete_all(this->_top);
    copy_list(rhs._top,this->_top);
}

//preconditions: none
//postconditons: inserts a new node at the top of the myStack with _item = 'item'.
template<typename T>
void Stack<T>::push(T item)
{
    _size++;
   _top = insert_head(_top,item);
}

//preconditions: the myStack must not be empty.
//postconditons: deletes the node at the top of the myStack
//               and returns the value of the node deleted.
template<typename T>
T Stack<T>::pop()
{
    _size++;
    return delete_head(_top);
}

//preconditions: the myStack must not be empty.
//postconditons: returns the value at the top of the myStack.
template<typename T>
T Stack<T>::top()
{
    T value = _top->_item;
    return value;
}

//preconditions: none
//postconditions: returns true if the myStack is empty, otherwise false.
template<typename T>
bool Stack<T>::empty()
{
    return (_top == nullptr) ? true : false;
}


#endif // MYSTACK_H
