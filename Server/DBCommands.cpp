// OBSŁUGA ZAPYTAŃ I ODPOWIEDZI DO/Z BAZY DANYCH
#include "DBCommands.h"


// INFORMACJE ZWROTNE Z BAZY DANYCH
static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   return 0;
}

static int callbackLoginData(void *param, int argc, char **argv, char **azColName) {
    int* id = (int*)param;       
    if (strcmp(azColName[0], "USER_ID") == 0) *id = atoi(argv[0]);
    return 0;
}

static int callbackCheckConversationsID(void *param, int argc, char **argv, char **azColName) {
    int* id = (int*)param;   
    if (strcmp(azColName[0], "CONVERSATIONS_ID") == 0) *id = atoi(argv[0]);
    return 0;
}

static int callbackCheckDataIDs(void *param, int argc, char **argv, char **azColName) {
    std::vector<int>* ids = (std::vector<int>*)param;
    if (strcmp(argv[0], "DATA_ID") != 0) ids->push_back(atoi(argv[0]));
    return 0;
}

static int callbackGetMessageByID(void *param, int argc, char **argv, char **azColName) {
    std::string* message = (std::string*)param;   
    if (strcmp(azColName[0], "MESSAGE") == 0) *message = argv[0];
    return 0;
}

static int callbackGetConversatorsIDs(void *param, int argc, char **argv, char **azColName) {
    std::vector<int>* ids = (std::vector<int>*)param;
    const char* userID = std::to_string(ids->at(0)).c_str();

    if (strcmp(argv[0], userID) != 0) ids->push_back(atoi(argv[0]));
    else ids->push_back(atoi(argv[1]));

    return 0;
}

static int callbackGetNick(void *param, int argc, char **argv, char **azColName) {
    std::string* nick = (std::string*)param;       
    if (strcmp(azColName[0], "NICK") == 0) *nick = argv[0];
    return 0;
}

static int callbackAddInvitation(void *param, int argc, char **argv, char **azColName)
{
    int* id = (int*)param;   
    if (strcmp(azColName[0], "INVITATIONS_ID") == 0) *id = atoi(argv[0]);
    return 0;
}

static int callbackAcceptInvitation(void *param, int argc, char **argv, char **azColName)
{
    int* id = (int*)param;   
    if (strcmp(azColName[0], "FROM_ID") == 0) *id = atoi(argv[0]);
    return 0;
}

static int callbackGetInvitationsUsersIDs(void *param, int argc, char **argv, char **azColName) {
    std::vector<int>* ids = (std::vector<int>*)param;
    const char* userID = std::to_string(ids->at(0)).c_str();

    if (strcmp(argv[0], userID) != 0) ids->push_back(atoi(argv[0]));
    else ids->push_back(atoi(argv[1]));

    return 0;
}



DBCommands::DBCommands()
{
    OpenDatabase();
    if (!db) exit(1);   
}

DBCommands::~DBCommands()
{
    if (db) sqlite3_close(db);
}



void DBCommands::OpenDatabase()
{
    int rc;
    rc = sqlite3_open("database.db", &db);

    if (rc) db = nullptr;
}

// returns: -1: sql error; 0: successfull
int DBCommands::AddUser(parsedMessage mess)
{
    char *zErrMsg = 0;
    int rc; 


    std::string sql = "INSERT INTO USERS (NICK, LOGIN, PASSWORD) "  \
            "VALUES ('" + mess.senderNick + "', '" + mess.senderLogin + "', '" + mess.senderPassword + "');";

    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    
    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    } 
    
    //fprintf(stdout, "User added successfully\n");

    return 0;   
}

// returns: -1: sql error; 0: no user_id; >0: user_id
int DBCommands::CheckLoginData(parsedMessage mess) 
{
    char *zErrMsg = NULL;
    int rc; 
    int* recvData = new int;
    *recvData = 0;
    
    std::string sql = "SELECT USER_ID FROM USERS WHERE \
            LOGIN = '" + mess.senderLogin + "' AND PASSWORD = '" + mess.senderPassword + "';";

    rc = sqlite3_exec(db, sql.c_str(), callbackLoginData, recvData, &zErrMsg);
    
    int userID = *recvData;
    delete recvData;

    if(rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    } 
    
    //if (user_id == 0) fprintf(stderr, "Login error: Wrong login or password\n");
    //else fprintf(stdout, "Logged in successfully \n");
    return userID; 
}

