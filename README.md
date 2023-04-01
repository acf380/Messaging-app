# Messaging-app
The Server must be run linux!
Server directory:
  Make sure that the database exists. 
  If not, run the command "g++ create_database.cpp -o database -l sqlite3". Then, run database to create the database.
  To compile the server code, use the command "g++ -Wall Server.cpp DBCommands.cpp ServerCommands.cpp -o gg -l sqlite3". Run gg.

Client has been tested on Windows, but you can try on Linux!
Client directory: 
  You need to set proper server ip in Controller.py
  python3 Controller.py
Note that the server supports adding friends. In the current version, we need to know the ID from the database for the user we want to invite (yes, we know, it's silly - maybe nex update, ;-)).
