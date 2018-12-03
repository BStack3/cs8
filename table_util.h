#ifndef TABLE_UTIL_H
#define TABLE_UTIL_H

#include <iostream>
#include <fstream>
#include "constants.h"
using namespace std;

//Fill all cells of the array with -1
void initTable(int _table[][MAX_COLUMNS]);

//Mark this state (row) with a 1 (success)
void markSuccess(int _table[][MAX_COLUMNS], int state);

//Mark this state (row) with a 0 (fail)
void markFail(int _table[][MAX_COLUMNS], int state);

//true if state is a success state
bool isSuccess(int _table[][MAX_COLUMNS], int state);

//Mark a range of cells in the array.
void markCells(int row, int _table[][MAX_COLUMNS], int from, int to, int state);

//Mark columns represented by the string columns[] for this row
void markCells(int row, int _table[][MAX_COLUMNS], const char columns[], int state);

//Mark columns with the column numbers in columns[]
void markCells(int row, int _table[][MAX_COLUMNS], const typeID columns[], int size, int state);

//Mark this row and column
void markCell(int row, int table[][MAX_COLUMNS], int column, int state);

//This can realistically be used on a small table
void printTable(int _table[][MAX_COLUMNS], ostream& out = cout);

//show string s and mark this position on the string:
//hello world   pos: 7
//      ^
void showString(char s[], int _pos);


#endif // TABLE_UTIL_H