// returns: -1: sql error; 0: successfull
int DBCommands::_AddToFriends(std::string userID1, std::string userID2)
{
    char *zErrMsg = NULL;
    int rc; 
    int* recvData = new int;
    *recvData = 0;

    std::string sql = "SELECT CONVERSATIONS_ID FROM CONVERSATIONS WHERE \
    (USER_ID1 = '" + userID1 + "' AND USER_ID2 = '" + userID2 + "') \
    OR (USER_ID1 = '" + userID2 + "' AND USER_ID2 = '" + userID1 + "');";

    rc = sqlite3_exec(db, sql.c_str(), callbackCheckConversationsID, recvData, &zErrMsg);

    if(rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        delete recvData;
        return -1;
    }
    
    int conversationsID = *recvData;

    if(conversationsID == 0)
    {

        sql = "INSERT INTO CONVERSATIONS (USER_ID1, USER_ID2)\
        VALUES ('" + userID1 + "', '" + userID2 + "');";

        rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);

        if(rc != SQLITE_OK )
        {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
            return -1;
        } 
        //fprintf(stdout, "Conversation created successfully\n");

        sql = "SELECT CONVERSATIONS_ID FROM CONVERSATIONS WHERE \
        (USER_ID1 = '" + userID1 + "' AND USER_ID2 = '" + userID2 + "') \
        OR (USER_ID1 = '" + userID2 + "' AND USER_ID2 = '" + userID1 + "');";

        rc = sqlite3_exec(db, sql.c_str(), callbackCheckConversationsID, recvData, &zErrMsg);
        
        conversationsID = *recvData;

    }

    delete recvData;

    sql = "INSERT INTO FRIENDS (CONVERSATIONS_ID, USER_ID1, USER_ID2)\
    VALUES ('" + std::to_string(conversationsID) + "', '"+ userID1 + "', '" + userID2 + "');";

    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);

    if(rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    } 
    //fprintf(stdout, "Friendship created successfully\n");


    return 0;
}

// returns: -1: sql error; 0: successfull
int DBCommands::DeleteFromFriends(parsedMessage mess)
{
    char *zErrMsg = 0;
    int rc; 

    std::string sql = "DELETE FROM FRIENDS WHERE \
            (USER_ID1 = '" + mess.senderId + "' AND USER_ID2 = '" + mess.receiverId + "') \
            OR (USER_ID1 = '" + mess.receiverId + "' AND USER_ID2 = '" + mess.senderId + "');";


    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    
    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    } 
    //fprintf(stdout, "Friendship deleted successfully\n");
    
    return 0; 
}

// returns: -1: sql error; id dodanej widomości: successfull
int DBCommands::AddMessage(parsedMessage mess)
{
    char *zErrMsg = NULL;
    int rc; 
    int* recvData = new int;
    *recvData = 0;

    std::string sql = "SELECT CONVERSATIONS_ID FROM CONVERSATIONS WHERE \
    (USER_ID1 = '" + mess.senderId + "' AND USER_ID2 = '" + mess.receiverId + "') \
    OR (USER_ID1 = '" + mess.receiverId + "' AND USER_ID2 = '" + mess.senderId + "');";

    rc = sqlite3_exec(db, sql.c_str(), callbackCheckConversationsID, recvData, &zErrMsg);
    
    int conversationsID = *recvData;
    delete recvData;

    sql = "INSERT INTO CONVERSATIONS_DATA (CONVERSATIONS_ID, FROM_ID, TO_ID, MESSAGE) "  \
            "VALUES ('" + std::to_string(conversationsID) + "', '" + mess.senderId + "', '" + mess.receiverId + "', '" + mess.message + "');";

    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    
    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    } 

    return sqlite3_last_insert_rowid(db); 
}

// returns: wektor z identyfikatorami użytkowników konwersujących z użytkownikiem proszącym
std::vector<int> DBCommands::GetUserConversations(parsedMessage mess)
{
    char *zErrMsg = NULL;
    int rc; 
    std::vector<int>* recvData = new std::vector<int>;
    recvData->push_back(std::stoi(mess.senderId));
    
    std::string sql = "SELECT USER_ID1, USER_ID2 FROM CONVERSATIONS WHERE \
            USER_ID1 = '" + mess.senderId + "' OR USER_ID2 = '" + mess.senderId + "';";

    rc = sqlite3_exec(db, sql.c_str(), callbackGetConversatorsIDs, recvData, &zErrMsg);
    
    std::vector<int> usersID = *recvData;

    delete recvData;
    
    if(rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } 
    
    return usersID; 
}

// returns: nazwa użytkownika o podanym id
std::string DBCommands::GetUserNickname(parsedMessage mess)
{
    char *zErrMsg = NULL;
    int rc; 
    std::string *nick = new std::string;
    
    std::string sql = "SELECT NICK FROM USERS WHERE \
            USER_ID = '" + mess.receiverId + "';";

    rc = sqlite3_exec(db, sql.c_str(), callbackGetNick, nick, &zErrMsg);
    
    std::string retNick = *nick;

    delete nick;
    
    if(rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } 
    
    return retNick; 
}

