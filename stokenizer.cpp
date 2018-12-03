#include "stokenizer.h"
Map<string,typeID> STokenizer::_typeMap;

//preconditions: none
//postconditions: class members will be initialized.
STokenizer::STokenizer()
{
    initInterpreterMaps();
    makeTable();
    bufferSize = 0;
    pos = 0;
}

//preconditions: none
//postconditions: class members will be initialized and
//    the buffer will be set to the recieved char arrar.
STokenizer::STokenizer(char str[])
{
    initInterpreterMaps();
    makeTable();
    setString(str);
    bufferSize = strlen(buffer);
    pos = 0;
}

//set a new string as the input string
//preconditions: the length of the new string must be less than the MAX_BUFFER
//postconditions: uses cstring::strcpy to copy the recieved string to the buffer.
void STokenizer::setString(char str[])
{
    assert(strlen(str) < MAX_BUFFER);

    strcpy(buffer,str);
    bufferSize = strlen(buffer);
    pos = 0;
}

//preconditions: buffer must be null terminated.
//postconditions: returns true if pos > the length of the buffer, otherwise false.
bool STokenizer::done()
{
    return (pos > bufferSize);
}

//preconditions: buffer must be null terminated.
//postconditions: returns true if pos <= the length of the buffer, otherwise false.
bool STokenizer::more()
{
    return (pos <= bufferSize && bufferSize > 0);
}

//extract one token (very similar to the way cin >> works)
//preconditions: none
//postconditions: return the highest priority token in the buffer starting at _pos
//  Priority: non-ascii character > digit > alpha > punctuation > space > unknown / undefined.
STokenizer& operator>> (STokenizer& s, Token& t)
{
    //get a valid 'primitive' token
    string token = "";
    typeID type;
    bool tokenFound = false;

    while(s.more() && !tokenFound)
    {
        if(s.getToken(DIGIT_START, token)) //Try to get a token of type digit
        {
            type = DIGIT;
            tokenFound = true;
        }
        else if(s.getToken(COMPARISON_START,token))
        {
            type = s._typeMap[token];
            tokenFound = true;
        }
        else if(s.getToken(NON_QUOTED_START,token))
        {
            tokenFound = true;
            if(s._typeMap.contains(token))
                type = s._typeMap[token];
            else
            {
                string tokenUpper = token;
                for (char & c: tokenUpper) c = toupper(c);

                if(s._typeMap.contains(tokenUpper))
                    type = s._typeMap[tokenUpper];
                else
                {
                    //If the token is not an sql keyword or comma or asterisk,
                    // Mark it as unknown so we know to classify it later.
                    type = UNKNOWN;
                }
            }
        }
        else if(s.getToken(QUOTED_STRICT_START,token))
        {
            //Mark it as unknown, the token could be either a value, field name or db name.
            // We will need to determine that later.
            tokenFound = true;
            type = UNKNOWN;
            token = s.removeQuotes(token);
        }
        else if(s.getToken(QUOTED_START,token))
        {
            tokenFound = true;
            type = VALUE;
            token = s.removeQuotes(token);
        }
        else
        {
            if(s.buffer[s.pos] == '(')
            {
                type = OPEN_P;
                token = "(";
                tokenFound = true;

            }
            else if(s.buffer[s.pos] == ')')
            {
                type = CLOSE_P;
                token = ")";
                tokenFound = true;
            }
            s.pos++;
        }
    }
    t = Token(token,((tokenFound) ? type : EMPTY));

    return s;
}

string STokenizer::removeQuotes(string token)
{
    assert(token.at(0) == '\"' && token.at(token.size()-1) == '\"');
    string noQuotes = token;
    noQuotes.erase(0,1);
    noQuotes.erase(noQuotes.size()-1,1);
    return noQuotes;
}

void STokenizer::initInterpreterMaps()
{
    static bool initialized = false;
    if(!initialized)
    {
        for(int i = 0; i < NUM_SQL_KEYWORDS; i++)
            _typeMap.insert(SQL_KEY_WORDS[i],SQL_KEYWORD_TYPE_ID[i]);

        for(int i = 0; i < NUM_COMPARISON_OPERATORS; i++)
            _typeMap.insert(COMPARISON_OPERATORS[i],COMPARISON_TYPE_ID[i]);
        initialized = true;
    }
}

