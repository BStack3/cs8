#ifndef VARTYPE_H
#define VARTYPE_H

#include <ostream>
#include <cstdlib>
#include <string>
#include <iostream>
using namespace std;

struct varType
{
    enum id{STRING,DOUBLE} _type;
    union
    {
        string _s;
        double _d;
    };
    varType() : _type(DOUBLE), _d(0)
    {
    }
    varType(string sData) : _type(STRING), _s(sData) {}
    varType(double dData) : _type(DOUBLE), _d(dData) {}

    ~varType()
    {
        if(_type == STRING)
            _s.~basic_string();
    }

    varType(const varType& other)
    {
        _type = other._type;
        if(_type == STRING)
            new (&this->_s) auto(other._s);
        else
            this->_d = other._d;
    }

    varType& operator=(const varType& rhs)
    {
        _type = rhs._type;
        if(_type == DOUBLE)
            _d = rhs._d;
        else
            new (&this->_s) auto(rhs._s);

        return *this;
    }

    friend ostream& operator<<(ostream& outs, const varType& data)
    {
        switch(data._type)
        {
            case STRING:
            {
                outs << data._s;
                break;
            }
            case DOUBLE:
            {
                outs << data._d;
                break;
            }
        }
        return outs;
    }
    friend bool operator<(const varType& lhs, const varType& rhs)
    {
        bool result = false;
        if((lhs._type == rhs._type))
        {
            if(lhs._type == DOUBLE)
                result = (lhs._d < rhs._d);
            else
                result = (lhs._s < rhs._s);
        }
        return result;
    }
    friend bool operator>(const varType& lhs, const varType& rhs)
    {
        bool result = false;
        if((lhs._type == rhs._type))
        {
            if(lhs._type == DOUBLE)
                result = (lhs._d > rhs._d);
            else
                result = (lhs._s > rhs._s);
        }
        return result;
    }
    friend bool operator==(const varType& lhs, const varType& rhs)
    {
        bool result = false;
        if((lhs._type == rhs._type))
        {
            if(lhs._type == DOUBLE)
                result = (lhs._d == rhs._d);
            else
                result = (lhs._s == rhs._s);
        }
        return result;
    }
    friend bool operator!=(const varType& lhs, const varType& rhs)
    {
        bool result = false;
        if((lhs._type == rhs._type))
        {
            if(lhs._type == DOUBLE)
                result = (lhs._d != rhs._d);
            else
                result = (lhs._s != rhs._s);
        }
        return result;
    }
    friend bool operator>=(const varType& lhs, const varType& rhs)
    {
        bool result = false;
        if((lhs._type == rhs._type))
        {
            if(lhs._type == DOUBLE)
                result = (lhs._d >= rhs._d);
            else
                result = (lhs._s >= rhs._s);
        }
        return result;
    }
    friend bool operator<=(const varType& lhs, const varType& rhs)
    {
        bool result = false;
        if((lhs._type == rhs._type))
        {
            if(lhs._type == DOUBLE)
                result = (lhs._d <= rhs._d);
            else
                result = (lhs._s <= rhs._s);
        }
        return result;
    }
};

#endif // VARTYPE_H
