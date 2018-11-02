class Patient(object):

    def __init__(self, name, id):
        self.name = name
        self.id = id

    def __str__(self):
        return "name: " + self.name + "| id: " + self.id
