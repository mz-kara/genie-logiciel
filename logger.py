import datetime

class Logger:
    def __init__(self):
        self.fileName = "log.txt"

    def Setter(self, fileName):
        self.fileName = fileName

    def write(self, content):
        timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        with open(self.fileName, "a") as file:
            file.write(f"[{timestamp}] {content}\n")

class FileWriter(Logger):
    def write(self, content):
        self.fileName = "log.txt"
        timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        with open(self.fileName, "a") as file:
            file.write(f"[{timestamp}] {content}\n")

class MockFW(Logger):
    def write(self, content):
        self.fileName = "mock.txt"
        timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        with open(self.fileName, "a") as file:
            file.write(f"[{timestamp}] {content}\n")