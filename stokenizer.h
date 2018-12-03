#ifndef STOKENIZER_H
#define STOKENIZER_H

#include <cstring>
#include <cassert>
#include <iostream>
#include "table_util.h"
#include "token.h"
#include "map.h"

class STokenizer
{
    //extract one token (very similar to the way cin >> works)
    friend STokenizer& operator >> (STokenizer& s, Token& t);

public:
    STokenizer();
    STokenizer(char str[]);
    bool done();            //true: there are no more tokens
    bool more();            //true: there are more tokens
    void setString(char str[]);  //set a new string as the input string

private:
    //create table for all the tokens we will recognize
    //                      (e.g. doubles, words, etc.)
    void makeTable();
    void initInterpreterMaps();

    //returns true if a token is found, otherwise false.
    bool getToken(int start_state, string& token);
    string removeQuotes(string token);

    char buffer[MAX_BUFFER];
    size_t bufferSize;
    size_t pos;                                 //current position in the string
    int _table[MAX_ROWS][MAX_COLUMNS];          //adjacency matrix
    static Map<string,typeID> _typeMap;
};

#endif //STOKENIZER_H
