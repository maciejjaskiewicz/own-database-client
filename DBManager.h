#ifndef DBMANAGER
#define DBMANAGER

#include <iostream>
#include <conio.h>

#include "UI/UserInterface.h"
#include "UI/ConsoleManager.h"
#include "UI/Menu.h"

#include "Database/Database.h"
#include "Database/DbFileManager.h"

#include "Networking/Client.h"

using namespace std;

class DBManager {
public:
    DBManager();
    void Init();
    void programLoop();

private:
    static void printContentOfTableCallback();
    static void addRecordToTableCallback();
    static void deleteRecordFromTableCallback();
    static void printAllTableCallback();
    static void addTableCallback();
    static void aboutProgramCallback();
    static void deleteTableCallback();
    static void serverOptionCallback();
    static void exitProgramCallback();
    static void goBackToParentNode();

    void waitToEscape();

private:
    ConsoleManager console;
    UserInterface userInterface;
    MenuNode *currentMenu;
    Database db;
    DbFileManager dbFileManager;
    Client client;

    bool serverMode = false;
};

static DBManager *dbmPtr;

#endif // DBMANAGER
