class Patient(object):

    def __init__(self, name, id, age, sex):
        self.name = name
        self.id = id
        self.age = age
        self.sex = sex

    def __str__(self):
        return "name: " + self.name + "| id: " + self.id + "| age: " + self.age + "| sex: " + self.sex
