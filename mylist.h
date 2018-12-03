#ifndef MYLIST_H
#define MYLIST_H

#include <iostream>
#include <cassert>
using namespace std;

template <class T>
struct node{
    T _item;
    node<T>* _next;
    node(const T& item = T(), node<T>* next = NULL):_item(item), _next(next){}
    friend ostream& operator << (ostream& outs, const node<T>& print_me){
        outs<<"["<<print_me._item<<"]->";
        return outs;
    }
};

//initializes head to NULL
template <class T>
node<T>* init_head(node<T>* &head);

//deletes all the nodes in the list
template<class T>
void delete_all(node<T>*&head);

//true if head is NULL, false otherwise.
template <class T>
bool empty(const node<T>* head);

//makes a copy of the list, returns a pointer to the last node:
template <class T>
node<T>* copy_list(const node<T>* head, node<T>* & cpy);

//insert at the beginning of the list:
template <class T>
node<T>* insert_head(node<T>* &head, T item);

//insert_after: if after is NULL, inserts at head
template <class T>
node<T>* insert_after(node<T>* &head, node<T>* after, const T& item);

//delete the node at the head of the list, reuturn the item:
template <class T>
T delete_head(node<T>* &head);

//print the list and return outs
template<class T>
ostream& print_list(const node<T>* head, ostream& outs=cout);

//initializes head to NULL
template <class T>
node<T>* init_head(node<T>* &head)
{
    head = nullptr;
    return head;
}

//preconditions: none
//postconditions: all nodes in the list will be deleted and head will be set to null.
template<class T>
void delete_all(node<T>*&head)
{
    while(head != nullptr)
    {
        node<T>* tempNode = head->_next;
        delete head;
        head = tempNode;
    }
}

//preconditions: none
//postconditions: returns true if the list is empty, otherwise false.
template <class T>
bool empty(const node<T>* head)
{
    return (head == nullptr) ? true : false;
}

//preconditions: none
//postconditions: makes a copy of the list, returns a pointer to the last node:
template <class T>
node<T>* copy_list(const node<T>* head, node<T>* & cpy)
{
    //delete contents of cpy
    delete_all(cpy);

    node<T>* tailOfCpy = nullptr;

    //case 1: empty list
    if(head == nullptr)
    {
        cpy = nullptr;
    }
    //check for self assignment
    else if(head == cpy)
    {
        exit(EXIT_FAILURE);
    }
    else
    {
        const node<T>* currentNode = head;

        //Since we are inserting at the tail to preserve order,
        //      maintain a pointer to the head of the new list.
        cpy = new node<T>(head->_item);
        tailOfCpy = cpy;
        currentNode = currentNode->_next;

        //Copy all other items to the tail of the copy.
        while(currentNode != nullptr)
        {
            tailOfCpy->_next = new node<T>(currentNode->_item);
            tailOfCpy = tailOfCpy->_next;
            currentNode = currentNode->_next;
        }
    }

    return tailOfCpy;
}

//preconditons: none
//postconditions: A new node with value = item is inserted before head.
//                A pointer to the new node is returned.
template <class T>
node<T>* insert_head(node<T>* &head, T item)
{
    node<T>* newNode = new node<T>(item);
    newNode->_next = head;
    head = newNode;
    return head;
}

//preconditions: none
//postconditions: if after is NULL, inserts after head, otherwise insert after after.
template <class T>
node<T>* insert_after(node<T>* &head, node<T>* after, const T& item)
{
    node<T>* newNode = new node<T>(item);

    if(head == nullptr)
        head = newNode;
    else if(after == nullptr)
    {
        newNode->_next = head->_next;
        head->_next = newNode;
    }
    else
    {
        newNode->_next = after->_next;
        after->_next = newNode;
    }

    return newNode;
}

//Preconditions: the list must not be empty.
//Postconditions: the node pointed to by head is deleted,
//   the value of the deleted node is returned as type T.
template <class T>
T delete_head(node<T>* &head)
{
    assert(head != nullptr);

    node<T>* head_next = head->_next;
    T returnval = head->_item;
    delete head;
    head = head_next;

    return returnval;
}

//Preconditions: None
//Postconditions: The contents of the list are printed to the provided output stream (cout by default)
template<class T>
ostream& print_list(const node<T>* head, ostream& outs)
{
    if(head != nullptr)
    {
        const node<T>* current = head;
        while(current != nullptr)
        {
            outs << "["<<current->_item <<"]->";
            current = current->_next;
        }
    }
    outs << "|||";

    return outs;
}

#endif // MYLIST_H
