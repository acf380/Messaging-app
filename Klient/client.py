import socket, time, codes, threading


class Client():
    def __init__(self, host, port, user, rwlock):
        self.__HOST = host
        self.__PORT = port
        self.__user = user
        self.__rwlock = rwlock
        self.__SOCKET = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.__keep_listen = True
        self.__conversations_class = None
        self.__messaging_class = None

        while not self.connect():
            time.sleep(2)
    
    def set_conversations_class(self, c):
        self.__conversations_class = c

    def set_messaging_class(self, c):
        self.__messaging_class = c

    # funkcjie tworzące odpowiednie buffory do komunikacji z serwerem
    def add_user(self, nick, login, password):
        buff = codes.CODE_ADD_USER                                          # type
        buff += b'\x00'*10                                                  # id  
        buff += bytes(nick, encoding="utf-8").ljust(20, b'\x00')            # nick
        buff += bytes(login, encoding="utf-8").ljust(20, b'\x00')           # login
        buff += bytes(password, encoding="utf-8").ljust(30, b'\x00')        # password
        buff += b'\x00'*10                                                  # receiver id
        buff += b'\x00'*512                                                 # message

        self.__send_request(buff)

    def add_message(self, conversator_id, message):
        buff = codes.CODE_ADD_MESSAGE                       # type
        buff += self.__user.id                              # id
        buff += b'\x00'*(10 - len(self.__user.id))   
        buff += b'\x00'*20                                  # nick
        buff += b'\x00'*20                                  # login
        buff += b'\x00'*30                                  # password
        buff += conversator_id                              # receiver id
        buff += b'\x00'*(10 - len(conversator_id))
        buff += bytes(message, encoding="utf-8")            # message
        buff += b'\x00'*(512 - len(bytes(message, encoding="utf-8"))) 

        self.__send_request(buff)

    def add_invitation(self, conversator_id):
        buff = codes.CODE_ADD_INVITATION                    # type
        buff += self.__user.id                              # id
        buff += b'\x00'*(10 - len(self.__user.id))   
        buff += b'\x00'*20                                  # nick
        buff += b'\x00'*20                                  # login
        buff += b'\x00'*30                                  # password
        buff += bytes(conversator_id, encoding="utf-8")     # receiver id
        buff += b'\x00'*(10 - len(bytes(conversator_id, encoding="utf-8"))) 
        buff += b'\x00'*512                                  # message

        self.__send_request(buff)

    def accept_invitation(self, conversator_id):
        buff = codes.CODE_ACCEPT_INVITATOIN                 # type
        buff += self.__user.id                              # id
        buff += b'\x00'*(10 - len(self.__user.id))   
        buff += b'\x00'*20                                  # nick
        buff += b'\x00'*20                                  # login
        buff += b'\x00'*30                                  # password
        buff += conversator_id                              # receiver id
        buff += b'\x00'*(10 - len(conversator_id)) 
        buff += b'\x00'*512                                  # message

        self.__send_request(buff)

    def decline_invitation(self, conversator_id):
        buff = codes.CODE_DECLINE_INVITATOIN                # type
        buff += self.__user.id                              # id
        buff += b'\x00'*(10 - len(self.__user.id))   
        buff += b'\x00'*20                                  # nick
        buff += b'\x00'*20                                  # login
        buff += b'\x00'*30                                  # password
        buff += conversator_id                              # receiver id
        buff += b'\x00'*(10 - len(conversator_id)) 
        buff += b'\x00'*512                                  # message

        self.__send_request(buff)

    def send_login_data(self, login, password):
        buff = codes.CODE_CHECK_LOGIN_DATA                                  # type
        buff += b'\x00'*10                                                  # id  
        buff += b'\x00'*20                                                  # nick
        buff += bytes(login, encoding="utf-8").ljust(20, b'\x00')           # login
        buff += bytes(password, encoding="utf-8").ljust(30, b'\x00')        # password
        buff += b'\x00'*10                                                  # receiver id
        buff += b'\x00'*512                                                 # message

        self.__send_request(buff)

    def get_conversators_ids(self):
        buff = codes.CODE_GET_CONVERSATORS_IDS          # type
        buff += self.__user.id                          # id
        buff += b'\x00'*(10 - len(self.__user.id))   
        buff += b'\x00'*20                              # nick
        buff += b'\x00'*20                              # login
        buff += b'\x00'*30                              # password
        buff += b'\x00'*10                              # receiver id
        buff += b'\x00'*512                             # message

        self.__send_request(buff)

    def get_user_nick(self, receiver_id):
        buff = codes.CODE_GET_USER_NICKNAME         # type
        buff += b'\x00'*10                          # id
        buff += b'\x00'*20                          # nick
        buff += b'\x00'*20                          # login
        buff += b'\x00'*30                          # password
        buff += receiver_id                         # receiver id
        buff += b'\x00'*(10 - len(receiver_id)) 
        buff += b'\x00'*512                         # message

        self.__send_request(buff)

    def get_messages_ids(self, conversator_id):
        buff = codes.CODE_GET_MESSAGES_IDS              # type
        buff += self.__user.id                          # id
        buff += b'\x00'*(10 - len(self.__user.id))   
        buff += b'\x00'*20                              # nick
        buff += b'\x00'*20                              # login
        buff += b'\x00'*30                              # password
        buff += conversator_id                          # receiver id
        buff += b'\x00'*(10 - len(conversator_id)) 
        buff += b'\x00'*512                             # message

        self.__send_request(buff)

    def get_message_by_id(self, conversator_id):
        buff = codes.CODE_GET_MESSAGE_BY_ID             # type                       
        buff += b'\x00'*10                              # id
        buff += b'\x00'*20                              # nick
        buff += b'\x00'*20                              # login
        buff += b'\x00'*30                              # password
        buff += conversator_id                          # receiver id
        buff += b'\x00'*(10 - len(conversator_id)) 
        buff += b'\x00'*512                             # message

        self.__send_request(buff)

    def get_invitators_ids(self):
        buff = codes.CODE_GET_INVITATIONS_USERS_IDS     # type
        buff += self.__user.id                          # id
        buff += b'\x00'*(10 - len(self.__user.id))   
        buff += b'\x00'*20                              # nick
        buff += b'\x00'*20                              # login
        buff += b'\x00'*30                              # password
        buff += b'\x00'*10                              # receiver id
        buff += b'\x00'*512                             # message

        self.__send_request(buff)

    # nawiązywanie połączenia z serwerem
    def connect(self):
        try:
            self.__SOCKET.connect((self.__HOST, self.__PORT))
            return True
        except Exception as e:
            print(f"ERROR WHILE CONNECTING: {e}")

        return False

    # wysyłanie do serwera
    def __send_request(self, buffor):
        bytes_sent = 0
        sz = len(buffor)
        while bytes_sent < sz:
            try:
                bytes_sent += self.__SOCKET.send(buffor)
            except Exception as e:
                print(f"ERROR: send: {e}!")                

        return True

    # czytanie z serwera do bajtu "end"
    def __recv_until(self, end=b'\x03'):
        buff = b""
        temp = None
        while True:
            temp = self.__SOCKET.recv(1)
            if temp == end:
                break
            buff += temp

        return buff

    # obsługiwanie requestów update z serwera
    def __call_update(self, buff):
        ch = buff[0]
        if chr(ch) == codes.CODE_UPDATE_INVITATIONS.decode("ascii"):
                try:
                    self.__conversations_class.force_update_invitations()
                except:
                    pass 
        elif chr(ch) == codes.CODE_UPDATE_CONVERSATIONS.decode("ascii"):
                try:
                    self.__conversations_class.force_update_conversations()
                except:
                    pass 
        elif chr(ch) == codes.CODE_UPDATE_MESSAGES.decode("ascii"):
                try:
                    self.__messaging_class.force_update_messages(buff[1:])
                except:
                    pass
        return

    # nasłuchiwanie
    def listen(self):
        while self.__keep_listen:
            buff = None
            try:
                buff = self.__recv_until()
            except Exception as e:
                print(f"ERROR: listen: {e}!")

            if buff:
                self.__call_update(buff)

            with self.__rwlock.gen_wlock():
                self.__user.responses.append(buff)
        return 

    def exit_listen(self):
        self.__keep_listen = False
        self.__SOCKET.shutdown(1)
        self.__SOCKET.close()

