#include "ftokenizer.h"

//preconditions: fname should be a valid file path
//postconditions: opens the file at the recieved path with _f.open(fName),
// sets _pos to 0, _blockPos to 0, _more to true, and sets _stk to be a STokenizer.
FTokenizer::FTokenizer(string fname)
{
    cout << "Opening file" << endl;
    _f.open(fname.c_str(),ios::binary);

    cout << "opening in ftokenizer: " << fname << endl;
    assert(_f.is_open());
    _pos = 0;
    _blockPos = 0;
    _more = true;
    _stk = STokenizer();
}

//preconditions: none
//postconditions: if the file is open, close it.
FTokenizer::~FTokenizer()
{
   if(_f.is_open())
   {
       _f.close();
   }
}

//preconditions: none
//postconditions: returns the current value of _more
bool FTokenizer::more()
{
    return _more;
}

//preconditions: none
//postconditions: returns the value of _pos
size_t FTokenizer::pos()
{
    return _pos;
}

//preconditions: none
//postconditions: returns the value of _blockPos
size_t FTokenizer::block_pos()
{
    return _blockPos;
}

//preconditions: _stk must have another token.
//postconditions: returns the next token from _stk
Token FTokenizer::next_token()
{
    Token t;
    if(_stk.more())
        _stk >> t;

    return t;
}

//preconditions: none
//postconditions: if we are not at the end of the file,
// if we are at the end of the block, get a new block,
// otherwise the next token from the current block.
FTokenizer& operator >> (FTokenizer& f, Token& t)
{
    //if we have reached the end of the current block, get a new block.
    if(!f._stk.more())
        f._more = f.get_new_block();

    //if the current block has more tokens, get the next one.
    if(f._stk.more())
        t = f.next_token();

    return f;
}

//preconditions: none
//postconditions: gets the new block from the file
bool FTokenizer::get_new_block()
{
    if(!_f.eof())
    {
        _f.read(_block,(MAX_BLOCK-1));

        size_t blockSize = _f.gcount();
        size_t initialPos = _f.tellg();

        //make sure the block ends with a space.
        if(!_f.eof())
        {
            while(_block[blockSize-1] != ' ')
            {
                initialPos--;
                blockSize--;
            }
            _f.seekg(initialPos);
        }
        _block[blockSize] = '\0';

        _stk.setString(_block); //set the block of the stokenizer to new block.
        return true;
    }
    else
    {
        return false;
    }
}
