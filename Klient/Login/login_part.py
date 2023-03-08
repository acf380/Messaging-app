import codes, time
from PySide6.QtWidgets import QWidget
from PySide6 import QtCore
from Login.ui_form import Ui_Widget


class Login(QWidget):
    switch_to_conversations = QtCore.Signal()
    switch_to_signup = QtCore.Signal()

    def __init__(self, parent=None):
        super().__init__(parent)
        self.ui = Ui_Widget()
        self.ui.setupUi(self)
        self.ui.LogInButton.clicked.connect(self.__log_in)
        self.ui.pushButton.clicked.connect(self.__sign_up)

    def setup(self, client, user, rwlock):
        self.__client = client
        self.__user = user
        self.__rwlock = rwlock
        self.__logged_in = False

    def __sign_up(self): # przejście do ekranu rejestracji
        self.close()
        self.switch_to_signup.emit()

    def __log_in(self): # wysyła dane logowania na serwer
        login = self.ui.LoginLineEdit.text()
        password = self.ui.PasswordLineEdit.text()
        self.__client.send_login_data(login, password)

        self.__read_server_response()

        if self.__logged_in:
            self.close()
            self.switch_to_conversations.emit()

    
    def __read_server_response(self):  # szuka i analizuje odpowiedź serwera 
        while True:
            with self.__rwlock.gen_rlock():
                for i, resp in enumerate(self.__user.responses):
                    if chr(resp[0]) == codes.CODE_CHECK_LOGIN_DATA.decode("ascii"):
                        if resp[1:] != b"ERROR":
                            print("SUCCESS")
                            self.__user.id = resp[1:]
                            self.__logged_in = True
                        else:
                            print("ERROR")
                            self.ui.ErrorMessageLabel.setText("Wrong user or password!")
                        del self.__user.responses[i]
                    return 
            time.sleep(0.1)
    



         
