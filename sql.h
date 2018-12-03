#ifndef SQL_H
#define SQL_H
#include <fstream>
#include <iostream>
#include <cassert>
#include "stokenizer.h"
#include "table.h"
#include "arrayutil.h"
#include "myqueue.h"
#include "mystack.h"
#include "set.h"

using namespace std;

class SQL
{
public:
    SQL()
    {
        _exit = false;
        loadSavedTables();
        initAdjacencyMatrix();
        recieveCommands();
    }
    SQL(const char* batchFileName)
    {
        _exit = false;
        _batchInput.open(batchFileName,ios_base::binary);
        assert(_batchInput.is_open());

        //loadSavedTables();
        initAdjacencyMatrix();
        recieveCommands(_batchInput);
    }
    ~SQL()
    {
        ofstream savedTables(SAVED_TABLES,ios_base::binary);
        assert(savedTables.is_open());

        Record rec;
        Map<string,Table*>::Iterator it  = _tables.begin();
        while(it != _tables.end())
        {
            vector<string> temp;
            temp.push_back((*it).value()->getTableName());
            rec.writeToFile(savedTables,temp);
            delete (*it).value();
            ++it;
        }
        savedTables.close();
    }
private:
    bool getPTree(int startState, typeID startType);
    void executeCommand();
    void loadSavedTables();
    bool verifyFields(vector<string>& fields);
    bool verifyPTree();
    void initAdjacencyMatrix();
    void recieveCommands(istream& inStream = cin);
    bool toPostfix();
    void processSelect();

    Queue<Token> _postfix;
    Queue<Token> _infix;

    typeID _currentAction;
    ifstream _batchInput;
    bool _exit;
    Map<string,Table*> _tables;
    STokenizer _preProcessor;
    Map<int,vector<string> > _PTree;
    char _command[200];

    int _SQLAdjacencyMatrix[MAX_ROWS][MAX_COLUMNS];
    int _rowColSQLStates[MAX_ROWS][MAX_COLUMNS];
};

//preconditions: any table whose path is in SAVED_TABLES must exist.
//postconditions: any existing tables saved on disk will be loaded.
//                a table object will be constructed for each table.
void SQL::loadSavedTables()
{
    ifstream savedTables(SAVED_TABLES,ios_base::binary);

    if(savedTables.is_open())
    {
        Record paths;
        vector<string> tableInfo;
        while(paths.readFromFile(savedTables,tableInfo) > 0 && tableInfo.size() == 1)
        {
            _tables.insert(tableInfo.at(0),new Table(tableInfo.at(0)));
            tableInfo.clear();
        }
        savedTables.close();
    }
}

