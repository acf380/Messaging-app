#include "Server.h"


Server::Server() {
  commands = new ServerCommands;
  for (int i=0; i<NUMBER_OF_USERS; i++) usersIDs.push(i);

  try {
    setupServer();
  } catch (const std::runtime_error & e) {
    LogMessage(e.what(), 'e');
    exit(EXIT_FAILURE);
  }
}

Server::~Server() {
  delete commands;
  close(sfd);
}

void Server::setupServer() {
  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = inet_addr("0.0.0.0");
  saddr.sin_port = htons(1234);

  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1) {
    std::string mess = "ERROR: socket returned -1.";
    throw std::runtime_error(mess);
  }

  if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (char * ) & on, sizeof(on)) == -1) {
    std::string mess = "ERROR: setsockopt: " + (std::string) strerror(errno) + ".";
    throw std::runtime_error(mess);
  }

  if (bind(sfd, (struct sockaddr * ) & saddr, sizeof(saddr)) == -1) {
    std::string mess = "ERROR: bind: " + (std::string) strerror(errno) + ".";
    throw std::runtime_error(mess);
  }

  if (listen(sfd, 32) == -1) {
    std::string mess = "ERROR: listen: " + (std::string) strerror(errno) + ".";
    throw std::runtime_error(mess);
  }

  if (ioctl(sfd, FIONBIO, (char * ) & on) == -1) {
    std::string mess = "ERROR: ioctl: " + (std::string) strerror(errno) + ".";
    throw std::runtime_error(mess);
  }

  FD_ZERO( & rmask);
  FD_ZERO( & wmask);
  FD_ZERO( & rmaskS);
  FD_ZERO( & wmaskS);
  for (int i = 0; i < NUMBER_OF_USERS; i++) FD_ZERO( & users[i]);

  fdmax = sfd;
}

void Server::handleNewUser() {
  fds -= 1;
  slt = sizeof(caddr);
  cfd = accept(sfd, (struct sockaddr * ) & caddr, & slt);

  if (cfd < 0) {
    if (errno != EWOULDBLOCK) {
      char * mess = "accept failed. Exiting...";
      LogMessage(mess, 'i');
      delete commands;
      close(sfd);
      throw std::runtime_error(mess);
    }
  }

  FD_SET(cfd, & rmask);
  int id = usersIDs.top(); // nadajemy mu id maski
  usersIDs.pop();

  FD_SET(cfd, & users[id]); // przypisujemy maskę użytkownika do cfd

  std::string mess = "new user connected! descriptor: " + std::to_string(cfd) + ", id: " + std::to_string(id) + "\n";
  LogMessage(mess.c_str(), 'i');

  if (cfd > fdmax) fdmax = cfd;
}

void Server::handleReadFromClient() {
  fds -= 1;
  int retRead = 0;
  int temp = 0;

  while (retRead != READ_BUFFOR_SIZE) {
    temp = read(i, buf + retRead, READ_BUFFOR_SIZE - retRead);
    if (temp == 0 || temp == -1) break;
    retRead += temp;
  }

  if (retRead < READ_BUFFOR_SIZE) {
    int idToDel = -1;
    for (int j = 0; j < NUMBER_OF_USERS; j++) {
      if (FD_ISSET(i, & users[j])) {
        idToDel = j;
        break;
      }
    }

    if (idToDel != -1) {
      FD_CLR(i, & rmask);
      FD_CLR(i, & users[idToDel]);
      usersIDs.push(idToDel);

    }

    commands -> RemoveIDFromMap(idToDel);

    LogMessage("user disconnected", 'i');
  } else {
    FD_CLR(i, & rmask);
    FD_SET(i, & wmask);
    int id = -1;
    for (int j = 0; j < NUMBER_OF_USERS; j++) {
      if (FD_ISSET(i, & users[j])) {
        id = j;
        break;
      }
    }

    commands -> ParseMessage(buf); // parsujemy wiadomość użytkownika
    commands -> ExecuteFunction(id); // wykonujemy odpowiednią akcję

    /*
        Poniższa funkcja sprawdza czy należy poinformować innego użytkownika o wyniku operacji.
        Przykład:
        Użytkownik A wysyła do użytkownika B wiadomość
        A - dostanie wiadomość zwrotną o powodzeniu/niepowodzeniu operacji
        B - jeśli A doda wiadomość do bazy danych dostanie informacje, że powinien doładować ostatnią wiadomość 

        ustawiamy więc, że B jest gotowy do czytania
    */
    int additionalReceiver = commands -> CheckForAdditionalReceiver();
    if (additionalReceiver != -1) {
      int idA = -1;
      for (int j = sfd + 1; j <= fdmax; j++) {
        if (FD_ISSET(j, & users[additionalReceiver])) {
          idA = j;
          break;
        }
      }
      if (idA == -1) LogMessage("ERROR", 'e');
      else FD_SET(idA, & wmask);
    }
  }

  if (i == fdmax)
    while (fdmax > sfd && !FD_ISSET(fdmax, & wmask))
      fdmax -= 1;
}

