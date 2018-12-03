#ifndef MAP_H
#define MAP_H
#include <iostream>
#include "bplustree.h"
using namespace std;

template <typename K, typename V>
struct Pair
{
public:
    const K key(){return _key;}
    V& value(){return _value;}

    Pair(const K& k=K(), const V& v=V()): _key(k), _value(v) { }

    Pair(const Pair<K,V>& other)
    {
        _key = other._key;
        _value = other._value;
    }

    Pair<K,V> operator=(const Pair<K,V>& rhs)
    {
        _key = rhs._key;
        _value = rhs._value;
        return *this;
    }

    friend std::ostream& operator <<(std::ostream& outs, const Pair<K, V>& printMe)
    {
        outs << "(" <<printMe._key << ": " << printMe._value << ")";
        return outs;
    }
    friend bool operator ==(const Pair<K, V>& lhs, const Pair<K, V>& rhs)
    {
        return (lhs._key == rhs._key);
    }
    friend bool operator < (const Pair<K, V>& lhs, const Pair<K, V>& rhs)
    {
        return (lhs._key < rhs._key);
    }
    friend bool operator > (const Pair<K, V>& lhs, const Pair<K, V>& rhs)
    {
        return (lhs._key > rhs._key);
    }
    friend bool operator <= (const Pair<K, V>& lhs, const Pair<K, V>& rhs)
    {
        return (lhs._key <= rhs._key);
    }
    friend bool operator >= (const Pair<K, V>& lhs, const Pair<K, V>& rhs)
    {
        return (lhs._key >= rhs._key);
    }

private:
    K _key;
    V _value;
};

template <typename K, typename V>
class Map
{
public:
    class Iterator
    {
    public:
        friend class Map;

        Iterator(typename BPlusTree<Pair<K,V> >::Iterator _it)
        {
            _treeIt = _it;
        }

        Iterator operator ++()
        {
            if(!_treeIt.is_null())
                ++_treeIt;

            return *this;
        }

        Iterator operator ++(int unused)
        {
            Iterator temp = *this;
            this->operator++();
            return temp;
        }

        Pair<K,V>& operator *()
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
        typename BPlusTree<Pair<K,V> >::Iterator _treeIt;
    };

    Map(): _map(false) {}

//  Capacity
    int size() const;
    bool empty() const;

//  Element Access
    V& operator[](const K& key);
    V& at(const K& key);
    const V& at(const K& key) const;

//  Modifiers
    bool insert(const K& k, const V& v);
    bool erase(const K& key);
    void clear();
    V& get(const K& key);

//  Operations:
   // bool contains(const Pair<K, V>& target);
    bool contains(const K &key);

    bool isValid(){return _map.isValid();}

    friend ostream& operator<<(ostream& outs, const Map<K, V>& printMe)
    {
        outs<<printMe._map<<endl;
        return outs;
    }

    Iterator begin(){return Map<K,V>::Iterator(_map.begin());}
    Iterator end(){return Map<K,V>::Iterator(_map.end());}

private:
    BPlusTree<Pair<K, V> > _map;
};

//preconditions: none
//postconditions: return the size of the BTree (i.e., the map)
template <typename K, typename V>
int Map<K,V>::size() const
{
    return _map.size();
}

//preconditions: none
//postconditions: if the BTree is empty, return true, otherwise false.
template <typename K, typename V>
bool Map<K,V>::empty() const
{
    return (_map.size() == 0);
}

//preconditions: none
//postconditions: returns the value of the pair with the recieved key.
// if no such pair already exists a pair with a default constructed value
// will be inserted, and a reference to it will be returned.
template<typename K, typename V>
V& Map<K,V>::operator[](const K &key)
{
    return _map.get(Pair<K,V>(key)).value();
}

//preconditions: none
//postconditions: returns the value of the pair with the recieved key.
// if no such pair already exists a pair with a default constructed value
// will be inserted, and a reference to it will be returned.
template<typename K, typename V>
V& Map<K,V>::at(const K& key)
{
    return _map.get(Pair<K,V>(key,V())).value();
}

//preconditions: none
//postconditions: returns the value of the pair with the recieved key.
// if no such pair already exists a pair with a default constructed value
// will be inserted, and a reference to it will be returned.
template<typename K, typename V>
const V& Map<K,V>::at(const K& key) const
{
    return _map.get(Pair<K,V>(key,V())).value();
}

//preconditions: none
//postconditions: if duplicates are allowed in the BTree, or the value
// associated with the recieved key is still default constructed
// (indicating that get could not find an existing pair, so it inserted a new one)
// then reassign the value the recieved value (v), and return true.
// otherwise, return false.
template<typename K, typename V>
bool Map<K,V>::insert(const K &k, const V &v)
{
    //V *temp = &this->operator[](k);
    return _map.insert(Pair<K,V>(k,v));

//    if(_map.areDupsOk() || *temp == V()) //need a different way to check if a default valued pair was inserted.
//    {                                    // since == is not defined for maps / mmaps, this means that map<int, map<int,int> > nested maps impossible to create.
//        *temp = v;
//        return true;
//    }
//    else
//        return false;
}

//preconditions: none
//postconditions: removes the pair with the recieved key from the map,
// returning true if the pair was removed, otherwise false.
template<typename K, typename V>
bool Map<K,V>::erase(const K &key)
{
    return _map.remove(Pair<K,V>(key,V()));
}

//preconditions: none
//postconditions: calls clear on the BTree, erasing all items from it.
template<typename K, typename V>
void Map<K,V>::clear()
{
    _map.clearTree();
}

//preconditions: none
//postconditions: returns the value of the pair with the recieved key.
// if no such pair already exists a pair with a default constructed value
// will be inserted, and a reference to it will be returned.
template<typename K, typename V>
V& Map<K,V>::get(const K &key)
{
    return _map.get(Pair<K,V>(key,V())).value();
}

//preconditions: none
//postconditions: returns true if the target exists in the Map, otherwise false.
//template<typename K, typename V>
//bool Map<K,V>::contains(const Pair<K, V> &target)
//{
//    return _map.contains(target);
//}

template<typename K, typename V>
bool Map<K,V>::contains(const K &key)
{
    return _map.contains(Pair<K,V>(key));
}
#endif // MAP_H
