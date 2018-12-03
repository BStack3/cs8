#include "table_util.h"

//preconditions: none
//postconditions: all elements will be initialized to -1
void initTable(int _table[][MAX_COLUMNS])
{
    for(int i = 0; i < MAX_ROWS; i++)
        for(int j = 0; j < MAX_COLUMNS; j++)
            _table[i][j] = -1;
}

//preconditions: none
//postconditions: set the value of the cell at row = state, col = 0 to 1
void markSuccess(int _table[][MAX_COLUMNS], int state)
{
    _table[state][0] = 1;
}

//preconditions: none
//postconditions: set the value of the cell at row = state, col = 0 to 0
void markFail(int _table[][MAX_COLUMNS], int state)
{
    _table[state][0] = 0;
}

//preconditions: none
//postconditions: set the value of the cell at row = state, col = 0 to 1
bool isSuccess(int _table[][MAX_COLUMNS], int state)
{
    return (_table[state][0] == 1);
}

//Mark a range of cells in the array.
//preconditions: row must be a defined row index in table,
//  and the range : (from, to) must be a defined set of column indices.
//postconditions: set all elements in the recieved row, and column(s) to state.
void markCells(int row, int _table[][MAX_COLUMNS], int from, int to, int state)
{
    //currently includes both endpoints.
    for(int i = from; i <= to; i++)
    {
        _table[row][i] = state;
    }
}

//Mark columns represented by the string columns[] for this row
//preconditions: row must be a defined row index in table,
//  and the range : (columns[]) must be a defined set of column indices.
//postconditions: set all elements in the recieved row, and column(s) to state.
void markCells(int row, int _table[][MAX_COLUMNS], const char columns[], int state)
{
    const char* current = columns;

    //check if we are at the end of the null terminated char array
    while(*current != '\0')
    {
        //since the ascii values corespond to the columns,
        //  cast the current character to its ascii representation.
        _table[row][static_cast<int>((*current))] = state;
        current++; //use pointer arithmetic to move to the next index.
    }
}

//Mark columns represented the array of column numbers.
void markCells(int row, int _table[][MAX_COLUMNS], const typeID columns[], int size, int state)
{
    for(int i = 0; i < size; ++i)
        _table[row][columns[i]] = state;
}

//Mark this row and column
//void mark_cell(int row, int table[][MAX_COLUMNS], int column, int state);
void markCell(int row, int table[][MAX_COLUMNS], int column, int state)
{
    table[row][column] = state;
}

//This can realistically be used on a small table
//preconditions: out must be initialized and ready to recieve output.
//postconditions: the _table will be displayed to the output stream (cout by default)
void printTable(int _table[][MAX_COLUMNS], ostream& out)
{
    for(int i = 0; i < MAX_ROWS; i++)
    {
        for(int j = 1; j < MAX_COLUMNS; j++)
        {
            out << _table[i][j] << " ";
        }
        out << endl;
    }
}

//preconditions: none
//postconditions: the string and pos will be output as such:
//hello world   pos: 7
//      ^
void showString(char s[], int _pos)
{
    string spaces(_pos,' ');
    cout << s << "   pos: " << _pos << endl
         << spaces << "^" << endl;
}
