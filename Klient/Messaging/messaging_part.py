# klasa odpowiedzialna za widok wiadomości 

import codes
from PySide6.QtWidgets import QWidget
from Conversations.ui_form import Ui_Widget
from PySide6.QtCore import Signal, QThreadPool, QWaitCondition, QMutex, Slot, QRunnable, QObject

from Messaging.ui_form import Ui_Widget

waitCondition = QWaitCondition()
mutex = QMutex()
cond= True


class WorkerSignals(QObject):  
    finished = Signal()  
    done = Signal()

class Bg_update_mess(QRunnable): # dummy thread startuje doładowanie ostatniej wiadomości po wybudzeniu
    def __init__(self):
        super(Bg_update_mess, self).__init__()
        self.signals = WorkerSignals()

    @Slot()
    def run(self):
        global cond
        while cond:
            mutex.lock()
            waitCondition.wait(mutex)
            mutex.unlock()  
            if cond:
                self.signals.done.emit()
        return



class Messaging(QWidget):
    switch_to_conversations = Signal()

    def __init__(self, parent=None):
        super().__init__(parent)
        self.ui = Ui_Widget()
        self.ui.setupUi(self)
        self.ui.BackPushButton.clicked.connect(self.__go_back)
        self.ui.SendButton.clicked.connect(self.__add_new_message)
        self.__messages_ids = [[], []]
        self.__messages_data = [[], []]
        self.__threadpoll = QThreadPool()
        self.__bg_update_mess = Bg_update_mess()
        self.__last = None

    def closeEvent(self, event): # obsługa zamknięcia okna
        global cond
        cond = False
        waitCondition.wakeAll()
        
        del self.__bg_update_mess

        self.close()
    
    def setup(self, client, user, rwlock):
        global cond
        cond = True

        self.__client = client
        self.__client.set_messaging_class(self)
        self.__user = user
        self.__rwlock = rwlock

        self.__bg_update_mess.signals.done.connect(self.__load_last_message)
        self.__threadpoll.start(self.__bg_update_mess)

        self.__update_messages()

    def __go_back(self): # powrót do wszystkich konwerasji 
        global cond
        cond = False
        waitCondition.wakeAll()

        self.__user.current_messaging = None
        self.switch_to_conversations.emit()
        self.close()

    def __update_messages(self):
        self.__load__messages_ids()
        self.__load_messages_data()
        self.__show_messages()

    def __load__messages_ids(self): # ładuje id wszystkich wiadomości z konwersaji
        self.__client.get_messages_ids(self.__user.current_messaging)

        self.__messages_ids = [[], []]

        while True:
            with self.__rwlock.gen_rlock():
                for i, resp in enumerate(self.__user.responses):
                    if chr(resp[0]) == codes.CODE_GET_MESSAGES_IDS.decode("ascii"):
                        x, y = resp[1:].split(codes.CODE_GET_MESSAGES_IDS)
                        self.__messages_ids[0] = [b for b in x.split(b";") if b != b'']
                        self.__messages_ids[1] = [b for b in y.split(b";") if b != b'']
                        del self.__user.responses[i]
                        return 

    def __load_messages_data(self): # ładuje wszystkie wiadomości dla konwersacji używając załadowanych wcześniej id
        self.__messages_data = [[], []]
        self.__messages_ids[0] = sorted(self.__messages_ids[0], key=int)
        self.__messages_ids[1] = sorted(self.__messages_ids[1], key=int)

        x, y = self.__messages_ids
        for id in  x:
            if id != b'':
                self.__client.get_message_by_id(id)
                cond = True
                while cond:
                    with self.__rwlock.gen_rlock():
                        for i, resp in enumerate(self.__user.responses):
                            if chr(resp[0]) == codes.CODE_GET_MESSAGE_BY_ID.decode("ascii"):
                                self.__messages_data[0].append(resp[1:])
                                del self.__user.responses[i]
                                cond = False
                                break

        for id in  y:
            if id != b'':
                self.__client.get_message_by_id(id)
                cond = True
                while cond:
                    with self.__rwlock.gen_rlock():
                        for i, resp in enumerate(self.__user.responses):
                            if chr(resp[0]) == codes.CODE_GET_MESSAGE_BY_ID.decode("ascii"):
                                self.__messages_data[1].append(resp[1:])
                                del self.__user.responses[i]
                                cond = False
                                break

    def __load_last_message(self): # doładowywuje ostatinią wiadomość na prośbę serwera
        self.__client.get_message_by_id(self.__last)
        mess = None
        received = False
        while not received:
            with self.__rwlock.gen_rlock():
                for i, resp in enumerate(self.__user.responses):
                    if chr(resp[0]) == codes.CODE_GET_MESSAGE_BY_ID.decode("ascii"):
                        mess = resp[1:]
                        del self.__user.responses[i]
                        received = True

        self.ui.add_receiver_label(mess.decode('utf-8'))

    def __show_messages(self):
        sc, rc = 0, 0
        while sc < len(self.__messages_ids[0]) and rc < len(self.__messages_ids[1]):
            if int(self.__messages_ids[0][sc].decode('utf-8')) < int(self.__messages_ids[1][rc].decode('utf-8')):
                self.ui.add_sender_label(self.__messages_data[0][sc].decode('utf-8'))
                sc += 1
            else:
                self.ui.add_receiver_label(self.__messages_data[1][rc].decode('utf-8'))
                rc +=1

        if sc < len(self.__messages_ids[0]):
            for i in range(sc, len(self.__messages_ids[0])):
                self.ui.add_sender_label(self.__messages_data[0][i].decode('utf-8'))
        
        elif rc < len(self.__messages_ids[1]):
            for i in range(rc, len(self.__messages_ids[1])):
                self.ui.add_receiver_label(self.__messages_data[1][i].decode('utf-8'))
                rc = i

                
    def __add_new_message(self): # obsługuje wysyłanie wiadomości
        text = self.ui.SendingMessageLineEdit.text()
        if text and len(text) < 512:
            self.__client.add_message(self.__user.current_messaging, text)

            def check_response():
                while True:
                    with self.__rwlock.gen_rlock():
                        for i, resp in enumerate(self.__user.responses):
                            if chr(resp[0]) == codes.CODE_ADD_MESSAGE.decode("ascii"):
                                if resp[1:] == b"SUCCESSFUL":
                                    del self.__user.responses[i]
                                    return True
                                del self.__user.responses[i]
                                return False
        
        if check_response():
            self.ui.add_sender_label(text)
            
    def force_update_messages(self, id): # wybudza dummy thread na prośbę serwera
        self.__last = id
        waitCondition.wakeAll()


    
