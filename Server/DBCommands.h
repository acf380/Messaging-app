#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <iostream>
#include <sqlite3.h> 
#include <vector>
#include <unordered_map>


class DBCommands 
{   
    private:
        void OpenDatabase();
        sqlite3* db;

        int _AddToFriends(std::string userID1, std::string userID2);

    public:
        struct parsedMessage  // bardo ważna struktura zawiera informacje sprarsowane od klienta
        {
            char type;
            std::string senderId;
            std::string senderNick;
            std::string senderLogin;
            std::string senderPassword;
            std::string receiverId;
            std::string message;
        };

        DBCommands();
        ~DBCommands();
        
        int AddUser(parsedMessage mess);
        int CheckLoginData(parsedMessage mess);
        int DeleteFromFriends(parsedMessage mess);
        int AddMessage(parsedMessage mess);
        int AddInvitation(parsedMessage mess);
        int AcceptInvitation(parsedMessage mess);
        int DeclineInvitation(parsedMessage mess);

        std::vector<int> GetUserConversations(parsedMessage mess);
        std::pair<std::vector<int>, std::vector<int>> GetMessegesIDs(parsedMessage mess);
        std::vector<int> GetInvitationsUsersIDs(parsedMessage mess);

        std::string GetUserNickname(parsedMessage mess);
        std::string GetMessageByID(parsedMessage mess);    
};
