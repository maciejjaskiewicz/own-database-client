#include "PacketParser.h"

vector<string> PacketParser::ParseTableNamesToVec(string packetString) {
    STR_OPERATIONS::removeFirstAndLastChar(packetString);
    return STR_OPERATIONS::split(packetString, ',');
}

Table PacketParser::GetTableFromTableString(string tableStr) {

    vector<string> segList, structSeg, tabColumns;
    string segment, tableLabel;
    TableStruct tabStruct;

    segList = STR_OPERATIONS::split(tableStr, ' ');

    segment = segList[0];
    STR_OPERATIONS::removeFirstAndLastChar(segment);
    tableLabel = segment.substr(0, segment.find('{'));
    segment.erase(0, segment.find('{'));

    tabStruct = dbm.getTableStructFromStr(segment);

    Table tab(tableLabel, tabStruct);

    if(segList.size() == 2) {
        segment = segList[1];
        STR_OPERATIONS::removeFirstAndLastChar(segment);

        dbm.fillTableByRecords(segment, &tab);
    }

    return tab;
}

string PacketParser::GetStringOfNewTable(Table tab) {
    return dbm.getStructString(tab);
}

string PacketParser::GetStringOfNewRecord(string tableName, TableRecord tabRecord) {
    stringstream recordStr;
    recordStr<<"{";
    recordStr<<tableName<<",";

    for(int i=0; i<tabRecord.fields.size(); i++) {
        recordStr<<tabRecord.fields[i];
        if(i != tabRecord.fields.size()-1) recordStr<<",";
    }

    recordStr<<"}";

    return recordStr.str();
}
