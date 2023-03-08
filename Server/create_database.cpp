/* PROGRAM TWORZĄCY BAZĘ DANYCH DLA GG*/
#include <sqlite3.h> 
#include <stdio.h>
#include <cstring>


static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

int main()
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;

    /* Open database */
    rc = sqlite3_open("database.db", &db);

    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    } else {
        fprintf(stdout, "Opened database successfully\n");
    }

    sql = strdup("CREATE TABLE USERS("  \
        "USER_ID        INTEGER     PRIMARY KEY  AUTOINCREMENT NOT NULL ," \
        "NICK           TEXT        CONSTRAINT CK_Users_Nick   CHECK ((LENGTH(NICK) >= 3) AND (LENGTH(NICK) <= 20)) NOT NULL," \
        "LOGIN          TEXT        CONSTRAINT CK_Users_Login   CHECK ((LENGTH(LOGIN) >= 3) AND (LENGTH(LOGIN) <= 20)) NOT NULL UNIQUE," \
        "PASSWORD       TEXT        CONSTRAINT CK_Users_Password   CHECK ((LENGTH(PASSWORD) >= 8) AND (LENGTH(PASSWORD) <= 30)) NOT NULL);");

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } 
    else 
    {
        fprintf(stdout, "Table created successfully\n");
    }

    sql = strdup("CREATE TABLE CONVERSATIONS_DATA("  \
        "DATA_ID                INTEGER     PRIMARY KEY AUTOINCREMENT NOT NULL," \
        "CONVERSATIONS_ID       INTEGER     FOREIGN_KEY REFERENCES CONVERSATIONS(CONVERSATIONS_ID)," \
        "FROM_ID                INT         FOREIGN_KEY REFERENCES USERS(USER_ID)," \
        "TO_ID                  INT         FOREIGN_KEY REFERENCES USERS(USER_ID)," \
        "MESSAGE                TEXT        CONSTRAINT CK_Messages_Length  CHECK ((LENGTH(MESSAGE) >= 1) AND (LENGTH(MESSAGE) <= 512)) NOT NULL);");

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } 
    else 
    {
        fprintf(stdout, "Table created successfully\n");
    }

    sql = strdup("CREATE TABLE CONVERSATIONS("  \
        "CONVERSATIONS_ID       INTEGER         PRIMARY KEY AUTOINCREMENT NOT NULL," \
        "USER_ID1               INT             FOREIGN_KEY REFERENCES USERS(USER_ID)," \
        "USER_ID2               INT             FOREIGN_KEY REFERENCES USERS(USER_ID));");

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } 
    else 
    {
        fprintf(stdout, "Table created successfully\n");
    }

    sql = strdup("CREATE TABLE FRIENDS("  \
        "FRIENDSHIP_ID          INTEGER         PRIMARY KEY AUTOINCREMENT NOT NULL," \
        "CONVERSATIONS_ID       INTEGER         FOREIGN_KEY REFERENCES CONVERSATIONS(CONVERSATIONS_ID)," \
        "USER_ID1               INT             FOREIGN_KEY REFERENCES USERS(USER_ID)," \
        "USER_ID2               INT             FOREIGN_KEY REFERENCES USERS(USER_ID));");

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } 
    else 
    {
        fprintf(stdout, "Table created successfully\n");
    }


    sql = strdup("CREATE TABLE INVITATIONS("  \
        "INVITATION_ID              INTEGER         PRIMARY KEY AUTOINCREMENT NOT NULL," \
        "FROM_ID                    INT             FOREIGN_KEY REFERENCES USERS(USER_ID)," \
        "TO_ID                      INT             FOREIGN_KEY REFERENCES USERS(USER_ID));");

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } 
    else 
    {
        fprintf(stdout, "Table created successfully\n");
    }


    sqlite3_close(db);
    return 0;
}