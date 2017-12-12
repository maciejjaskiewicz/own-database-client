#include "UserInterface.h"

UserInterface::UserInterface() {
    console.clearConsole();
    console.hideCursor();

    activeMenuField = 0;
    currentMenuSize = 0;
}

void UserInterface::printMenu(MenuNode *menu, bool clientMode) {
    int sizeOfLongestMenuLabel = getSizeOfLongestLabel(menu);
    currentMenuSize = menu->getMenuNodeSize();

    console.setCursor(0, 0);
    console.setColorAttribute(CONSOLE_COLOR_LIGHT_YELLOW | CONSOLE_COLOR_BG_PURPLE);

    if(menu->headerExist()) {
        cout<<menu->getHeader()<<endl<<endl;
    } else {
        if(!clientMode) {
            cout<<"##########BAZA DANYCH - MENU##########";
            console.restoreDefaultColors();
            console.setColorAttribute(CONSOLE_COLOR_LIGHT_RED);
            cout<<" -local "<<endl<<endl;
        }
        else {
            cout<<"##########BAZA DANYCH - MENU##########";
            console.restoreDefaultColors();
            console.setColorAttribute(CONSOLE_COLOR_LIGHT_GREEN);
            cout<<" -server "<<endl<<endl;
        }
    }

    for(int i=0; i<menu->getMenuNodeSize(); i++) {
        console.restoreDefaultColors();
        if(i == activeMenuField) {
            console.setColorAttribute(CONSOLE_COLOR_BG_WHITE);
            cout<<"-> "<<i+1<<". "<<menu->getChild(i)->getLabel()
                <<STR_OPERATIONS::fillSpaces(menu->getChild(i)->getLabel(), sizeOfLongestMenuLabel)<<endl;
        } else {
            cout<<i+1<<". "<<menu->getChild(i)->getLabel()
                <<STR_OPERATIONS::fillSpaces(menu->getChild(i)->getLabel(), sizeOfLongestMenuLabel+3)<<endl;
        }
    }
}

void UserInterface::reset() {
    activeMenuField = 0;
    currentMenuSize = 0;
}

int UserInterface::getSizeOfLongestLabel(MenuNode *menu) {
    int longestMenuLabel = menu->getChild(0)->getLabel().length();

    for(int i=1; i<menu->getMenuNodeSize(); i++) {
        if(menu->getChild(i)->getLabel().length() > longestMenuLabel)
            longestMenuLabel = menu->getChild(i)->getLabel().length();
    }

    return longestMenuLabel;
}

int UserInterface::getSizeOfLongestChoosLabel(string arr[], int n) {
    int longestMenuLabel = arr[0].length();

    for(int i=1; i<n; i++) {
        if(arr[i].length() > longestMenuLabel)
            longestMenuLabel = arr[i].length();
    }

    return longestMenuLabel;
}

void UserInterface::scrollMenuDown() {
    if(activeMenuField < currentMenuSize-1)
        activeMenuField++;
}

void UserInterface::scrollMenuUp() {
    if(activeMenuField > 0)
        activeMenuField--;
}

MenuNode *UserInterface::getSelectedMenuNode(MenuNode *menu) {
    return menu->getChild(activeMenuField);
}

void UserInterface::printAllTablesStruct(Database db) {
    int numberOfTables = db.getNumberOfTables();
    console.setCursor(0, 0);
    int currentCoordY = 0;

    for(int i=0; i<numberOfTables; i++) {
        console.setCursor(0, currentCoordY);
        console.setColorAttribute(CONSOLE_COLOR_LIGHT_YELLOW | CONSOLE_COLOR_BG_PURPLE);
        cout<<"Tabela: "<<console.PL(db.getTable(i).getName());
        console.restoreDefaultColors();
        console.setCursor(4, currentCoordY+2);
        printTableStruct(db.getTable(i).getStruct(), console.getCursorPosition());

        currentCoordY = console.getCursorPosition().Y+2;
    }

    console.setCursor(0, console.getCursorPosition().Y+2);

}

