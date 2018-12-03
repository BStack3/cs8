#ifndef MULTIMAP_H
#define MULTIMAP_H
#include "bplustree.h"
#include "set.h"
#include "constants.h"
#include <vector>
using namespace std;


template <typename K, typename V, typename container>
K _key;
container _values;

class MPair
{
public:
    K key() { return _key; } //return by value... the original key is immutable.
    container& values() { return _values; }

    MPair(const K& k=K())
    {
        _key = k;
    }

    MPair(const K& k, const V& v)
    {
        _key = k;
        _values;
        _values.push_back(v);
    }

    MPair(const K& k, const vector<V>& vlist)
    {
        _key = k;
        _values(vlist);
    }
    //--------------------------------------------------------------------------------

    friend std::ostream& operator <<(std::ostream& outs, const MPair<K, V>& printMe)
    {
        outs << printMe._key << ": " << printMe._values;
        return outs;
    }
    friend bool operator ==(const  MPair<K, V>& lhs, const MPair<K, V>& rhs)
    {
        return (lhs._key == rhs._key);
    }
    friend bool operator < (const MPair<K, V>& lhs, const MPair<K, V>& rhs)
    {
        return (lhs._key < rhs._key);
    }
    friend bool operator > (const MPair<K, V>& lhs, const MPair<K, V>& rhs)
    {
        return (lhs._key > rhs._key);
    }
    friend bool operator <= (const MPair<K, V>& lhs, const MPair<K, V>& rhs)
    {
        return (lhs._key <= rhs._key);
    }
    friend bool operator >= (const MPair<K, V>& lhs, const MPair<K, V>& rhs)
    {
        return (lhs._key >= rhs._key);
    }

private:
    K _key;
    container _values;
};

template <typename K, typename V>
class MMap
{
public:
    class Iterator
    {
    public:
        friend class MMap;

//        Iterator(typename BPlusTree<MPair<K,V> >::Iterator _it)
//        {
//            _treeIt = _it;

//            if(!_treeIt.is_null())
//            {
//                _values = &((*_treeIt).values);
//                _valueIt = _values->begin();
//            }
//        }

        Iterator(typename BPlusTree<MPair<K,V> >::Iterator _it)
        {
            _treeIt = _it;
        }

        Iterator operator ++(int unused)
        {
            Iterator temp = *this;
            this->operator++();
            return temp;
        }

//        Iterator operator ++()
//        {
//            //if we are at the end of the current vector
//            // advance to the next vector, else advance to the next item in the current vector.
//            if(_valueIt == _values->end())
//                _treeIt++;
//            else
//            {
//                _valueIt++;

//                //if we are now at the end of the current vector, advance to the next vector.
//                if(_valueIt == _values->end())
//                {
//                    _treeIt++;
//                    //if we are not at the end of the _treeIt, update _valueIt.
//                    if(_treeIt != nullptr)
//                    {
//                        _values = &((*_treeIt).values);
//                        _valueIt = _values->begin();
//                    }
//                }
//            }
//            return *this;
//        }

        Iterator operator ++()
        {
            _treeIt++;
            return *this;
        }

        MPair<K,V>& operator *()
        {
            return *_treeIt;
        }

        friend bool operator ==(const Iterator& lhs, const Iterator& rhs)
        {
            return(lhs._treeIt == rhs._treeIt);
        }

        friend bool operator !=(const Iterator& lhs, const Iterator& rhs)
        {
            return(lhs._treeIt != rhs._treeIt);
        }

    private:
        typename BPlusTree<MPair<K,V> >::Iterator _treeIt;
        //typename std::vector<V>::iterator _valueIt;
        //typename std::vector<V> *_values;
    };

public:
    MMap() : _mmap(true){}

    //  Capacity
    int size() const;
    bool empty() const;

    //  Element Access
    //const vector<V>& operator[](const K& key) const;
    //vector<V>& operator[](K key);

    const Set<V>& operator[](const K& key) const;
    Set<V>& operator[](K key);

    //  Modifiers
    bool insert(const K& k, const V& v);
    bool erase(const K& key);
    void clear();

    //  Operations:
    bool contains(const K& key) const;
    //vector<V> &get(const K& key);
    Set<V> &get(const K& key);