//preconditions: none
//postconditions: the adjacency matricies used in the SQL state machine will be initialize.
void SQL::initAdjacencyMatrix()
{
    //Select SQL Command
    initTable(_SQLAdjacencyMatrix);
    markFail(_SQLAdjacencyMatrix,SELECT_START);
    markFail(_SQLAdjacencyMatrix,SELECT_START+1);
    markFail(_SQLAdjacencyMatrix,SELECT_START+2);
    markFail(_SQLAdjacencyMatrix,SELECT_START+3);
    markFail(_SQLAdjacencyMatrix,SELECT_START+4);
    markFail(_SQLAdjacencyMatrix,SELECT_START+5);
    markSuccess(_SQLAdjacencyMatrix,SELECT_START+6);
    markCell(SELECT_START,_SQLAdjacencyMatrix,SELECT,SELECT_START+1);
    markCell(SELECT_START+1,_SQLAdjacencyMatrix,ASTERISK,SELECT_START+2);
    markCell(SELECT_START+1,_SQLAdjacencyMatrix,UNKNOWN,SELECT_START+3);
    markCell(SELECT_START+2,_SQLAdjacencyMatrix,FROM,SELECT_START+5);
    markCell(SELECT_START+3,_SQLAdjacencyMatrix,COMMA,SELECT_START+4);
    markCell(SELECT_START+3,_SQLAdjacencyMatrix,FROM,SELECT_START+5);
    markCell(SELECT_START+4,_SQLAdjacencyMatrix,UNKNOWN,SELECT_START+3);
    markCell(SELECT_START+5,_SQLAdjacencyMatrix,UNKNOWN,SELECT_START+6);

    //Select where clause
    markFail(_SQLAdjacencyMatrix,SELECT_START+7);
    markFail(_SQLAdjacencyMatrix,SELECT_START+8);
    markFail(_SQLAdjacencyMatrix,SELECT_START+9);
    markSuccess(_SQLAdjacencyMatrix,SELECT_START+10);
    markCell(SELECT_START+6,_SQLAdjacencyMatrix,WHERE,SELECT_START+7);
    markCell(SELECT_START+7,_SQLAdjacencyMatrix,UNKNOWN,SELECT_START+8);
    markCell(SELECT_START+8,_SQLAdjacencyMatrix,LESS,SELECT_START+9);
    markCell(SELECT_START+8,_SQLAdjacencyMatrix,GREATER,SELECT_START+9);
    markCell(SELECT_START+8,_SQLAdjacencyMatrix,GREATER_OR_EQUAL,SELECT_START+9);
    markCell(SELECT_START+8,_SQLAdjacencyMatrix,LESS_OR_EQUAL,SELECT_START+9);
    markCell(SELECT_START+8,_SQLAdjacencyMatrix,EQUALS,SELECT_START+9);
    markCell(SELECT_START+7,_SQLAdjacencyMatrix,OPEN_P,SELECT_START+7);
    markCell(SELECT_START+10,_SQLAdjacencyMatrix,CLOSE_P,SELECT_START+10);
    markCell(SELECT_START+9,_SQLAdjacencyMatrix,UNKNOWN,SELECT_START+10);
    markCell(SELECT_START+9,_SQLAdjacencyMatrix,DIGIT,SELECT_START+10);
    markCell(SELECT_START+9,_SQLAdjacencyMatrix,VALUE,SELECT_START+10);
    markCell(SELECT_START+10,_SQLAdjacencyMatrix,AND,SELECT_START+7);
    markCell(SELECT_START+10,_SQLAdjacencyMatrix,OR,SELECT_START+7);

    //Second 2d array will map row(state), col(type) to the SQL type.
    initTable(_rowColSQLStates);
    markCell(SELECT_START+1,_rowColSQLStates,SELECT,SELECT);
    markCell(SELECT_START+2,_rowColSQLStates,ASTERISK,ASTERISK);
    markCell(SELECT_START+3,_rowColSQLStates,UNKNOWN,FIELD_NAME);
    markCell(SELECT_START+4,_rowColSQLStates,COMMA,COMMA);
    markCell(SELECT_START+5,_rowColSQLStates,FROM,FROM);
    markCell(SELECT_START+6,_rowColSQLStates,UNKNOWN,DB_NAME);
    markCell(SELECT_START+7,_rowColSQLStates,WHERE,WHERE);
    markCell(SELECT_START+8,_rowColSQLStates,UNKNOWN,RELATIONAL_FIELD);
    markCell(SELECT_START+9,_rowColSQLStates,LESS,LESS);
    markCell(SELECT_START+9,_rowColSQLStates,GREATER,GREATER);
    markCell(SELECT_START+9,_rowColSQLStates,EQUALS,EQUALS);
    markCell(SELECT_START+9,_rowColSQLStates,GREATER_OR_EQUAL,GREATER_OR_EQUAL);
    markCell(SELECT_START+9,_rowColSQLStates,LESS_OR_EQUAL,LESS_OR_EQUAL);
    markCell(SELECT_START+10,_rowColSQLStates,UNKNOWN,VALUE);
    markCell(SELECT_START+10,_rowColSQLStates,VALUE,VALUE);
    markCell(SELECT_START+10,_rowColSQLStates,DIGIT,VALUE);
    markCell(SELECT_START+7,_rowColSQLStates,AND,AND);
    markCell(SELECT_START+7,_rowColSQLStates,OR,OR);
    markCell(SELECT_START+10,_rowColSQLStates,CLOSE_P,CLOSE_P);
    markCell(SELECT_START+7,_rowColSQLStates,OPEN_P,OPEN_P);

    //Make SQL Command
    markFail(_SQLAdjacencyMatrix,CREATE_START);
    markFail(_SQLAdjacencyMatrix,CREATE_START+1);
    markFail(_SQLAdjacencyMatrix,CREATE_START+2);
    markFail(_SQLAdjacencyMatrix,CREATE_START+3);
    markFail(_SQLAdjacencyMatrix,CREATE_START+4);
    markFail(_SQLAdjacencyMatrix,CREATE_START+6);
    markSuccess(_SQLAdjacencyMatrix,CREATE_START+5);
    markCell(CREATE_START,_SQLAdjacencyMatrix,MAKE,CREATE_START+1);
    markCell(CREATE_START+1,_SQLAdjacencyMatrix,TABLE,CREATE_START+2);
    markCell(CREATE_START+2,_SQLAdjacencyMatrix,UNKNOWN,CREATE_START+3);
    markCell(CREATE_START+3,_SQLAdjacencyMatrix,FIELDS,CREATE_START+4);
    markCell(CREATE_START+4,_SQLAdjacencyMatrix,UNKNOWN,CREATE_START+5);
    markCell(CREATE_START+5,_SQLAdjacencyMatrix,COMMA,CREATE_START+6);
    markCell(CREATE_START+6,_SQLAdjacencyMatrix,UNKNOWN,CREATE_START+5);

    //Second 2d array will map row(state), col(type) to the SQL type.
    markCell(CREATE_START+1,_rowColSQLStates,MAKE,MAKE);
    markCell(CREATE_START+2,_rowColSQLStates,TABLE,TABLE);
    markCell(CREATE_START+3,_rowColSQLStates,UNKNOWN,DB_NAME);
    markCell(CREATE_START+4,_rowColSQLStates,FIELDS,FIELDS);
    markCell(CREATE_START+5,_rowColSQLStates,UNKNOWN,FIELD_NAME);
    markCell(CREATE_START+6,_rowColSQLStates,COMMA,COMMA);

    //Insert SQL Command:
    markFail(_SQLAdjacencyMatrix,INSERT_START);
    markFail(_SQLAdjacencyMatrix,INSERT_START+1);
    markFail(_SQLAdjacencyMatrix,INSERT_START+2);
    markFail(_SQLAdjacencyMatrix,INSERT_START+3);
    markFail(_SQLAdjacencyMatrix,INSERT_START+4);
    markFail(_SQLAdjacencyMatrix,INSERT_START+6);
    markSuccess(_SQLAdjacencyMatrix,INSERT_START+5);
    markCell(INSERT_START,_SQLAdjacencyMatrix,INSERT,INSERT_START+1);
    markCell(INSERT_START+1,_SQLAdjacencyMatrix,INTO,INSERT_START+2);
    markCell(INSERT_START+2,_SQLAdjacencyMatrix,UNKNOWN,INSERT_START+3);
    markCell(INSERT_START+3,_SQLAdjacencyMatrix,VALUES,INSERT_START+4);
    markCell(INSERT_START+4,_SQLAdjacencyMatrix,UNKNOWN,INSERT_START+5);
    markCell(INSERT_START+4,_SQLAdjacencyMatrix,DIGIT,INSERT_START+5);
    markCell(INSERT_START+4,_SQLAdjacencyMatrix,VALUE,INSERT_START+5);
    markCell(INSERT_START+5,_SQLAdjacencyMatrix,COMMA,INSERT_START+6);
    markCell(INSERT_START+6,_SQLAdjacencyMatrix,UNKNOWN,INSERT_START+5);
    markCell(INSERT_START+6,_SQLAdjacencyMatrix,DIGIT,INSERT_START+5);
    markCell(INSERT_START+6,_SQLAdjacencyMatrix,VALUE,INSERT_START+5);

    //Second 2d array will map row(state), col(type) to the SQL type.
    markCell(INSERT_START+1,_rowColSQLStates,INSERT,INSERT);
    markCell(INSERT_START+2,_rowColSQLStates,INTO,INTO);
    markCell(INSERT_START+3,_rowColSQLStates,UNKNOWN,DB_NAME);
    markCell(INSERT_START+4,_rowColSQLStates,VALUES,VALUES);
    markCell(INSERT_START+5,_rowColSQLStates,UNKNOWN,VALUE);
    markCell(INSERT_START+5,_rowColSQLStates,DIGIT,VALUE);
    markCell(INSERT_START+5,_rowColSQLStates,VALUE,VALUE);
    markCell(INSERT_START+6,_rowColSQLStates,COMMA,COMMA);
}