void UserInterface::printTableStruct(TableStruct tStruct, COORD startCoords) {
    int colNumber = tStruct.types.size();
    int colsSize[colNumber];
    int colHeight = 1;
    int startCoordX = startCoords.X;
    int startCoordY = startCoords.Y;
    int currentCoordY = startCoordY;
    int rowNumber = 2;

    fillArrayOfMaxStructColsSize(tStruct, colsSize);

    console.setCursor(startCoordX, startCoordY);

    for(int i=0; i<colNumber; i++) {
        if(i==0) cout<<(char)TABLE_LEFT_TOP_ASCII_CHAR;

        for(int j=0; j<colsSize[i]; j++)
            cout<<(char)TABLE_HORIZONTAL_LINE_ASCII_CHAR;

        if(i==colNumber-1) {
            cout<<(char)TABLE_RIGHT_TOP_ASCII_CHAR;
        } else {
            cout<<(char)TABLE_HORIZONTAL_BOTTOM_CONNECTION_ASCII_CHAR;
        }
    }
    currentCoordY++;

    for(int i=1; i<=rowNumber; i++) {

        for(int j=1; j<=colHeight; j++) {
            console.setCursor(startCoordX, currentCoordY);

            for(int k=0; k<colNumber; k++) {
                cout<<(char)TABLE_VERTICAL_LINE_ASCII_CHAR;

                if(i==1) {
                    console.setColorAttribute(CONSOLE_COLOR_LIGHT_YELLOW);
                    cout<<" "<<console.PL(tStruct.columns[k])<<STR_OPERATIONS::fillSpaces(tStruct.columns[k], colsSize[k]-2);
                    console.restoreDefaultColors();
                } else {
                    cout<<" "<<console.PL(tStruct.types[k])<<STR_OPERATIONS::fillSpaces(tStruct.types[k], colsSize[k]-2);
                }

                if(k==colNumber-1) cout<<(char)TABLE_VERTICAL_LINE_ASCII_CHAR;
            }
            currentCoordY++;
        }
        console.setCursor(startCoordX, currentCoordY);

        if(i!=rowNumber) {
            for(int j=0; j<colNumber; j++) {
                if(j==0) cout<<(char)TABLE_VERTICAL_RIGHT_CONNECTION_ASCII_CHAR;

                for(int k=0; k<colsSize[j]; k++)
                    cout<<(char)TABLE_HORIZONTAL_LINE_ASCII_CHAR;

                if(j==colNumber-1) {
                    cout<<(char)TABLE_VERTICAL_LEFT_CONNECTION_ASCII_CHAR;
                } else {
                    cout<<(char)TABLE_CROSS_CONNECTION_ASCII_CHAR;
                }
            }
            currentCoordY++;
        }
    }
    console.setCursor(startCoordX, currentCoordY);

    for(int i=0; i<colNumber; i++) {
        if(i==0) cout<<(char)TABLE_LEFT_BOTTOM_ASCII_CHAR;

        for(int j=0; j<colsSize[i]; j++)
            cout<<(char)TABLE_HORIZONTAL_LINE_ASCII_CHAR;

        if(i==colNumber-1) {
            cout<<(char)TABLE_RIGHT_BOTTOM_ASCII_CHAR;
        } else {
            cout<<(char)TABLE_HORIZONTAL_TOP_CONNECTION_ASCII_CHAR;
        }
    }
    currentCoordY++;
}

