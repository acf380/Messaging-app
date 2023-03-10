// SERWER OPARTY NA SELECT

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
#include "Logger.cpp"


#define NUMBER_OF_USERS 1000
#define READ_BUFFOR_SIZE 603


int main()
{
    LogMessage("trying to initialize variables", 'i');

    int sfd, cfd, fdmax, fds, rc, i, on = 1;
    socklen_t slt;
    struct sockaddr_in saddr, caddr;
    static struct timeval timeout;

    fd_set rmask, wmask, rmaskS, wmaskS;
    fd_set users[NUMBER_OF_USERS]; // masks that "mapps" user to descriptor
    
    std::stack<int> usersIDs; // stores unused indexes users buffor
    for (int i=0; i<NUMBER_OF_USERS; i++) usersIDs.push(i);

    ServerCommands* commands = new ServerCommands; // initialize main class

    char buf[READ_BUFFOR_SIZE];  // buffor for read

    LogMessage("done", 'd');
    LogMessage("trying to setup server", 'i');
    
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr("0.0.0.0");
    saddr.sin_port = htons(1234);
    
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) 
    {
        LogMessage("ERROR: socket returned -1. Exiting...", 'e');
        return -1;
    }

    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) == -1)
    {
        std::string mess = "ERROR: setsockopt: " + (std::string)strerror(errno) + ". Exiting...";
        LogMessage(mess.c_str(), 'e'); 
        return -1;
    }
    
    if (bind(sfd, (struct sockaddr*)&saddr, sizeof(saddr)) == -1)
    {
        std::string mess = "ERROR: bind: " + (std::string)strerror(errno) + ". Exiting...";
        LogMessage(mess.c_str(), 'e'); 
        return -1;
    }

    if (listen(sfd, 32) == -1)
    {
        std::string mess = "ERROR: listen: " + (std::string)strerror(errno) + ". Exiting...";
        LogMessage(mess.c_str(), 'e'); 
        return -1;
    }

    if (ioctl(sfd, FIONBIO, (char *)&on) == -1)
    {
        std::string mess = "ERROR: ioctl: " + (std::string)strerror(errno) + ". Exiting...";
        LogMessage(mess.c_str(), 'e'); 
        return -1;
    }

    FD_ZERO(&rmask);
    FD_ZERO(&wmask);
    FD_ZERO(&rmaskS);
    FD_ZERO(&wmaskS);
    for (int i=0; i<NUMBER_OF_USERS; i++) FD_ZERO(&users[i]);
    
    fdmax = sfd;

    LogMessage("done", 'd');

    
    while(1) // g??owna p??tla serwera
    {        
        timeout.tv_sec = 5*60;
        timeout.tv_usec = 0;

        rmaskS = rmask;
        wmaskS = wmask;
        FD_SET(sfd, &rmaskS); 

        rc = select(fdmax+1, &rmaskS, &wmaskS, (fd_set*)0, &timeout);
        if (rc == -1)
        {
            std::string mess = "ERROR: select: " + (std::string)strerror(errno) + ". Exiting...";
            LogMessage(mess.c_str(), 'e'); 
            return -1;
        }
        if(rc == 0)
        {
            LogMessage("timed out", 'i');
            continue;
        }
        fds = rc;

        if(FD_ISSET(sfd, &rmaskS)) // nowy u??ytkownik pr??buje dostac si?? do serwera
        {
            fds -= 1;
            slt = sizeof(caddr);
            cfd = accept(sfd, (struct sockaddr*)&caddr, &slt);

            if (cfd < 0)
            {
                if (errno != EWOULDBLOCK)
                {
                    LogMessage("accept failed. Exiting...", 'i');
                    delete commands;
                    close(sfd);
                    return -1;
                }
            }

            FD_SET(cfd, &rmask); 
            int id = usersIDs.top(); // nadajemy mu id maski
            usersIDs.pop();

            FD_SET(cfd, &users[id]); // przypisujemy mask?? u??ytkownika do cfd

            std::string mess = "new user connected! descriptor: " + std::to_string(cfd) + ", id: " + std::to_string(id) + "\n";
            LogMessage(mess.c_str(), 'i');

            if(cfd > fdmax) fdmax = cfd;
        }

        for(i=sfd+1; i <= fdmax; i++)
        {
            if(FD_ISSET(i, &wmaskS)) // pisanie do klienta
            {
                LogMessage("writing", 'i');
                fds -= 1;
                int id = -1;
                for (int j = 0; j < NUMBER_OF_USERS; j++) // szukamy u??ytkownika, kt??ry czeka na wiadomo????
                {
                    if (FD_ISSET(i, &users[j]))
                    {
                        id = j;
                        break;
                    }
                }

                printf("writing to %d\n", id);

                std::vector<std::pair<char*, int>> responseData = commands->GetServerResponse(id); // wyci??gamy odpowiedzi serwera

                for (auto ele:responseData) // przesy??amy odpowiedzi
                {
                    printf("wysy??am: %d\n", ele.first[0]);

                    char* response = ele.first;
                    int responseSize = ele.second; 
                    int retWrite = 0; 
                    int temp = 0;

                    while (retWrite != responseSize) 
                    {
                        temp = write(i, response + retWrite, responseSize - retWrite);
                        if(temp == 0 || temp == -1) break;
                        retWrite += temp;
                    }

                    LogMessage("writing done", 'i');

                    if (retWrite < responseSize) // stracono po????czenie z klientem
                    {
                        FD_CLR(i, &rmask);
                        FD_CLR(i, &users[id]);
                        usersIDs.push(id); // zwracamy id maski w celu ponownego jej u??ycia w przysz??o??ci

                        commands->RemoveIDFromMap(id);
                        LogMessage("user disconnected", 'i');

                        break;
                    }

                }

                FD_CLR(i, &wmask);
                FD_SET(i, &rmask);

                if(i == fdmax)
                    while(fdmax > sfd && !FD_ISSET(fdmax, &rmask))
                        fdmax -= 1;
                continue;
                
            }

            if(FD_ISSET(i, &rmaskS))
            {
                LogMessage("reading", 'i');

                fds -= 1;
                int retRead = 0;
                int temp = 0;

                while (retRead != 603) 
                {
                    temp = read(i, buf + retRead, 603 - retRead);
                    if(temp == 0 || temp == -1) break;
                    retRead += temp;
                 }
                
                if (retRead < 603) 
                {
                    int idToDel = -1;
                    for (int j = 0; j < NUMBER_OF_USERS; j++)
                    {
                        if (FD_ISSET(i, &users[j]))
                        {
                            idToDel = j;
                            break;
                        }
                    }

                    if (idToDel != -1)
                    {
                        FD_CLR(i, &rmask);
                        FD_CLR(i, &users[idToDel]);
                        usersIDs.push(idToDel);
                        
                    }

                    commands->RemoveIDFromMap(idToDel);

                    LogMessage("user disconnected", 'i');
                }
                else 
                {
                    LogMessage("reading done", 'i');
                    
                    FD_CLR(i, &rmask);
                    FD_SET(i, &wmask);
                    int id = -1;
                    for (int j = 0; j < NUMBER_OF_USERS; j++)
                    {
                        if (FD_ISSET(i, &users[j]))
                        {
                            id = j;
                            break;
                        }
                    }

                    commands->ParseMessage(buf); // parsujemy wiadomo???? u??ytkownika
                    commands->ExecuteFunction(id); // wykonujemy odpowiedni?? akcj??

                    /*
                        Poni??sza funkcja sprawdza czy nale??y poinformowa?? innego u??ytkownika o wyniku operacji.
                        Przyk??ad:
                        U??ytkownik A wysy??a do u??ytkownika B wiadomo????
                        A - dostanie wiadomo???? zwrotn?? o powodzeniu/niepowodzeniu operacji
                        B - je??li A doda wiadomo???? do bazy danych dostanie informacje, ??e powinien do??adowa?? ostatni?? wiadomo???? 

                        ustawiamy wi??c, ??e B jest gotowy do czytania
                    */
                    int additionalReceiver = commands->CheckForAdditionalReceiver();
                    if (additionalReceiver != -1) 
                    {
                        int idA = -1;
                        for (int j = sfd+1; j <= fdmax; j++)
                        {
                            if (FD_ISSET(j, &users[additionalReceiver]))
                            {
                                idA = j;
                                break;
                            }
                        }
                        if (idA == -1) puts("ERRROR!");
                        else FD_SET(idA, &wmask);   
                    }
                }
                
                if(i == fdmax)
                    while(fdmax > sfd && !FD_ISSET(fdmax, &wmask))
                        fdmax -= 1;
            }
        }
    }

    delete commands;
    close(sfd);
    return EXIT_SUCCESS;
}