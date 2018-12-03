#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <iostream>
using namespace std;

const int MAX_VALUES = 10; // max values per record
const int MAX_CHARS = 10;  // max chars per value

//this is where the paths of tables created in previous sessions will be stored.
const char SAVED_TABLES[] = "SQL_Tables.txt";

// col, row, buffer
const int MAX_COLUMNS = 256;
const int MAX_ROWS = 50;
const int MAX_BUFFER = 500;

//alpha, digits, operators, spaces
const char DIGITS[] = "0123456789";
const char ALPHA[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char SPACES[] = {' ','\f','\n','\r','\t','v'};
const char PUNCUATION[] = {'.',',','?','!',':',';'};

//Lower level state machine (char by char)
const int COMPARISON_START = 0;
const int NON_QUOTED_START = 10;
const int QUOTED_STRICT_START = 20;
const int QUOTED_START = 30;
const int DIGIT_START = 40;

//Higher level state machine (token by token)
const int SELECT_START = 0;
const int CREATE_START = 20;
const int INSERT_START = 30;

enum typeID{EMPTY,SELECT,CREATE,INSERT,FROM,WHERE,AND,OR,ASTERISK,COMMA,EQUALS,GREATER,LESS,GREATER_OR_EQUAL,LESS_OR_EQUAL,VALUE,FIELD_NAME,RELATIONAL_FIELD,DB_NAME,UNKNOWN,DIGIT,MAKE,TABLE,FIELDS,INTO,VALUES,OPEN_P,CLOSE_P};
enum groupID{LOGICAL_OP,RELATIONAL_OP,PARENTHESES,OPERAND,OTHER};

//this is for debugging.
const int TOTAL_TYPES = 28;
const string THE_TYPE_STRINGS[TOTAL_TYPES] = {"EMPTY","SELECT","CREATE","INSERT","FROM","WHERE","AND","OR","ASTERISK","COMMA","EQUALS","GREATER","LESS"
                             ,"GREATER_OR_EQUAL","LESS_OR_EQUAL","VALUE","FIELD_NAME","RELATIONAL_FIELD","DB_NAME","UNKNOWN","DIGIT","MAKE","TABLE","FIELDS","INTO","VALUES","(",")"};

const int NUM_COMPARISON_OPERATORS = 5;
const typeID COMPARISON_TYPE_ID[NUM_COMPARISON_OPERATORS] = {EQUALS,GREATER,LESS,GREATER_OR_EQUAL,LESS_OR_EQUAL};
const string COMPARISON_OPERATORS[NUM_COMPARISON_OPERATORS] = {"=",">","<",">=","<="};

const int NUM_SQL_KEYWORDS = 14;
const string SQL_KEY_WORDS[NUM_SQL_KEYWORDS] = {"SELECT","CREATE","FROM","WHERE","AND","OR",",","*","MAKE","TABLE","FIELDS","INTO","VALUES","INSERT"};
const typeID SQL_KEYWORD_TYPE_ID[NUM_SQL_KEYWORDS] = {SELECT,CREATE,FROM,WHERE,AND,OR,COMMA,ASTERISK,MAKE,TABLE,FIELDS,INTO,VALUES,INSERT};



//<CREATE> : {  create <TABLE_NAME> fields <FIELD_NAME> [, <FIELD_NAME>...]  }
//<INSERT> : { insert <INTO> <TABLE_NAME> values <VALUE> [, <VALUE>...]      }
//<SELECT> : {  select <* | FIELD_NAME> [, ,FIELD_NAME>...]
//					from <TABLE_NAME>
//					where <FIELD_NAME> <RELATIONAL_OPERATOR> <VALUE>
//						[<LOGICAL_OPERATOR>
//							<FIELD_NAME> <RELATIONAL_OPERATOR> <VALUE>...]
//			}

//<VALUE>  : A string of alphanumeric characters, or a string of alphanumeric
// 			characters and spaces enclosed by double quotation marks:
// 			"Jean Luise", Finch, 1923
//<RELATIONAL OPERATOR> : [ = | > | < | >= | <= ]
//<LOGICAL OPERATOR>    : [and | or]

#endif // CONSTANTS_H
