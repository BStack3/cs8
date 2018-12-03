#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <iostream>
#include "arrayutil.h"
using namespace std;

template <typename T>
class BPlusTree
{
public:
    class Iterator
    {
    public:
        friend class BPlusTree;
        Iterator(BPlusTree<T>* _node=nullptr, int _keyPtr = 0):node(_node), keyPtr(_keyPtr) {}

        //preconditions: node != nullptr, keyPtr < node->dataCount
        //postconditions: return data[keyPtr] of node.
        T& operator *()
        {
            assert(node && keyPtr < node->dataCount);
            return node->data[keyPtr];
        }

        //preconditions: node != nullptr
        //postconditions: make a copy of this, then,
        // call the prefix increment operator on this and return the copy.
        Iterator operator++(int unUsed)
        {
            Iterator temp = *this;
            this->operator++();
            return temp;
        }

        //preconditions: node != nullptr
        //postconditions: advance keyPtr if there are more items in the current leaf,
        // otherwise, advance node to the next leaf and set the current position within the leaf to 0.
        Iterator operator++()
        {
            if(node != nullptr)
            {
                if(keyPtr+1 < node->dataCount)
                    keyPtr++;
                else
                {
                    node = node->nextSubset;
                    keyPtr = 0;
                }
            }
            return *this;
        }

        friend bool operator ==(const Iterator& lhs, const Iterator& rhs)
        {
            return (lhs.node == rhs.node && (lhs.keyPtr == rhs.keyPtr));
        }

        friend bool operator !=(const Iterator& lhs, const Iterator& rhs)
        {
            return (lhs.node != rhs.node || (lhs.keyPtr != rhs.keyPtr));
        }

        //preconditions: none
        //postconditions: prints the 'list' using the iterator,
        // starting from the current data item.
        void printIterator()
        {
            Iterator temp = *this;
            while(temp.node != nullptr)
            {
                cout << *temp << " -> ";
                temp++;
            }
            cout << "|||" << endl;
        }

        bool is_null(){return !node;}

    private:
        BPlusTree<T>* node;
        int keyPtr;
    };

    BPlusTree(bool dups = false);

    //big three:
    BPlusTree(const BPlusTree<T>& other);
    ~BPlusTree();
    BPlusTree<T>& operator =(const BPlusTree<T>& RHS);

    bool areDupsOk() const { return dupsOk; }

    bool insert(const T& entry);                //insert entry into the tree
    bool remove(const T& entry);                //remove entry from the tree

    void clearTree();                           //clear this object (delete all nodes etc.)

    bool contains(const T& entry) const;              //true if entry can be found in the array
    T& get(const T& entry);                     //return a reference to entry in the tree
    const T& get(const T &entry) const;

    T* find(const T& entry);                    //return a pointer to this key. NULL if not there.

    int size() const;                           //count the number of elements in the tree
    bool empty() const;                         //true if the tree is empty

    void printTree(int level = 0, int index = 0, ostream &outs=cout) const; //print a readable version of the tree

    bool isValid() const;                       //verify that the tree satisfies all Btree rules.

    friend ostream& operator<<(ostream& outs, const BPlusTree<T>& printMe)
    {
        printMe.printTree(0, 0, outs);
        return outs;
    }

    Iterator getIteratorAtEntry(const T& entry); //return an iterator to this key. NULL if not there.
    Iterator begin();
    Iterator end();

private:
    void removeDuplicate(T key);
    T getSmallest();
    BPlusTree<T>* getMinNode();

    static const int MINIMUM = 1;
    static const int MAXIMUM = 2 * MINIMUM;

    bool dupsOk;                                   //true if duplicate keys may be inserted
    int dataCount;                                 //number of data elements
    T data[MAXIMUM + 1];                           //holds the keys
    int childCount;                                //number of children
    BPlusTree* subset[MAXIMUM + 2];                    //subtrees
    BPlusTree* nextSubset;
    size_t _size;

    //void copyTree(const BPlusTree<T>& other);
    void copyTree(const BPlusTree<T>& other, BPlusTree<T>*& lastLeaf);