//preconditions: isStream must be an open input stream.
// commands recieved from inStream must be deliminated by a new line character.
//postconditions: the stokenizer will be initialized to the recieved command,
// then, the command token will be obtained, (SELECT, INSERT, MAKE)
//       the command token will determine the state state in which
//       a state machine will read subsequent tokens and create the PTree.
void SQL::recieveCommands(istream& inStream)
{
    while(!_exit && !inStream.eof())
    {
        bool isSuccess = false;
        cout << endl << string(50,'-') << endl << "Enter a command " << endl;
        inStream.getline(_command,100,'\n');
        _command[inStream.gcount()] = '\0';
        //cout << _command << endl;

        if(strcmp(_command,"exit") == 0 || inStream.gcount() == 0)
            _exit = true;
        else
        {
            _preProcessor.setString(_command);
            Token commandToken;
            vector<string> commandVector;

            _preProcessor >> commandToken;
            commandVector.push_back(commandToken.token_str());
            //check each start state until a success state is found:
            //  (i.e., Select_start, Insert_start, Create_start...)
            if(commandToken.type() == SELECT)
            {
                _PTree.insert(SELECT,commandVector);
                isSuccess = getPTree(SELECT_START,SELECT);
            }
            else if(commandToken.type() == INSERT)
            {
                _PTree.insert(INSERT,commandVector);
                isSuccess = getPTree(INSERT_START,INSERT);
            }
            else if(commandToken.type() == MAKE)
            {
                _PTree.insert(MAKE,commandVector);
                isSuccess = getPTree(CREATE_START,MAKE);
            }
            else
                cout << "Error, SQL command not recognized as SELECT, INSERT, or MAKE." <<endl;

            if(isSuccess)
            {
                _currentAction = commandToken.type();
                if(verifyPTree())
                {
                    if(!_infix.empty())
                    {
                        if(!toPostfix())
                        {
                            isSuccess = false;
                            cout << "Error when converting expression to postfix: " << endl << _postfix << endl;
                        }
                    }
                    if(isSuccess)
                        executeCommand();
                }
                else
                    cout << "Error, no such table or fields exist." << endl;
            }
            else
            {
                cout << "Error, invalid SQL grammer " << endl;
                cout << endl << _PTree << endl;
            }

            _PTree.clear();
            _infix.clear();
            _currentAction = EMPTY;
            _postfix.clear();
        }
    }
}

