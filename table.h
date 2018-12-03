#ifndef TABLE_H
#define TABLE_H

#include <cassert>
#include <fstream>
#include <iostream>
#include "multimap.h"
#include "map.h"
#include "set.h"
#include "record.h"
#include "vartype.h"
using namespace std;

class Table
{
public:
    ~Table();
    Table(string tableName);
    Table(string tableName, vector<string> fieldNames);
    int insertInto(vector<string>& values);
    //void selectFields(vector<string>& fields, Table &t);
    const vector<string>& getFieldNames() {return _fieldNames;}
    const string getTableName(){return _tableName;}
    bool containsField(const string & fieldName) {return _fieldNameToIndex.contains(fieldName);}
    Set<int> createSet(string fieldName, typeID opType, string value);
    void printSelection(vector<string>& fields, Set<int>& selection, bool allFields = false);
    void printAll(vector<string>& fields, bool allFields = false);

private:
    Record _rec;
    ofstream _fileOuts;
    ifstream _fileIns;
    vector<string> _fieldNames;
    string _tableName;
    string _fileName;
    size_t _recordCount;
    Map<int, MMap<varType, int> > _indiciesMap; //testing
    //Map<int, MMap<string, int> > _indiciesMap;

    Map<string,int> _fieldNameToIndex;

    void updateIndiciesMaps(vector<string>& values);
    void populateIndiciesMaps();

};

Table::Table(string tableName)
{
    _recordCount = 0;
    string filePath = tableName + ".txt";
    ifstream fileIn(filePath.c_str(), ios_base::binary);
    assert(fileIn.is_open());
    _tableName = tableName;
    _fileName = filePath;

    //get the field names from the first record.
    _rec.readFromFile(fileIn,_fieldNames);

    //create the field name indicies map to save the
    // relative positions of fields in the file.
    int index = 0;
    for(string s : _fieldNames)
    {
        _fieldNameToIndex.insert(s,index);
        index++;
    }
    fileIn.close();
    populateIndiciesMaps();

    //open the output stream to allow additional items to be output.
    //note, ios_base:app will open the file at the end (app = append)
//    _fileOuts.open(filePath.c_str(),(ios_base::binary | ios_base::app));
//    assert(_fileOuts.is_open());
}

Table::Table(string tableName, vector<string> fieldNames)
{
    string filePath = tableName + ".txt";
    _fieldNames = fieldNames;
    _tableName = tableName;
    _fileName = filePath;

    int index = 0;
    for(string s : _fieldNames)
    {
        _fieldNameToIndex.insert(s,index);
        index++;
    }

    //create a new file with tableName and write the fieldNames to the first record.

    _fileOuts.open(filePath.c_str(),ios_base::binary);
    assert(_fileOuts.is_open());
    _rec.writeToFile(_fileOuts, fieldNames);
    _fileOuts.close();
}

Table::~Table()
{
    //clean up, maybe save indicie maps?
}

void Table::populateIndiciesMaps()
{
    _fileIns.open(_fileName,ios_base::binary);
    _fileIns.seekg(_fileIns.beg + (MAX_VALUES * MAX_CHARS));

    vector<string> recordValues;
    while(_rec.readFromFile(_fileIns,recordValues))
    {
        for(unsigned i = 0; i < recordValues.size(); i++)
        {
            //testing
            varType temp(recordValues.at(i));
            _indiciesMap[_fieldNameToIndex[_fieldNames.at(i)]].insert(temp,_recordCount+1);

//           _indiciesMap[_fieldNameToIndex[_fieldNames.at(i)]].insert(recordValues.at(i),_recordCount+1);
        }
        recordValues.clear();
        _recordCount++;
    }
//    for(int i = 0 ; i < _indiciesMap.size(); i++)
//        cout <<"[" << i << "] : " << (_indiciesMap[i]) << endl;
//    cout << "RecordCount: " << _recordCount << endl;
    _fileIns.close();
}

void Table::updateIndiciesMaps(vector<string>& values)
{
    for(int i = 0; i < values.size(); ++i)
        _indiciesMap[i][values.at(i)].insert(_recordCount);
}

