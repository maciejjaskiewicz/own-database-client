#include "Client.h"

Client::Client() {
    clientPtr = this;
}

bool Client::Init(Database *_db, string IP, int PORT) {
    WORD DLLVersion = MAKEWORD(2, 1);
    if(WSAStartup(DLLVersion, &wsaData) != 0) {
        MessageBoxA(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
        exit(1);
    }

    addr.sin_addr.s_addr = inet_addr(IP.c_str());
    addr.sin_port = htons(PORT); // Port
    addr.sin_family = AF_INET; // IPv4
    db = _db;
}

bool Client::Connect() {

    Connection = socket(AF_INET, SOCK_STREAM, NULL);
    if(connect(Connection, (SOCKADDR*)&addr, sizeofaddr) != 0) {
        MessageBoxA(NULL, "Failed to Connect", "Error", MB_OK | MB_ICONERROR);
        return false;
    }
    socketOpen = true;
    serverConsoleBuffer<<"Connected!\n";
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientThread, NULL, NULL, NULL);
    return true;
}

bool Client::CloseConnection() {
    if(closesocket(Connection) == SOCKET_ERROR) {
        if(WSAGetLastError() == WSAENOTSOCK)
            return true;

        stringstream errorMessage;
        errorMessage<<"Failed to close the socket. Winsock Error:"<<WSAGetLastError()<<".";
        MessageBoxA(NULL, errorMessage.str().c_str(), "Error", MB_OK | MB_ICONERROR);
        return false;
    }
    socketOpen = false;
}

bool Client::IsConnectedToServer() {
    return socketOpen == true;
}

string Client::getServerConsoleBuffer() {
    return serverConsoleBuffer.str();
}

bool Client::ProcessPacket(Packet _packetType) {
    switch(_packetType) {
    case P_ConsoleMessage: {
        string message;
        if(!GetString(message))
            return false;
        serverConsoleBuffer<<"[SERVER] "<<message<<endl;
        break;
    }
	case P_TableTableNamesString: {
        string reciveStr;
        if(!GetString(reciveStr))
            return false;

        serverConsoleBuffer<<"Getting table names form server.\n";

        vector<string> tableNames = parser.ParseTableNamesToVec(reciveStr);

        for(int i=0; i<tableNames.size(); i++)
            RequestTable(tableNames[i]);
        break;
	}
	case P_TableTableString: {
        string reciveStr;
        if(!GetString(reciveStr))
            return false;

        serverConsoleBuffer<<"Getting table string from server.\n";
        serverConsoleBuffer<<reciveStr<<"\n";
        db->addTable(parser.GetTableFromTableString(reciveStr));
        break;
	}
    default:
        serverConsoleBuffer<<"Unrecognized packet: "<<_packetType<<"\n";
        break;
    }

    return true;
}

bool Client::RequestDatabase(string dbName) {
    serverConsoleBuffer<<"Requesting database from server \n";
    if (!SendString(dbName, P_TableRequestTableNamesString))
        return false;
	return true;
}

bool Client::RequestTable(string tableName) {
    serverConsoleBuffer<<"Requesting table \""<<tableName<<"\" from server \n";
    if (!SendString(tableName, P_TableRequestTableString))
        return false;
	return true;
}

bool Client::RequestAddTable(Table tab) {
    serverConsoleBuffer<<"Requesting add new table \""<<tab.getName()<<"\" to server database \n";
    string newTableStr = parser.GetStringOfNewTable(tab);
    if (!SendString(newTableStr, P_TableRequestAddNewTable))
        return false;
	return true;
}

bool Client::RequestRemoveTable(string tabName) {
    serverConsoleBuffer<<"Requesting remove table \""<<tabName<<"\" from server database \n";

    if (!SendString(tabName, P_TableRequestRemoveTable))
        return false;
	return true;
}

bool Client::RequestAddNewRecordToTable(string tableName, TableRecord tabRecord) {
    serverConsoleBuffer<<"Requesting add new record to table \""<<tableName<<"\"\n";
    string addRecordStr = parser.GetStringOfNewRecord(tableName, tabRecord);
    serverConsoleBuffer<<addRecordStr<<"\n";
    if (!SendString(addRecordStr, P_TableRequestAddNewRecordFormTable))
        return false;
	return true;
}

bool Client::RequestRemoveRecordFromTable(string tableName, string removeRecordStr) {
    serverConsoleBuffer<<"Requesting remove record from table \""<<tableName<<"\"\n";
    serverConsoleBuffer<<removeRecordStr<<"\n";
    if (!SendString(removeRecordStr, P_TableRequestRemoveRecordFormTable))
        return false;
	return true;
}

void Client::ClientThread() {

    Packet packetType;
    while(true) {
        if(!clientPtr->GetPacketType(packetType))
            break;

        if(!clientPtr->ProcessPacket(packetType))
            break;
    }
    clientPtr->serverConsoleBuffer<<"Lost connection to the server.\n";

    if(clientPtr->CloseConnection()) {
        clientPtr->serverConsoleBuffer<<"Socket to the server was closed successfuly.\n";
    } else {
        clientPtr->serverConsoleBuffer<<"Socket was not able to be closed.\n";
    }
}

bool Client::SendAll(char * data, int totalBytes) {
	int bytesSent = 0;
	while (bytesSent < totalBytes) {
		int retCheck = send(Connection, data + bytesSent, totalBytes - bytesSent, NULL);
		if (retCheck == SOCKET_ERROR)
			return false;
		bytesSent += retCheck;
	}
	return true;
}

bool Client::RecvAll(char * data, int totalBytes) {
	int bytesReceived = 0;
	while (bytesReceived < totalBytes) {
		int retCheck = recv(Connection, data + bytesReceived, totalBytes - bytesReceived, NULL);
		if (retCheck == SOCKET_ERROR)
			return false;
		bytesReceived += retCheck;
	}
	return true;
}

bool Client::SendInt(int32_t _int) {
    _int = htonl(_int); // Host Byte Order to Network Byte Order
    if (!SendAll((char*)&_int, sizeof(int32_t)))
		return false;
	return true;
}

bool Client::GetInt(int32_t &_int) {
    if (!RecvAll((char*)&_int, sizeof(int32_t)))
		return false;
    _int = ntohl(_int);
	return true;
}

bool Client::SendPacketType(Packet _packetType) {
    if (!SendInt(_packetType))
        return false;
    return true;
}

bool Client::GetPacketType(Packet &_packetType) {
    int packetType;
    if (!GetInt(packetType))
        return false;
    _packetType = (Packet)packetType;
    return true;
}

bool Client::SendString(string &_string, Packet _packetType) {
    if(!SendPacketType(_packetType))
        return false;

    int bufferLength = _string.size();
    if(!SendInt(bufferLength))
        return false;

    if (!SendAll((char*)_string.c_str(), bufferLength))
        return false;

    return true;
}

bool Client::GetString(string &_string) {
    int bufferLength;
    if(!GetInt(bufferLength))
        return false;

    char *buffer = new char[bufferLength+1];
    buffer[bufferLength] = '\0';

    if (!RecvAll(buffer, bufferLength)) {
		delete[] buffer;
		return false;
	}
	_string = buffer;
	delete[] buffer;
	return true;
}