    bool isLeaf() const {return childCount==0;}    //true if this is a leaf node

    //insert element functions
    bool looseInsert(const T& entry);              //allows MAXIMUM+1 data elements in the root
    void fixExcess(int i);                         //fix excess of data elements in child i

    //remove element functions:
    bool looseRemove(const T& entry);              //allows MINIMUM-1 data elements in the root
    void fixShortage(int i);                       //fix shortage of data elements in child i

    void removeBiggest(T& entry);                  //remove the biggest child of this tree->entry
    void rotateLeft(int i);                        //transfer one element LEFT from child i
    void rotateRight(int i);                       //transfer one element RIGHT from child i
    void mergeWithNextSubset(int i);               //merge subset i with subset i+1
    void mergeWithPreviousSubset(int i);

    //used by isValid() to verify that data[i] > all of subtree[i], and data[i] < all of subtree[i+1]
    bool isLargerThanTree(const T &item) const;      //returns true if item is larger than all data items in tree.
    bool isSmallerThanTree(const T &item) const;     //returns true if item is smaller than all data items in tree.
    bool verifyDepth() const;                        //verify that all leaf nodes occur at the same recursive depth, relative to this node.
    bool verifyRelativePositionsOfDataItems() const; //verify that all data[]s in the tree are sorted, and that subtree[i] < data[i].
    int maxDepth() const;                            //used by verifyDepth to obtain the depth of a particular subtree.
};

//preconditions: none
//postconditions: if all conditions for a valid BTree are met,
// return true, otherwise false.
template<typename T>
bool BPlusTree<T>::isValid() const
{
    return (verifyDepth() && verifyRelativePositionsOfDataItems());
}

//preconditions: none.
//postcontions: returns true if the depth of the tree is constant, otherwise false.
template<typename T>
bool BPlusTree<T>::verifyDepth() const
{
    bool depthOk = true;
    int theDepth = 0;

    if(!isLeaf())
    {
        theDepth = subset[0]->maxDepth(); //the max depth of the 0th subtree.
        for(int i = 1; i < childCount && depthOk; i++)
        {
            //verify that the max depth of subset[i] is the same as the max depth of subset[0]
            if(theDepth != subset[i]->maxDepth())
                depthOk = false;
        }

        for(int i = 0; i < childCount && depthOk; i++)
        {
            //call verifyDepth on the ith subset, being sure not to overwrite depthOk if it is false.
            bool subTreeDepthStatus = subset[i]->verifyDepth();
            if(depthOk)
                depthOk = subTreeDepthStatus;
        }
    }

    return depthOk;
}

//preconditions: none.
//postcontions: traverse the tree to find all leaf nodes,
// returning the largest depth that a leaf node was encountered at.
template<typename T>
int BPlusTree<T>::maxDepth() const
{
    int theMaxDepth = 1;

    if(isLeaf())
        return theMaxDepth;
    else
    {
        for(int i = 0; i < childCount; i++)
        {
            int subTreeDepth = subset[i]->maxDepth() + 1;

            if(subTreeDepth > theMaxDepth)
                theMaxDepth = subTreeDepth;
        }
        return theMaxDepth;
    }
}

//preconditions: none.
//postcontions: returns true if item is larger than all data items in tree, otherwise false.
template<typename T>
bool BPlusTree<T>::isLargerThanTree(const T &item) const
{
    bool isLargest = true;

    for(int i = 0; i < dataCount; i++)
        if(data[i] > item)
            isLargest = false;

    for(int i = 0; i < childCount && isLargest; i++)
    {
        bool returnVal = subset[i]->isLargerThanTree(item);

        //if we have found that a branch returned false, do not overwrite isLargest.
        if(isLargest)
            isLargest = returnVal;
    }

    return isLargest;
}

