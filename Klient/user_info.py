class User():
    def __init__(self):
        self.id = None
        self.responses = [] 
        self.conversators_ids = [] 
        self.conversators_nicks = []
        self.invitators_ids = []
        self.invitators_nicks = []
        self.current_messaging = None

    def set_id(self):
        self.id = id

    def get_id(self):
        return self.id
