# klasa zajmuje się widokiem konwersacji 

import codes, functools
from PySide6.QtWidgets import QWidget
from Conversations.ui_form import Ui_Widget
from PySide6.QtCore import Signal, QThreadPool, QWaitCondition, QMutex, Slot, QRunnable, QObject


waitConditionI, waitConditionC = QWaitCondition(), QWaitCondition()
mutexI, mutexC = QMutex(), QMutex()
condI, condC = True, True


class WorkerSignals(QObject):
    finished = Signal()  
    done = Signal()

class Bg_update_inv(QRunnable): # dummy thread - gdy się budzi, robi update dla invitations 
    def __init__(self):
        super(Bg_update_inv, self).__init__()
        self.signals = WorkerSignals()

    @Slot()
    def run(self):
        global condI
        while condI:
            mutexI.lock()
            waitConditionI.wait(mutexI)
            mutexI.unlock()  
            if condI:
                self.signals.done.emit()
        return

class Bg_update_conv(QRunnable): # dummy thread - gdy się budzi, robi update dla conversations
    def __init__(self):
        super(Bg_update_conv, self).__init__()
        self.signals = WorkerSignals()

    @Slot()
    def run(self):
        global condC
        while condC:
            mutexC.lock()
            waitConditionC.wait(mutexC)
            mutexC.unlock()  
            if condC:
                self.signals.done.emit()
        return