//preconditions: none.
//postcontions: returns true if item is smaller than all data items in tree, otherwise false.
template<typename T>
bool BPlusTree<T>::isSmallerThanTree(const T &item) const
{
    bool isSmallest = true;

    for(int i = 0; i < dataCount; i++)
        if(item > data[i])
            isSmallest = false;

    for(int i = 0; i < childCount && isSmallest; i++)
    {
        bool returnVal = subset[i]->isSmallerThanTree(item);

        //if we have found that a branch returned false, do not overwrite isSmallest.
        if(isSmallest)
            isSmallest = returnVal;
    }

    return isSmallest;
}

//preconditions: none.
//postcontions: returns true if all of the following are true:
// 1) if a node has k data items, it has k+1 subsets (with the exception of leaf nodes)
// 2) for all non-leaf nodes, data[i] > all items in subtree[i]
// 3) for all non-leaf nodes, data[i] < all items in subtree[i+1]
// 4) for any node, data[i] < data[i+1]
// -- otherwise, returns false.
template<typename T>
bool BPlusTree<T>::verifyRelativePositionsOfDataItems() const
{
    static const bool DEBUG = true;

    //initially, assume that a tree / subtree is valid.
    bool treeIsValid = true;

    //verify that data[i] < data[i+1]
    if(DEBUG)
        assert(arrayIsSorted(data,dataCount));
    else if(!arrayIsSorted(data,dataCount))
        treeIsValid = false;

    if(!isLeaf())
    {
        //verify that a non-leaf with k data items, has k+1 subsets.
        if(DEBUG)
            assert(dataCount == childCount-1);
        else if(dataCount != childCount-1)
            treeIsValid = false;

        //verify that data[i] > all of subset[i], for all data items in this node.
        for(int i = 0; i < dataCount; i++)
        {
            if(DEBUG)
                assert(subset[i]->isLargerThanTree(data[i]));
            else if(!subset[i]->isLargerThanTree(data[i]))
                treeIsValid = false;

            if(DEBUG)
                assert(data[i]==subset[i+1]->getSmallest());
            else if(!(data[i] == subset[i+1]->getSmallest()))
                treeIsValid = false;
        }

        //Call this function all all subtrees recursively,
        //  being sure not to overwrite a false value for treeIsValid.
        for(int i = 0; i < childCount && treeIsValid; i++)
        {
            bool returnVal = subset[i]->isValid();
            if(treeIsValid)
                treeIsValid = returnVal;
        }
    }
    return treeIsValid;
}

//preconditions: none
//postconditions: BTree will be initialized to allow duplicates if dups.
template<typename T>
BPlusTree<T>::BPlusTree(bool dups)
{
    nextSubset = nullptr;
    dupsOk = dups;
    dataCount = 0;
    childCount = 0;
    _size = 0;
}

//preconditions: none
//postconditions: BTree will be initialized to allow dups if other allows them,
// and the tree structure / contents of other will be copied to this tree.
template<typename T>
BPlusTree<T>::BPlusTree(const BPlusTree<T> &other)
{
    _size = other._size;
    nextSubset = nullptr;
    dupsOk = other.dupsOk;
    BPlusTree<T>* temp = nullptr;
    copyTree(other,temp);
}

//preconditions: none
//postconditions: BTree will be initialized to allow dups if RHS allows them,
//  all dynamic memory of the current tree will be deallocated by clearTree(),
//  and the tree structure / contents of RHS will be copied to this tree.
template<typename T>
BPlusTree<T>& BPlusTree<T>::operator =(const BPlusTree<T>& RHS)
{
    assert(&RHS != this);

    clearTree();
    BPlusTree<T>* temp = nullptr;
    _size = RHS._size;
    nextSubset = nullptr;
    dupsOk = RHS.dupsOk;

    copyTree(RHS,temp);
    return *this;
}

//preconditions: none
//postconditions: All dynamic memory will be deallocated by clearTree().
template<typename T>
BPlusTree<T>::~BPlusTree()
{
    clearTree();
}

//preconditions: none
//postconditions: All dynamic memory will be freed by traversing the tree from left to right.
// note that after clearing all children of a node, childCount will be set to 0, so when
// the parent of this node calls delete, double deletion errors wil be prevented.
template<typename T>
void BPlusTree<T>::clearTree()
{
    if(childCount > 0)
    {
        for (int i = 0; i < childCount; i++)
        {
            subset[i]->clearTree();
            delete subset[i];
        }
        childCount = 0;
        dataCount =0;
    }
}