void UserInterface::printTableRecords(Table tab) {
    int colNumber = tab.getStruct().columns.size();
    int colsSize[colNumber];
    int colHeight = 1;
    int startCoordX = 4;
    int startCoordY = 2;
    int currentCoordY = startCoordY;
    int rowNumber = tab.getNumberOfRecords()+1;

    fillArrayOfMaxRecordColsSize(tab, colsSize);

    console.clearConsole();
    console.setCursor(0, 0);
    console.setColorAttribute(CONSOLE_COLOR_LIGHT_YELLOW | CONSOLE_COLOR_BG_PURPLE);
    cout<<"Tabela: "<<console.PL(tab.getName());
    console.restoreDefaultColors();

    if(rowNumber > 1) {

        console.setCursor(startCoordX, startCoordY);

        for(int i=0; i<colNumber; i++) {
            if(i==0) cout<<(char)TABLE_LEFT_TOP_ASCII_CHAR;

            for(int j=0; j<colsSize[i]; j++)
                cout<<(char)TABLE_HORIZONTAL_LINE_ASCII_CHAR;

            if(i==colNumber-1) {
                cout<<(char)TABLE_RIGHT_TOP_ASCII_CHAR;
            } else {
                cout<<(char)TABLE_HORIZONTAL_BOTTOM_CONNECTION_ASCII_CHAR;
            }
        }
        currentCoordY++;

        for(int i=1; i<=rowNumber; i++) {

            for(int j=1; j<=colHeight; j++) {
                console.setCursor(startCoordX, currentCoordY);

                for(int k=0; k<colNumber; k++) {
                    cout<<(char)TABLE_VERTICAL_LINE_ASCII_CHAR;

                    if(i==1) {
                        console.setColorAttribute(CONSOLE_COLOR_LIGHT_YELLOW);
                        cout<<" "<<console.PL(tab.getStruct().columns[k])<<STR_OPERATIONS::fillSpaces(tab.getStruct().columns[k], colsSize[k]-2);
                        console.restoreDefaultColors();
                    } else {
                        cout<<" "<<console.PL(tab.getRecord(i-2).fields[k])<<STR_OPERATIONS::fillSpaces(tab.getRecord(i-2).fields[k], colsSize[k]-2);
                    }

                    if(k==colNumber-1) cout<<(char)TABLE_VERTICAL_LINE_ASCII_CHAR;
                }
                currentCoordY++;
            }
            console.setCursor(startCoordX, currentCoordY);

            if(i!=rowNumber) {
                for(int j=0; j<colNumber; j++) {
                    if(j==0) cout<<(char)TABLE_VERTICAL_RIGHT_CONNECTION_ASCII_CHAR;

                    for(int k=0; k<colsSize[j]; k++)
                        cout<<(char)TABLE_HORIZONTAL_LINE_ASCII_CHAR;

                    if(j==colNumber-1) {
                        cout<<(char)TABLE_VERTICAL_LEFT_CONNECTION_ASCII_CHAR;
                    } else {
                        cout<<(char)TABLE_CROSS_CONNECTION_ASCII_CHAR;
                    }
                }
                currentCoordY++;
            }
        }
        console.setCursor(startCoordX, currentCoordY);

        for(int i=0; i<colNumber; i++) {
            if(i==0) cout<<(char)TABLE_LEFT_BOTTOM_ASCII_CHAR;

            for(int j=0; j<colsSize[i]; j++)
                cout<<(char)TABLE_HORIZONTAL_LINE_ASCII_CHAR;

            if(i==colNumber-1) {
                cout<<(char)TABLE_RIGHT_BOTTOM_ASCII_CHAR;
            } else {
                cout<<(char)TABLE_HORIZONTAL_TOP_CONNECTION_ASCII_CHAR;
            }
        }
        currentCoordY++;
    } else {
        console.setColorAttribute(CONSOLE_COLOR_LIGHT_RED);
        cout<<endl<<endl<<"Tabela nie ma ¿adniej zawartoœci!";
        console.restoreDefaultColors();
    }
}

void UserInterface::fillArrayOfMaxStructColsSize(TableStruct tStruct, int *arr) {
    for(int i=0; i<tStruct.types.size(); i++) {
        arr[i] = tStruct.types[i].length();
        if(tStruct.columns[i].length() > arr[i]) arr[i] = tStruct.columns[i].length();
        arr[i] += 2;
    }
}

void UserInterface::fillArrayOfMaxRecordColsSize(Table tab, int *arr) {
    for(int i=0; i<tab.getStruct().columns.size(); i++) {
        arr[i] = tab.getStruct().columns[i].length();
    }

    for(int i=0; i<tab.getNumberOfRecords(); i++) {
        for(int j=0; j<tab.getStruct().columns.size(); j++) {
            if(tab.getRecord(i).fields[j].length() > arr[j]) arr[j] = tab.getRecord(i).fields[j].length();
            if(i==tab.getNumberOfRecords()-1) arr[j] += 2;
        }
    }
}

int UserInterface::choosMenu(string label, string arr[], int n) {
    int activeChoosMenuField = 0;
    int sizeOfLongestChoosMenuLabel = getSizeOfLongestChoosLabel(arr, n);
    int cursorYPosition = console.getCursorPosition().Y+2;
    bool firstIter = true;

    cout<<console.PL(label);

    while(true) {
        console.setCursor(0, cursorYPosition);
        for(int i=0; i<n; i++) {
            console.restoreDefaultColors();
            if(i == activeChoosMenuField) {
                console.setColorAttribute(CONSOLE_COLOR_BG_WHITE);
                cout<<"-> "<<i+1<<". "<<console.PL(arr[i])<<STR_OPERATIONS::fillSpaces(arr[i], sizeOfLongestChoosMenuLabel)<<endl;
            } else {
                cout<<i+1<<". "<<console.PL(arr[i])<<STR_OPERATIONS::fillSpaces(arr[i], sizeOfLongestChoosMenuLabel+3)<<endl;
            }
        }

        if(GetAsyncKeyState(VK_DOWN) && !firstIter) {
            if(activeChoosMenuField < n-1)
                activeChoosMenuField++;
        }

        if(GetAsyncKeyState(VK_UP) && !firstIter) {
            if(activeChoosMenuField > 0)
                activeChoosMenuField--;
        }

        if(GetAsyncKeyState(VK_RETURN) && !firstIter) {
            return activeChoosMenuField;
            break;
        }
        Sleep(100);

        if(firstIter) {
            Sleep(100);
            firstIter = false;
        }
    }
}