class Conversations(QWidget):
    switch_to_messeging = Signal()

    def __init__(self, parent=None):
        super().__init__(parent)
        self.ui = Ui_Widget()
        self.ui.setupUi(self)
        self.ui.inv_but.clicked.connect(self.__add_invitation)
        self.__threadpoll = QThreadPool()
        self.__bg_update_inv = Bg_update_inv()
        self.__bg_update_conv = Bg_update_conv()

    def closeEvent(self, event): # obsługa zamknięcia okna
        global condI, condC
        condI, condC = False, False
        
        waitConditionI.wakeAll()
        waitConditionC.wakeAll()
        
        del self.__bg_update_inv
        del self.__bg_update_conv

        self.close()

    def setup(self, client, user, rwlock): # pierwsze uruchomienie
        self.__client = client
        self.__client.set_conversations_class(self)
        self.__user = user
        self.__rwlock = rwlock
        self.__conversations_buttons = None


        self.__bg_update_inv.signals.done.connect(self.__update_invitations_tab)
        self.__bg_update_conv.signals.done.connect(self.__update_conversations_tab)

        self.__threadpoll.start(self.__bg_update_inv)
        self.__threadpoll.start(self.__bg_update_conv)

        self.__update_conversations_tab()
        self.__update_invitations_tab()

    def __update_conversations_tab(self): 
        self.__load_conversators_ids()
        self.__load_conversations_names()
        self.__setup_conversations_buttons()
        return 

    def __update_invitations_tab(self):
        self.__load_invitators_ids()
        self.__load_invitators_names()
        self.__setup_invitations_buttons()
        return 

    def __load_conversators_ids(self): # ładuje id konwersacji
        self.__client.get_conversators_ids()

        while True:
            with self.__rwlock.gen_rlock():
                for i, resp in enumerate(self.__user.responses):
                    if chr(resp[0]) == codes.CODE_GET_CONVERSATORS_IDS.decode("ascii"):
                        self.__user.conversators_ids = [b for b in resp[1:].split(b';') if (b != b'' and b != self.__user.id)]
                        del self.__user.responses[i]
                        return 


    def __load_invitators_ids(self): # ładuje id zaproszeń
        self.__client.get_invitators_ids()

        while True:
            with self.__rwlock.gen_rlock():
                for i, resp in enumerate(self.__user.responses):
                    if chr(resp[0]) == codes.CODE_GET_INVITATIONS_USERS_IDS.decode("ascii"):
                        self.__user.invitators_ids = resp[1:].split(b';')[1:]
                        del self.__user.responses[i]
                        return  
        

    def __load_conversations_names(self): # ładuje nazwy konwersacji używając poprzednio załadowanych id konwersacji
        self.__user.conversators_nicks = []
        for id in self.__user.conversators_ids:
            if id != b'':
                self.__client.get_user_nick(id)
                cond = True
                while cond:
                    with self.__rwlock.gen_rlock():
                        for i, resp in enumerate(self.__user.responses):
                            if chr(resp[0]) == codes.CODE_GET_USER_NICKNAME.decode("ascii"):
                                self.__user.conversators_nicks.append(resp[1:])
                                del self.__user.responses[i]
                                cond = False
                                break


    def __load_invitators_names(self): # ładuje nazwy użytkowników używając poprzednio załadowanych id zaproszeń
        self.__user.invitators_nicks = []
        for id in self.__user.invitators_ids:
            if id != b'':
                self.__client.get_user_nick(id)
                cond = True
                while cond:
                    with self.__rwlock.gen_rlock():
                        for i, resp in enumerate(self.__user.responses):
                            if chr(resp[0]) == codes.CODE_GET_USER_NICKNAME.decode("ascii"):
                                self.__user.invitators_nicks.append(resp[1:])
                                del self.__user.responses[i]
                                cond = False
                                break


    def __setup_conversations_buttons(self):
        self.__user.conversators_nicks = [nick.decode('utf-8') for nick in self.__user.conversators_nicks]
        self.__conversations_buttons = self.ui.add_person_buttons(self.__user.conversators_nicks)
        for i in range(len(self.__conversations_buttons)):
            self.__conversations_buttons[i].clicked.connect(functools.partial(self.__conversation_button_onclick, i))

    
    def __setup_invitations_buttons(self):
        self.__user.invitators_nicks = [nick.decode('utf-8') for nick in self.__user.invitators_nicks]
        self.__inv_buttons = self.ui.add_dialog_buttons(self.__user.invitators_nicks)

        for i in range(len(self.__inv_buttons)):
            self.__inv_buttons[i][1].clicked.connect(functools.partial(self.__accept_invitation, self.__inv_buttons[i], i))
            self.__inv_buttons[i][2].clicked.connect(functools.partial(self.__decline_invitation, self.__inv_buttons[i], i))


    def __conversation_button_onclick(self, i):
        self.__user.current_messaging = self.__user.conversators_ids[i]
        self.hide()
        self.switch_to_messeging.emit()

    def __accept_invitation(self, button, i): 
        self.__client.accept_invitation(self.__user.invitators_ids[i])

        button[0].deleteLater()
        button[1].deleteLater()
        button[2].deleteLater()

        def check_response():
            while True:
                with self.__rwlock.gen_rlock():
                    for i, resp in enumerate(self.__user.responses):
                        if chr(resp[0]) == codes.CODE_ACCEPT_INVITATOIN.decode("ascii"):
                            if resp[1:] == b"SUCCESSFUL":
                                del self.__user.responses[i]
                                return True
                            del self.__user.responses[i]
                            return False
        
        if check_response():
            self.__update_conversations_tab()

    def __decline_invitation(self, button, i):
        print(self.__user.invitators_nicks[i], self.__user.invitators_ids[i], button[0].text())
        self.__client.decline_invitation(self.__user.invitators_ids[i])

        button[0].deleteLater()
        button[1].deleteLater()
        button[2].deleteLater()

        while True:
            with self.__rwlock.gen_rlock():
                for i, resp in enumerate(self.__user.responses):
                    if chr(resp[0]) == codes.CODE_DECLINE_INVITATOIN.decode("ascii"):
                        if resp[1:] == b"SUCCESSFUL":
                            del self.__user.responses[i]
                            return True
                        del self.__user.responses[i]
                        return False

    def __add_invitation(self):
        id = self.ui.idInputInvite.text()
        self.__client.add_invitation(id)

        self.ui.nickInputInvite.setText("")
        self.ui.idInputInvite.setText("")

    def force_update_invitations(self): # funkcja do obudzenia dummy thread na prośbę serwera
        waitConditionI.wakeAll()

    def force_update_conversations(self): # funkcja do obudzenia dummy thread na prośbę serwera
        waitConditionC.wakeAll()