//preconditions: none
//postconditions: other will be traversed recursively to copy the data and
// structure of other tree to this tree.
template<typename T>
void BPlusTree<T>::copyTree(const BPlusTree<T>& other, BPlusTree<T>*& lastLeaf)
{
    //copy the data of the root from source to dest.
    copyArray(data,other.data,dataCount,other.dataCount);
    childCount = other.childCount;

    //if the root is not a leaf:
    if(!other.isLeaf())
    {
        for(int i = 0; i < other.childCount; i++)
        {
            subset[i] = new BPlusTree<T>(other.dupsOk);
            subset[i]->copyTree(*other.subset[i],lastLeaf);
        }
    }
    else
    {
        //since the leaf nodes are being instantiated from left to right
        // lastLeaf will be null for the first leaf, for all subsequent leaves,
        // it will point to the previous node.
        if(lastLeaf)
            lastLeaf->nextSubset = this;
        lastLeaf = this;
    }
}

//preconditions: none
//postconditions: the entry will be inserted into the BTree using looseInsert,
// which when returning, may cause an excess in the root, which will be resolved by:
// 1) creating a new node, copying all the contents of this root into it,
// 2) clearing the root node,
// 3) making the new node this root's only child (subset[0])
// 4) calling fixExcess on this only subset (subset[0])
template <typename T>
bool BPlusTree<T>::insert(const T& entry)
{
    bool itemInserted = looseInsert(entry);
    if(itemInserted)
    {
        _size++;
        if(dataCount == MAXIMUM + 1)
        {
            //create a new node, copy all the contents of this root into it,
            BPlusTree<T> * newNode = new BPlusTree<T>(dupsOk);
            copyArray(newNode->data, data, newNode->dataCount, dataCount);
            copyArray(newNode->subset, subset, newNode->childCount, childCount);

            dataCount = 0;
            childCount = 1;

            //make the new node this root's only child (subset[0])
            subset[0] = newNode;
            fixExcess(0);
        }
    }
    return itemInserted;
}

//preconditions: none
//postcondition: looseRemove will be called to remove the target from the tree,
// the tree will be valid when returning, except that the root may have no data
// and only a single subset - This will be resolved by:
//  1) create a temporary pointer (shrink_ptr) and point it to this root's only subset
//  2) copy all the data and subsets of this subset into the root (through shrink_ptr)
//  3) now, the root contains all the data and poiners of it's old child.
//  4) simply delete shrink_ptr (blank out child), and the tree has shrunk by one level.
// Note, the root node of the tree will always be the same, it's the child node we delete
template<typename T>
bool BPlusTree<T>::remove(const T& entry)
{
    bool itemRemoved = looseRemove(entry);
    if(itemRemoved)
    {
        _size--;
        if(dataCount <= 1 && childCount == 1)
        {
            BPlusTree<T>* shrinkPtr = subset[0];
            copyArray(data,shrinkPtr->data,dataCount,shrinkPtr->dataCount);
            copyArray(subset,shrinkPtr->subset,childCount,shrinkPtr->childCount);
            shrinkPtr->childCount = 0;
            delete shrinkPtr;
        }
    }
    return itemRemoved;
}

//preconditions: none
//postconditions: if the key exists in this subtree,
// and the key is not at a leaf, it will be replaced
// by the smallest item in subset[index+1] of where it was found.
template<typename T>
void BPlusTree<T>::removeDuplicate(T key)
{
    int index = firstGE(data,dataCount,key);
    bool found = (index < dataCount && key == data[index]);

    if(found && !isLeaf())
        data[index] = subset[index+1]->getSmallest();
    else if(!found && !isLeaf())
        subset[index]->removeDuplicate(key);
}

//preconditions: none
//postconditions: returns the smallest item in this subtree.
template<typename T>
T BPlusTree<T>::getSmallest()
{
    if(!isLeaf())
        return subset[0]->getSmallest();
    else
        return data[0];
}

