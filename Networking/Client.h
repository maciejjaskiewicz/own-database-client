#ifndef CLIENT_H
#define CLIENT_H

#include <winsock2.h>
#include <string>
#include <windows.h>
#include <sstream>
#include <fstream>
#include <iostream>

#include "PacketParser.h"
#include "../Database/Database.h"
#include "../Database/Table.h"
#include "../Database/DbFileManager.h"
#include "../UI/UserInterface.h"
#include "../Utils/StringOperations.h"

/// KLIENT

using namespace std;

enum Packet {
    P_ConsoleMessage,
	P_TableRequestTableNamesString,
	P_TableTableNamesString,
	P_TableRequestTableString,
	P_TableTableString,
	P_TableRequestAddNewTable,
	P_TableRequestRemoveTable,
	P_TableRequestAddNewRecordFormTable,
	P_TableRequestRemoveRecordFormTable
};

class Client{
public:
    Client();
    bool Init(Database *_db, string IP, int PORT);
    bool Connect();
    bool CloseConnection();
    bool IsConnectedToServer();
    bool SendString(string & _string, Packet _packetType = P_ConsoleMessage);
    bool RequestDatabase(string dbName);
    bool RequestTable(string tableName);
    bool RequestAddTable(Table tab);
    bool RequestRemoveTable(string tabName);
    bool RequestAddNewRecordToTable(string tableName, TableRecord tabRecord);
    bool RequestRemoveRecordFromTable(string tableName, string removeRecordStr);

    string getServerConsoleBuffer();
private:
    bool ProcessPacket(Packet _packetType);
    static void ClientThread();

    bool SendAll(char * data, int totalbytes);
    bool SendInt(int32_t _int);
	bool SendPacketType(Packet _packetType);

    bool RecvAll(char * data, int totalBytes);
	bool GetInt(int32_t & _int);
	bool GetPacketType(Packet & _packetType);
	bool GetString(string & _string);

private:
    WSAData wsaData;
    SOCKET Connection;
    PacketParser parser;
	SOCKADDR_IN addr;
	Database *db;

	int sizeofaddr = sizeof(addr);
	bool socketOpen = false;
	stringstream serverConsoleBuffer;
};

static Client *clientPtr;

#endif // CLIENT_H