//preconditions: none
//postconditions: if the table has not yet been initialized, it will be.
//     initialization consists of setting all elements in the array to -1,
//     marking fail / success states with 0 /1 in the first colum,
//     mark cells according to the graph / adjacency matrix.
void STokenizer::makeTable()
{
    //set all cells to -1
    initTable(_table);

    //Digits
    markFail(_table, DIGIT_START);
    markSuccess(_table, DIGIT_START+1);
    markFail(_table, DIGIT_START+2);
    markSuccess(_table, DIGIT_START+3);
    markCells(DIGIT_START, _table, DIGITS, DIGIT_START+1);      //state [0] --- DIGITS ---> [1]
    markCells(DIGIT_START, _table, '.', '.', DIGIT_START+2);    //state [0] --- '.' ------> [2]
    markCells(DIGIT_START+1, _table, DIGITS, DIGIT_START+1);    //state [1] --- DIGITS ---> [1]
    markCells(DIGIT_START+1, _table, '.', '.', DIGIT_START+2);  //state [1] --- '.' ------> [2]
    markCells(DIGIT_START+2, _table, DIGITS, DIGIT_START+3);    //state [2] --- DIGITS ---> [3]
    markCells(DIGIT_START+3, _table, DIGITS, DIGIT_START+3);    //state [3] --- DIGITS ---> [3]

    //Comparison operators:
    // This will identify <, <=, >, <=, =
    markFail(_table,COMPARISON_START);
    markSuccess(_table,COMPARISON_START+1);
    markSuccess(_table,COMPARISON_START+2);
    markCell(COMPARISON_START,_table,'<',COMPARISON_START+1);
    markCell(COMPARISON_START,_table,'>',COMPARISON_START+1);
    markCell(COMPARISON_START,_table,'=',COMPARISON_START+2);
    markCell(COMPARISON_START+1,_table,'=',COMPARISON_START+2);

    //Alpha Numeric, underscore.
    // This will identify: SQL keywords, comma, star, non-quoted values, fields, and db names.
    markFail(_table,NON_QUOTED_START);
    markSuccess(_table,NON_QUOTED_START+1);
    markSuccess(_table,NON_QUOTED_START+2);
    markCells(NON_QUOTED_START,_table,ALPHA,NON_QUOTED_START+1);
    markCells(NON_QUOTED_START,_table,DIGITS,NON_QUOTED_START+1);
    markCell(NON_QUOTED_START,_table,'_',NON_QUOTED_START+1);
    markCell(NON_QUOTED_START,_table,'*',NON_QUOTED_START+2);
    markCell(NON_QUOTED_START,_table,',',NON_QUOTED_START+2);
    markCells(NON_QUOTED_START+1,_table,ALPHA,NON_QUOTED_START+1);
    markCells(NON_QUOTED_START+1,_table,DIGITS,NON_QUOTED_START+1);
    markCell(NON_QUOTED_START+1,_table,'_',NON_QUOTED_START+1);

    //Quotes, strict (Alpha-numeric, underscore):
    // This will identify: quoted values, fields, db names.
    // Note that underscores are allowed in this case.
    // A field or db name may not contain whitespace, but a value may.
    markFail(_table,QUOTED_STRICT_START);
    markFail(_table,QUOTED_STRICT_START+1);
    markFail(_table,QUOTED_STRICT_START+2);
    markSuccess(_table,QUOTED_STRICT_START+3);
    markCell(QUOTED_STRICT_START,_table,'\"',QUOTED_STRICT_START+1);
    markCells(QUOTED_STRICT_START+1,_table,ALPHA,QUOTED_STRICT_START+2);
    markCells(QUOTED_STRICT_START+1,_table,DIGITS,QUOTED_STRICT_START+2);
    markCell(QUOTED_STRICT_START+1,_table,'_',QUOTED_STRICT_START+2);
    markCells(QUOTED_STRICT_START+2,_table,ALPHA,QUOTED_STRICT_START+2);
    markCells(QUOTED_STRICT_START+2,_table,DIGITS,QUOTED_STRICT_START+2);
    markCell(QUOTED_STRICT_START+2,_table,'_',QUOTED_STRICT_START+2);
    markCell(QUOTED_STRICT_START+2,_table,'\"',QUOTED_STRICT_START+3);

    //Quotes, (Anything):
    // This will identify: a value, since this case will be checked last.
    markFail(_table,QUOTED_START);
    markFail(_table,QUOTED_START+1);
    markFail(_table,QUOTED_START+2);
    markSuccess(_table,QUOTED_START+3);
    markCell(QUOTED_START,_table,'\"',QUOTED_START+1);
    markCells(QUOTED_START+1,_table,ALPHA,QUOTED_START+2);
    markCells(QUOTED_START+1,_table,DIGITS,QUOTED_START+2);
    markCells(QUOTED_START+1,_table,SPACES,QUOTED_START+2);
    markCells(QUOTED_START+1,_table,PUNCUATION,QUOTED_START+2);
    markCell(QUOTED_START+1,_table,'_',QUOTED_START+2);
    markCells(QUOTED_START+2,_table,ALPHA,QUOTED_START+2);
    markCells(QUOTED_START+2,_table,DIGITS,QUOTED_START+2);
    markCells(QUOTED_START+2,_table,SPACES,QUOTED_START+2);
    markCells(QUOTED_START+2,_table,PUNCUATION,QUOTED_START+2);
    markCell(QUOTED_START+2,_table,'_',QUOTED_START+2);
    markCell(QUOTED_START+2,_table,'\"',QUOTED_START+3);
}

//preconditions: _pos must be between 0 and MAX_COLUMNS
//postconditions: If a token is found, it will be returned, along with the position
//  after the last character in the token and the return value of true. Otherwise,
//  reset the position to what it was when this function was called, and return false.
bool STokenizer::getToken(int startState, string& token)
{
    //store the token and pos when a success state is found.
    token = "";
    bool isSuccess = false; //true as soon as a character in the buffer triggers a success state.
    size_t lastSuccessPos = pos;
    string lastSuccessToken = token;

    //pre-read while loop
    char nextChar= buffer[pos];
    startState = _table[startState][static_cast<int>(nextChar)];

    //true until the end of the buffer is reached or the next state is a fail state.
    while(startState != -1 && nextChar != '\0')
    {
        //add the current char to the token.
        token += nextChar;

        //if this state is a success state, save the current token and position.
        if(_table[startState][0] == 1)
        {
            lastSuccessPos = pos;
            lastSuccessToken = token;
            isSuccess = true;
        }

        //incremement pos, nextChar and startState
        pos++;
        nextChar = buffer[pos];
        startState = _table[startState][static_cast<int>(nextChar)];
    }

    //set pos to the last pos a success state was found at.
    pos = lastSuccessPos;

    //if a success state was found, increment pos so that it is at the character after the token.
    if(isSuccess)
        pos++;

    token = lastSuccessToken;
    return isSuccess;
}