// returns: para wektor wiadomości użytkownika nadającego i wektor wiadomości odbierającego 
std::pair<std::vector<int>, std::vector<int>> DBCommands::GetMessegesIDs(parsedMessage mess)
{
    char *zErrMsg = NULL;
    int rc; 
    std::vector<int>* senderMess = new std::vector<int>;
    
    std::string sql = "SELECT DATA_ID FROM CONVERSATIONS_DATA WHERE \
            (FROM_ID = '" + mess.senderId + "' AND TO_ID = '" + mess.receiverId + "');";

    rc = sqlite3_exec(db, sql.c_str(), callbackCheckDataIDs, senderMess, &zErrMsg);
    
    std::vector<int> sIDs = *senderMess;

    delete senderMess;
    
    if(rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } 

    std::vector<int>* recvMess = new std::vector<int>;

    sql = "SELECT DATA_ID FROM CONVERSATIONS_DATA WHERE \
            (FROM_ID = '" + mess.receiverId + "' AND TO_ID = '" + mess.senderId + "');";

    rc = sqlite3_exec(db, sql.c_str(), callbackCheckDataIDs, recvMess, &zErrMsg);
    
    std::vector<int> rIDs = *recvMess;

    delete recvMess;
    
    if(rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } 
    
    return std::make_pair(sIDs, rIDs); 
}

// returns: wiadomośc o podanym id
std::string DBCommands::GetMessageByID(parsedMessage mess)
{
    char *zErrMsg = NULL;
    int rc; 
    std::string* recvData = new std::string;
    
    std::string sql = "SELECT MESSAGE FROM CONVERSATIONS_DATA WHERE \
            (DATA_ID = " + mess.receiverId + ");";

    rc = sqlite3_exec(db, sql.c_str(), callbackGetMessageByID, recvData, &zErrMsg);
    
    std::string message = *recvData;

    delete recvData;
    
    if(rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } 
    
    return message; 
}


// returns: -1: sql error 0: success
int DBCommands::AddInvitation(parsedMessage mess)
{
    char *zErrMsg = NULL;
    int rc; 
    int* recvData = new int;
    *recvData = 0;

    std::string sql = "SELECT INVITATION_ID FROM INVITATIONS WHERE \
    (FROM_ID = '" + mess.senderId + "' AND TO_ID = '" + mess.receiverId + "') \
    OR (FROM_ID = '" + mess.receiverId + "' AND TO_ID = '" + mess.senderId + "');";

    rc = sqlite3_exec(db, sql.c_str(), callbackAddInvitation, recvData, &zErrMsg);
    
    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    } 

    int invitaionID = *recvData;
    delete recvData;

    if(invitaionID == 0)
    {

        sql = "INSERT INTO INVITATIONS (FROM_ID, TO_ID)\
        VALUES ('" + mess.senderId + "', '" + mess.receiverId + "');";

        rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);

        if(rc != SQLITE_OK )
        {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
            return -1;
        } 
    }
    else 
    {
        return -1;
    }

    return 0;   
}


// returns: -1: sql error 0: success
int DBCommands::AcceptInvitation(parsedMessage mess)
{
    char *zErrMsg = NULL;
    int rc; 
    int* recvData = new int;
    *recvData = -1;

    std::string sql = "SELECT FROM_ID FROM INVITATIONS WHERE \
    TO_ID = '" + mess.senderId + "';";

    rc = sqlite3_exec(db, sql.c_str(), callbackAcceptInvitation, recvData, &zErrMsg);
    
    int fromID = *recvData;
    delete recvData;

    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    } 

    if (fromID == -1)
    {
        puts("Unknown user request!");
        return -1;
    }
    else
    {
        sql = "DELETE FROM INVITATIONS WHERE \
        TO_ID = '" + mess.senderId + "';";

        rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);

        if( rc != SQLITE_OK )
        {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
            return -1;
        } 

        return _AddToFriends(mess.senderId, std::to_string(fromID));
    }

    
}

// returns: -1: sql error 0: success
int DBCommands::DeclineInvitation(parsedMessage mess)
{
    char *zErrMsg = NULL;
    int rc; 

    std::string sql = "DELETE FROM INVITATIONS WHERE \
        FROM_ID = '" + mess.receiverId + "' AND TO_ID = '" + mess.senderId + "';";

    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);

    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    } 

    return 0;
}

// returns: identyfikatory użytkowników zapraszających
std::vector<int> DBCommands::GetInvitationsUsersIDs(parsedMessage mess)
{
    char *zErrMsg = NULL;
    int rc; 
    std::vector<int>* recvData = new std::vector<int>;
    recvData->push_back(std::stoi(mess.senderId));
    
    std::string sql = "SELECT FROM_ID, TO_ID FROM INVITATIONS WHERE \
            TO_ID = '" + mess.senderId + "';";

    rc = sqlite3_exec(db, sql.c_str(), callbackGetInvitationsUsersIDs, recvData, &zErrMsg);
    
    std::vector<int> usersID = *recvData;

    delete recvData;
    
    if(rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } 
    
    return usersID; 
}
