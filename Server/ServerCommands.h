#include "DBCommands.h"
#include <sys/select.h>

class ServerCommands
    : DBCommands
{
    private:
        
        int buffSize = 603;
        std::unordered_map<int, std::vector<std::string>> bufforServerUser; // MAPOWANIE ID UŻYTKOWNIKA Z SERWERA DO VEKTORA BUFORÓW Z WIADOMOŚCIAMI
        std::unordered_map<std::string, int> bufforDatabaseServer; // MAPOWANIE ID Z BAZY DANYCH DO ID Z SERWERA
        std::unordered_map<int, std::string> bufforServerDatabase; // MAPOWANIE ID Z SERWERA DO ID Z BAZY DANYCH

        parsedMessage messStruct;
        
        int additionalReceiver = -1;

    public:
        ServerCommands();
        ~ServerCommands();

        void ParseMessage(char buffor[]);
        void ExecuteFunction(int id);
        
        int CheckForAdditionalReceiver();

        int RemoveIDFromMap(int id);

        std::vector<std::pair<char*, int>> GetServerResponse(int id);
};