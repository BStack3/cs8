#ifndef RECORD_H
#define RECORD_H

#include "constants.h"
#include <string>
#include <vector>
#include <fstream>
#include "set.h"
class Record
{
public:
    Record()
    {
        _charsPerRecord = MAX_CHARS * MAX_VALUES;
    }

    void initBuffer()
    {
        for(int i = 0; i < MAX_VALUES; i++)
            for(int j = 0; j < MAX_CHARS; j++)
                _record[i][j] = '\0';
    }

    //return pos in file.
    int writeToFile(ofstream& fouts, const vector<string>& theFields)
    {
        initBuffer();
        assert(fouts.is_open());
        assert(theFields.size() < MAX_VALUES);
        for(unsigned i = 0; i < theFields.size(); i++)
            strncpy(_record[i],theFields.at(i).c_str(),MAX_CHARS);
        fouts.write(&_record[0][0],_charsPerRecord);

        return (fouts.tellp() / _charsPerRecord);
    }

    //return gcount.
    int readFromFile(ifstream& fins, vector<string>& theFields, int recordNum = -1)
    {
        initBuffer();
        assert(fins.is_open());

        //if recordNum < 0, continue reading from the last position saved internally by the ifstream.
        if(recordNum >= 0)
            fins.seekg((recordNum * _charsPerRecord), ios_base::beg);

        if(fins.read(&_record[0][0], _charsPerRecord))
        {
            for(int i = 0; i < MAX_VALUES; i++)
            {
                string item(_record[i]);
                if(item.size() > 0) theFields.push_back(item);
            }
        }
        return fins.gcount();
    }

private:
    char _record[MAX_CHARS][MAX_VALUES];
    int _charsPerRecord;
};

#endif // RECORD_H
