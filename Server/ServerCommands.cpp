#include "ServerCommands.h"

// KODY AKCJI 
#define CODE_ADD_USER                       '\x10'
#define CODE_ADD_MESSAGE                    '\x11'
#define CODE_ADD_INVITATION                 '\x12'
#define CODE_ACCEPT_INVITATOIN              '\x13'
#define CODE_DECLINE_INVITATOIN             '\x14'
#define CODE_DELETE_FROM_FRIENDS            '\x15'
#define CODE_CHECK_LOGIN_DATA               '\x16'
#define CODE_GET_CONVERSATORS_IDS           '\x17'
#define CODE_GET_USER_NICKNAME              '\x18'
#define CODE_GET_MESSAGES_IDS               '\x19'
#define CODE_GET_MESSAGE_BY_ID              '\x20'
#define CODE_GET_INVITATIONS_USERS_IDS      '\x21'
#define CODE_UPDATE_INVITATIONS             '\x22'
#define CODE_UPDATE_CONVERSATIONS           '\x23'
#define CODE_UPDATE_MESSAGES                '\x24'

ServerCommands::ServerCommands() 
{
}

ServerCommands::~ServerCommands() 
{
}

// PARSOWANIE WIADOMOŚCI
void ServerCommands::ParseMessage(char buffor[])
{   
    parsedMessage retStruct = {};
    int bCnt = 0;
    
    // reading message type
    retStruct.type = buffor[bCnt++];

    // read sender id
    if (buffor[bCnt] != '\0')
    {
        for (int i=0; i<10; i++)
        {
            if (buffor[bCnt] == '\0') 
            {
                bCnt++;
                continue;
            }
            retStruct.senderId += buffor[bCnt++];

        }
    }
    else bCnt += 10;

    // read sender nick
    if (buffor[bCnt] != '\0')
    {
        for (int i=0; i<20; i++) 
        {
            if (buffor[bCnt] == '\0') 
            {
                bCnt++;
                continue;
            }
            retStruct.senderNick += buffor[bCnt++];
        }
    }
    else bCnt += 20;

    // read sender login
    if (buffor[bCnt] != '\0')
    {
        for (int i=0; i<20; i++) 
        {
            if (buffor[bCnt] == '\0') 
            {
                bCnt++;
                continue;
            }
            retStruct.senderLogin += buffor[bCnt++];
        }
    }
    else bCnt += 20;
    
    // read sender password
    if (buffor[bCnt] != '\0')
    {
        for (int i=0; i<30; i++)
        {
            if (buffor[bCnt] == '\0') 
            {
                bCnt++;
                continue;
            }
            retStruct.senderPassword += buffor[bCnt++];
        }
    }
    else bCnt += 30;

    // read receiver id
    if (buffor[bCnt] != '\0')
    {
        for (int i=0; i<10; i++)
        {
            if (buffor[bCnt] == '\0') 
            {
                bCnt++;
                continue;
            }
            retStruct.receiverId += buffor[bCnt++];
        }
    }
    else bCnt += 10;

    // read sender password
    if (buffor[bCnt] != '\0')
    {
        for (int i=0; i<512; i++)
        {
            if (buffor[bCnt] == '\0') 
            {
                bCnt++;
                continue;
            }
            retStruct.message += buffor[bCnt++];
        }
    }

    messStruct = retStruct;
}


