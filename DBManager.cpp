#include "DBManager.h"

DBManager::DBManager() {
    cout<<"Loading...";
    dbmPtr = this;
}

void DBManager::Init() {
    MenuNode menu("Menu", NULL);
    MenuNode table("Tabele", NULL, &menu);

    MenuNode printTable(console.PL("Wyœwietl zawartoœæ tabeli"), printContentOfTableCallback);
    MenuNode addRecordToTable("Dodaj rekord do tabeli", addRecordToTableCallback);
    MenuNode deleteRecordFromTable(console.PL("Usuñ rekord z tabeli"), deleteRecordFromTableCallback);
    MenuNode returnToParent(console.PL("Wróæ"), goBackToParentNode);

    table.add(&printTable); table.add(&addRecordToTable);
    table.add(&deleteRecordFromTable); table.add(&returnToParent);

    MenuNode printAllTables(console.PL("Wyœwietl strukturê wszystkich tabel"), printAllTableCallback);
    MenuNode addTable("Dodaj tabele", addTableCallback);
    MenuNode deleteTable(console.PL("Usuñ tabele"), deleteTableCallback);
    MenuNode serverOption(console.PL("Opcje serwera"), serverOptionCallback);
    MenuNode aboutProgram("O programie", aboutProgramCallback);
    MenuNode exitProgram(console.PL("Zamknij program"), NULL, &menu, console.PL("Czy na pewno chcesz zakoñczyæ program"));

    MenuNode exitProgramYes("Tak", exitProgramCallback);
    MenuNode exitProgramNo("Nie", goBackToParentNode);

    exitProgram.add(&exitProgramYes); exitProgram.add(&exitProgramNo);

    menu.add(&table); menu.add(&printAllTables);
    menu.add(&addTable); menu.add(&deleteTable);
    menu.add(&serverOption); menu.add(&aboutProgram);
    menu.add(&exitProgram);

    currentMenu = &menu;
    //db.createExampleTables();
    dbFileManager.loadDbFromFiles(&db);
    programLoop();
}

void DBManager::programLoop() {
    while(true) {
        console.setCursor(0, 0);
        userInterface.printMenu(currentMenu, client.IsConnectedToServer());

        if(GetAsyncKeyState(VK_DOWN)) {
            userInterface.scrollMenuDown();
        }

        if(GetAsyncKeyState(VK_UP)) {
            userInterface.scrollMenuUp();
        }

        if(GetAsyncKeyState(VK_RETURN)) {
            if(userInterface.getSelectedMenuNode(currentMenu)->callbackExist()) {
                userInterface.getSelectedMenuNode(currentMenu)->runCallback();
            } else if(userInterface.getSelectedMenuNode(currentMenu)->getMenuNodeSize() != 0) {
                console.clearConsole();
                currentMenu = userInterface.getSelectedMenuNode(currentMenu);
                userInterface.reset();
            }
        }

        if(GetAsyncKeyState(VK_ESCAPE)) {
            if(currentMenu->parentExist()) {
                console.clearConsole();
                currentMenu = currentMenu->getParent();
            }
        }

        if(serverMode) {
            if(!client.IsConnectedToServer()) {
                db.clearDatabase();
                dbFileManager.loadDbFromFiles(&db);
                serverMode = false;
            }
        }

        Sleep(100);
    }
}

void DBManager::printContentOfTableCallback() {
    dbmPtr->console.clearConsole();

    dbmPtr->userInterface.printAllTablesStruct(dbmPtr->db);
    string choosMenu[dbmPtr->db.getNumberOfTables()+1];

    for(int i=0; i<dbmPtr->db.getNumberOfTables(); i++) {
        choosMenu[i] = dbmPtr->db.getTable(i).getName();
    }

    choosMenu[dbmPtr->db.getNumberOfTables()] = dbmPtr->console.PL("Wróæ");

    int userChoos = dbmPtr->userInterface.choosMenu(
        dbmPtr->console.PL("Wyberz tabele, której zawartoœæ chcesz wyœwietliæ:"),
        choosMenu,
        dbmPtr->db.getNumberOfTables()+1
    );

    if(userChoos !=dbmPtr->db.getNumberOfTables()) {
        dbmPtr->userInterface.printTableRecords(dbmPtr->db.getTable(userChoos));
        dbmPtr->waitToEscape();
    }
    dbmPtr->console.clearConsole();
}

