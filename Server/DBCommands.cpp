// OBSŁUGA ZAPYTAŃ I ODPOWIEDZI DO/Z BAZY DANYCH
#include "DBCommands.h"


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
    std::string sql = "INSERT INTO USERS (NICK, LOGIN, PASSWORD) VALUES (?, ?, ?);";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement\n";
        return -1;
    }

    if (sqlite3_bind_text(stmt, 1, mess.senderNick.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 2, mess.senderLogin.c_str(), -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 3, mess.senderPassword.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
        std::cerr << "Failed to bind parameters\n";
        sqlite3_finalize(stmt);
        return -1;
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Execution failed: " << sqlite3_errmsg(db) << "\n";
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);
    return 0;   
}

// returns: -1: sql error; 0: no user_id; >0: user_id
int DBCommands::CheckLoginData(parsedMessage mess) 
{
    sqlite3_stmt *stmt;
    int rc; 

    std::string sql = "SELECT USER_ID FROM USERS WHERE LOGIN = ? AND PASSWORD = ?;";

    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
    
    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_text(stmt, 1, mess.senderLogin.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, mess.senderPassword.c_str(), -1, SQLITE_STATIC);

    int userID = 0;
    rc = sqlite3_step(stmt);
    if(rc == SQLITE_ROW) {
        userID = sqlite3_column_int(stmt, 0);
    } else if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    
    sqlite3_finalize(stmt);
    return userID;
}


// returns: -1: sql error; 0: successfull
int DBCommands::_AddToFriends(std::string userID1, std::string userID2)
{
    sqlite3_stmt *stmt;
    int rc; 

    std::string sqlSelect = "SELECT COALESCE((SELECT CONVERSATIONS_ID FROM CONVERSATIONS WHERE \
    (USER_ID1 = ? AND USER_ID2 = ?) OR (USER_ID1 = ? AND USER_ID2 = ?)), 0);";

    rc = sqlite3_prepare_v2(db, sqlSelect.c_str(), -1, &stmt, 0);

    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, userID1.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, userID2.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, userID2.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, userID1.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_ROW) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    } 

    int conversationsID = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    if(conversationsID == 0)
    {
        std::string sqlInsert = "INSERT INTO CONVERSATIONS (USER_ID1, USER_ID2) \
        VALUES (?, ?);";

        rc = sqlite3_prepare_v2(db, sqlInsert.c_str(), -1, &stmt, 0);

        if(rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
            return -1;
        }

        sqlite3_bind_text(stmt, 1, userID1.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, userID2.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);

        if (rc != SQLITE_DONE) {
            fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            return -1;
        } 

        conversationsID = sqlite3_last_insert_rowid(db);
        sqlite3_finalize(stmt);
    }

    std::string sqlInsertFriends = "INSERT INTO FRIENDS (CONVERSATIONS_ID, USER_ID1, USER_ID2) \
    VALUES (?, ?, ?);";

    rc = sqlite3_prepare_v2(db, sqlInsertFriends.c_str(), -1, &stmt, 0);

    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, conversationsID);
    sqlite3_bind_text(stmt, 2, userID1.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, userID2.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    } 

    sqlite3_finalize(stmt);
    return 0;
}


// returns: -1: sql error; 0: successfull
int DBCommands::DeleteFromFriends(parsedMessage mess)
{
    sqlite3_stmt *stmt;
    int rc; 

    std::string sql = "DELETE FROM FRIENDS WHERE \
            (USER_ID1 = ? AND USER_ID2 = ?) \
            OR (USER_ID1 = ? AND USER_ID2 = ?);";

    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
    
    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_text(stmt, 1, mess.senderId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, mess.receiverId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, mess.receiverId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, mess.senderId.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    } 

    sqlite3_finalize(stmt);
    return 0;
}


// returns: -1: sql error; id dodanej widomości: successfull
int DBCommands::AddMessage(parsedMessage mess)
{
    sqlite3_stmt *stmt;
    int rc; 

    std::string sqlSelect = "SELECT CONVERSATIONS_ID FROM CONVERSATIONS WHERE \
    (USER_ID1 = ? AND USER_ID2 = ?) \
    OR (USER_ID1 = ? AND USER_ID2 = ?);";

    rc = sqlite3_prepare_v2(db, sqlSelect.c_str(), -1, &stmt, 0);

    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_text(stmt, 1, mess.senderId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, mess.receiverId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, mess.receiverId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, mess.senderId.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_ROW) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    } 

    int conversationsID = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    
    std::string sqlInsert = "INSERT INTO CONVERSATIONS_DATA (CONVERSATIONS_ID, FROM_ID, TO_ID, MESSAGE) "  \
            "VALUES (?, ?, ?, ?);";

    rc = sqlite3_prepare_v2(db, sqlInsert.c_str(), -1, &stmt, 0);

    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, conversationsID);
    sqlite3_bind_text(stmt, 2, mess.senderId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, mess.receiverId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, mess.message.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    } 

    int lastId = sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);

    return lastId; 
}