//preconditions: none
//postconditions: returns an interator to entry, if it exists in the tree.
//                otherwise return an iterator to null.
template<typename T>
typename BPlusTree<T>::Iterator BPlusTree<T>::getIteratorAtEntry(const T& entry)
{
    int index = firstGE(data,dataCount,entry);
    bool found = (index < dataCount && entry == data[index]);

    if(found)
    {
        if(isLeaf())
            return BPlusTree<T>::Iterator(this,index);
        else
            return subset[index+1]->getIteratorAtEntry(entry);
    }
    else
    {
        if(isLeaf())
            return BPlusTree<T>::Iterator();
        else
            return subset[index]->getIteratorAtEntry(entry);
    }
}

//preconditions: none
//postconditions: returns an interator to the first data item in the leaf nodes.
template<typename T>
typename BPlusTree<T>::Iterator BPlusTree<T>::begin()
{
    if(!this->empty())
    {
        BPlusTree<T> * temp = this;
        while(!temp->isLeaf())
            temp = temp->subset[0];

        return BPlusTree<T>::Iterator(temp,0);
    }
    else
        return BPlusTree<T>::Iterator();
}

//preconditions: none
//postconditions: returns an interator null.
template<typename T>
typename BPlusTree<T>::Iterator BPlusTree<T>::end()
{
    return BPlusTree<T>::Iterator();
}

//preconditions: none
//postconditions: returns a reference to the entry in the tree.
// if no such entry exists, the entry will be inserted.
// otherwise, just return the reference to the existing entry.
template<typename T>
T &BPlusTree<T>::get(const T &entry)
{
    T * temp = find(entry);
    if(!temp)
    {
        insert(entry);
        temp = find(entry);

    }
    return *temp;
}

//preconditions: none
//postconditions: returns a reference to the entry in the tree.
// if no such entry exists, the entry will be inserted.
// otherwise, just return the reference to the existing entry.
template<class T>
const T& BPlusTree<T>::get(const T &entry) const
{
    T * temp = find(entry);
    assert(temp);
    return *temp;
}

//preconditions: none
//postconditions: returns true if the entry exists in the tree, otherwise false.
template<typename T>
bool BPlusTree<T>::contains(const T &entry) const
{
    int index = firstGE(data,dataCount,entry);
    bool found = (index < dataCount && entry == data[index]);
    if(found)
    {
        if(isLeaf())
            return true;
        else
            return subset[index+1]->contains(entry);
    }
    else
    {
        if(isLeaf())
            return false;
        else
            return subset[index]->contains(entry);
    }
}

//preconditions: none
//postconditions: returns a pointer to the entry in the tree if it exists,
// otherwise returns nullptr.
template<typename T>
T *BPlusTree<T>::find(const T &entry)
{
    int index = firstGE(data,dataCount,entry);
    bool found = (index < dataCount && entry == data[index]);

    if(found)
    {
        if(isLeaf())
            return &data[index];
        else
            return subset[index+1]->find(entry);
    }
    else
    {
        if(isLeaf())
            return nullptr;
        else
            return subset[index]->find(entry);
    }
}

//preconditions: none
//postconditions: returns the total number of data items in the tree.
template<typename T>
int BPlusTree<T>::size() const
{
    return _size;
}

//preconditions: none
//postconditions: returns true if this node
// has no children or data items, otherwise false.
template<typename T>
bool BPlusTree<T>::empty() const
{
    if(childCount == 0 && dataCount == 0)
        return true;
    else
        return false;
}

//preconditions: none
//postconditions: the tree will be printed
template <typename T>
void BPlusTree<T>::printTree(int level, int index, ostream& outs) const
{
    //1. print the last child (if any)
    //2. print all the rest of the data and children
    if(childCount > 0)
        for(int i = childCount-1; i >= 0; i--)
            if(subset[i])
                subset[i]->printTree(level+1,i,outs);

    outs << setw(level*10) << index << " : "; //buffer the list based on the depth.
    printArray(data,dataCount);
}

