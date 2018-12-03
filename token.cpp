#include "token.h"

//preconditions: none
//postcondition: members are initialized.
Token::Token()
{
    _token = "";
    _type = EMPTY;
}

//preconditions: none
//postcondition: members are initialized to recieved values.
Token::Token(string str, typeID type)
{
    _token = str;
    _type = type;

    if(type == LESS || type == GREATER || type == GREATER_OR_EQUAL || type == LESS_OR_EQUAL || type == EQUALS)
        _group = RELATIONAL_OP;
    else if(type == AND || type == OR)
        _group = LOGICAL_OP;
    else if(type == OPEN_P || type == CLOSE_P)
        _group = PARENTHESES;
    else if(type == VALUE || type == FIELD_NAME || type == RELATIONAL_FIELD)
        _group = OPERAND;
    else
        _group = OTHER;
}

//preconditions: none
//postcondition: outputs the type and token string to the recieved output stream.
ostream& operator <<(ostream& outs, const Token& t)
{
    outs << "[Type: \"" << t._type << "\" , Token: \"" << t._token << "\"]";
    return outs;
}

//preconditions: none
//postcondition: returns _type as an integer
typeID Token::type()
{
    return _type;
}
int Token::priority()
{
    switch (_type)
    {
    case OPEN_P:
    {
        return 5;
    }
    case CLOSE_P:
    {
        return 4;
    }
    case AND:
    {
        return 1;
    }
    case OR:
    {
        return 1;
    }
    case LESS:
    {
        return 2;
    }
    case GREATER:
    {
        return 2;
    }
    case GREATER_OR_EQUAL:
    {
        return 2;
    }
    case LESS_OR_EQUAL:
    {
        return 2;
    }
    case EQUALS:
    {
        return 2;
    }
    default:
    {
        return -1;
    }
    }
}

//preconditions: none
//postcondition: returns the name of the token type as a string.
//string Token::type_string()
//{
//    //be sure that _type is in array range
//    if(_type >= DIGIT_TYPE && _type <= NON_ASCII)
//    {
//        return TOKEN_NAMES[_type];
//    }
//}

//preconditions: none
//postcondition: returns the value of the token as a string.
string Token::token_str()
{
    return _token;
}