    int count(const K& key);

    bool isValid();

    friend ostream& operator<<(ostream& outs, const MMap<K, V>& print_me)
    {
        outs<<print_me._mmap<<endl;
        return outs;
    }

    Iterator begin()
    {
        return MMap<K,V>::Iterator(_mmap.begin());
    }

    Iterator end()
    {
        return MMap<K,V>::Iterator(_mmap.end());
    }

    Iterator iteratorAtKey(K key)
    {
        return MMap<K,V>::Iterator(_mmap.getIteratorAtEntry(_mmap.find(key)));
    }

private:
    BPlusTree<MPair<K,V> > _mmap;
};

//preconditions: none
//postconditions: returns the total number of keys in the MMap.
template<typename K, typename V>
int MMap<K,V>::size() const
{
    return _mmap.size();
}

//preconditions: none
//postconditions: returns true if the BTree is empty, otherwise false.
template<typename K, typename V>
bool MMap<K,V>::empty() const
{
    return (_mmap.size() == 0);
}

//preconditions: none
//postconditions: the vector of the associated key will be
// returned from the BTree, if no MPair with the recieved key
// already exists, an Mpair containing an empty vector will be inserted.
//template<typename K, typename V>
//const vector<V>& MMap<K,V>::operator[](const K &key) const
//{
//    return _mmap.get(MPair<K,V>(key)).values;
//}

//testing
template<typename K, typename V>
const Set<V>& MMap<K,V>::operator[](const K &key) const
{
    return _mmap.get(MPair<K,V>(key)).values();
}

//preconditions: none
//postconditions: the vector of the associated key will be
// returned from the BTree, if no MPair with the recieved key
// already exists, an Mpair containing an empty vector will be inserted.
//template<typename K, typename V>
//vector<V>& MMap<K,V>::operator[](K key)
//{
//    return _mmap.get(MPair<K,V>(key)).values;
//}

//testing
template<typename K, typename V>
Set<V>& MMap<K,V>::operator[](K key)
{
    return _mmap.get(MPair<K,V>(key)).values();
}

//preconditions: none
//postconditions: obtain the vector of values associated with the recieved key,
// if it already exists, otherwise it will be created now.
// Then, call push_back to insert the new value (v) to the vector.
//template<typename K, typename V>
//bool MMap<K,V>::insert(const K &k, const V &v)
//{
//    vector<V> * temp = &this->operator[](k);
//    temp->push_back(v);
//    return true;
//}

//testing
template<typename K, typename V>
bool MMap<K,V>::insert(const K &k, const V &v)
{
    Set<V> * temp = &this->operator[](k);
    temp->insert(v);
    return true;
}

//preconditions: none
//postconditions: removes the Mpair with the recieved key from the map,
// returning true if the pair was removed, otherwise false.
template<typename K, typename V>
bool MMap<K,V>::erase(const K &key)
{
    return _mmap.remove(MPair<K,V>(key));
}

//preconditions: none
//postconditions: calls clear on the BTree, erasing all items from it.
template<typename K, typename V>
void MMap<K,V>::clear()
{
    _mmap.clearTree();
}

//preconditions: none
//postconditions: returns true if the target key exists in the Map, otherwise false.
template<typename K, typename V>
bool MMap<K,V>::contains(const K& key) const
{
    return _mmap.contains(MPair<K,V>(key));
}

//template<typename K, typename V>
//vector<V>& MMap<K,V>::get(const K& key)
//{
//    return _mmap.get(MPair<K,V>(key)).values;
//}

//testing
template<typename K, typename V>
Set<V>& MMap<K,V>::get(const K& key)
{
    return _mmap.get(MPair<K,V>(key)).values();
}

//preconditions: none
//postconditions: returns the size of the vector associated with the key
template<typename K, typename V>
int MMap<K,V>::count(const K& key)
{
    return _mmap.get(MPair<K,V>(key)).values().size();
}

//preconditions: none
//postconditions: returns true if all the conditions
// required for a valid BTree are met, otherwise false.
template<typename K, typename V>
bool MMap<K,V>::isValid()
{
    return _mmap.isValid();
}

#endif // MULTIMAP_H
