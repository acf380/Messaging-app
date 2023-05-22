#include "Server.h"

int main()
{   
    Server *server = new Server;

    
    try {
        server->run();
    } 
    catch (const std::runtime_error & e) {
        LogMessage(e.what(), 'e');
        exit(EXIT_FAILURE);
    }

    delete server;
}