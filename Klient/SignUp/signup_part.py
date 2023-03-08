import time, codes
from PySide6.QtWidgets import QWidget
from PySide6 import QtCore
from SignUp.ui_form import Ui_Widget


class SignUp(QWidget):
    switch_to_login = QtCore.Signal()

    def __init__(self, parent=None):
        super().__init__(parent)
        self.ui = Ui_Widget()
        self.ui.setupUi(self)
        self.ui.pushButton.clicked.connect(self.__go_back)
        self.ui.pushButton_2.clicked.connect(self.__sign_in)

    def setup(self, client, user, rwlock):
        self.__clinet = client
        self.__rwlock = rwlock
        self.__user = user

    def __go_back(self): # powrót do ekranu logowania
        self.close()
        self.switch_to_login.emit()
        
    def __sign_in(self): # wysyła dane do rejestracji
        nick = self.ui.lineEdit.text()
        login = self.ui.lineEdit_2.text()
        password = self.ui.lineEdit_3.text()

        self.__clinet.add_user(nick, login, password) 

        if self.__check_server_response():
            self.__go_back()

    def __check_server_response(self): # szuka i analizuje odpowiedź serwera 
        while True:
            with self.__rwlock.gen_rlock():
                for i, resp in enumerate(self.__user.responses):
                    if chr(resp[0]) == codes.CODE_ADD_USER.decode("ascii"):
                        if resp[1:] == b"SUCCESSFUL":
                            print("New user added")
                            del self.__user.responses[i]
                            return True
                        else:
                            print("ERROR")
                            del self.__user.responses[i]
                            return False
            time.sleep(0.1)


            
        

    