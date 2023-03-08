# przełączanie pomiędzy widokami oraz start nasłuchującego wątku

import sys, threading, functools, os
from readerwriterlock import rwlock
from PySide6 import QtWidgets
from client import Client

from Login.login_part import Login
from SignUp.signup_part import SignUp
from Conversations.conversations_part import Conversations
from Messaging.messaging_part import Messaging
from user_info import User

class Controller:

    def __init__(self):
        self.user = User()
        self.rwlock = rwlock.RWLockFairD() # problem czytelników - pisarzy 

        self.client = Client("192.168.1.126", 1234, self.user, self.rwlock) # klasa odpowiedzialna za komunikacje

        self.listen_thread = threading.Thread(target=self.client.listen)
        self.listen_thread.start()

        self.login, self.signup, self.conv, self.mess = None, None, None, None

    def __del__(self):
        try:
            self.client.exit_listen()
        except Exception as e:
            print(f"ERROR: {e}!")
        try:
            del self.login
        except Exception as e:
            print(f"ERROR: {e}!")
        try:
            del self.conv
        except Exception as e:
            print(f"ERROR: {e}!")
        try:
            del self.signup
        except Exception as e:
            print(f"ERROR: {e}!")
        try:
            del self.mess
        except Exception as e:
            print(f"ERROR: {e}!")

    def show_login(self): # generowanie strony logowania
        self.login = Login()
        self.login.setup(self.client, self.user, self.rwlock)
        self.login.switch_to_signup.connect(self.show_signup)
        self.login.switch_to_conversations.connect(self.show_conversations)
        self.login.show()

    def show_signup(self): # generowanie strony rejestracji
        self.signup = SignUp()
        self.signup.setup(self.client, self.user, self.rwlock)
        self.signup.switch_to_login.connect(self.show_login)
        self.signup.show()

    def show_conversations(self, mess=False): # generowanie konwersacji
        if not mess:
            self.conv = Conversations()
            self.conv.setup(self.client, self.user, self.rwlock)
            self.conv.switch_to_messeging.connect(self.show_messeging)
        else:
            del self.mess
        self.conv.show()

    def show_messeging(self): # generowanie strony konkretnej rozmowy
        self.mess = Messaging()
        self.mess.setup(self.client, self.user, self.rwlock)
        self.mess.switch_to_conversations.connect(functools.partial(self.show_conversations, mess=True))
        self.mess.show()


def main():
    app = QtWidgets.QApplication(sys.argv)
    controller = Controller()
    controller.show_login()
    app.exec() 
    del controller
    os._exit(1)


if __name__ == '__main__':
    main()