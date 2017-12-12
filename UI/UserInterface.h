#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#define TABLE_LEFT_TOP_ASCII_CHAR 218
#define TABLE_RIGHT_TOP_ASCII_CHAR 191

#define TABLE_LEFT_BOTTOM_ASCII_CHAR 192
#define TABLE_RIGHT_BOTTOM_ASCII_CHAR 217

#define TABLE_VERTICAL_LINE_ASCII_CHAR 179
#define TABLE_HORIZONTAL_LINE_ASCII_CHAR 196

#define TABLE_VERTICAL_RIGHT_CONNECTION_ASCII_CHAR 195
#define TABLE_VERTICAL_LEFT_CONNECTION_ASCII_CHAR 180

#define TABLE_HORIZONTAL_BOTTOM_CONNECTION_ASCII_CHAR 194
#define TABLE_HORIZONTAL_TOP_CONNECTION_ASCII_CHAR 193

#define TABLE_CROSS_CONNECTION_ASCII_CHAR 197

#define TABLE_EMPTY_ASCII_CHAR 32

#include <iostream>
#include <sstream>
#include <cstdio>
#include <limits>
#include "../Database/Table.h"
#include "../Database/Database.h"
#include "ConsoleManager.h"
#include "Menu.h"

#include "../Utils/StringOperations.h"

using namespace std;

class UserInterface {

private:
    ConsoleManager console;
    int activeMenuField;
    int currentMenuSize;
    int getSizeOfLongestLabel(MenuNode *menu);
    int getSizeOfLongestChoosLabel(string arr[], int n);

    bool checkValueTypeValid(string val, string type);
    void fillArrayOfMaxStructColsSize(TableStruct tStruct, int *arr);
    void fillArrayOfMaxRecordColsSize(Table tab, int *arr);

public:
    UserInterface();
    void printMenu(MenuNode *menu, bool clientMode);
    void reset();
    void scrollMenuDown();
    void scrollMenuUp();
    MenuNode *getSelectedMenuNode(MenuNode *menu);

    void printTableStruct(TableStruct tStruct, COORD startCoords);
    void printTableRecords(Table tab);
    void printAllTablesStruct(Database db);
    void printProgramAboutInformation();

    int choosMenu(string label, string arr[], int n);
    bool createNewTab(string &label, TableStruct *tabStruct);
    bool createNewRecord(Table tab, TableRecord *tabRecord);
    bool printSortedRecordsFormTable(Table tab);
    int getRecordToRemove(Table tab);
};

#endif // USERINTERFACE_H