void DBManager::addRecordToTableCallback() {
    dbmPtr->console.clearConsole();
    TableRecord tabRecord;

    string choosMenu[dbmPtr->db.getNumberOfTables()+1];
    for(int i=0; i<dbmPtr->db.getNumberOfTables(); i++) {
        choosMenu[i] = dbmPtr->db.getTable(i).getName();
    }

    choosMenu[dbmPtr->db.getNumberOfTables()] = dbmPtr->console.PL("Wróæ");

    int userChoos = dbmPtr->userInterface.choosMenu(
        dbmPtr->console.PL("Wyberz tabele do której chcesz dodaæ rekord:"),
        choosMenu,
        dbmPtr->db.getNumberOfTables()+1
    );

    if(userChoos !=dbmPtr->db.getNumberOfTables()) {
        if(dbmPtr->userInterface.createNewRecord(dbmPtr->db.getTable(userChoos), &tabRecord)) {
            dbmPtr->db.addRecordToTable(userChoos, tabRecord);
            cout<<dbmPtr->console.PL(tabRecord.fields[0]);
            dbmPtr->console.clearConsole();
            dbmPtr->console.setColorAttribute(CONSOLE_COLOR_LIGHT_GREEN);
            cout<<"Dodano nowy rekord do tabeli "<<dbmPtr->console.PL(dbmPtr->db.getTable(userChoos).getName())<<"!";
            Sleep(1000);
        } else {
            dbmPtr->console.clearConsole();
            dbmPtr->console.setColorAttribute(CONSOLE_COLOR_LIGHT_RED);
            cout<<dbmPtr->console.PL("Wyst¹pi³ b³¹d podczas dodawanie nowego rekordu do tabeli ")
            <<dbmPtr->console.PL(dbmPtr->db.getTable(userChoos).getName())<<"!";
            Sleep(1000);
        }
    }
    if(!dbmPtr->serverMode)
        dbmPtr->dbFileManager.saveDbToFiles(dbmPtr->db);
    else
        dbmPtr->client.RequestAddNewRecordToTable(dbmPtr->db.getTable(userChoos).getName(), tabRecord);

    dbmPtr->console.clearConsole();
    dbmPtr->goBackToParentNode();
}

void DBManager::deleteRecordFromTableCallback() {
    dbmPtr->console.clearConsole();
    string choosMenu[dbmPtr->db.getNumberOfTables()+1];
    for(int i=0; i<dbmPtr->db.getNumberOfTables(); i++) {
        choosMenu[i] = dbmPtr->db.getTable(i).getName();
    }

    choosMenu[dbmPtr->db.getNumberOfTables()] = dbmPtr->console.PL("Wróæ");

    int userChoos = dbmPtr->userInterface.choosMenu(
        dbmPtr->console.PL("Wyberz tabele z której chcesz usun¹æ rekord:"),
        choosMenu,
        dbmPtr->db.getNumberOfTables()+1
    );

    if(userChoos !=dbmPtr->db.getNumberOfTables()) {
        int recordToRemove = dbmPtr->userInterface.getRecordToRemove(dbmPtr->db.getTable(userChoos));

        if(recordToRemove != -1) {
            if(dbmPtr->serverMode) {
                stringstream removeRecordStr;
                removeRecordStr<<"{"<<dbmPtr->db.getTable(userChoos).getName()<<","<<recordToRemove<<"}";
                dbmPtr->client.RequestRemoveRecordFromTable(dbmPtr->db.getTable(userChoos).getName(), removeRecordStr.str());
            }
            dbmPtr->db.removeRecordFromTable(userChoos, recordToRemove);
            dbmPtr->console.clearConsole();
            dbmPtr->console.setColorAttribute(CONSOLE_COLOR_LIGHT_GREEN);
            cout<<dbmPtr->console.PL("Poprawnie usuniêto rekord z tabeli ")
            <<dbmPtr->console.PL(dbmPtr->db.getTable(userChoos).getName())<<"!";
            Sleep(1000);
        } else {
            dbmPtr->console.clearConsole();
            dbmPtr->console.setColorAttribute(CONSOLE_COLOR_LIGHT_RED);
            cout<<dbmPtr->console.PL("Wyst¹pi³ b³¹d podczas usuwania rekordu z tabeli ")
            <<dbmPtr->console.PL(dbmPtr->db.getTable(userChoos).getName())<<"!";
            Sleep(1000);
        }
    }

    if(!dbmPtr->serverMode) dbmPtr->dbFileManager.saveDbToFiles(dbmPtr->db);

    dbmPtr->console.clearConsole();
    dbmPtr->goBackToParentNode();
}

void DBManager::printAllTableCallback() {
    dbmPtr->console.clearConsole();

    dbmPtr->userInterface.printAllTablesStruct(dbmPtr->db);
    dbmPtr->waitToEscape();
}

void DBManager::addTableCallback() {
    dbmPtr->console.clearConsole();
    TableStruct tabStruct;
    string label;

    dbmPtr->userInterface.createNewTab(label, &tabStruct);
    Table tab(label, tabStruct);
    dbmPtr->db.addTable(tab);

    if(dbmPtr->serverMode) {
        dbmPtr->client.RequestAddTable(tab);
    } else {
        dbmPtr->dbFileManager.saveDbToFiles(dbmPtr->db);
    }
}