bool UserInterface::createNewTab(string &label, TableStruct *tabStruct) {
    console.setCursor(0, 0);
    console.clearConsole();
    console.setColorAttribute(CONSOLE_COLOR_LIGHT_YELLOW | CONSOLE_COLOR_BG_PURPLE);

    cout<<"Kreator nowej tabeli"<<endl<<endl;
    console.restoreDefaultColors();
    console.showCursor();

    int numberOfColumns = 0;
    bool userInputValid = false;

    cout<<"Nazwa tabeli: ";
    console.setColorAttribute(CONSOLE_COLOR_LIGHT_YELLOW);
    cin>>label;
    console.restoreDefaultColors();

    while(userInputValid == false) {
        cout<<console.PL("Iloœæ kolumn: ");
        console.setColorAttribute(CONSOLE_COLOR_LIGHT_YELLOW);
        cin>>numberOfColumns;
        if(cin.good()) {
            if(numberOfColumns < 2) {
                console.setColorAttribute(CONSOLE_COLOR_LIGHT_RED);
                cout<<console.PL("Tabela musi siê sk¹³caæ conajmniej z 2 kolumn!")<<endl;
                console.restoreDefaultColors();
            } else if(numberOfColumns > 16) {
                console.setColorAttribute(CONSOLE_COLOR_LIGHT_RED);
                cout<<console.PL("Tabela nie mo¿e mieæ wiêcej ni¿ 16 kolumn!")<<endl;
                console.restoreDefaultColors();
            } else {
                userInputValid = true;
            }
        } else {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(),'\n');
            console.setColorAttribute(CONSOLE_COLOR_LIGHT_RED);
            cout<<console.PL("Podaj poprawn¹ wartoœæ!")<<endl;
            console.restoreDefaultColors();
        }
    }
    userInputValid = false;
    console.restoreDefaultColors();

    for(int i=0; i<numberOfColumns; i++) {
        cout<<"Kolumna "<<i+1<<": ";
        console.setColorAttribute(CONSOLE_COLOR_LIGHT_YELLOW);
        string column; cin>>column;
        tabStruct->columns.push_back(column);
        console.restoreDefaultColors();
    }

    string types[4] = {"string", "int", "char", "float"};
    console.hideCursor();
    for(int i=0; i<numberOfColumns; i++) {
        stringstream ss;
        ss<<"Typ dla kolumny "<<tabStruct->columns[i]<<":";
        cout<<endl;
        int userChoos = choosMenu(ss.str(), types, 4);
        tabStruct->types.push_back(types[userChoos]);
    }

    console.clearConsole();

    console.setColorAttribute(CONSOLE_COLOR_LIGHT_YELLOW | CONSOLE_COLOR_BG_PURPLE);
    cout<<"Tabela: "<<console.PL(label);
    console.restoreDefaultColors();
    COORD printCood = {4, 2};
    printTableStruct(*tabStruct, printCood);

    string choosArr[2] = {"Tak", "Nie"};

    console.setCursor(0, console.getCursorPosition().Y+2);
    int userChoos = choosMenu("Czy chcesz zapisaæ now¹ tabelê o powy¿szej strukturze:", choosArr, 2);

    if(userChoos == 0) {
        console.clearConsole();
        console.setColorAttribute(CONSOLE_COLOR_LIGHT_GREEN);
        cout<<console.PL("Dodano now¹ tabele do bazy danych!");
        Sleep(500);
        return true;
    }
    console.clearConsole();
    console.restoreDefaultColors();

    return false;
}

void UserInterface::printProgramAboutInformation() {
    console.clearConsole();
    console.setColorAttribute(CONSOLE_COLOR_LIGHT_YELLOW | CONSOLE_COLOR_BG_PURPLE);
    cout<<"O programie"<<endl<<endl;
    console.restoreDefaultColors();

    console.setColorAttribute(CONSOLE_COLOR_LIGHT_YELLOW);
    cout<<"Autor: ";
    console.restoreDefaultColors();
    cout<<console.PL("Maciej Jaœkiewicz.")<<endl;
    console.setColorAttribute(CONSOLE_COLOR_LIGHT_YELLOW);
    cout<<"Informacje: ";
    console.restoreDefaultColors();
    cout<<console.PL("Prosta \"implementacja\" struktury i systemu bazy danych (klient/serwer).")<<endl;
}

