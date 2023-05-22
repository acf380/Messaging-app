#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stack>
#include <utility>
#include <sys/ioctl.h>
#include <errno.h>

#include "ServerCommands.h"
#include "Logger.h"


#define NUMBER_OF_USERS 1000
#define READ_BUFFOR_SIZE 603

class Server {
private:
    int sfd, cfd, fdmax, fds, rc, i, on = 1;
    socklen_t slt;
    struct sockaddr_in saddr, caddr;
    struct timeval timeout;

    fd_set rmask, wmask, rmaskS, wmaskS;
    fd_set users[NUMBER_OF_USERS];
    
    std::stack<int> usersIDs;
    ServerCommands* commands;

    char buf[READ_BUFFOR_SIZE];

    void setupServer();
    void handleNewUser();
    void handleWriteToClient();
    void handleReadFromClient();

public:
    Server();
    ~Server();
    void run();
};