// URUCHAMIANIE ODPOWIEDNIEJ FUNKCJI ZGODNIE Z KODEM AKCJI
void ServerCommands::ExecuteFunction(int id)
{
    additionalReceiver = -1;
    char idx = messStruct.type;
    int ret = 0;
    std::string sBuf;

    switch (idx)
    {
        case CODE_ADD_USER:
        {
            ret = DBCommands::AddUser(messStruct);

            if (ret == 0) sBuf = "SUCCESSFUL";
            else sBuf = "ERROR";

            break;
        }
        case CODE_ADD_MESSAGE:
        {
            ret = DBCommands::AddMessage(messStruct);

            if (ret != -1) 
            {
                sBuf = "SUCCESSFUL";
                if (bufforDatabaseServer.find(messStruct.receiverId) != bufforDatabaseServer.end()) 
                {
                    std::string temp = "";
                    temp += CODE_UPDATE_MESSAGES;
                    temp += std::to_string(ret);
                    temp += '\x03';
                    bufforServerUser[bufforDatabaseServer[messStruct.receiverId]].push_back(temp);
                    additionalReceiver = bufforDatabaseServer[messStruct.receiverId];
                }
            }
            else sBuf = "ERROR";

            break;
        }
        case CODE_ADD_INVITATION:
        {
            ret = DBCommands::AddInvitation(messStruct);

            if (ret == 0) 
            {
                sBuf = "SUCCESSFUL";
                if (bufforDatabaseServer.find(messStruct.receiverId) != bufforDatabaseServer.end()) 
                {
                    std::string temp = "";
                    temp += CODE_UPDATE_INVITATIONS;
                    temp += '\x03';
                    bufforServerUser[bufforDatabaseServer[messStruct.receiverId]].push_back(temp);
                    additionalReceiver = bufforDatabaseServer[messStruct.receiverId];
                }
            }
            else sBuf = "ERROR";

            break;
        }
        case CODE_ACCEPT_INVITATOIN:
        {
            ret = DBCommands::AcceptInvitation(messStruct);
            if (ret == 0) 
            {
                sBuf = "SUCCESSFUL";
                if (bufforDatabaseServer.find(messStruct.receiverId) != bufforDatabaseServer.end()) 
                {
                    std::string temp = "";
                    temp += CODE_UPDATE_CONVERSATIONS;
                    temp += '\x03';
                    bufforServerUser[bufforDatabaseServer[messStruct.receiverId]].push_back(temp);
                    additionalReceiver = bufforDatabaseServer[messStruct.receiverId];
                }
            }
            else sBuf = "ERROR";

            break;
        }
        case CODE_DECLINE_INVITATOIN:
        {
            ret = DBCommands::DeclineInvitation(messStruct);

            if (ret == 0) sBuf = "SUCCESSFUL";
            else sBuf = "ERROR";

            break;
        }
        case CODE_CHECK_LOGIN_DATA:
        {
            ret = DBCommands::CheckLoginData(messStruct);

            if (ret >= 1) 
            {
                std::string dbID = std::to_string(ret);
                sBuf = dbID;
                bufforDatabaseServer[dbID] = id;
                bufforServerDatabase[id] = dbID;
            }
            else sBuf = "ERROR";

            break;
        }
        case CODE_GET_CONVERSATORS_IDS:
        {
           std::vector<int> ids = DBCommands::GetUserConversations(messStruct);

            for (auto ele: ids)
            {
                sBuf += std::to_string(ele);
                sBuf += ";";
            }

            break;
        }
        case CODE_GET_USER_NICKNAME:
        {
            std::string nickname = DBCommands::GetUserNickname(messStruct);
            sBuf = nickname;

            break;
        }
        case CODE_GET_MESSAGES_IDS:
        {
            std::pair<std::vector<int>, std::vector<int>> ids = DBCommands::GetMessegesIDs(messStruct);

            for (auto ele: ids.first)
            {
                sBuf += std::to_string(ele);
                sBuf += ";";
            }
            sBuf += (char)(idx);
            for (auto ele: ids.second)
            {
                sBuf += std::to_string(ele);
                sBuf += ";";
            }

            break;
        }
        case CODE_GET_MESSAGE_BY_ID:
        {
            std::string message = DBCommands::GetMessageByID(messStruct);
            sBuf = message;

            break;
        }
        case CODE_GET_INVITATIONS_USERS_IDS:
        {
            
            std::vector<int> ids = DBCommands::GetInvitationsUsersIDs(messStruct);

            for (auto ele: ids)
            {
                sBuf += std::to_string(ele);
                sBuf += ";";
            }

            break;
        }
    }

    std::string temp = "";
    temp = (char)(idx);
    temp += sBuf;
    temp += '\x03';
    bufforServerUser[id].push_back(temp);
}

// FUNKCJA SPRAWDZA CZY ISTNIEJE DODATKOWY UŻYTKOWNIK, KTÓRY POWINIEN DOSTAĆ INFORMACJIE O AKCJI - NP. O DODANIU WIADOMOŚCI 
int ServerCommands::CheckForAdditionalReceiver()
{
    return additionalReceiver;
}

int ServerCommands::RemoveIDFromMap(int id)
{
    std::string userID = bufforServerDatabase[id];
    bufforServerDatabase.erase(id);
    bufforDatabaseServer.erase(userID);

    return 0;
}

// ZWRACA ODPOWIEZI SERWERA NA ZAPYTANIA
std::vector<std::pair<char*, int>> ServerCommands::GetServerResponse(int id)
{
    std::vector<std::pair<char*, int>> ret;

    for (long unsigned int i=0; i<bufforServerUser[id].size(); i++)
    {
        int size = bufforServerUser[id][i].length();

        char* data = new char[size];
        for (int j=0; j<size; j++) 
        {
            data[j] = bufforServerUser[id][i][j];
        }

        ret.push_back(std::make_pair(data, size));
    }

    bufforServerUser[id].clear();

    return ret;
}