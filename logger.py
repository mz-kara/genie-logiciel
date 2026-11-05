class Logger:
    def __init__(self):
        self.fileName = "log.txt"

    def Setter(self, fileName):
        self.fileName = fileName

    def write(self, content):
        with open(self.fileName, "a") as file:
            file.write(content)

class FileWriter(Logger):
    def write(self, content):
        self.fileName = "log.txt"
        with open(self.fileName, "a") as file:
            file.write(content)

class MockFW(Logger):
    def write(self, content):
        self.fileName = "mock.txt"
        with open(self.fileName, "a") as file:
            file.write(content)