#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>
#include "constants.h"
using namespace std;

class Token
{
    //prints the type and token to the output stream.
    friend ostream& operator <<(ostream& outs, const Token& t);

public:
    Token();                      //sets _type to 0 and token to "".
    Token(string str, typeID _type); //sets this->_type to _type and token to str
    typeID type();                   //returns the value of _type
    string type_string();         //returns the name of the the type as a string.
    string token_str();           //returns the value of token.
    void setType(typeID type) {_type = type; }

    int priority();
    groupID group() {return _group;}

private:
    string _token;
    typeID _type;
    groupID _group;
};

#endif //TOKEN_H