void Server::handleWriteToClient() {
  LogMessage("writing", 'i');
  fds -= 1;
  int id = -1;
  for (int j = 0; j < NUMBER_OF_USERS; j++) // szukamy użytkownika, który czeka na wiadomość
  {
    if (FD_ISSET(i, & users[j])) {
      id = j;
      break;
    }
  }

  std::vector < std::pair < char * , int >> responseData = commands -> GetServerResponse(id); // wyciągamy odpowiedzi serwera

  for (auto ele: responseData) // przesyłamy odpowiedzi
  {
    char * response = ele.first;
    int responseSize = ele.second;
    int retWrite = 0;
    int temp = 0;

    while (retWrite != responseSize) {
      temp = write(i, response + retWrite, responseSize - retWrite);
      if (temp == 0 || temp == -1) break;
      retWrite += temp;
    }

    LogMessage("writing done", 'i');

    if (retWrite < responseSize) // stracono połączenie z klientem
    {
      FD_CLR(i, & rmask);
      FD_CLR(i, & users[id]);
      usersIDs.push(id); // zwracamy id maski w celu ponownego jej użycia w przyszłości

      commands -> RemoveIDFromMap(id);
      LogMessage("user disconnected", 'i');

      break;
    }

  }

  FD_CLR(i, & wmask);
  FD_SET(i, & rmask);

  if (i == fdmax)
    while (fdmax > sfd && !FD_ISSET(fdmax, & rmask))
      fdmax -= 1;
}

void Server::run() {
  while (1) {
    timeout.tv_sec = 5 * 60;
    timeout.tv_usec = 0;

    rmaskS = rmask;
    wmaskS = wmask;
    FD_SET(sfd, & rmaskS);

    rc = select(fdmax + 1, & rmaskS, & wmaskS, (fd_set * ) 0, & timeout);
    if (rc == -1) {
      std::string mess = "ERROR: select: " + (std::string) strerror(errno) + ". Exiting...";
      throw std::runtime_error(mess);
    }
    if (rc == 0) {
      LogMessage("timed out", 'i');
      continue;
    }
    fds = rc;

    if (FD_ISSET(sfd, & rmaskS)) {
      try {
        handleNewUser();
      } catch (const std::runtime_error & e) {
        LogMessage(e.what(), 'e');
        exit(EXIT_FAILURE);
      }

    }

    for (i = sfd + 1; i <= fdmax; i++) {
      if (FD_ISSET(i, & wmaskS)) // pisanie do klienta
      {

        try {
          handleWriteToClient();
        } catch (const std::runtime_error & e) {
          LogMessage(e.what(), 'e');
          exit(EXIT_FAILURE);
        }
      }

      if (FD_ISSET(i, & rmaskS)) {
        try {
          handleReadFromClient();
        } catch (const std::runtime_error & e) {
          LogMessage(e.what(), 'e');
          exit(EXIT_FAILURE);
        }
      }
    }
  }
}