bool UserInterface::checkValueTypeValid(string val, string type) {
    if(type == "string") {
        return true;
    }

    if(type == "int") {
        if(STR_OPERATIONS::is_number(val)) return true;
        return false;
    }

    if(type == "char") {
        if(val.size() > 1) return false;
        return true;
    }

    return false;
}

bool UserInterface::createNewRecord(Table tab, TableRecord *tabRecord) {
    console.clearConsole();
    console.setCursor(0, 0);
    console.setColorAttribute(CONSOLE_COLOR_LIGHT_YELLOW | CONSOLE_COLOR_BG_PURPLE);

    tabRecord->table = tab.getStruct();
    string buffer;

    cout<<"Kreator noewgo rekordu"<<endl<<endl;
    console.restoreDefaultColors();
    console.showCursor();

    for(int i=0; i<tab.getStruct().columns.size(); i++) {
        bool userInputValid = false;

        while(userInputValid == false) {
            cout<<console.PL("Wartoœæ dla kolumny ")<<console.PL(tab.getStruct().columns[i])<<": ";
            console.setColorAttribute(CONSOLE_COLOR_LIGHT_YELLOW);
            cin>>buffer;
            console.restoreDefaultColors();

            if(checkValueTypeValid(buffer, tab.getStruct().types[i])) {
                userInputValid = true;
                tabRecord->fields.push_back(buffer);
            } else {
                console.setColorAttribute(CONSOLE_COLOR_LIGHT_RED);
                cout<<console.PL("Podaj poprawn¹ wartoœæ!")<<endl;
                console.restoreDefaultColors();
            }
        }
    }

    console.hideCursor();
    return true;
}

bool UserInterface::printSortedRecordsFormTable(Table tab) {
    Table sortedTable(tab.getName(), tab.getStruct());
    console.clearConsole();
    console.setColorAttribute(CONSOLE_COLOR_LIGHT_YELLOW | CONSOLE_COLOR_BG_PURPLE);

    string choosColMenuTab[tab.getStruct().columns.size()+1];

    for(int i=0; i<tab.getStruct().columns.size(); i++) {
        choosColMenuTab[i] = tab.getStruct().columns[i];
    }

    choosColMenuTab[tab.getStruct().columns.size()] = console.PL("Wróæ");

    int sortByColumn = choosMenu(console.PL("Wybierz kolumnê po której chcesz posortowaæ rekordy w tabeli:"), choosColMenuTab, tab.getStruct().columns.size()+1);

    if(sortByColumn == tab.getStruct().columns.size()) return -1;

    while(true) {}
    console.clearConsole();
}

int UserInterface::getRecordToRemove(Table tab) {
    console.clearConsole();
    console.setColorAttribute(CONSOLE_COLOR_LIGHT_YELLOW | CONSOLE_COLOR_BG_PURPLE);

    string choosColMenuTab[tab.getStruct().columns.size()+1];

    for(int i=0; i<tab.getStruct().columns.size(); i++) {
        choosColMenuTab[i] = tab.getStruct().columns[i];
    }

    choosColMenuTab[tab.getStruct().columns.size()] = console.PL("Wróæ");

    int removeByColumn = choosMenu(console.PL("Wybierz kolumnê wed³ug której chcesz usun¹æ rekord:"), choosColMenuTab, tab.getStruct().columns.size()+1);

    if(removeByColumn == tab.getStruct().columns.size()) return -1;

    console.clearConsole();
    console.setCursor(0, 0);
    printTableRecords(tab);

    string choosColValMenuTab[tab.getNumberOfRecords()+1];

    choosColValMenuTab[tab.getNumberOfRecords()] = console.PL("Wróæ");

    for(int i=0; i<tab.getNumberOfRecords(); i++) {
        stringstream ss;
        ss<<tab.getStruct().columns[removeByColumn]<<".";
        ss<<tab.getRecord(i).fields[removeByColumn];
        choosColValMenuTab[i] = ss.str();
    }

    cout<<endl<<endl;
    int removeByColumnVal = choosMenu(console.PL("Wybierz wartoœæ kolumny wed³ug której chcesz usun¹æ rekord: "), choosColValMenuTab, tab.getNumberOfRecords()+1);

    if(removeByColumnVal == tab.getNumberOfRecords()) return -1;

    return removeByColumnVal;
}