Set<int> Table::createSet(string fieldName, typeID opType, string value)
{
    Set<int> result;
    MMap<varType,int>::Iterator beginIt = _indiciesMap[_fieldNameToIndex[fieldName]].begin();
    MMap<varType,int>::Iterator endIt = _indiciesMap[_fieldNameToIndex[fieldName]].end();

//    MMap<string,int>::Iterator beginIt = _indiciesMap[_fieldNameToIndex[fieldName]].begin();
//    MMap<string,int>::Iterator endIt = _indiciesMap[_fieldNameToIndex[fieldName]].end();
    switch (opType)
    {
    case LESS:
    {
        while(beginIt != endIt && ((*beginIt).key() < value))
        {
            result += (*beginIt).values();
            ++beginIt;
        }
        break;
    }
    case GREATER:
    {
        while(beginIt != endIt && ((*beginIt).key() <= value))
            ++beginIt;

        while(beginIt != endIt)
        {
            result += (*beginIt).values();
            ++beginIt;
        }
        break;
    }
    case EQUALS:
    {
        if(_indiciesMap[_fieldNameToIndex[fieldName]].contains(value))
            result = _indiciesMap[_fieldNameToIndex[fieldName]][value];
        break;
    }
    case LESS_OR_EQUAL:
    {
        while(beginIt != endIt && ((*beginIt).key() <= value))
        {
            result += (*beginIt).values();
            ++beginIt;
        }
        break;
    }
    case GREATER_OR_EQUAL:
    {
        while(beginIt != endIt && ((*beginIt).key() < value))
            ++beginIt;

        while(beginIt != endIt)
        {
            result += (*beginIt).values();
            ++beginIt;
        }
        break;
    }
    default:
    {
        cout << "Error, operation not defined." << endl;
        break;
    }
    }
    return result;
}

void Table::printAll(vector<string>& fields, bool allFields)
{
    vector<string> fieldsToPrint = (allFields) ? _fieldNames : fields;
    _fileIns.open(_fileName,ios_base::binary);
    _fileIns.seekg(_fileIns.beg + (MAX_VALUES * MAX_CHARS));

    for(const string &s : fieldsToPrint)
        cout << s << ", ";
    cout << endl << string(50,'-') << endl;

    vector<string> fromFile;
    while(_rec.readFromFile(_fileIns,fromFile))
    {
        for(const string &s : fieldsToPrint)
        {
            int index = _fieldNameToIndex[s];
            cout << fromFile.at(index) << ", ";
        }
        cout << endl;
        fromFile.clear();
    }
    _fileIns.close();
}

//preconditions: selection is a valid set of positions in the file
//postconditions: if allFields is true, all fields of the records specified in selection
//  will be printed to the console, otherwise, for each record in selection,
//  the fields specified in the vector: fields will be printed.
void Table::printSelection(vector<string>& fields, Set<int>& selection, bool allFields)
{
    vector<string> fieldsToPrint = (allFields) ? _fieldNames : fields;
    _fileIns.open(_fileName,ios_base::binary);
    Set<int>::Iterator it = selection.begin();
    Set<int>::Iterator end = selection.end();

    for(string &s : fieldsToPrint)
        cout << s << ", ";
    cout << endl << string(50,'-') << endl;

    while(it != end)
    {
        vector<string> fromFile;
        _rec.readFromFile(_fileIns,fromFile,*it);

        for(string &s : fieldsToPrint)
        {
            int index = _fieldNameToIndex[s];
            cout << fromFile.at(index) << ", ";
        }
        cout << endl;
        ++it;
    }
    _fileIns.close();
}

//void Table::selectFields(vector<string>& fieldsToSelect, Table &t)
//{
//    for(string field : fieldsToSelect)
//    {
//        t._fieldNameToIndex.insert(field,t._fieldNames.size());
//        t._fieldNames.push_back(field);
//        t._indiciesMap[t._fieldNameToIndex[field]] = this->_indiciesMap[this->_fieldNameToIndex[field]];
//    }
//}

int Table::insertInto(vector<string>& values)
{ 
    _fileOuts.open(_fileName.c_str(),ios_base::binary | ios_base::app);
    _recordCount = _rec.writeToFile(_fileOuts, values);
    updateIndiciesMaps(values);
    _fileOuts.close();
    return _recordCount;
}

#endif // TABLE_H
