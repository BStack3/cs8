#ifndef SELECTION_H
#define SELECTION_H
#include "set.h"
#include "myqueue.h"
#include "token.h"

class Selection
{
public:
    Selection();
    ~Selection();

private:
    Set<int> _recordNumbers;
    Queue<Token> _postfixExpression;
    string _tableName;
};

#endif // SELECTION_H
