class Patient(object):

    def __init__(self, name, id, sex):
        self.name = name
        self.id = id
        self.sex = sex

    def __str__(self):
        return "name: " + self.name + "| id: " + self.id
