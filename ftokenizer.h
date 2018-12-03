#ifndef FTOKENIZER_H
#define FTOKENIZER_H

#include "constants.h"
#include "token.h"
#include "stokenizer.h"
#include <fstream>

class FTokenizer
{
    //extract one token (very similar to the way cin >> works)
    friend FTokenizer& operator >> (FTokenizer& f, Token& t);

public:
    const size_t MAX_BLOCK = MAX_BUFFER; //set the MAX_BLOCK size to be MAX_BUFFER (defined in constants.h)
    FTokenizer(string fname); //creates this Ftokenizer and opens the file with the name recieved.
    ~FTokenizer();
    Token next_token();  //used by operator >>
    bool more();         //returns the current value of _more
    size_t pos();           //returns the value of _pos
    size_t block_pos();     //returns the value of _blockPos

private:
    bool get_new_block(); //gets the new block from the file
    std::ifstream _f;     //file being tokenized
    STokenizer _stk;      //The STokenizer object to tokenize current block
    size_t _pos;             //Current position in the file
    size_t _blockPos;        //Current position in the current block
    bool _more;           //false if last token of the last block
                          // has been processed and now we are at
                          // the end of the last block.
    char _block[MAX_BUFFER];
};
#endif // FTOKENIZER_H