void DBManager::aboutProgramCallback() {
    dbmPtr->userInterface.printProgramAboutInformation();
    dbmPtr->waitToEscape();
}

void DBManager::deleteTableCallback() {
    dbmPtr->console.clearConsole();
    string choosMenu[dbmPtr->db.getNumberOfTables()+1];
    for(int i=0; i<dbmPtr->db.getNumberOfTables(); i++) {
        choosMenu[i] = dbmPtr->db.getTable(i).getName();
    }

    choosMenu[dbmPtr->db.getNumberOfTables()] = dbmPtr->console.PL("Wróæ");

    int userChoos = dbmPtr->userInterface.choosMenu(
        dbmPtr->console.PL("Wyberz tabele, któr¹ chcesz usun¹æ:"),
        choosMenu,
        dbmPtr->db.getNumberOfTables()+1
    );

    if(userChoos !=dbmPtr->db.getNumberOfTables()) {
        if(dbmPtr->serverMode) dbmPtr->client.RequestRemoveTable(dbmPtr->db.getTable(userChoos).getName());
        dbmPtr->db.removeTable(userChoos);
        dbmPtr->console.clearConsole();
        dbmPtr->console.setColorAttribute(CONSOLE_COLOR_LIGHT_GREEN);
        cout<<dbmPtr->console.PL("Usuniêto tabele z bazy danych!");
        Sleep(1000);
    }
    if(!dbmPtr->serverMode) dbmPtr->dbFileManager.saveDbToFiles(dbmPtr->db);
    dbmPtr->console.clearConsole();
}

void DBManager::serverOptionCallback() {
    dbmPtr->console.clearConsole();
    int userChoos = 0;
    if(dbmPtr->serverMode == true) {
        string choosMenu[3] = {"Konsola serwerowa", "Roz³¹cz od serwera", "Wróæ"};

        userChoos = dbmPtr->userInterface.choosMenu(
            dbmPtr->console.PL("Opcje serwera"),
            choosMenu, 3
        );

        if(userChoos == 0) {
            dbmPtr->console.clearConsole();
            cout<<dbmPtr->client.getServerConsoleBuffer();
            dbmPtr->waitToEscape();
        }

        if(userChoos == 1) {
            dbmPtr->console.clearConsole();
            dbmPtr->client.CloseConnection();
            dbmPtr->console.setColorAttribute(CONSOLE_COLOR_LIGHT_GREEN);
            cout<<dbmPtr->console.PL("Roz³¹czono od serwera");
            dbmPtr->console.restoreDefaultColors();
            Sleep(1000);
        }


    } else {
        string choosMenu[2] = {"Po³¹cz od serwera", "Wróæ"};

        userChoos = dbmPtr->userInterface.choosMenu(
            dbmPtr->console.PL("Opcje serwera"),
            choosMenu, 2
        );

        if(userChoos == 0) {
            dbmPtr->client.Init(&dbmPtr->db, "127.0.0.1", 1121);

            dbmPtr->console.clearConsole();
            cout<<dbmPtr->console.PL("£¹czenie...")<<endl;
            if(!dbmPtr->client.Connect()) {
                dbmPtr->console.clearConsole();
                dbmPtr->console.setColorAttribute(CONSOLE_COLOR_LIGHT_RED);
                cout<<dbmPtr->console.PL("B³¹d podczas ³¹czenia siê do serwera: 127.0.0.1:1121")<<endl;
                dbmPtr->console.restoreDefaultColors();
                Sleep(1000);
            } else {
                dbmPtr->console.clearConsole();
                dbmPtr->console.setColorAttribute(CONSOLE_COLOR_LIGHT_GREEN);
                cout<<dbmPtr->console.PL("Po³¹czono do serwera")<<endl;
                dbmPtr->console.restoreDefaultColors();
                dbmPtr->db.clearDatabase();
                dbmPtr->serverMode = true;
                dbmPtr->client.RequestDatabase("db");
                Sleep(1000);
            }
        }
    }

    dbmPtr->console.clearConsole();
}

void DBManager::exitProgramCallback() {
    dbmPtr->dbFileManager.saveDbToFiles(dbmPtr->db);
    exit(0);
}

void DBManager::goBackToParentNode() {
    dbmPtr->console.clearConsole();
    dbmPtr->currentMenu = dbmPtr->currentMenu->getParent();
    dbmPtr->userInterface.reset();
}

void DBManager::waitToEscape() {
    while(true) {
        if(GetAsyncKeyState(VK_ESCAPE)) {
            console.clearConsole();
            break;
        }
    }
}
