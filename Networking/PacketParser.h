#ifndef PACKETPARSER_H
#define PACKETPARSER_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "../Utils/StringOperations.h"

#include "../Database/Database.h"
#include "../Database/Table.h"
#include "../Database/DbFileManager.h"

using namespace std;

class PacketParser {
public:
    vector<string> ParseTableNamesToVec(string packetString);
    Table GetTableFromTableString(string tableStr);
    string GetStringOfNewTable(Table tab);
    string GetStringOfNewRecord(string tableName, TableRecord tabRecord);

private:
    DbFileManager dbm;
};


#endif // PACKETPARSER_H