//preconditions: none
//postconditions: the entry will be inserted into the tree,
// if it does not already exist, or if duplicates are allowed.
// The item will be inserted as follows:
//  1) find the index of the first item in data[] of this node that is not less than the entry.
//     if no such entry exists, index will be set to dataCount.
//  2) check if the entry exists in the tree
//  3) if the entry was found,
//     a) if dupsOk, insert it here.
//     b) else, return false.
//  4) if this node is a leaf, insert the entry here,
//     otherwise, call looseInsert on subset[i]
template <typename T>
bool BPlusTree<T>::looseInsert(const T& entry)
{
    bool itemInserted = false;
    int i = firstGE(data, dataCount, entry);
    bool found = (i < dataCount && entry == data[i]);

    if(found)
    {
        // if the node is not a leaf, we have not found the data item yet.
        // howerver, we know it is the leftmost data item in subse[i+1]
        if(!isLeaf())
        {
            itemInserted = subset[i+1]->looseInsert(entry);
            fixExcess(i+1);
        }
        else
        {
            if(!dupsOk)
            {
                itemInserted = false;
            }
        }
    }
    else
    {
        if(isLeaf())
        {
            orderedInsert(data,dataCount,entry);
            itemInserted = true;
        }
        else
            itemInserted = subset[i]->looseInsert(entry);

        if(itemInserted)
        {
            fixExcess(i);
        }
    }

    return itemInserted;
}

//preconditions: subset[i] must have an exces,  index < maximum+1, childCount <= maximum+1
//postconditions: the excess in subset[i] will be resolved as follows:
//  1) add a new subset at location i+1 of this node
//  2) split subset[i] (both the subset array and the data array)
//  and move half into subset[i+1] (this is the subset we created in step 1.)
//  3) detach the last data item of subset[i] and bring it and insert it into this node's data[]
//Note that this last step may cause this node to have too many items. This is OK. This will be
//dealt with at the higher recursive level. (my parent will fix it!)
template <typename T>
void BPlusTree<T>::fixExcess(int i)
{
    assert(i <= MAXIMUM+1 && childCount <= MAXIMUM +1);

    if(i < childCount && subset[i]->dataCount > MAXIMUM)
    {
        if(subset[i]->isLeaf())
        {
            insertItem(subset,i+1,childCount,new BPlusTree<T>(dupsOk));
            split(subset[i]->data,subset[i]->dataCount,subset[i+1]->data,subset[i+1]->dataCount,true);
            split(subset[i]->subset,subset[i]->childCount,subset[i+1]->subset,subset[i+1]->childCount);
            T temp = subset[i+1]->data[0];
            orderedInsert(data,dataCount, temp);

            //preserve the 'linked list' when inserting a leaf to the right of i
            BPlusTree<T>* nextFromI = subset[i]->nextSubset;
            subset[i]->nextSubset = subset[i+1];
            subset[i+1]->nextSubset = nextFromI;
        }
        else
        {
            insertItem(subset,i+1,childCount,new BPlusTree<T>(dupsOk));
            split(subset[i]->data,subset[i]->dataCount,subset[i+1]->data,subset[i+1]->dataCount);
            split(subset[i]->subset,subset[i]->childCount,subset[i+1]->subset,subset[i+1]->childCount);
            orderedInsert(data,dataCount, detachItem(subset[i]->data,subset[i]->dataCount));
        }
    }
}

//preconditions: none
//postconditions: the entry, if it exists, will be removed from the tree.
// a) leaf && not found target: there is nothing to do
// b) leaf && found target: just remove the target
// c) not leaf and not found target: recursive call to loose_remove
// d) not leaf and found: replace target with largest child of subset[i]
template <typename T>
bool BPlusTree<T>::looseRemove(const T& entry)
{
    int index = firstGE(data,dataCount,entry);
    bool found = (index < dataCount && data[index] == entry);
    bool itemRemoved = false;

    if(!isLeaf())
    {
        if(found)
        {
            itemRemoved = subset[index+1]->looseRemove(entry);
            if(subset[index+1]->dataCount < MINIMUM)
                fixShortage(index+1);
        }
        else
        {
            itemRemoved = subset[index]->looseRemove(entry);
            if(subset[index]->dataCount < MINIMUM)
                fixShortage(index);
        }
        removeDuplicate(entry);
    }
    else
    {
        if(found)
        {
            deleteItem(data,index,dataCount);
            itemRemoved = true;
        }
        else
            itemRemoved = false;
    }

    return itemRemoved;
}