//preconditions: none
//postconditions: a PTree will be constructed by traversing the adjacency matrix starting
// from (row, col): startState, startType. At this level, parentheses after the where clause,
// do not have any impact on the validity of an expression.
// Any tokens found after the where keyword will be pushed to
bool SQL::getPTree(int startState, typeID startType)
{
    startState = _SQLAdjacencyMatrix[startState][static_cast<int>(startType)];
    bool isSuccess = false;
    bool foundWhere = false;
    Map<int,vector<string> > Ptree = _PTree;
    Token lastNonParentheses;
    Queue<Token> tempInfixQueue;

    //pre-read while loop
    Token nextToken;
    _preProcessor >> nextToken;
    startState = _SQLAdjacencyMatrix[startState][static_cast<int>(nextToken.type())];
    while(startState != -1 && nextToken.type() != EMPTY)
    {
        int tokenType = _rowColSQLStates[startState][nextToken.type()];

        if(foundWhere)
        {
            Token t(nextToken.token_str(),static_cast<typeID>(tokenType));
            tempInfixQueue.push(t);
        }
        else
        {
            if(!Ptree.contains(tokenType))
            {
                vector<string> temp;
                temp.push_back(nextToken.token_str());
                Ptree.insert(tokenType,temp);
            }
            else
                Ptree[tokenType].push_back(nextToken.token_str());
        }

        //if a success state is found, save the current state of the PTree and Infixqueue
        if(_SQLAdjacencyMatrix[startState][0] == 1)
        {
            isSuccess = true;
            _PTree = Ptree;
            _infix = tempInfixQueue;
        }

        if(nextToken.type() == WHERE)
            foundWhere = true;

        _preProcessor >> nextToken;
        startState = _SQLAdjacencyMatrix[startState][static_cast<int>(nextToken.type())];
    }
    return isSuccess;
}