std::vector<int> DBCommands::GetUserConversations(parsedMessage mess)
{
    sqlite3_stmt *stmt;
    int rc; 
    std::vector<int> conversationsID;

    std::string sql = "SELECT USER_ID1, USER_ID2 FROM CONVERSATIONS WHERE USER_ID1 = ? OR USER_ID2 = ?;";

    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
    
    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return conversationsID;
    }

    sqlite3_bind_text(stmt, 1, mess.senderId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, mess.senderId.c_str(), -1, SQLITE_STATIC);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        std::string userID1 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string userID2 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

        conversationsID.push_back(userID1 == mess.senderId ? std::stoi(userID2) : std::stoi(userID1));
    }

    if(rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    return conversationsID; 
}

std::string DBCommands::GetUserNickname(parsedMessage mess)
{
    sqlite3_stmt *stmt;
    int rc; 
    std::string nickname;

    std::string sql = "SELECT NICK FROM USERS WHERE USER_ID = ?;";

    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
    
    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return nickname;
    }

    sqlite3_bind_text(stmt, 1, mess.receiverId.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    if(rc == SQLITE_ROW) {
        nickname = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    } else if(rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    return nickname; 
}

std::pair<std::vector<int>, std::vector<int>> DBCommands::GetMessegesIDs(parsedMessage mess)
{
    sqlite3_stmt *stmt;
    int rc; 
    std::vector<int> senderMsgs, receiverMsgs;

    std::string sql = "SELECT DATA_ID, FROM_ID FROM CONVERSATIONS_DATA WHERE (FROM_ID = ? AND TO_ID = ?) OR (FROM_ID = ? AND TO_ID = ?);";

    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
    
    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return std::make_pair(senderMsgs, receiverMsgs);
    }

    sqlite3_bind_text(stmt, 1, mess.senderId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, mess.receiverId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, mess.receiverId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, mess.senderId.c_str(), -1, SQLITE_STATIC);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int dataID = sqlite3_column_int(stmt, 0);
        std::string fromID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

        if(fromID == mess.senderId)
            senderMsgs.push_back(dataID);
        else
            receiverMsgs.push_back(dataID);
    }

    if(rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    return std::make_pair(senderMsgs, receiverMsgs); 
}


std::string DBCommands::GetMessageByID(parsedMessage mess)
{
    sqlite3_stmt *stmt;
    int rc; 
    std::string message;

    std::string sql = "SELECT MESSAGE FROM CONVERSATIONS_DATA WHERE DATA_ID = ?;";

    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
    
    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return message;
    }

    sqlite3_bind_text(stmt, 1, mess.receiverId.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    if(rc == SQLITE_ROW) {
        message = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    } else if(rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    return message; 
}

int DBCommands::AddInvitation(parsedMessage mess)
{
    sqlite3_stmt *stmt;
    int rc; 
    int invitationID = 0;

    std::string sql = "SELECT INVITATION_ID FROM INVITATIONS WHERE (FROM_ID = ? AND TO_ID = ?) OR (FROM_ID = ? AND TO_ID = ?);";

    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
    
    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, mess.senderId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, mess.receiverId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, mess.receiverId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, mess.senderId.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    if(rc == SQLITE_ROW) {
        invitationID = sqlite3_column_int(stmt, 0);
    } else if(rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);

    if(invitationID == 0) {
        sql = "INSERT INTO INVITATIONS (FROM_ID, TO_ID) VALUES (?, ?);";

        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);

        if(rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
            return -1;
        }

        sqlite3_bind_text(stmt, 1, mess.senderId.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, mess.receiverId.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);

        if(rc != SQLITE_DONE) {
            fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        }

        sqlite3_finalize(stmt);
    } else {
        return -1;
    }

    return 0;   
}

int DBCommands::AcceptInvitation(parsedMessage mess)
{
    sqlite3_stmt *stmt;
    int rc; 
    int fromID = -1;

    std::string sql = "SELECT FROM_ID FROM INVITATIONS WHERE TO_ID = ?;";

    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
    
    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, mess.senderId.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    if(rc == SQLITE_ROW) {
        fromID = sqlite3_column_int(stmt, 0);
    } else if(rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);

    if(fromID == -1) {
        puts("Unknown user request!");
        return -1;
    } else {
        sql = "DELETE FROM INVITATIONS WHERE TO_ID = ?;";

        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);

        if(rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
            return -1;
        }

        sqlite3_bind_text(stmt, 1, mess.senderId.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);

        if(rc != SQLITE_DONE) {
            fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        }

        sqlite3_finalize(stmt);

        return _AddToFriends(mess.senderId, std::to_string(fromID));
    }
}

int DBCommands::DeclineInvitation(parsedMessage mess)
{
    char *zErrMsg = NULL;
    int rc; 

    std::string sql = "DELETE FROM INVITATIONS WHERE FROM_ID = ? AND TO_ID = ?;";

    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);

    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, mess.receiverId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, mess.senderId.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    if(rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);

    return 0;
}

std::vector<int> DBCommands::GetInvitationsUsersIDs(parsedMessage mess)
{
    sqlite3_stmt *stmt;
    int rc; 
    std::vector<int> usersID;
    usersID.push_back(std::stoi(mess.senderId));

    std::string sql = "SELECT FROM_ID, TO_ID FROM INVITATIONS WHERE TO_ID = ?;";

    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
    
    if(rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return usersID;
    }

    sqlite3_bind_text(stmt, 1, mess.senderId.c_str(), -1, SQLITE_STATIC);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int fromID = sqlite3_column_int(stmt, 0);
        usersID.push_back(fromID);
    }

    if(rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    return usersID; 
}