//preconnditions: none
//postconditions: the largest entry in this subtree will be deleted and placed in entry.
template <typename T>
void BPlusTree<T>::removeBiggest(T& entry)
{
    // Keep looking in the last subtree (recursive) until you get to a leaf.
    // Then, detach the last (biggest) data item, after the recursive call, fix shortage.
    if(!isLeaf())
    {
        subset[childCount - 1]->removeBiggest(entry);
        if(subset[childCount-1]->dataCount < MINIMUM)
            fixShortage(childCount - 1);
    }
    else
        entry = detachItem(data,dataCount);
}

//preconditions: subset[i] must have a shortage.
//postconditions: the shortaged at subset[i] will be resolved by:
// 1) if: i+1 < childCount && subset[i+1]->dataCount > MINIMUM, then rotateLeft
// 2) if: i > 0 && i < childCount && subset[i-1]->dataCount > MINIMUM, then rotateRight
// 3) if i+1 < childCount, then mergeWithNextSubset
// 4) otherwise, mergeWithPreviousSubset
template <typename T>
void BPlusTree<T>::fixShortage(int i)
{
    bool merge = false;

    if(i+1 < childCount && subset[i+1]->dataCount > MINIMUM)
        rotateLeft(i);
    else if(i > 0 && i < childCount && subset[i-1]->dataCount > MINIMUM)
        rotateRight(i);
    else if(i+1 < childCount)
    {
        merge = true;
        mergeWithNextSubset(i);
    }
    else
    {
        merge = true;
        mergeWithPreviousSubset(i);
    }

    //there is no guarentee that data[j] == subset[j+1]->data[0] for all 0 <= j < dataCount
    // after returning a rotate or merge function.
    if(subset[0]->isLeaf())
    {
        //if a merge took place at the leaf level, this guarentees that
        // the item to be removed from data should be data[dataCount-1]
        // note that only data[i-1] to data[dataCount-1] needs to be verified.
        for(int j = ((i == 0) ? 0 : i-1); j < dataCount; j++)
            if(!(data[j] == subset[j+1]->data[0]))
                data[j] = subset[j+1]->data[0];

        //note that for merge, a subset was deleted, after the above shift
        // the item that needs to be removed from data[] is now at data[dataCount-1].
        if(merge)
            deleteItem(data,dataCount-1,dataCount);
    }
}

//preconditions: (i + 1 < childCount)
//postconditions: The shortage in subset[i] will be resolved by the following:
//                1) transfer data[i] to the front of subset[i+1] data
//                2) transfer all data and subsets from subset[i] to front of subset[i+1]
//                3) delete subset[i]
template <typename T>
void BPlusTree<T>::mergeWithNextSubset(int i)
{
    assert(childCount > i+1);

    if(subset[i]->isLeaf())
    {
        //transfer subset[i+1]->data[] to the end of subset[i]->data[]
        mergeArrays(subset[i]->data,subset[i]->dataCount,subset[i+1]->data,subset[i+1]->dataCount);

        //bypass and delete subset[i+1]
        subset[i]->nextSubset = subset[i+1]->nextSubset;
        delete deleteItem(subset,i+1,childCount);
    }
    else
    {
        insertItem(subset[i+1]->data,0,subset[i+1]->dataCount,deleteItem(data,i,dataCount));
        mergeFront(subset[i+1]->data,subset[i+1]->dataCount,subset[i]->data,subset[i]->dataCount);
        mergeFront(subset[i+1]->subset,subset[i+1]->childCount,subset[i]->subset,subset[i]->childCount);

        delete deleteItem(subset,i,childCount);
    }
}