//preconditions: none
//postconditions>: if _currentAction is MAKE and a table with the name provided already exists,
//                    return false, otherwise true.
//                 if _currentAction is SELECT, and a table with the provided name does exist,
//                    and an asterisk is entered, or all fields entered exist in the table
//                    and, if a where clause was provided, all relational fields must also exist in the table
//                    return true, otherwise false.
//                 if _currentAction is INSERT, and a table with the provided name does exist,
//                    and, the number of values entered is the same as the number of fields in the target table,
//                    return true, otherwise false.
bool SQL::verifyPTree()
{
    bool isValid = true;
    cout << _PTree << endl;

    if(_currentAction == MAKE)
    {
        if(_tables.contains(_PTree[DB_NAME].at(0)))
        {
            cout << "Error, Table: " << _PTree[DB_NAME].at(0) << " already exists." << endl;
            isValid = false;
        }
        else
            isValid = true;
    }
    else if(_currentAction == SELECT)
    {
        if(_tables.contains(_PTree[DB_NAME].at(0)))
        {
            if(!_PTree.contains(ASTERISK) && !verifyFields(_PTree[FIELD_NAME]))
                isValid = false;

            if(_PTree.contains(WHERE) && !verifyFields(_PTree[RELATIONAL_FIELD]))
                isValid = false;
        }
        else
        {
            cout << "Error, cannot select from : " << _PTree[DB_NAME] << ", because no such table exists." << endl;
            isValid = false;
        }
    }
    else if(_currentAction == INSERT)
    {
        if(_tables.contains(_PTree[DB_NAME].at(0)))
        {
            if(_PTree[VALUE].size() != _tables[_PTree[DB_NAME].at(0)]->getFieldNames().size())
            {
                cout << "Error, Table: " << _PTree[DB_NAME].at(0)
                     << " contains " <<  _tables[_PTree[DB_NAME].at(0)]->getFieldNames().size()
                     << " fields, but only " << _PTree[VALUE].size()  << " values were specified." << endl;
                isValid = false;
            }
        }
        else
        {
            cout << "Error, cannot insert into : " << _PTree[DB_NAME] << ", because no such table exists." << endl;
            isValid = false;
        }
    }
    else
    {
        cout << "Error, command invalid." << endl;
        isValid = false;
    }
    return isValid;
}

//Note, table::field_names should be stored in a map.
bool SQL::verifyFields(vector<string>& fields)
{
    bool fieldsExist = true;
    //vector<string> const* fieldsToSelect = &(_PTree[FIELD_NAME]);

    for(unsigned i = 0; i < fields.size() && fieldsExist; ++i)
        if(!_tables[_PTree[DB_NAME].at(0)]->containsField(fields.at(i)))
            fieldsExist = false;
    return fieldsExist;
}

bool SQL::toPostfix()
{
    /* This implementation does not implement composite functions,functions with variable number of arguments, and unary operators. */
//    while there are tokens to be read:
//        read a token.
//        if the token is a number, then:
//            push it to the output queue.
//        if the token is a function then:
//            push it onto the operator stack
//        if the token is an operator, then:
//            while ((there is a function at the top of the operator stack)
//                   or (there is an operator at the top of the operator stack with greater or equal precedence)
//                  and (the operator at the top of the operator stack is not a left bracket):
//                pop operators from the operator stack onto the output queue.
//            push it onto the operator stack.
//        if the token is a left bracket (i.e. "("), then:
//            push it onto the operator stack.
//        if the token is a right bracket (i.e. ")"), then:
//            while the operator at the top of the operator stack is not a left bracket:
//                pop the operator from the operator stack onto the output queue.
//            pop the left bracket from the stack.
//            /* if the stack runs out without finding a left bracket, then there are mismatched parentheses. */
//    if there are no more tokens to read:
//        while there are still operator tokens on the stack:
//            /* if the operator token on the top of the stack is a bracket, then there are mismatched parentheses. */
//            pop the operator from the operator stack onto the output queue.
//    exit.

    Stack<Token> operatorStack;
    bool isValid = true;

    while(!_infix.empty())
    {
        Token current = _infix.pop();
        if(current.priority() == -1)
        {
            _postfix.push(current);
        }
        else if(current.type() != OPEN_P && current.type() != CLOSE_P)
        {
            while((!operatorStack.empty()) && (operatorStack.top().priority() >= current.priority()) && (operatorStack.top().type() != OPEN_P))
            {
                _postfix.push(operatorStack.pop());
            }
            operatorStack.push(current);
        }
        else if(current.type() == OPEN_P)
        {
            operatorStack.push(current);
        }
        else if(current.type() == CLOSE_P)
        {
            while(!operatorStack.empty() && operatorStack.top().type() != OPEN_P)
            {
                _postfix.push(operatorStack.pop());
            }
            if(operatorStack.empty())
                isValid = false;
            else
                operatorStack.pop();
        }
    }
    while(!operatorStack.empty())
    {
        Token temp = operatorStack.pop();
        if(temp.type() == OPEN_P || temp.type() == CLOSE_P)
            isValid = false;
        else
        {
            _postfix.push(temp);
            cout << _postfix << endl;
        }
    }

    return isValid;
}

//preconditions: the Ptree is valid, any required fields or tables exist.
//postconditions>: if _currentAction is MAKE, create a new table with the fields entered and insert it to _tables
//                 if _currentAction is SELECT, process the selection, additonal work is required here.
//                 if _currentAction is INSERT, insert the specified values to the matching fields of the target table.
void SQL::executeCommand()
{
    if(_currentAction == MAKE)
    {
        cout << "Inserting new table: " << *(_PTree[DB_NAME].begin()) << " with field names = " << _PTree[FIELD_NAME] << endl;
        _tables.insert(_PTree[DB_NAME].at(0),new Table(_PTree[DB_NAME].at(0), _PTree[FIELD_NAME]));
    }
    else if(_currentAction == INSERT)
    {
        cout << "Inserting: " << _PTree[VALUE] << " to " << _PTree[DB_NAME] << endl;
        _tables[_PTree[DB_NAME].at(0)]->insertInto(_PTree[VALUE]);
    }
    else if(_currentAction == SELECT)
    {
        processSelect();
    }

    //clear command after executing it.
    _PTree.clear();
    _currentAction = EMPTY;
}

//preconditions: the current action must be SELECT
//postconditions: if there are items in the _postfix queue, i.e., there was a where clause.
// the following algorithm will be used to obtain a set where all contained record numbers
// meet the conditions in the where clause.
void SQL::processSelect()
{
    //the selection contains a where clause
    if(_PTree.contains(WHERE))
    {
        //while _postFix queue is not yet empty:
        //  pop the front token:
        //  if the front token is NOT an operator
        //      push it to the operand stack.
        //  else if, the token IS a RELATIONAL_OPERATOR,
        //      a) pop 2 tokens from the operand stack.
        //         notice that the SECOND token popped will be the field_name.
        //      b) obtain a set from the (field, RELATIONAL_OPERATOR, value)
        //         and push the newly created set to setStack.
        //  else, the token IS a LOGICAL_OPERATOR
        //      a) pop 2 sets from the setStack
        //      b) obtain a new set that is either the intersection or
        //         union of the 2 sets, and push that set back to the setStack.

        //postconditions, one set will be obtained that meets all of the
        // conditions specified in the where clause.

        Stack<Token> operandStack;
        Stack<Set<int>> setStack;
        while(!_postfix.empty())
        {
            Token current = _postfix.pop();
            if(current.group() == OPERAND)
            {
                operandStack.push(current);
            }
            else if(current.group() == RELATIONAL_OP)
            {
                Token rhs = operandStack.pop();
                Token lhs = operandStack.pop();
                Set<int> result;
                setStack.push(_tables[_PTree[DB_NAME].at(0)]->createSet(lhs.token_str(),current.type(),rhs.token_str()));
            }
            else if(current.group() == LOGICAL_OP)
            {
                Set<int> result;
                Set<int> rhs = setStack.pop();
                Set<int> lhs = setStack.pop();

                //Either find union or intersection and save to result
                if(current.type() == AND)
                {
                    lhs.intersection(rhs, result);
                }
                else if(current.type() == OR)
                {
                    lhs += rhs;
                    result = lhs;
                }
                setStack.push(result);
            }
        }
        Set<int> result = setStack.pop();
        _tables[_PTree[DB_NAME].at(0)]->printSelection(_PTree[FIELD_NAME],result,_PTree.contains(ASTERISK));
    }
    else //no where clause.
    {
        _tables[_PTree[DB_NAME].at(0)]->printAll(_PTree[FIELD_NAME],_PTree.contains(ASTERISK));
    }
}
#endif //SQL_H