//preconditions: (i > 0)
//postconditions: The shortage in subset[i] will be resolved by the following:
//                1) transfer all data and subsets from subset[i] to end of subset[i-1]
//                2) transfer all data and subsets from subset[i] to end of subset[i-1]
//                3) delete subset[i]
template <typename T>
void BPlusTree<T>::mergeWithPreviousSubset(int i)
{
    assert(i > 0);

    if(subset[i]->isLeaf())
    {
        //transfer subset[i]->data[] to the end of subset[i-1]->data[]
        mergeArrays(subset[i-1]->data,subset[i-1]->dataCount,subset[i]->data,subset[i]->dataCount);

        //bypass and delete subset[i-1]
        subset[i-1]->nextSubset = subset[i]->nextSubset;
        delete deleteItem(subset,i,childCount);
    }
    else
    {
        attachItem(subset[i-1]->data,subset[i-1]->dataCount,deleteItem(data,i-1,dataCount));
        mergeArrays(subset[i-1]->data,subset[i-1]->dataCount,subset[i]->data,subset[i]->dataCount);
        mergeArrays(subset[i-1]->subset,subset[i-1]->childCount,subset[i]->subset,subset[i]->childCount);

        delete deleteItem(subset,i,childCount);
    }
}

//preconditions: (dataCount > i) && (subset[i]->dataCount < MAXIMUM+1) && (subset[i+1]->dataCount > MINIMUM)
//postconditions: The shortage in subset[i] will be resolved by the following:
//                1) transfer data[i+1] to end of subset[i]->data
//                2) transfer first item from subset[i+1]->data to data[i].
//                3) If subset[i+1] has children, transfer subset[i+1]->subset[0] child to end of subset[i]->subset
template <typename T>
void BPlusTree<T>::rotateLeft(int i)
{
    assert((dataCount > i) && (subset[i]->dataCount < MAXIMUM+1) && (subset[i+1]->dataCount > MINIMUM));

    if(subset[i]->isLeaf())
    {
        T firstFromNext = deleteItem(subset[i+1]->data,0,subset[i+1]->dataCount);
        attachItem(subset[i]->data,subset[i]->dataCount, firstFromNext);
    }
    else
    {
        attachItem(subset[i]->data,subset[i]->dataCount,deleteItem(data,i,dataCount));
        insertItem(data,i,dataCount,deleteItem(subset[i+1]->data,0,subset[i+1]->dataCount));

        if(subset[i+1]->childCount > 0)
            attachItem(subset[i]->subset,subset[i]->childCount,deleteItem(subset[i+1]->subset,0,subset[i+1]->childCount));
    }
}

//preconditions: (i > 0) && (subset[i]->dataCount < MAXIMUM+1) && (subset[i-1]->dataCount > MINIMUM)
//postconditions: The shortage in subset[i] will be resolved by the following:
//                1) transfer data[i-1] to front of subset[i]->data
//                2) transfer final item from subset[i-1]->data to data[i-1].
//                3) If subset[i-1] has children, transfer final child to front of subset[i]->subset
template <typename T>
void BPlusTree<T>::rotateRight(int i)
{
    assert((i > 0) && (subset[i]->dataCount < MAXIMUM+1) && (subset[i-1]->dataCount > MINIMUM));

    if(subset[i]->isLeaf())
    {
        insertItem(subset[i]->data,0,subset[i]->dataCount,detachItem(subset[i-1]->data,subset[i-1]->dataCount));
    }
    else
    {
        insertItem(subset[i]->data,0,subset[i]->dataCount, deleteItem(data,i-1,dataCount));
        insertItem(data,i-1,dataCount, detachItem(subset[i-1]->data,subset[i-1]->dataCount));
        if(subset[i-1]->childCount > 0)
            insertItem(subset[i]->subset,0,subset[i]->childCount,detachItem(subset[i-1]->subset,subset[i-1]->childCount));
    }
}

#endif // BPLUSTREE_H
